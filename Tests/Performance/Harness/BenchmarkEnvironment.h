/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef PERFORMANCE_BENCHMARKENVIRONMENT_H_INCLUDED
#define PERFORMANCE_BENCHMARKENVIRONMENT_H_INCLUDED

#include "BenchmarkOptions.h"

#include <cstdint>
#include <string>
#include <vector>

namespace Performance
{
	// Where and how results were measured. It is written with them, so that two runs are only
	// compared when they are comparable.
	struct BenchmarkEnvironment
	{
		std::string fmtVersion;
		std::string fmtBuildDate;
		// Optional components compiled into FMTlib (OSI, MOSEK, GDAL...).
		std::vector<std::string> features;
		// Allocator that serves malloc in the process: "CRT heap", or "mimalloc <version>" when
		// mimalloc redirects the C runtime. Durations only compare under the same allocator.
		std::string allocator;
		// Commit of the sources when the benchmarks were built, and whether tracked files were then
		// modified.
		std::string commit;
		bool dirty = false;
		std::string buildType;
		// False when the benchmarks were compiled without optimization: their times are not a
		// reference.
		bool optimized = false;
		std::string compiler;
		std::string compilerVersion;
		std::string operatingSystem;
		std::string processor;
		unsigned int logicalCores = 0;
		// Logical processors the measuring thread runs on, and whether Windows may throttle the
		// process: set by the suite with ProcessorPolicy.
		std::string processors;
		// Physical memory available at the start. The complex-yield cache of FMT empties itself when
		// less than 10 GB are available.
		std::uint64_t availableMemoryBytes = 0;
		// Start of the run, in UTC.
		std::string timestamp;
		BenchmarkMode mode = BenchmarkMode::Smoke;

		// Collects the environment of the running process.
		static BenchmarkEnvironment collect(BenchmarkMode p_mode);
	};
}

#endif
