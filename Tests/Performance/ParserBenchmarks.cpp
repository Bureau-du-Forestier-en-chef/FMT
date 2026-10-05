/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

// Reads of a project by Parser::FMTModelParser::readproject, with a new parser at every call, as
// an application does. The models read are destroyed before the call returns: the memory a call
// keeps then shows what a read leaves behind, such as a leak or a cache that keeps growing.

#include "ParserBenchmarks.h"

#include "FMTModel.h"
#include "FMTModelParser.h"

#include "Benchmark.h"
#include "BenchmarkSuite.h"
#include "QuietFmt.h"

#include <filesystem>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace
{
	// Reads the scenarios of p_primaryFile at every call, and returns the sum of the initial
	// areas of the models read, which the .are files give by hand.
	class ReadProjectBenchmark final : public Performance::Benchmark
	{
	public:
		ReadProjectBenchmark(std::string p_variant, std::vector<std::string> p_scenarios, std::string p_primaryFile);
		std::string getName() const override;
		std::string getDataset() const override;
		Performance::RunSettings getSettings(Performance::BenchmarkMode p_mode) const override;
		void prepare() override;
		double run() override;

	private:
		std::string m_variant;
		std::vector<std::string> m_scenarios;
		std::string m_primaryFile;
	};

	ReadProjectBenchmark::ReadProjectBenchmark(std::string p_variant, std::vector<std::string> p_scenarios,
		std::string p_primaryFile) :
		m_variant(std::move(p_variant)),
		m_scenarios(std::move(p_scenarios)),
		m_primaryFile(std::move(p_primaryFile))
	{
	}

	std::string ReadProjectBenchmark::getName() const
	{
		return m_variant.empty() ? std::string("Parser.ReadProject") : "Parser.ReadProject." + m_variant;
	}

	std::string ReadProjectBenchmark::getDataset() const
	{
		std::string scenarios;
		for (const std::string& SCENARIO : m_scenarios)
		{
			scenarios += (scenarios.empty() ? "" : "+") + SCENARIO;
		}
		return std::filesystem::path(m_primaryFile).stem().string() + "/" + scenarios;
	}

	// A read lasts milliseconds.
	Performance::RunSettings ReadProjectBenchmark::getSettings(Performance::BenchmarkMode p_mode) const
	{
		return Performance::RunSettings::forSlowCalls(p_mode);
	}

	// FMT logs through a logger shared by the whole process: made quiet here, it lets the measured
	// reads write nothing.
	void ReadProjectBenchmark::prepare()
	{
		Performance::quietFmt();
	}

	double ReadProjectBenchmark::run()
	{
		Parser::FMTModelParser parser;
		const std::vector<Models::FMTModel> MODELS = parser.readproject(m_primaryFile, m_scenarios);
		double area = 0.0;
		for (const Models::FMTModel& MODEL : MODELS)
		{
			area += MODEL.getInitialArea();
		}
		return area;
	}
}

namespace Performance
{
	void addParserBenchmarks(BenchmarkSuite& p_suite, const std::string& p_primaryFile)
	{
		p_suite.add(std::make_unique<ReadProjectBenchmark>("", std::vector<std::string>{ "ROOT" }, p_primaryFile));
		// A scenario read with the root it overrides, in the same call.
		p_suite.add(std::make_unique<ReadProjectBenchmark>("TwoScenarios", std::vector<std::string>{ "ROOT", "perfyields" },
			p_primaryFile));
	}
}
