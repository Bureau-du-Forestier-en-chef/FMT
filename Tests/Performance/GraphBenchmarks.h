/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef PERFORMANCE_GRAPHBENCHMARKS_H_INCLUDED
#define PERFORMANCE_GRAPHBENCHMARKS_H_INCLUDED

#include <string>

namespace Performance
{
	class BenchmarkSuite;

	// Adds the benchmarks of the traversal of the graph of a model: one output of the LP scenario of
	// p_primaryFile, built with its schedule, at one period.
	void addGraphBenchmarks(BenchmarkSuite& p_suite, const std::string& p_primaryFile);
}

#endif
