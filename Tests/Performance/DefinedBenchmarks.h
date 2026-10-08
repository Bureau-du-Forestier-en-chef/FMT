/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef PERFORMANCE_DEFINEDBENCHMARKS_H_INCLUDED
#define PERFORMANCE_DEFINEDBENCHMARKS_H_INCLUDED

#include "BenchmarkOptions.h"

#include <filesystem>
#include <string>
#include <vector>

namespace Performance
{
	class BenchmarkSuite;

	// Adds the benchmarks defined by rows of the expectations file, such as the private benchmarks
	// of performance-private.csv: the start of the name gives the kind of benchmark, its arguments
	// give the model. p_workFolder receives what a benchmark writes.
	void addDefinedBenchmarks(BenchmarkSuite& p_suite, const std::vector<DefinedBenchmark>& p_rows,
		const std::filesystem::path& p_workFolder);

	// True when p_name is the name of a benchmark of the kind p_kind, such as Flow.Optimize: the kind
	// followed by a point and a variant.
	bool isOfKind(const std::string& p_name, const std::string& p_kind);
	// Splits the arguments of the benchmark p_name, separated by |, which must be as many as those of
	// p_usage, such as "<primary file>|<scenario>".
	std::vector<std::string> splitArguments(const std::string& p_name, const std::string& p_arguments, const std::string& p_usage);
	// Reads an argument of the benchmark p_name that must be a positive integer.
	int positiveArgument(const std::string& p_name, const std::string& p_text);
}

#endif
