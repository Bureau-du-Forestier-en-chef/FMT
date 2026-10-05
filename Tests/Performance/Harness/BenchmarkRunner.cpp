/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "BenchmarkRunner.h"

#include "AllocationMonitor.h"
#include "MemoryMonitor.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <numeric>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

namespace
{
	using Clock = std::chrono::steady_clock;

	// A sample never times more calls than this, however fast the operation.
	constexpr std::size_t MAXIMUM_CALLS_PER_SAMPLE = std::size_t(1) << 24;
	// Tolerance on results, relative to the expected value: FMT rounds complex yields to 8 decimals.
	constexpr double RESULT_TOLERANCE = 1e-6;
	// Peak memory bounds are written in megabytes.
	constexpr double BYTES_PER_MEGABYTE = 1e6;

	bool isExpected(double p_value, double p_expected)
	{
		return std::abs(p_value - p_expected) <= RESULT_TOLERANCE * std::max(1.0, std::abs(p_expected));
	}

	std::string toText(double p_value)
	{
		std::ostringstream text;
		text.precision(12);
		text << p_value;
		return text.str();
	}

	double medianOfSorted(const std::vector<double>& p_sorted)
	{
		const std::size_t MIDDLE = p_sorted.size() / 2;
		if (p_sorted.size() % 2 == 0)
		{
			return (p_sorted.at(MIDDLE - 1) + p_sorted.at(MIDDLE)) / 2.0;
		}
		return p_sorted.at(MIDDLE);
	}

	Performance::TimingStatistics timingStatistics(std::vector<double> p_nanosecondsPerCall, std::size_t p_callsPerSample)
	{
		Performance::TimingStatistics statistics;
		statistics.samples = p_nanosecondsPerCall.size();
		statistics.callsPerSample = p_callsPerSample;
		if (p_nanosecondsPerCall.empty())
		{
			return statistics;
		}
		std::sort(p_nanosecondsPerCall.begin(), p_nanosecondsPerCall.end());
		const double COUNT = static_cast<double>(p_nanosecondsPerCall.size());
		statistics.minimumNs = p_nanosecondsPerCall.front();
		statistics.maximumNs = p_nanosecondsPerCall.back();
		statistics.medianNs = medianOfSorted(p_nanosecondsPerCall);
		statistics.meanNs = std::accumulate(p_nanosecondsPerCall.begin(), p_nanosecondsPerCall.end(), 0.0) / COUNT;
		if (p_nanosecondsPerCall.size() > 1)
		{
			double squares = 0.0;
			for (const double DURATION : p_nanosecondsPerCall)
			{
				squares += (DURATION - statistics.meanNs) * (DURATION - statistics.meanNs);
			}
			statistics.standardDeviationNs = std::sqrt(squares / (COUNT - 1.0));
		}
		return statistics;
	}

	Performance::PhaseStatistics phaseStatistics(const std::string& p_name, std::vector<double> p_nanosecondsPerCall)
	{
		Performance::PhaseStatistics statistics;
		statistics.name = p_name;
		if (p_nanosecondsPerCall.empty())
		{
			return statistics;
		}
		std::sort(p_nanosecondsPerCall.begin(), p_nanosecondsPerCall.end());
		statistics.minimumNs = p_nanosecondsPerCall.front();
		statistics.medianNs = medianOfSorted(p_nanosecondsPerCall);
		statistics.maximumNs = p_nanosecondsPerCall.back();
		return statistics;
	}

	Performance::AllocationStatistics allocationStatistics(std::vector<std::int64_t> p_allocationsPerCall,
		std::vector<std::int64_t> p_bytesPerCall, std::vector<std::int64_t> p_retainedBytesPerCall,
		const Performance::AllocationCounts& p_total)
	{
		Performance::AllocationStatistics statistics;
		statistics.calls = p_allocationsPerCall.size();
		statistics.total = p_total;
		if (p_allocationsPerCall.empty())
		{
			return statistics;
		}
		std::sort(p_allocationsPerCall.begin(), p_allocationsPerCall.end());
		std::sort(p_bytesPerCall.begin(), p_bytesPerCall.end());
		std::sort(p_retainedBytesPerCall.begin(), p_retainedBytesPerCall.end());
		const std::size_t LOWER_MIDDLE = (p_allocationsPerCall.size() - 1) / 2;
		statistics.minimumPerCall = p_allocationsPerCall.front();
		statistics.medianPerCall = p_allocationsPerCall.at(LOWER_MIDDLE);
		statistics.maximumPerCall = p_allocationsPerCall.back();
		statistics.medianBytesPerCall = p_bytesPerCall.at(LOWER_MIDDLE);
		statistics.medianRetainedBytesPerCall = p_retainedBytesPerCall.at(LOWER_MIDDLE);
		statistics.maximumRetainedBytesPerCall = p_retainedBytesPerCall.back();
		return statistics;
	}

	// Counts the allocations of the calls made during its lifetime. When it ends, also because a
	// call threw, it stops counting and removes the redirection of the heap functions.
	class CountingSession
	{
	public:
		explicit CountingSession(Performance::ThreadScope p_scope)
		{
			try
			{
				Performance::AllocationMonitor::install();
			}
			catch (...)
			{
				Performance::AllocationMonitor::uninstall();
				throw;
			}
			Performance::AllocationMonitor::start(p_scope);
		}

		~CountingSession()
		{
			Performance::AllocationMonitor::stop();
			Performance::AllocationMonitor::uninstall();
		}

		CountingSession(const CountingSession&) = delete;
		CountingSession& operator=(const CountingSession&) = delete;
	};

	// The bound applies to a typical call, the median of the counted calls: a value put in a cache,
	// or a container that grows now and then, makes some calls allocate more than the others.
	void checkAllocations(Performance::BenchmarkResult& p_result, std::int64_t p_bound)
	{
		const Performance::AllocationStatistics& ALLOCATIONS = p_result.allocations;
		if (!ALLOCATIONS.measured)
		{
			p_result.skipReason = "allocation bound not checked: no allocation monitor on this platform";
			return;
		}
		if (p_bound == 0)
		{
			const bool NONE = ALLOCATIONS.maximumPerCall == 0;
			p_result.checks.push_back({ NONE, NONE ? std::string("no counted call allocates (bound 0)")
				: "a counted call allocates " + std::to_string(ALLOCATIONS.maximumPerCall) + " times, the bound 0 allows none" });
			return;
		}
		std::string description = "a typical call allocates " + std::to_string(ALLOCATIONS.medianPerCall)
			+ " times (bound " + std::to_string(p_bound) + ")";
		if (ALLOCATIONS.medianPerCall < p_bound)
		{
			description += ": the bound can be lowered to " + std::to_string(ALLOCATIONS.medianPerCall);
		}
		p_result.checks.push_back({ ALLOCATIONS.medianPerCall <= p_bound, description });
	}

	// A call retains what it allocates and has not freed when it returns: a value kept in a cache,
	// or a leak. The bound applies to a typical call, like the allocation bound.
	void checkRetainedBytes(Performance::BenchmarkResult& p_result, std::int64_t p_bound)
	{
		const Performance::AllocationStatistics& ALLOCATIONS = p_result.allocations;
		if (!ALLOCATIONS.measured)
		{
			p_result.skipReason = "retained memory not checked: no allocation monitor on this platform";
			return;
		}
		if (p_bound == 0)
		{
			const bool NONE = ALLOCATIONS.maximumRetainedBytesPerCall <= 0;
			p_result.checks.push_back({ NONE, NONE ? std::string("no counted call retains memory (bound 0)")
				: "a counted call retains " + std::to_string(ALLOCATIONS.maximumRetainedBytesPerCall) + " bytes, the bound 0 allows none" });
			return;
		}
		std::string description = "a typical call retains " + std::to_string(ALLOCATIONS.medianRetainedBytesPerCall)
			+ " bytes (bound " + std::to_string(p_bound) + ")";
		if (ALLOCATIONS.medianRetainedBytesPerCall < p_bound)
		{
			description += ": the bound can be lowered to " + std::to_string(std::max<std::int64_t>(ALLOCATIONS.medianRetainedBytesPerCall, 0));
		}
		p_result.checks.push_back({ ALLOCATIONS.medianRetainedBytesPerCall <= p_bound, description });
	}

	// The peak is the one of the process, from its start: it gives what the benchmark needs only
	// when the benchmark runs alone in its process, as under ctest.
	void checkPeakMemory(Performance::BenchmarkResult& p_result, double p_boundMegabytes, bool p_firstInProcess)
	{
		if (!Performance::MemoryMonitor::isSupported())
		{
			p_result.skipReason = "peak memory not checked: the memory of the process cannot be read on this platform";
			return;
		}
		if (!p_firstInProcess)
		{
			p_result.skipReason = "peak memory not checked: another benchmark ran before in this process";
			return;
		}
		const double PEAK = std::round(static_cast<double>(p_result.processPeakPrivateBytes) / BYTES_PER_MEGABYTE * 10.0) / 10.0;
		p_result.checks.push_back({ PEAK <= p_boundMegabytes, "peak private memory of the process " + toText(PEAK)
			+ " MB (bound " + toText(p_boundMegabytes) + " MB)" });
	}

	void checkResult(Performance::BenchmarkResult& p_result, std::size_t p_mismatches, bool p_firstInProcess)
	{
		if (!p_result.expectation.has_value())
		{
			p_result.checks.push_back({ false, "no expected result for " + p_result.benchmark + " in the expectations file" });
			return;
		}
		const Performance::Expectation& EXPECTATION = *p_result.expectation;
		p_result.checks.push_back({ isExpected(p_result.result, EXPECTATION.result),
			"result " + toText(p_result.result) + " (expected " + toText(EXPECTATION.result) + ")" });
		p_result.checks.push_back({ p_mismatches == 0, p_mismatches == 0
			? std::string("every counted call returned the expected result")
			: std::to_string(p_mismatches) + " of " + std::to_string(p_result.allocations.calls)
				+ " counted calls returned another result" });
		if (EXPECTATION.maximumAllocations.has_value())
		{
			checkAllocations(p_result, *EXPECTATION.maximumAllocations);
		}
		if (EXPECTATION.maximumRetainedBytes.has_value())
		{
			checkRetainedBytes(p_result, *EXPECTATION.maximumRetainedBytes);
		}
		if (EXPECTATION.maximumPeakMegabytes.has_value())
		{
			checkPeakMemory(p_result, *EXPECTATION.maximumPeakMegabytes, p_firstInProcess);
		}
	}
}

namespace Performance
{
	BenchmarkRunner::BenchmarkRunner(const RunSettings& p_settings) :
		m_settings(p_settings)
	{
	}

	BenchmarkResult BenchmarkRunner::run(Benchmark& p_benchmark, const std::optional<Expectation>& p_expectation, bool p_firstInProcess)
	{
		BenchmarkResult result;
		result.benchmark = p_benchmark.getName();
		result.group = result.benchmark.substr(0, result.benchmark.find('.'));
		result.dataset = p_benchmark.getDataset();
		result.expectation = p_expectation;
		const std::string UNAVAILABLE = p_benchmark.getUnavailableReason();
		if (!UNAVAILABLE.empty())
		{
			result.skipReason = UNAVAILABLE;
			return result;
		}
		p_benchmark.prepare();
		result.datasetFingerprint = p_benchmark.getDatasetFingerprint();
		_warmUp(p_benchmark);
		result.timing = _time(p_benchmark, _calibrate(p_benchmark), result.phases);
		const CountedCalls COUNTED = _count(p_benchmark, p_expectation);
		result.allocations = COUNTED.allocations;
		result.result = COUNTED.lastResult;
		result.processPeakPrivateBytes = MemoryMonitor::read().peakPrivateBytes;
		checkResult(result, COUNTED.mismatches, p_firstInProcess);
		return result;
	}

	void BenchmarkRunner::_warmUp(Benchmark& p_benchmark)
	{
		for (std::size_t call = 0; call < m_settings.warmUpCalls; ++call)
		{
			m_sink = m_sink + p_benchmark.run();
		}
	}

	// Doubles the number of calls until they last at least the minimum duration of a sample.
	// Without a minimum duration, a sample is one call, and nothing needs to run.
	std::size_t BenchmarkRunner::_calibrate(Benchmark& p_benchmark)
	{
		if (m_settings.minimumSampleDuration.count() == 0)
		{
			return 1;
		}
		std::size_t calls = 1;
		while (calls < MAXIMUM_CALLS_PER_SAMPLE)
		{
			const Clock::time_point START = Clock::now();
			for (std::size_t call = 0; call < calls; ++call)
			{
				m_sink = m_sink + p_benchmark.run();
			}
			if (Clock::now() - START >= m_settings.minimumSampleDuration)
			{
				break;
			}
			calls *= 2;
		}
		return calls;
	}

	// The phases of a benchmark are timed in the same calls as the whole operation.
	TimingStatistics BenchmarkRunner::_time(Benchmark& p_benchmark, std::size_t p_callsPerSample, std::vector<PhaseStatistics>& p_phases)
	{
		const std::size_t PHASES = p_benchmark.getPhaseNames().size();
		std::vector<double> nanosecondsPerCall(m_settings.samples);
		std::vector<std::vector<double>> phaseNanosecondsPerCall(PHASES, std::vector<double>(m_settings.samples));
		const double CALLS = static_cast<double>(p_callsPerSample);
		for (std::size_t sample = 0; sample < m_settings.samples; ++sample)
		{
			p_benchmark.clearPhaseDurations();
			const Clock::time_point START = Clock::now();
			for (std::size_t call = 0; call < p_callsPerSample; ++call)
			{
				m_sink = m_sink + p_benchmark.run();
			}
			const std::chrono::duration<double, std::nano> ELAPSED = Clock::now() - START;
			nanosecondsPerCall[sample] = ELAPSED.count() / CALLS;
			const std::vector<double>& DURATIONS = p_benchmark.getPhaseDurations();
			for (std::size_t phase = 0; phase < PHASES; ++phase)
			{
				phaseNanosecondsPerCall[phase][sample] = DURATIONS.at(phase) / CALLS;
			}
		}
		p_phases.clear();
		for (std::size_t phase = 0; phase < PHASES; ++phase)
		{
			p_phases.push_back(phaseStatistics(p_benchmark.getPhaseNames().at(phase), phaseNanosecondsPerCall[phase]));
		}
		return timingStatistics(nanosecondsPerCall, p_callsPerSample);
	}

	// The calls are counted one at a time: the median of the counts is the cost of a typical call,
	// and a cache that serves some calls shows as a lower minimum.
	BenchmarkRunner::CountedCalls BenchmarkRunner::_count(Benchmark& p_benchmark, const std::optional<Expectation>& p_expectation)
	{
		const std::size_t CALLS = std::max<std::size_t>(m_settings.countedCalls, 1);
		// Sized before counting starts, so that the runner itself allocates nothing during the calls.
		std::vector<std::int64_t> allocationsPerCall(CALLS);
		std::vector<std::int64_t> bytesPerCall(CALLS);
		std::vector<std::int64_t> retainedBytesPerCall(CALLS);
		CountedCalls counted;
		AllocationCounts total;
		{
			std::optional<CountingSession> session;
			if (AllocationMonitor::isSupported())
			{
				session.emplace(p_benchmark.getThreadScope());
			}
			for (std::size_t call = 0; call < CALLS; ++call)
			{
				const AllocationCounts BEFORE = AllocationMonitor::getCounts();
				counted.lastResult = p_benchmark.run();
				const AllocationCounts AFTER = AllocationMonitor::getCounts();
				allocationsPerCall[call] = AFTER.allocations - BEFORE.allocations;
				bytesPerCall[call] = AFTER.allocatedBytes - BEFORE.allocatedBytes;
				retainedBytesPerCall[call] = AFTER.liveBytes - BEFORE.liveBytes;
				if (p_expectation.has_value() && !isExpected(counted.lastResult, p_expectation->result))
				{
					++counted.mismatches;
				}
			}
			total = AllocationMonitor::getCounts();
		}
		counted.allocations = allocationStatistics(allocationsPerCall, bytesPerCall, retainedBytesPerCall, total);
		counted.allocations.measured = AllocationMonitor::isSupported();
		return counted;
	}
}
