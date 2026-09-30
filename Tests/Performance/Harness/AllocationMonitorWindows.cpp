/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "AllocationMonitor.h"

#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <psapi.h>

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

namespace
{
	using AllocateFunction = void*(__cdecl*)(std::size_t);
	using AllocateZeroedFunction = void*(__cdecl*)(std::size_t, std::size_t);
	using ReallocateFunction = void*(__cdecl*)(void*, std::size_t);
	using ReleaseFunction = void(__cdecl*)(void*);
	using AllocateAlignedFunction = void*(__cdecl*)(std::size_t, std::size_t);
	using ReallocateAlignedFunction = void*(__cdecl*)(void*, std::size_t, std::size_t);
	using BlockSizeFunction = std::size_t(__cdecl*)(void*);

	// The heap functions of the C runtime (ucrtbase.dll), to which the counting functions forward.
	struct RuntimeHeap
	{
		AllocateFunction allocate = nullptr;
		AllocateZeroedFunction allocateZeroed = nullptr;
		ReallocateFunction reallocate = nullptr;
		ReleaseFunction release = nullptr;
		AllocateAlignedFunction allocateAligned = nullptr;
		ReallocateAlignedFunction reallocateAligned = nullptr;
		ReleaseFunction releaseAligned = nullptr;
		BlockSizeFunction blockSize = nullptr;
	};

	// The counts, updated by every thread that allocates.
	struct HeapCounters
	{
		std::atomic<bool> active{ false };
		std::atomic<bool> allThreads{ false };
		std::atomic<DWORD> thread{ 0 };
		std::atomic<std::int64_t> allocations{ 0 };
		std::atomic<std::int64_t> deallocations{ 0 };
		std::atomic<std::int64_t> allocatedBytes{ 0 };
		std::atomic<std::int64_t> liveBytes{ 0 };
		std::atomic<std::int64_t> peakLiveBytes{ 0 };
	};

	// An import table entry redirected by install, with the function it held before.
	struct RedirectedEntry
	{
		ULONG_PTR* entry;
		ULONG_PTR original;
	};

	RuntimeHeap runtimeHeap;
	HeapCounters heapCounters;
	std::vector<RedirectedEntry> redirectedEntries;

	bool isCounted()
	{
		if (!heapCounters.active.load(std::memory_order_relaxed))
		{
			return false;
		}
		return heapCounters.allThreads.load(std::memory_order_relaxed)
			|| heapCounters.thread.load(std::memory_order_relaxed) == GetCurrentThreadId();
	}

	void addLiveBytes(std::int64_t p_bytes)
	{
		const std::int64_t LIVE_BYTES = heapCounters.liveBytes.fetch_add(p_bytes, std::memory_order_relaxed) + p_bytes;
		std::int64_t peak = heapCounters.peakLiveBytes.load(std::memory_order_relaxed);
		while (LIVE_BYTES > peak
			&& !heapCounters.peakLiveBytes.compare_exchange_weak(peak, LIVE_BYTES, std::memory_order_relaxed))
		{
		}
	}

	void countAllocation(std::size_t p_bytes, bool p_live)
	{
		const std::int64_t BYTES = static_cast<std::int64_t>(p_bytes);
		heapCounters.allocations.fetch_add(1, std::memory_order_relaxed);
		heapCounters.allocatedBytes.fetch_add(BYTES, std::memory_order_relaxed);
		if (p_live)
		{
			addLiveBytes(BYTES);
		}
	}

	void countDeallocation(std::size_t p_bytes, bool p_live)
	{
		heapCounters.deallocations.fetch_add(1, std::memory_order_relaxed);
		if (p_live)
		{
			addLiveBytes(-static_cast<std::int64_t>(p_bytes));
		}
	}

	// The functions below replace the heap functions of the C runtime in the import tables. They
	// must not allocate: they only count, then call the function they replace.

	void* __cdecl countedAllocate(std::size_t p_size)
	{
		void* const BLOCK = runtimeHeap.allocate(p_size);
		if (BLOCK != nullptr && isCounted())
		{
			countAllocation(p_size, true);
		}
		return BLOCK;
	}

	void* __cdecl countedAllocateZeroed(std::size_t p_count, std::size_t p_size)
	{
		void* const BLOCK = runtimeHeap.allocateZeroed(p_count, p_size);
		if (BLOCK != nullptr && isCounted())
		{
			countAllocation(p_count * p_size, true);
		}
		return BLOCK;
	}

	void* __cdecl countedReallocate(void* p_block, std::size_t p_size)
	{
		if (!isCounted())
		{
			return runtimeHeap.reallocate(p_block, p_size);
		}
		const std::size_t OLD_SIZE = p_block != nullptr ? runtimeHeap.blockSize(p_block) : 0;
		void* const BLOCK = runtimeHeap.reallocate(p_block, p_size);
		// realloc frees the old block when it succeeds, or when the new size is zero.
		if (p_block != nullptr && (BLOCK != nullptr || p_size == 0))
		{
			countDeallocation(OLD_SIZE, true);
		}
		if (BLOCK != nullptr)
		{
			countAllocation(p_size, true);
		}
		return BLOCK;
	}

	void __cdecl countedRelease(void* p_block)
	{
		if (p_block != nullptr && isCounted())
		{
			countDeallocation(runtimeHeap.blockSize(p_block), true);
		}
		runtimeHeap.release(p_block);
	}

	void* __cdecl countedAllocateAligned(std::size_t p_size, std::size_t p_alignment)
	{
		void* const BLOCK = runtimeHeap.allocateAligned(p_size, p_alignment);
		if (BLOCK != nullptr && isCounted())
		{
			countAllocation(p_size, false);
		}
		return BLOCK;
	}

	void* __cdecl countedReallocateAligned(void* p_block, std::size_t p_size, std::size_t p_alignment)
	{
		void* const BLOCK = runtimeHeap.reallocateAligned(p_block, p_size, p_alignment);
		if (isCounted())
		{
			if (p_block != nullptr && (BLOCK != nullptr || p_size == 0))
			{
				countDeallocation(0, false);
			}
			if (BLOCK != nullptr)
			{
				countAllocation(p_size, false);
			}
		}
		return BLOCK;
	}

	void __cdecl countedReleaseAligned(void* p_block)
	{
		if (p_block != nullptr && isCounted())
		{
			countDeallocation(0, false);
		}
		runtimeHeap.releaseAligned(p_block);
	}

	template<typename Function>
	Function runtimeFunction(HMODULE p_runtime, const char* p_name)
	{
		const FARPROC ADDRESS = GetProcAddress(p_runtime, p_name);
		if (ADDRESS == nullptr)
		{
			throw std::runtime_error(std::string("AllocationMonitor: ucrtbase.dll does not export ") + p_name);
		}
		return reinterpret_cast<Function>(ADDRESS);
	}

	void resolveRuntimeHeap(HMODULE p_runtime)
	{
		if (runtimeHeap.allocate != nullptr)
		{
			return;
		}
		RuntimeHeap heap;
		heap.allocate = runtimeFunction<AllocateFunction>(p_runtime, "malloc");
		heap.allocateZeroed = runtimeFunction<AllocateZeroedFunction>(p_runtime, "calloc");
		heap.reallocate = runtimeFunction<ReallocateFunction>(p_runtime, "realloc");
		heap.release = runtimeFunction<ReleaseFunction>(p_runtime, "free");
		heap.allocateAligned = runtimeFunction<AllocateAlignedFunction>(p_runtime, "_aligned_malloc");
		heap.reallocateAligned = runtimeFunction<ReallocateAlignedFunction>(p_runtime, "_aligned_realloc");
		heap.releaseAligned = runtimeFunction<ReleaseFunction>(p_runtime, "_aligned_free");
		heap.blockSize = runtimeFunction<BlockSizeFunction>(p_runtime, "_msize");
		runtimeHeap = heap;
	}

	// Returns the counting function that replaces p_function, or 0 when p_function is not a heap
	// function of the C runtime.
	ULONG_PTR replacementOf(ULONG_PTR p_function)
	{
		const ULONG_PTR REPLACEMENTS[][2] = {
			{ reinterpret_cast<ULONG_PTR>(runtimeHeap.allocate), reinterpret_cast<ULONG_PTR>(&countedAllocate) },
			{ reinterpret_cast<ULONG_PTR>(runtimeHeap.allocateZeroed), reinterpret_cast<ULONG_PTR>(&countedAllocateZeroed) },
			{ reinterpret_cast<ULONG_PTR>(runtimeHeap.reallocate), reinterpret_cast<ULONG_PTR>(&countedReallocate) },
			{ reinterpret_cast<ULONG_PTR>(runtimeHeap.release), reinterpret_cast<ULONG_PTR>(&countedRelease) },
			{ reinterpret_cast<ULONG_PTR>(runtimeHeap.allocateAligned), reinterpret_cast<ULONG_PTR>(&countedAllocateAligned) },
			{ reinterpret_cast<ULONG_PTR>(runtimeHeap.reallocateAligned), reinterpret_cast<ULONG_PTR>(&countedReallocateAligned) },
			{ reinterpret_cast<ULONG_PTR>(runtimeHeap.releaseAligned), reinterpret_cast<ULONG_PTR>(&countedReleaseAligned) } };
		for (const auto& REPLACEMENT : REPLACEMENTS)
		{
			if (p_function == REPLACEMENT[0])
			{
				return REPLACEMENT[1];
			}
		}
		return 0;
	}

	void redirect(ULONG_PTR* p_entry, ULONG_PTR p_replacement)
	{
		redirectedEntries.push_back(RedirectedEntry{ p_entry, *p_entry });
		DWORD protection = 0;
		if (VirtualProtect(p_entry, sizeof(ULONG_PTR), PAGE_READWRITE, &protection) == 0)
		{
			redirectedEntries.pop_back();
			throw std::runtime_error("AllocationMonitor: cannot write an import table entry");
		}
		*p_entry = p_replacement;
		VirtualProtect(p_entry, sizeof(ULONG_PTR), protection, &protection);
	}

	// Redirects the heap functions of the C runtime in the import table of p_module. The entries
	// already redirected no longer hold a runtime function and are left alone.
	void redirectModule(HMODULE p_module)
	{
		BYTE* const BASE = reinterpret_cast<BYTE*>(p_module);
		const IMAGE_DOS_HEADER* const DOS_HEADER = reinterpret_cast<const IMAGE_DOS_HEADER*>(BASE);
		if (DOS_HEADER->e_magic != IMAGE_DOS_SIGNATURE)
		{
			return;
		}
		const IMAGE_NT_HEADERS* const NT_HEADERS = reinterpret_cast<const IMAGE_NT_HEADERS*>(BASE + DOS_HEADER->e_lfanew);
		if (NT_HEADERS->Signature != IMAGE_NT_SIGNATURE)
		{
			return;
		}
		const IMAGE_DATA_DIRECTORY& IMPORTS = NT_HEADERS->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
		if (IMPORTS.VirtualAddress == 0)
		{
			return;
		}
		for (const IMAGE_IMPORT_DESCRIPTOR* descriptor = reinterpret_cast<const IMAGE_IMPORT_DESCRIPTOR*>(BASE + IMPORTS.VirtualAddress);
			descriptor->Name != 0; ++descriptor)
		{
			for (IMAGE_THUNK_DATA* entry = reinterpret_cast<IMAGE_THUNK_DATA*>(BASE + descriptor->FirstThunk);
				entry->u1.Function != 0; ++entry)
			{
				const ULONG_PTR REPLACEMENT = replacementOf(entry->u1.Function);
				if (REPLACEMENT != 0)
				{
					redirect(&entry->u1.Function, REPLACEMENT);
				}
			}
		}
	}

	std::vector<HMODULE> loadedModules()
	{
		std::vector<HMODULE> modules(256);
		for (;;)
		{
			DWORD neededBytes = 0;
			const DWORD AVAILABLE_BYTES = static_cast<DWORD>(modules.size() * sizeof(HMODULE));
			if (EnumProcessModules(GetCurrentProcess(), modules.data(), AVAILABLE_BYTES, &neededBytes) == 0)
			{
				throw std::runtime_error("AllocationMonitor: cannot list the modules of the process");
			}
			const std::size_t COUNT = neededBytes / sizeof(HMODULE);
			const bool COMPLETE = COUNT <= modules.size();
			modules.resize(COUNT);
			if (COMPLETE)
			{
				return modules;
			}
		}
	}
}

namespace Performance
{
	bool AllocationMonitor::isSupported()
	{
		return true;
	}

	void AllocationMonitor::install()
	{
		const HMODULE RUNTIME = GetModuleHandleW(L"ucrtbase.dll");
		if (RUNTIME == nullptr)
		{
			throw std::runtime_error("AllocationMonitor: ucrtbase.dll is not loaded");
		}
		resolveRuntimeHeap(RUNTIME);
		for (const HMODULE MODULE : loadedModules())
		{
			if (MODULE != RUNTIME)
			{
				redirectModule(MODULE);
			}
		}
	}

	void AllocationMonitor::uninstall()
	{
		for (auto redirected = redirectedEntries.rbegin(); redirected != redirectedEntries.rend(); ++redirected)
		{
			MEMORY_BASIC_INFORMATION memory{};
			if (VirtualQuery(redirected->entry, &memory, sizeof(memory)) == 0 || memory.State != MEM_COMMIT)
			{
				continue;
			}
			DWORD protection = 0;
			if (VirtualProtect(redirected->entry, sizeof(ULONG_PTR), PAGE_READWRITE, &protection) != 0)
			{
				*redirected->entry = redirected->original;
				VirtualProtect(redirected->entry, sizeof(ULONG_PTR), protection, &protection);
			}
		}
		redirectedEntries.clear();
	}

	void AllocationMonitor::start(ThreadScope p_scope)
	{
		heapCounters.active.store(false);
		heapCounters.allocations.store(0);
		heapCounters.deallocations.store(0);
		heapCounters.allocatedBytes.store(0);
		heapCounters.liveBytes.store(0);
		heapCounters.peakLiveBytes.store(0);
		heapCounters.allThreads.store(p_scope == ThreadScope::All);
		heapCounters.thread.store(GetCurrentThreadId());
		heapCounters.active.store(true);
	}

	AllocationCounts AllocationMonitor::getCounts()
	{
		AllocationCounts counts;
		counts.allocations = heapCounters.allocations.load();
		counts.deallocations = heapCounters.deallocations.load();
		counts.allocatedBytes = heapCounters.allocatedBytes.load();
		counts.liveBytes = heapCounters.liveBytes.load();
		counts.peakLiveBytes = heapCounters.peakLiveBytes.load();
		return counts;
	}

	AllocationCounts AllocationMonitor::stop()
	{
		heapCounters.active.store(false);
		return getCounts();
	}
}
