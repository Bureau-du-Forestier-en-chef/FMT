/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "BenchmarkResult.h"

#include "JsonWriter.h"

#include <algorithm>
#include <initializer_list>

namespace
{
	// Version of the JSON layout: change it with the layout, and describe the change in
	// Documentation/PerformanceTesting.md.
	constexpr std::int64_t SCHEMA_VERSION = 4;
	// Durations are in nanoseconds: three decimals are below the resolution of any clock.
	constexpr int DURATION_DECIMALS = 3;

	template<typename Value>
	void writeField(Performance::JsonWriter& p_writer, const char* p_name, const Value& p_value)
	{
		p_writer.key(p_name);
		p_writer.value(p_value);
	}

	void writeDuration(Performance::JsonWriter& p_writer, const char* p_name, double p_nanoseconds)
	{
		p_writer.key(p_name);
		p_writer.value(p_nanoseconds, DURATION_DECIMALS);
	}

	void writeEnvironment(Performance::JsonWriter& p_writer, const Performance::BenchmarkEnvironment& p_environment)
	{
		p_writer.beginObject();
		writeField(p_writer, "fmtVersion", p_environment.fmtVersion);
		writeField(p_writer, "fmtBuildDate", p_environment.fmtBuildDate);
		p_writer.key("features");
		p_writer.beginArray();
		for (const std::string& FEATURE : p_environment.features)
		{
			p_writer.value(FEATURE);
		}
		p_writer.endArray();
		writeField(p_writer, "allocator", p_environment.allocator);
		writeField(p_writer, "commit", p_environment.commit);
		writeField(p_writer, "dirty", p_environment.dirty);
		writeField(p_writer, "buildType", p_environment.buildType);
		writeField(p_writer, "optimized", p_environment.optimized);
		writeField(p_writer, "compiler", p_environment.compiler);
		writeField(p_writer, "compilerVersion", p_environment.compilerVersion);
		writeField(p_writer, "os", p_environment.operatingSystem);
		writeField(p_writer, "cpu", p_environment.processor);
		writeField(p_writer, "logicalCores", static_cast<std::uint64_t>(p_environment.logicalCores));
		writeField(p_writer, "processors", p_environment.processors);
		writeField(p_writer, "availableMemoryBytes", p_environment.availableMemoryBytes);
		writeField(p_writer, "timestamp", p_environment.timestamp);
		writeField(p_writer, "mode", Performance::toString(p_environment.mode));
		p_writer.endObject();
	}

	void writeTiming(Performance::JsonWriter& p_writer, const Performance::TimingStatistics& p_timing)
	{
		writeField(p_writer, "samples", static_cast<std::uint64_t>(p_timing.samples));
		writeField(p_writer, "callsPerSample", static_cast<std::uint64_t>(p_timing.callsPerSample));
		writeDuration(p_writer, "minNs", p_timing.minimumNs);
		writeDuration(p_writer, "maxNs", p_timing.maximumNs);
		writeDuration(p_writer, "medianNs", p_timing.medianNs);
		writeDuration(p_writer, "meanNs", p_timing.meanNs);
		writeDuration(p_writer, "stddevNs", p_timing.standardDeviationNs);
	}

	void writePhases(Performance::JsonWriter& p_writer, const std::vector<Performance::PhaseStatistics>& p_phases)
	{
		p_writer.key("phases");
		p_writer.beginArray();
		for (const Performance::PhaseStatistics& PHASE : p_phases)
		{
			p_writer.beginObject();
			writeField(p_writer, "name", PHASE.name);
			writeDuration(p_writer, "minNs", PHASE.minimumNs);
			writeDuration(p_writer, "medianNs", PHASE.medianNs);
			writeDuration(p_writer, "maxNs", PHASE.maximumNs);
			p_writer.endObject();
		}
		p_writer.endArray();
	}

	void writeAllocations(Performance::JsonWriter& p_writer, const Performance::AllocationStatistics& p_allocations)
	{
		writeField(p_writer, "allocationCalls", static_cast<std::uint64_t>(p_allocations.calls));
		const std::initializer_list<const char*> COUNT_NAMES = { "allocationsPerCallMin", "allocationsPerCallMedian",
			"allocationsPerCallMax", "allocatedBytesPerCallMedian", "retainedBytesPerCallMedian", "retainedBytesPerCallMax",
			"allocations", "deallocations", "allocatedBytes", "retainedBytes", "peakLiveHeapBytes" };
		if (!p_allocations.measured)
		{
			for (const char* const NAME : COUNT_NAMES)
			{
				p_writer.key(NAME);
				p_writer.nullValue();
			}
			return;
		}
		writeField(p_writer, "allocationsPerCallMin", p_allocations.minimumPerCall);
		writeField(p_writer, "allocationsPerCallMedian", p_allocations.medianPerCall);
		writeField(p_writer, "allocationsPerCallMax", p_allocations.maximumPerCall);
		writeField(p_writer, "allocatedBytesPerCallMedian", p_allocations.medianBytesPerCall);
		writeField(p_writer, "retainedBytesPerCallMedian", p_allocations.medianRetainedBytesPerCall);
		writeField(p_writer, "retainedBytesPerCallMax", p_allocations.maximumRetainedBytesPerCall);
		writeField(p_writer, "allocations", p_allocations.total.allocations);
		writeField(p_writer, "deallocations", p_allocations.total.deallocations);
		writeField(p_writer, "allocatedBytes", p_allocations.total.allocatedBytes);
		writeField(p_writer, "retainedBytes", p_allocations.total.liveBytes);
		writeField(p_writer, "peakLiveHeapBytes", p_allocations.total.peakLiveBytes);
	}

	void writeBound(Performance::JsonWriter& p_writer, const char* p_name, const std::optional<std::int64_t>& p_bound)
	{
		p_writer.key(p_name);
		if (p_bound.has_value())
		{
			p_writer.value(*p_bound);
		}
		else
		{
			p_writer.nullValue();
		}
	}

	void writeExpectation(Performance::JsonWriter& p_writer, const std::optional<Performance::Expectation>& p_expectation)
	{
		p_writer.key("expected");
		if (p_expectation.has_value())
		{
			p_writer.value(p_expectation->result);
		}
		else
		{
			p_writer.nullValue();
		}
		const std::optional<std::int64_t> NO_BOUND;
		writeBound(p_writer, "maxAllocationsPerCall", p_expectation.has_value() ? p_expectation->maximumAllocations : NO_BOUND);
		writeBound(p_writer, "maxRetainedBytesPerCall", p_expectation.has_value() ? p_expectation->maximumRetainedBytes : NO_BOUND);
		p_writer.key("maxPeakMemoryMB");
		if (p_expectation.has_value() && p_expectation->maximumPeakMegabytes.has_value())
		{
			p_writer.value(*p_expectation->maximumPeakMegabytes);
		}
		else
		{
			p_writer.nullValue();
		}
	}

	void writeResult(Performance::JsonWriter& p_writer, const Performance::BenchmarkResult& p_result)
	{
		p_writer.beginObject();
		writeField(p_writer, "benchmark", p_result.benchmark);
		writeField(p_writer, "group", p_result.group);
		writeField(p_writer, "dataset", p_result.dataset);
		writeField(p_writer, "datasetFingerprint", p_result.datasetFingerprint);
		writeField(p_writer, "threads", static_cast<std::uint64_t>(p_result.threads));
		writeTiming(p_writer, p_result.timing);
		writePhases(p_writer, p_result.phases);
		writeAllocations(p_writer, p_result.allocations);
		writeField(p_writer, "processPeakPrivateBytes", p_result.processPeakPrivateBytes);
		writeField(p_writer, "result", p_result.result);
		writeField(p_writer, "resultFingerprint", p_result.resultFingerprint);
		writeExpectation(p_writer, p_result.expectation);
		writeField(p_writer, "valid", p_result.isValid());
		writeField(p_writer, "skipped", p_result.isSkipped());
		writeField(p_writer, "skipReason", p_result.skipReason);
		p_writer.key("failures");
		p_writer.beginArray();
		for (const Performance::BenchmarkCheck& CHECK : p_result.checks)
		{
			if (!CHECK.passed)
			{
				p_writer.value(CHECK.description);
			}
		}
		p_writer.endArray();
		p_writer.endObject();
	}
}

namespace Performance
{
	bool BenchmarkResult::isValid() const
	{
		return std::all_of(checks.begin(), checks.end(), [](const BenchmarkCheck& p_check)
			{
			return p_check.passed;
			});
	}

	bool BenchmarkResult::isSkipped() const
	{
		return !skipReason.empty();
	}

	void writeResults(std::ostream& p_stream, const BenchmarkEnvironment& p_environment,
		const std::vector<BenchmarkResult>& p_results)
	{
		JsonWriter writer(p_stream);
		writer.beginObject();
		writeField(writer, "schemaVersion", SCHEMA_VERSION);
		writer.key("environment");
		writeEnvironment(writer, p_environment);
		writer.key("results");
		writer.beginArray();
		for (const BenchmarkResult& RESULT : p_results)
		{
			writeResult(writer, RESULT);
		}
		writer.endArray();
		writer.endObject();
	}
}
