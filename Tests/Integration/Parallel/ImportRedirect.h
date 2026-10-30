/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef TESTING_IMPORTREDIRECT_H_INCLUDED
#define TESTING_IMPORTREDIRECT_H_INCLUDED

#ifdef _WIN32
	#ifndef NOMINMAX
		#define NOMINMAX
	#endif
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif
	#include <windows.h>
#endif

#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <cstring>
#include <map>
#include <mutex>
#include <set>
#include <thread>

// Observes, from a test, the calls that a module of the process makes to a function of another module, without
// changing either: the entry of the function in the import table of the calling module is redirected to a function
// of the test, which records the call, then calls the function it replaces. The allocation monitor of the
// benchmarks counts the allocations the same way (Tests/Performance/Harness/AllocationMonitorWindows.cpp). Windows
// only: elsewhere, nothing is redirected.
namespace Testing
{
	// Redirects the import p_function of p_importer, from p_exporter, to p_replacement, and returns the address the
	// entry held, or nullptr when p_importer is not loaded or does not import p_function.
	inline void* redirectImport(const char* p_importer, const char* p_exporter, const char* p_function, void* p_replacement)
	{
	#ifdef _WIN32
		const HMODULE MODULE = GetModuleHandleA(p_importer);
		if (MODULE == nullptr)
		{
			return nullptr;
		}
		BYTE* const BASE = reinterpret_cast<BYTE*>(MODULE);
		const IMAGE_NT_HEADERS* const HEADERS = reinterpret_cast<const IMAGE_NT_HEADERS*>(
			BASE + reinterpret_cast<const IMAGE_DOS_HEADER*>(BASE)->e_lfanew);
		const IMAGE_DATA_DIRECTORY& IMPORTS = HEADERS->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
		for (const IMAGE_IMPORT_DESCRIPTOR* descriptor = reinterpret_cast<const IMAGE_IMPORT_DESCRIPTOR*>(BASE + IMPORTS.VirtualAddress);
			descriptor->Name != 0; ++descriptor)
		{
			if (_stricmp(reinterpret_cast<const char*>(BASE + descriptor->Name), p_exporter) != 0)
			{
				continue;
			}
			const IMAGE_THUNK_DATA* names = reinterpret_cast<const IMAGE_THUNK_DATA*>(BASE + descriptor->OriginalFirstThunk);
			IMAGE_THUNK_DATA* entries = reinterpret_cast<IMAGE_THUNK_DATA*>(BASE + descriptor->FirstThunk);
			for (; names->u1.AddressOfData != 0; ++names, ++entries)
			{
				if (IMAGE_SNAP_BY_ORDINAL(names->u1.Ordinal))
				{
					continue;
				}
				const IMAGE_IMPORT_BY_NAME* const NAME = reinterpret_cast<const IMAGE_IMPORT_BY_NAME*>(BASE + names->u1.AddressOfData);
				if (std::strcmp(reinterpret_cast<const char*>(NAME->Name), p_function) == 0)
				{
					DWORD protection = 0;
					VirtualProtect(&entries->u1.Function, sizeof(ULONG_PTR), PAGE_READWRITE, &protection);
					void* const ORIGINAL = reinterpret_cast<void*>(entries->u1.Function);
					entries->u1.Function = reinterpret_cast<ULONG_PTR>(p_replacement);
					VirtualProtect(&entries->u1.Function, sizeof(ULONG_PTR), protection, &protection);
					return ORIGINAL;
				}
			}
		}
	#endif
		return nullptr;
	}

	// Redirects an import for the life of the object, then gives the entry back the address it held.
	class RedirectedImport
	{
	public:
		RedirectedImport(const char* p_importer, const char* p_exporter, const char* p_function, void* p_replacement) :
			m_importer(p_importer),
			m_exporter(p_exporter),
			m_function(p_function),
			m_original(redirectImport(p_importer, p_exporter, p_function, p_replacement))
		{
		}

		~RedirectedImport()
		{
			if (m_original != nullptr)
			{
				redirectImport(m_importer, m_exporter, m_function, m_original);
			}
		}

		RedirectedImport(const RedirectedImport&) = delete;
		RedirectedImport& operator=(const RedirectedImport&) = delete;

		// The address the entry held, or nullptr when nothing was redirected.
		void* getOriginal() const
		{
			return m_original;
		}

	private:
		const char* m_importer;
		const char* m_exporter;
		const char* m_function;
		void* m_original;
	};

	// The calls made on objects, such as a MOSEK task or a message handler: by object, the threads that called it and
	// the most threads inside a call at the same time. With a meeting time, the first thread that enters for an object
	// waits up to that long for a second thread entering for the same object, then both go on at once: two calls that
	// can run at the same time then do, without any wait in FMT.
	class ObjectCalls
	{
	public:
		explicit ObjectCalls(std::chrono::milliseconds p_meeting = std::chrono::milliseconds(0)) :
			m_meeting(p_meeting)
		{
		}

		void enter(const void* p_object)
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			Calls& calls = m_calls[p_object];
			calls.threads.insert(std::this_thread::get_id());
			++calls.inside;
			calls.mostInside = std::max(calls.mostInside, calls.inside);
			if (m_meeting.count() == 0)
			{
				return;
			}
			if (calls.waiting)
			{
				calls.partner = true;
				m_condition.notify_all();
				return;
			}
			calls.waiting = true;
			calls.partner = false;
			m_condition.wait_for(lock, m_meeting, [&calls]() { return calls.partner; });
			calls.waiting = false;
		}

		void leave(const void* p_object)
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			--m_calls[p_object].inside;
		}

		// The most threads, other than p_ignored, that called one object.
		std::size_t getMostThreadsOfAnObject(std::thread::id p_ignored) const
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			std::size_t most = 0;
			for (const auto& OBJECT : m_calls)
			{
				std::set<std::thread::id> threads = OBJECT.second.threads;
				threads.erase(p_ignored);
				most = std::max(most, threads.size());
			}
			return most;
		}

		// The most threads inside a call for one object at the same time.
		int getMostThreadsAtOnce() const
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			int most = 0;
			for (const auto& OBJECT : m_calls)
			{
				most = std::max(most, OBJECT.second.mostInside);
			}
			return most;
		}

		bool empty() const
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			return m_calls.empty();
		}

	private:
		struct Calls
		{
			std::set<std::thread::id> threads;
			int inside = 0;
			int mostInside = 0;
			bool waiting = false;
			bool partner = false;
		};
		std::chrono::milliseconds m_meeting;
		mutable std::mutex m_mutex;
		std::condition_variable m_condition;
		std::map<const void*, Calls> m_calls;
	};
}

#endif
