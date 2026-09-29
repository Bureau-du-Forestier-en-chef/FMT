/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

// Evaluation of complex yields by Core::FMTYields::get, the "before" reference of issue #348.
//
// Each call asks for the yield of the next of 255 developments that differ by their period only:
// the same value, under 255 keys of the yield cache. FMTComplexYieldHandler::get puts a value in
// that cache only when its computation lasted more than 0.05 ms; the computations measured here
// last a few microseconds, so the values are computed at nearly every call. A computation slowed
// down beyond 0.05 ms, by a preemption for instance, puts its key in the cache for the rest of the
// process: with 255 keys, that changes one call in 255, and the minimum of the allocations per
// call shows it. The same key asked at every call would switch to the cache for good at the first
// such slowdown, so that its measure would depend on when that happens: it is not a benchmark.
//
// The yield requests are built and their yield data located before the measurement, as the graph
// of a model does before asking for yields.

#include "ComplexYieldBenchmarks.h"

#include "FMTDevelopment.h"
#include "FMTMask.h"
#include "FMTModel.h"
#include "FMTModelParser.h"
#include "FMTTheme.h"
#include "FMTYieldRequest.h"
#include "FMTYields.h"

#include "Benchmark.h"
#include "BenchmarkSuite.h"

#include <cstddef>
#include <filesystem>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace
{
	const std::string SCENARIO = "perfyields";
	// A development of TWD_land where volumetotal is 120: every expected value of performance.csv
	// is computed by hand from the yield table of the scenario.
	const std::string MASK = "UNITE1 PEUPLEMENT1 UTR1";
	constexpr int AGE = 7;
	// An age yield of every development, asked once per request to locate its yield data.
	const std::string AGE_YIELD = "VOLUMETOTAL";
	// The yield cache keeps the period of its keys on 8 bits: periods 1 to 255 give as many distinct
	// keys for the same value.
	constexpr std::size_t DISTINCT_PERIODS = 255;

	// The perfyields scenario and one yield request per period, prepared once for all the
	// complex-yield benchmarks of the process.
	class PerfYields
	{
	public:
		explicit PerfYields(std::string p_primaryFile);
		// Reads the scenario and prepares the requests. Only the first call does the work.
		void prepare();
		const Core::FMTYields& getYields() const;
		// Request of the development of period p_index + 1.
		const Core::FMTYieldRequest& getRequest(std::size_t p_index) const;
		std::string getDataset() const;

	private:
		std::string m_primaryFile;
		Core::FMTYields m_yields;
		// The requests point to these developments: reserved once, the vector never moves them.
		std::vector<Core::FMTDevelopment> m_developments;
		std::vector<Core::FMTYieldRequest> m_requests;
		bool m_prepared = false;
	};

	PerfYields::PerfYields(std::string p_primaryFile) :
		m_primaryFile(std::move(p_primaryFile))
	{
	}

	void PerfYields::prepare()
	{
		if (m_prepared)
		{
			return;
		}
		Parser::FMTModelParser parser;
		parser.setDefaultExceptionHandler();
		const std::vector<Models::FMTModel> MODELS = parser.readproject(m_primaryFile, std::vector<std::string>(1, SCENARIO));
		m_yields = MODELS.at(0).getYields();
		const std::vector<Core::FMTTheme> THEMES = MODELS.at(0).getThemes();
		m_developments.reserve(DISTINCT_PERIODS);
		m_requests.reserve(DISTINCT_PERIODS);
		for (std::size_t index = 0; index < DISTINCT_PERIODS; ++index)
		{
			const int PERIOD = static_cast<int>(index) + 1;
			m_developments.emplace_back(Core::FMTMask(MASK, THEMES), AGE, 0, PERIOD);
			m_requests.push_back(m_developments.back().getYieldRequest());
			// A new request locates the yield data of its development at its first use: done here,
			// outside the measured calls.
			m_yields.get(m_requests.back(), AGE_YIELD);
		}
		m_prepared = true;
	}

	const Core::FMTYields& PerfYields::getYields() const
	{
		return m_yields;
	}

	const Core::FMTYieldRequest& PerfYields::getRequest(std::size_t p_index) const
	{
		return m_requests.at(p_index);
	}

	std::string PerfYields::getDataset() const
	{
		return std::filesystem::path(m_primaryFile).stem().string() + "/" + SCENARIO;
	}

	// Evaluates one complex yield of the perfyields scenario at every call.
	class ComplexYieldBenchmark final : public Performance::Benchmark
	{
	public:
		ComplexYieldBenchmark(std::string p_operation, std::string p_yield, std::shared_ptr<PerfYields> p_perfYields);
		std::string getName() const override;
		std::string getDataset() const override;
		void prepare() override;
		double run() override;

	private:
		std::string m_operation;
		std::string m_yield;
		std::shared_ptr<PerfYields> m_perfYields;
		std::size_t m_nextRequest = 0;
	};

	ComplexYieldBenchmark::ComplexYieldBenchmark(std::string p_operation, std::string p_yield,
		std::shared_ptr<PerfYields> p_perfYields) :
		m_operation(std::move(p_operation)),
		m_yield(std::move(p_yield)),
		m_perfYields(std::move(p_perfYields))
	{
	}

	std::string ComplexYieldBenchmark::getName() const
	{
		return "ComplexYield." + m_operation;
	}

	std::string ComplexYieldBenchmark::getDataset() const
	{
		return m_perfYields->getDataset();
	}

	void ComplexYieldBenchmark::prepare()
	{
		m_perfYields->prepare();
	}

	double ComplexYieldBenchmark::run()
	{
		const Core::FMTYieldRequest& REQUEST = m_perfYields->getRequest(m_nextRequest);
		m_nextRequest = (m_nextRequest + 1) % DISTINCT_PERIODS;
		return m_perfYields->getYields().get(REQUEST, m_yield);
	}
}

namespace Performance
{
	void addComplexYieldBenchmarks(BenchmarkSuite& p_suite, const std::string& p_primaryFile)
	{
		// Operator measured, and the yield of the perfyields scenario that uses it.
		const std::pair<const char*, const char*> OPERATIONS[] = {
			{ "Sum", "TWICE" },
			{ "Multiply", "DOUBLE" },
			{ "Divide", "HALF" },
			{ "Subtract", "LESS" },
			{ "Shift", "SHIFTED" },
			{ "Equation", "EQUATIONSUM" },
			{ "RecursiveChain", "CHAIN5" } };
		const std::shared_ptr<PerfYields> PERF_YIELDS = std::make_shared<PerfYields>(p_primaryFile);
		for (const auto& OPERATION : OPERATIONS)
		{
			p_suite.add(std::make_unique<ComplexYieldBenchmark>(OPERATION.first, OPERATION.second, PERF_YIELDS));
		}
	}
}
