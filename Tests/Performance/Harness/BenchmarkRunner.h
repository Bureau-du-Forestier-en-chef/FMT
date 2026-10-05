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
#include "RunSettings.h"

#include <cstddef>
#include <optional>
#include <vector>

namespace Performance
{
	// Measures one benchmark: prepare, warm-up, timed samples without the allocation monitor, then
	// counted calls. The results of the counted calls, their allocations, the memory they keep and
	// the peak memory of the process are then checked against the expectation of the benchmark.
	class BenchmarkRunner
	{
	public:
		explicit BenchmarkRunner(const RunSettings& p_settings);
		// p_firstInProcess is false when another benchmark ran before in the same process: the peak
		// memory of the process then says nothing about this one, and its bound is not checked.
		BenchmarkResult run(Benchmark& p_benchmark, const std::optional<Expectation>& p_expectation, bool p_firstInProcess);

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
		TimingStatistics _time(Benchmark& p_benchmark, std::size_t p_callsPerSample, std::vector<PhaseStatistics>& p_phases);
		CountedCalls _count(Benchmark& p_benchmark, const std::optional<Expectation>& p_expectation);
	};
}

#endif
