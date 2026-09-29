/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "MemoryMonitor.h"

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <psapi.h>
#endif

namespace Performance
{
	bool MemoryMonitor::isSupported()
	{
#ifdef _WIN32
		return true;
#else
		return false;
#endif
	}

	ProcessMemory MemoryMonitor::read()
	{
		ProcessMemory memory;
#ifdef _WIN32
		PROCESS_MEMORY_COUNTERS_EX counters{};
		counters.cb = sizeof(counters);
		if (GetProcessMemoryInfo(GetCurrentProcess(), reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&counters), sizeof(counters)) != 0)
		{
			memory.privateBytes = counters.PrivateUsage;
			// The peak of the commit charge, which for a process is its private memory.
			memory.peakPrivateBytes = counters.PeakPagefileUsage;
			memory.workingSetBytes = counters.WorkingSetSize;
			memory.peakWorkingSetBytes = counters.PeakWorkingSetSize;
		}
#endif
		return memory;
	}
}
