/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef PERFORMANCE_YIELDBENCHMARKS_H_INCLUDED
#define PERFORMANCE_YIELDBENCHMARKS_H_INCLUDED

#include <memory>
#include <string>

namespace Performance
{
	class Benchmark;
	class BenchmarkSuite;

	// Adds the benchmarks of age-yield reads, which read the perfyields scenario of p_primaryFile.
	void addYieldBenchmarks(BenchmarkSuite& p_suite, const std::string& p_primaryFile);

	// Builds the yield benchmark named p_name from the arguments of its row in the expectations
	// file, or returns nothing when p_name is not the name of one: Yield.Model.<variant>, whose
	// arguments are <primary file>|<scenario>|<yield>|<developments>.
	std::unique_ptr<Benchmark> makeYieldBenchmark(const std::string& p_name, const std::string& p_arguments);
}

#endif
