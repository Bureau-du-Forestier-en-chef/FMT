/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef PERFORMANCE_PARSERBENCHMARKS_H_INCLUDED
#define PERFORMANCE_PARSERBENCHMARKS_H_INCLUDED

#include <string>

namespace Performance
{
	class BenchmarkSuite;

	// Adds the benchmarks of project reads: the root of p_primaryFile, alone and with a scenario.
	void addParserBenchmarks(BenchmarkSuite& p_suite, const std::string& p_primaryFile);
}

#endif
