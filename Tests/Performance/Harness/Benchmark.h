/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef PERFORMANCE_BENCHMARK_H_INCLUDED
#define PERFORMANCE_BENCHMARK_H_INCLUDED

#include "RunSettings.h"

#include <string>

namespace Performance
{
	// One measured operation. The runner calls prepare once, then run many times: warm-up, timed
	// samples, then counted calls. Everything run needs is built by prepare, which is not measured.
	class Benchmark
	{
	public:
		virtual ~Benchmark() = default;
		// Name in the form <group>.<operation>[.<variant>], unique in its executable.
		virtual std::string getName() const = 0;
		// Data read by the benchmark, written with its results.
		virtual std::string getDataset() const = 0;
		// How many times the runner calls run. The default suits an operation of microseconds or
		// less; a slower operation returns RunSettings::forSlowCalls.
		virtual RunSettings getSettings(BenchmarkMode p_mode) const
		{
			return RunSettings::forMode(p_mode);
		}
		// Builds everything run needs. Not measured.
		virtual void prepare() = 0;
		// Runs the measured operation once and returns its result, which the runner compares with
		// the expected result of the benchmark.
		virtual double run() = 0;
	};
}

#endif
