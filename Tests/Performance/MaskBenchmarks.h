/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef PERFORMANCE_MASKBENCHMARKS_H_INCLUDED
#define PERFORMANCE_MASKBENCHMARKS_H_INCLUDED

#include <string>

namespace Performance
{
	class BenchmarkSuite;

	// Adds the benchmarks of mask operations, on the themes of the root of p_primaryFile.
	void addMaskBenchmarks(BenchmarkSuite& p_suite, const std::string& p_primaryFile);
}

#endif
