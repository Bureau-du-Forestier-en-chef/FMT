/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef PERFORMANCE_BENCHMARKSUITE_H_INCLUDED
#define PERFORMANCE_BENCHMARKSUITE_H_INCLUDED

#include "Benchmark.h"
#include "BenchmarkEnvironment.h"
#include "BenchmarkOptions.h"
#include "BenchmarkResult.h"

#include <memory>
#include <vector>

namespace Performance
{
	// Exit code of a run whose every selected benchmark skipped a check. ctest reports such a test
	// as skipped: it is the code of Testing::SKIP_RETURN_CODE (Examples/C++/tests/TestTools.h).
	constexpr int SKIP_RETURN_CODE = 77;

	// The benchmarks of one executable. The command line selects them; they run one after the
	// other, and their results are printed and written to the results file.
	class BenchmarkSuite
	{
	public:
		explicit BenchmarkSuite(const BenchmarkOptions& p_options);
		// Adds p_benchmark, whose name must be new in the suite.
		void add(std::unique_ptr<Benchmark> p_benchmark);
		// Runs the selected benchmarks and returns the exit code of the executable: 0 when every one
		// is valid, 1 when one is not or when none is selected, SKIP_RETURN_CODE when every one
		// skipped a check.
		int run();

	private:
		BenchmarkOptions m_options;
		std::vector<std::unique_ptr<Benchmark>> m_benchmarks;

		std::vector<Benchmark*> _select() const;
		void _write(const BenchmarkEnvironment& p_environment, const std::vector<BenchmarkResult>& p_results) const;
	};
}

#endif
