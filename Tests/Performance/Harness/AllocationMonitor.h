/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef PERFORMANCE_ALLOCATIONMONITOR_H_INCLUDED
#define PERFORMANCE_ALLOCATIONMONITOR_H_INCLUDED

#include <cstdint>

namespace Performance
{
	// Heap activity counted since AllocationMonitor::start.
	struct AllocationCounts
	{
		std::int64_t allocations = 0;
		std::int64_t deallocations = 0;
		std::int64_t allocatedBytes = 0;
		// Memory allocated and not yet freed since start, the blocks freed since start deducted: the
		// difference between two readings is what the calls in between kept. It falls below zero
		// when blocks allocated before start are freed.
		std::int64_t liveBytes = 0;
		// Highest amount of memory allocated and not yet freed since start. Blocks allocated before
		// start and freed after it lower the live amount, so the peak can stay at zero.
		std::int64_t peakLiveBytes = 0;
	};

	// Threads whose allocations are counted.
	enum class ThreadScope
	{
		Current,
		All
	};

	// Counts the heap allocations of every module of the process, FMTlib.dll included.
	//
	// Under MSVC, operator new is linked into each module, so replacing it in the executable would
	// not see the allocations made inside FMTlib.dll. Every module ends up calling the heap
	// functions of the C runtime (malloc, free...) through its import table: install redirects
	// those entries, in every loaded module, to functions that count and then forward the call.
	// FMTlib is not modified, and the measured build is the delivered one.
	//
	// Not counted: allocations made inside the C runtime itself (strdup, stdio buffers...), direct
	// calls to HeapAlloc or VirtualAlloc, and modules linked to a static C runtime. Aligned blocks
	// are counted, but left out of the live bytes.
	//
	// install and uninstall change process-wide state: call them while no other thread allocates,
	// and do not unload a module in between. Counting itself is thread-safe.
	class AllocationMonitor
	{
	public:
		// Returns false where the redirection is not implemented, outside Windows.
		static bool isSupported();
		// Redirects the heap functions in every loaded module. Calling it again covers the modules
		// loaded since the previous call.
		static void install();
		// Restores the heap functions redirected by install.
		static void uninstall();
		// Resets the counts, then counts the allocations of p_scope made through the redirected
		// functions.
		static void start(ThreadScope p_scope);
		// Returns the counts since start, while counting goes on.
		static AllocationCounts getCounts();
		// Stops counting and returns the counts since start.
		static AllocationCounts stop();
	};
}

#endif
