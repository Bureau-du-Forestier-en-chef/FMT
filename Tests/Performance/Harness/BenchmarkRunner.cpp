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

	Performance::AllocationStatistics allocationStatistics(std::vector<std::int64_t> p_allocationsPerCall,
		std::vector<std::int64_t> p_bytesPerCall, const Performance::AllocationCounts& p_total)
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
		const std::size_t LOWER_MIDDLE = (p_allocationsPerCall.size() - 1) / 2;
		statistics.minimumPerCall = p_allocationsPerCall.front();
		statistics.medianPerCall = p_allocationsPerCall.at(LOWER_MIDDLE);
		statistics.maximumPerCall = p_allocationsPerCall.back();
		statistics.medianBytesPerCall = p_bytesPerCall.at(LOWER_MIDDLE);
		return statistics;
	}

	// Counts the allocations of the calls made during its lifetime. When it ends, also because a
	// call threw, it stops counting and removes the redirection of the heap functions.
	class CountingSession
	{
	public:
		CountingSession()
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
			Performance::AllocationMonitor::start(Performance::ThreadScope::Current);
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

	void checkResult(Performance::BenchmarkResult& p_result, std::size_t p_mismatches)
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
	}
}

namespace Performance
{
	RunSettings RunSettings::forMode(BenchmarkMode p_mode)
	{
		RunSettings settings;
		if (p_mode == BenchmarkMode::Full)
		{
			settings.warmUpCalls = 100;
			settings.samples = 30;
			settings.minimumSampleDuration = std::chrono::milliseconds(5);
			settings.countedCalls = 1000;
		}
		else
		{
			settings.warmUpCalls = 3;
			settings.samples = 3;
			settings.minimumSampleDuration = std::chrono::microseconds(100);
			settings.countedCalls = 20;
		}
		return settings;
	}

	BenchmarkRunner::BenchmarkRunner(const RunSettings& p_settings) :
		m_settings(p_settings)
	{
	}

	BenchmarkResult BenchmarkRunner::run(Benchmark& p_benchmark, const std::optional<Expectation>& p_expectation)
	{
		BenchmarkResult result;
		result.benchmark = p_benchmark.getName();
		result.group = result.benchmark.substr(0, result.benchmark.find('.'));
		result.dataset = p_benchmark.getDataset();
		result.expectation = p_expectation;
		p_benchmark.prepare();
		_warmUp(p_benchmark);
		result.timing = _time(p_benchmark, _calibrate(p_benchmark));
		const CountedCalls COUNTED = _count(p_benchmark, p_expectation);
		result.allocations = COUNTED.allocations;
		result.result = COUNTED.lastResult;
		result.processPeakPrivateBytes = MemoryMonitor::read().peakPrivateBytes;
		checkResult(result, COUNTED.mismatches);
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
	std::size_t BenchmarkRunner::_calibrate(Benchmark& p_benchmark)
	{
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

	TimingStatistics BenchmarkRunner::_time(Benchmark& p_benchmark, std::size_t p_callsPerSample)
	{
		std::vector<double> nanosecondsPerCall(m_settings.samples);
		for (double& sampleNanoseconds : nanosecondsPerCall)
		{
			const Clock::time_point START = Clock::now();
			for (std::size_t call = 0; call < p_callsPerSample; ++call)
			{
				m_sink = m_sink + p_benchmark.run();
			}
			const std::chrono::duration<double, std::nano> ELAPSED = Clock::now() - START;
			sampleNanoseconds = ELAPSED.count() / static_cast<double>(p_callsPerSample);
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
		CountedCalls counted;
		AllocationCounts total;
		{
			std::optional<CountingSession> session;
			if (AllocationMonitor::isSupported())
			{
				session.emplace();
			}
			for (std::size_t call = 0; call < CALLS; ++call)
			{
				const AllocationCounts BEFORE = AllocationMonitor::getCounts();
				counted.lastResult = p_benchmark.run();
				const AllocationCounts AFTER = AllocationMonitor::getCounts();
				allocationsPerCall[call] = AFTER.allocations - BEFORE.allocations;
				bytesPerCall[call] = AFTER.allocatedBytes - BEFORE.allocatedBytes;
				if (p_expectation.has_value() && !isExpected(counted.lastResult, p_expectation->result))
				{
					++counted.mismatches;
				}
			}
			total = AllocationMonitor::getCounts();
		}
		counted.allocations = allocationStatistics(allocationsPerCall, bytesPerCall, total);
		counted.allocations.measured = AllocationMonitor::isSupported();
		return counted;
	}
}
