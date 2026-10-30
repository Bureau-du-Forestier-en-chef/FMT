/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef PERFORMANCE_EXPRESSIONBENCHMARKS_H_INCLUDED
#define PERFORMANCE_EXPRESSIONBENCHMARKS_H_INCLUDED

namespace Performance
{
	class BenchmarkSuite;

	// Adds the benchmarks of the evaluation of expressions, which build their data in code.
	void addExpressionBenchmarks(BenchmarkSuite& p_suite);
}

#endif
