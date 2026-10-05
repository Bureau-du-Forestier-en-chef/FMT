/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef PERFORMANCE_BENCHMARKRESULT_H_INCLUDED
#define PERFORMANCE_BENCHMARKRESULT_H_INCLUDED

#include "AllocationMonitor.h"
#include "BenchmarkEnvironment.h"
#include "BenchmarkOptions.h"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <ostream>
#include <string>
#include <vector>

namespace Performance
{
	// Duration of one call of the measured operation over the timed samples, in nanoseconds.
	struct TimingStatistics
	{
		std::size_t samples = 0;
		std::size_t callsPerSample = 0;
		double minimumNs = 0.0;
		double maximumNs = 0.0;
		double medianNs = 0.0;
		double meanNs = 0.0;
		double standardDeviationNs = 0.0;
	};

	// Duration of one phase of a call over the timed samples, in nanoseconds (see
	// Benchmark::definePhase).
	struct PhaseStatistics
	{
		std::string name;
		double minimumNs = 0.0;
		double medianNs = 0.0;
		double maximumNs = 0.0;
	};

	// Heap activity of the counted calls, counted one call at a time (see AllocationMonitor).
	struct AllocationStatistics
	{
		// False where the allocation monitor is not available: the counts are then zero.
		bool measured = false;
		std::size_t calls = 0;
		std::int64_t minimumPerCall = 0;
		// Lower median, so that it is the count of an actual call.
		std::int64_t medianPerCall = 0;
		std::int64_t maximumPerCall = 0;
		std::int64_t medianBytesPerCall = 0;
		// Bytes a counted call keeps allocated when it returns: what it allocates minus what it
		// frees. Lower median, and maximum.
		std::int64_t medianRetainedBytesPerCall = 0;
		std::int64_t maximumRetainedBytesPerCall = 0;
		AllocationCounts total;
	};

	// One check made on a benchmark, printed "ok" or "FAILED".
	struct BenchmarkCheck
	{
		bool passed = false;
		std::string description;
	};

	// What was measured and checked for one benchmark.
	struct BenchmarkResult
	{
		std::string benchmark;
		std::string group;
		std::string dataset;
		// SHA-256 of the files of the dataset, or an empty text (see DatasetFingerprint).
		std::string datasetFingerprint;
		std::size_t threads = 1;
		TimingStatistics timing;
		// One entry per phase of the benchmark, in their order: none for an operation timed as a whole.
		std::vector<PhaseStatistics> phases;
		AllocationStatistics allocations;
		std::uint64_t processPeakPrivateBytes = 0;
		// Result of the last counted call.
		double result = 0.0;
		std::optional<Expectation> expectation;
		std::vector<BenchmarkCheck> checks;
		// Why a check could not be made, for instance an allocation bound without the monitor.
		std::string skipReason;

		// True when every check passed.
		bool isValid() const;
		bool isSkipped() const;
	};

	// Writes the results of one run and their environment as JSON: schema version 3, described in
	// Documentation/PerformanceTesting.md.
	void writeResults(std::ostream& p_stream, const BenchmarkEnvironment& p_environment,
		const std::vector<BenchmarkResult>& p_results);
}

#endif
