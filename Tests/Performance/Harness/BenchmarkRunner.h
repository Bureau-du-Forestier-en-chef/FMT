/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef PERFORMANCE_BENCHMARKRUNNER_H_INCLUDED
#define PERFORMANCE_BENCHMARKRUNNER_H_INCLUDED

#include "Benchmark.h"
#include "BenchmarkOptions.h"
#include "BenchmarkResult.h"

#include <chrono>
#include <cstddef>
#include <optional>

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

		static RunSettings forMode(BenchmarkMode p_mode);
	};

	// Measures one benchmark: prepare, warm-up, timed samples without the allocation monitor, then
	// counted calls. The results of the counted calls and their allocations are then checked
	// against the expectation of the benchmark.
	class BenchmarkRunner
	{
	public:
		explicit BenchmarkRunner(const RunSettings& p_settings);
		BenchmarkResult run(Benchmark& p_benchmark, const std::optional<Expectation>& p_expectation);

	private:
		// What the counted calls produced.
		struct CountedCalls
		{
			AllocationStatistics allocations;
			double lastResult = 0.0;
			// Calls whose result differs from the expected one.
			std::size_t mismatches = 0;
		};

		RunSettings m_settings;
		// Receives every result, so that the compiler cannot remove the measured calls.
		volatile double m_sink = 0.0;

		void _warmUp(Benchmark& p_benchmark);
		std::size_t _calibrate(Benchmark& p_benchmark);
		TimingStatistics _time(Benchmark& p_benchmark, std::size_t p_callsPerSample);
		CountedCalls _count(Benchmark& p_benchmark, const std::optional<Expectation>& p_expectation);
	};
}

#endif
