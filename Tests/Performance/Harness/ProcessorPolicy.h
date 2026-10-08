/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef PERFORMANCE_PROCESSORPOLICY_H_INCLUDED
#define PERFORMANCE_PROCESSORPOLICY_H_INCLUDED

#include <string>

namespace Performance
{
	// Where the threads of the process run: the measuring thread, and those FMT starts to run a task.
	//
	// Hybrid processors, such as the 13th generation of Intel Core, mix performance cores with
	// efficiency cores that run the same code up to 1.7 times slower. Windows may keep a thread on
	// either kind for a whole benchmark, and moves a process whose window is in the background to
	// the efficiency cores (EcoQoS, the efficiency mode of the task manager): the measured times
	// then jump. Two runs only compare when their calls ran on the same kind of cores.
	class ProcessorPolicy
	{
	public:
		// Asks Windows not to throttle the process, and restricts the calling thread, and every thread
		// the process starts afterwards, to the logical processors of the most performant class when
		// the processor has several classes. Returns what the threads may run on, written with the
		// results.
		static std::string apply();
	};
}

#endif
