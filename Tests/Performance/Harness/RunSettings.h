/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef PERFORMANCE_RUNSETTINGS_H_INCLUDED
#define PERFORMANCE_RUNSETTINGS_H_INCLUDED

#include "BenchmarkOptions.h"

#include <chrono>
#include <cstddef>

namespace Performance
{
	// How many times a benchmark is called.
	struct RunSettings
	{
		// Calls made before any measurement: lazy initializations, caches, first touch of memory.
		std::size_t warmUpCalls = 0;
		// Timed samples. Each one times enough calls to last at least minimumSampleDuration, far
		// above the resolution of the clock.
		std::size_t samples = 0;
		std::chrono::nanoseconds minimumSampleDuration{ 0 };
		// Calls whose allocations are counted one at a time, and whose results are checked.
		std::size_t countedCalls = 0;

		// Settings of an operation that lasts microseconds or less: many calls per sample.
		static RunSettings forMode(BenchmarkMode p_mode);
		// Settings of an operation that lasts milliseconds or more, such as reading a project: one
		// call per sample, and fewer calls, so that a measurement lasts seconds, not hours.
		static RunSettings forSlowCalls(BenchmarkMode p_mode);
		// Settings of a model flow, from milliseconds on TWD_land to minutes on a production model: a
		// few complete runs, one call per sample, and one counted call.
		static RunSettings forFlows(BenchmarkMode p_mode);
	};
}

#endif
