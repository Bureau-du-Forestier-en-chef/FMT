/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef PERFORMANCE_MEMORYMONITOR_H_INCLUDED
#define PERFORMANCE_MEMORYMONITOR_H_INCLUDED

#include <cstdint>

namespace Performance
{
	// Memory of the process, in bytes. Every member is zero where it cannot be read.
	struct ProcessMemory
	{
		// Memory committed for the process alone, and its highest value since the process started.
		std::uint64_t privateBytes = 0;
		std::uint64_t peakPrivateBytes = 0;
		// Memory of the process resident in physical memory, and its highest value.
		std::uint64_t workingSetBytes = 0;
		std::uint64_t peakWorkingSetBytes = 0;
	};

	// Reads the memory used by the current process.
	class MemoryMonitor
	{
	public:
		// Returns false where the memory of the process cannot be read, outside Windows.
		static bool isSupported();
		static ProcessMemory read();
	};
}

#endif
