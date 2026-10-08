/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef PERFORMANCE_FLOWBENCHMARKS_H_INCLUDED
#define PERFORMANCE_FLOWBENCHMARKS_H_INCLUDED

#include <filesystem>
#include <memory>
#include <string>

namespace Performance
{
	class Benchmark;
	class BenchmarkSuite;

	// Adds the model flows on TWD_land: optimization over 5 and 20 periods, replay of a schedule,
	// outputs of a scheduled model, non-spatial simulation and replanning. p_workFolder receives what
	// a flow writes.
	void addFlowBenchmarks(BenchmarkSuite& p_suite, const std::string& p_primaryFile, const std::filesystem::path& p_workFolder);

	// Builds the flow named p_name from the arguments of its row in the expectations file, or
	// returns nothing when p_name is not the name of a flow. The kind of flow is the start of the
	// name: Flow.Optimize., Flow.Replay., Flow.Outputs., Flow.Simulate. or Flow.Replanning.
	std::unique_ptr<Benchmark> makeFlowBenchmark(const std::string& p_name, const std::string& p_arguments,
		const std::filesystem::path& p_workFolder);
}

#endif
