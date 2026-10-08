/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef PERFORMANCE_COMPLEXYIELDBENCHMARKS_H_INCLUDED
#define PERFORMANCE_COMPLEXYIELDBENCHMARKS_H_INCLUDED

#include <string>

namespace Performance
{
	class BenchmarkSuite;

	// Adds the complex-yield benchmarks, one per operator, which read the perfyields scenario of
	// p_primaryFile.
	void addComplexYieldBenchmarks(BenchmarkSuite& p_suite, const std::string& p_primaryFile);
}

#endif
