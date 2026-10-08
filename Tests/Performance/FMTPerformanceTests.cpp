/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

// Performance and allocation benchmarks of FMT (issue #349). Each row of
// Tests/Performance/performance.csv registers one benchmark with ctest; each row of the local
// performance-private.csv also defines its benchmark, on a model outside the source tree. The
// options, the results and the way to add a benchmark are described in
// Documentation/PerformanceTesting.md.

#include "BenchmarkOptions.h"
#include "BenchmarkSuite.h"
#include "ComplexYieldBenchmarks.h"
#include "DefinedBenchmarks.h"
#include "FlowBenchmarks.h"
#include "MaskBenchmarks.h"
#include "ParserBenchmarks.h"
#include "TestTools.h"
#include "YieldBenchmarks.h"

int main(int argc, char* argv[])
{
	return Testing::runTest([&]()
		{
		const Performance::BenchmarkOptions OPTIONS = Performance::BenchmarkOptions::parse(argc, argv, "FMTPerformanceTests");
		Performance::BenchmarkSuite suite(OPTIONS);
		Performance::addComplexYieldBenchmarks(suite, OPTIONS.getModelFile());
		Performance::addYieldBenchmarks(suite, OPTIONS.getModelFile());
		Performance::addMaskBenchmarks(suite, OPTIONS.getModelFile());
		Performance::addParserBenchmarks(suite, OPTIONS.getModelFile());
		Performance::addFlowBenchmarks(suite, OPTIONS.getModelFile(), OPTIONS.getWorkFolder());
		Performance::addDefinedBenchmarks(suite, OPTIONS.getDefinedBenchmarks(), OPTIONS.getWorkFolder());
		return suite.run();
		});
}
