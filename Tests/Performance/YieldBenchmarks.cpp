/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

// Reads of an age yield by Core::FMTYields::get, the most frequent yield read of a model.
//
// Yield.Age asks through the prepared request of the next of the 255 developments of PerfYields,
// like the complex-yield benchmarks: the read finds the yield handler of the development, then
// interpolates the yield table. Yield.Age.NewRequest asks through a new request at every call, as
// happens when a development is met for the first time: the request first locates the yield data
// of its development.
//
// Yield.Model.<variant>, defined by a row of the expectations file, reads a yield for the first
// developments of the area of a production model, whose yield sections are many: a read then looks
// for the handler of its development among more of them than on TWD_land.

#include "YieldBenchmarks.h"

#include "FMTActualDevelopment.h"
#include "FMTDevelopment.h"
#include "FMTModel.h"
#include "FMTModelParser.h"
#include "FMTYieldRequest.h"
#include "FMTYields.h"

#include "Benchmark.h"
#include "BenchmarkSuite.h"
#include "DatasetFingerprint.h"
#include "DefinedBenchmarks.h"
#include "PerfYields.h"
#include "QuietFmt.h"

#include <algorithm>
#include <cstddef>
#include <filesystem>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace
{
	// Reads VOLUMETOTAL for the development UNITE1 PEUPLEMENT1 UTR1 at age 7: 120.
	class AgeYieldBenchmark final : public Performance::Benchmark
	{
	public:
		// With p_newRequest, every call builds a new request instead of using a prepared one.
		AgeYieldBenchmark(std::shared_ptr<Performance::PerfYields> p_perfYields, bool p_newRequest);
		std::string getName() const override;
		std::string getDataset() const override;
		void prepare() override;
		double run() override;

	private:
		std::shared_ptr<Performance::PerfYields> m_perfYields;
		bool m_newRequest;
		std::size_t m_nextRequest = 0;
	};

	AgeYieldBenchmark::AgeYieldBenchmark(std::shared_ptr<Performance::PerfYields> p_perfYields, bool p_newRequest) :
		m_perfYields(std::move(p_perfYields)),
		m_newRequest(p_newRequest)
	{
	}

	std::string AgeYieldBenchmark::getName() const
	{
		return m_newRequest ? "Yield.Age.NewRequest" : "Yield.Age";
	}

	std::string AgeYieldBenchmark::getDataset() const
	{
		return m_perfYields->getDataset();
	}

	void AgeYieldBenchmark::prepare()
	{
		m_perfYields->prepare();
	}

	double AgeYieldBenchmark::run()
	{
		if (m_newRequest)
		{
			const Core::FMTYieldRequest REQUEST = m_perfYields->getDevelopment(0).getYieldRequest();
			return m_perfYields->getYields().get(REQUEST, Performance::PerfYields::AGE_YIELD);
		}
		const Core::FMTYieldRequest& REQUEST = m_perfYields->getRequest(m_nextRequest);
		m_nextRequest = (m_nextRequest + 1) % Performance::PerfYields::REQUESTS;
		return m_perfYields->getYields().get(REQUEST, Performance::PerfYields::AGE_YIELD);
	}

	// Reads p_yield for the first p_developments developments of the area of a model, through
	// requests prepared once, and returns the sum of the values.
	class ModelYieldBenchmark final : public Performance::Benchmark
	{
	public:
		ModelYieldBenchmark(std::string p_name, std::string p_primaryFile, std::string p_scenario, std::string p_yield,
			std::size_t p_developments);
		std::string getName() const override;
		std::string getDataset() const override;
		std::string getDatasetFingerprint() const override;
		void prepare() override;
		double run() override;

	private:
		std::string m_name;
		std::string m_primaryFile;
		std::string m_scenario;
		std::string m_yield;
		std::size_t m_maximumDevelopments;
		std::string m_fingerprint;
		Core::FMTYields m_yields;
		// The requests point to these developments: reserved once, the vector never moves them.
		std::vector<Core::FMTDevelopment> m_developments;
		std::vector<Core::FMTYieldRequest> m_requests;
	};

	ModelYieldBenchmark::ModelYieldBenchmark(std::string p_name, std::string p_primaryFile, std::string p_scenario,
		std::string p_yield, std::size_t p_developments) :
		m_name(std::move(p_name)),
		m_primaryFile(std::move(p_primaryFile)),
		m_scenario(std::move(p_scenario)),
		m_yield(std::move(p_yield)),
		m_maximumDevelopments(p_developments)
	{
	}

	std::string ModelYieldBenchmark::getName() const
	{
		return m_name;
	}

	std::string ModelYieldBenchmark::getDataset() const
	{
		return std::filesystem::path(m_primaryFile).stem().string() + "/" + m_scenario;
	}

	std::string ModelYieldBenchmark::getDatasetFingerprint() const
	{
		return m_fingerprint;
	}

	void ModelYieldBenchmark::prepare()
	{
		Performance::quietFmt();
		Parser::FMTModelParser parser;
		const std::vector<Models::FMTModel> MODELS = parser.readproject(m_primaryFile, std::vector<std::string>(1, m_scenario));
		m_yields = MODELS.at(0).getYields();
		const std::vector<Core::FMTActualDevelopment> AREA = MODELS.at(0).getArea();
		const std::size_t COUNT = std::min(AREA.size(), m_maximumDevelopments);
		m_developments.reserve(COUNT);
		m_requests.reserve(COUNT);
		for (std::size_t index = 0; index < COUNT; ++index)
		{
			m_developments.emplace_back(AREA.at(index));
			m_requests.push_back(m_developments.back().getYieldRequest());
			// A new request locates the yield data of its development at its first use: done here,
			// outside the measured calls.
			m_yields.get(m_requests.back(), m_yield);
		}
		m_fingerprint = Performance::DatasetFingerprint::compute(m_primaryFile, std::vector<std::string>(1, m_scenario));
	}

	double ModelYieldBenchmark::run()
	{
		double sum = 0.0;
		for (const Core::FMTYieldRequest& REQUEST : m_requests)
		{
			sum += m_yields.get(REQUEST, m_yield);
		}
		return sum;
	}
}

namespace Performance
{
	void addYieldBenchmarks(BenchmarkSuite& p_suite, const std::string& p_primaryFile)
	{
		const std::shared_ptr<PerfYields> PERF_YIELDS = std::make_shared<PerfYields>(p_primaryFile);
		p_suite.add(std::make_unique<AgeYieldBenchmark>(PERF_YIELDS, false));
		p_suite.add(std::make_unique<AgeYieldBenchmark>(PERF_YIELDS, true));
	}

	std::unique_ptr<Benchmark> makeYieldBenchmark(const std::string& p_name, const std::string& p_arguments)
	{
		if (!isOfKind(p_name, "Yield.Model"))
		{
			return nullptr;
		}
		const std::vector<std::string> ARGUMENTS = splitArguments(p_name, p_arguments, "<primary file>|<scenario>|<yield>|<developments>");
		return std::make_unique<ModelYieldBenchmark>(p_name, ARGUMENTS.at(0), ARGUMENTS.at(1), ARGUMENTS.at(2),
			static_cast<std::size_t>(positiveArgument(p_name, ARGUMENTS.at(3))));
	}
}
