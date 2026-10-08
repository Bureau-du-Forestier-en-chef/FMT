/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "DefinedBenchmarks.h"

#include "Benchmark.h"
#include "BenchmarkSuite.h"
#include "FlowBenchmarks.h"
#include "YieldBenchmarks.h"

#include <memory>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace Performance
{
	void addDefinedBenchmarks(BenchmarkSuite& p_suite, const std::vector<DefinedBenchmark>& p_rows,
		const std::filesystem::path& p_workFolder)
	{
		for (const DefinedBenchmark& ROW : p_rows)
		{
			std::unique_ptr<Benchmark> benchmark = makeFlowBenchmark(ROW.name, ROW.arguments, p_workFolder);
			if (!benchmark)
			{
				benchmark = makeYieldBenchmark(ROW.name, ROW.arguments);
			}
			if (!benchmark)
			{
				throw std::invalid_argument(ROW.name + ": no kind of benchmark takes arguments under this name. The kinds are "
					"Flow.Optimize, Flow.Replay, Flow.Outputs, Flow.Simulate, Flow.Replanning and Yield.Model, followed by a point "
					"and a variant");
			}
			p_suite.add(std::move(benchmark));
		}
	}

	bool isOfKind(const std::string& p_name, const std::string& p_kind)
	{
		return p_name.size() > p_kind.size() + 1 && p_name.compare(0, p_kind.size(), p_kind) == 0 && p_name[p_kind.size()] == '.';
	}

	std::vector<std::string> splitArguments(const std::string& p_name, const std::string& p_arguments, const std::string& p_usage)
	{
		std::vector<std::string> arguments;
		std::stringstream stream(p_arguments);
		std::string argument;
		while (std::getline(stream, argument, '|'))
		{
			arguments.push_back(argument);
		}
		std::size_t expected = 1;
		for (const char CHARACTER : p_usage)
		{
			expected += CHARACTER == '|' ? 1 : 0;
		}
		if (arguments.size() != expected)
		{
			throw std::invalid_argument(p_name + ": expected the arguments " + p_usage + ", got \"" + p_arguments + "\"");
		}
		return arguments;
	}

	int positiveArgument(const std::string& p_name, const std::string& p_text)
	{
		std::size_t used = 0;
		int value = 0;
		try
		{
			value = std::stoi(p_text, &used);
		}
		catch (const std::exception&)
		{
			used = 0;
		}
		if (used == 0 || used != p_text.size() || value <= 0)
		{
			throw std::invalid_argument(p_name + ": \"" + p_text + "\" is not a positive integer");
		}
		return value;
	}
}
