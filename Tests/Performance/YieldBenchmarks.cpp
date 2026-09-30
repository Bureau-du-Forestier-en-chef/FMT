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

#include "YieldBenchmarks.h"

#include "FMTYieldRequest.h"
#include "FMTYields.h"

#include "Benchmark.h"
#include "BenchmarkSuite.h"
#include "PerfYields.h"

#include <cstddef>
#include <memory>
#include <string>
#include <utility>

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
}

namespace Performance
{
	void addYieldBenchmarks(BenchmarkSuite& p_suite, const std::string& p_primaryFile)
	{
		const std::shared_ptr<PerfYields> PERF_YIELDS = std::make_shared<PerfYields>(p_primaryFile);
		p_suite.add(std::make_unique<AgeYieldBenchmark>(PERF_YIELDS, false));
		p_suite.add(std::make_unique<AgeYieldBenchmark>(PERF_YIELDS, true));
	}
}
