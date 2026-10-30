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
// ComplexYield.CachedValue measures the other path of FMTComplexYieldHandler::get: the value served
// by the yields cache. Its key is put in the cache by prepare, under the period 0, which no other
// benchmark asks for: the cache keeps the period of its keys on 8 bits.
//
// The yield requests are built and their yield data located before the measurement, as the graph
// of a model does before asking for yields.

#include "ComplexYieldBenchmarks.h"

#include "FMTDevelopment.h"
#include "FMTYieldRequest.h"
#include "FMTYields.h"
#include "FMTYieldsCache.h"

#include "Benchmark.h"
#include "BenchmarkSuite.h"
#include "PerfYields.h"

#include <cstddef>
#include <memory>
#include <string>
#include <utility>

namespace
{
	// Evaluates one complex yield of the perfyields scenario at every call.
	class ComplexYieldBenchmark final : public Performance::Benchmark
	{
	public:
		ComplexYieldBenchmark(std::string p_operation, std::string p_yield, std::shared_ptr<Performance::PerfYields> p_perfYields);
		std::string getName() const override;
		std::string getDataset() const override;
		void prepare() override;
		double run() override;

	private:
		std::string m_operation;
		std::string m_yield;
		std::shared_ptr<Performance::PerfYields> m_perfYields;
		std::size_t m_nextRequest = 0;
	};

	ComplexYieldBenchmark::ComplexYieldBenchmark(std::string p_operation, std::string p_yield,
		std::shared_ptr<Performance::PerfYields> p_perfYields) :
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
		m_nextRequest = (m_nextRequest + 1) % Performance::PerfYields::REQUESTS;
		return m_perfYields->getYields().get(REQUEST, m_yield);
	}

	// The yield of ComplexYield.CachedValue: TWICE, _SUM(VOLUMETOTAL,VOLUMETOTAL).
	const std::string CACHED_YIELD = "TWICE";

	// Reads TWICE, _SUM(VOLUMETOTAL,VOLUMETOTAL), from the yields cache: 240, as ComplexYield.Sum
	// computes it.
	class CachedComplexYieldBenchmark final : public Performance::Benchmark
	{
	public:
		explicit CachedComplexYieldBenchmark(std::shared_ptr<Performance::PerfYields> p_perfYields);
		std::string getName() const override;
		std::string getDataset() const override;
		void prepare() override;
		double run() override;

	private:
		std::shared_ptr<Performance::PerfYields> m_perfYields;
		// The development of the request, at period 0: the request points to it.
		Core::FMTDevelopment m_development;
		std::unique_ptr<Core::FMTYieldRequest> m_request;
	};

	CachedComplexYieldBenchmark::CachedComplexYieldBenchmark(std::shared_ptr<Performance::PerfYields> p_perfYields) :
		m_perfYields(std::move(p_perfYields))
	{
	}

	std::string CachedComplexYieldBenchmark::getName() const
	{
		return "ComplexYield.CachedValue";
	}

	std::string CachedComplexYieldBenchmark::getDataset() const
	{
		return m_perfYields->getDataset();
	}

	void CachedComplexYieldBenchmark::prepare()
	{
		m_perfYields->prepare();
		m_development = m_perfYields->getDevelopment(0);
		m_development.setPeriod(0);
		m_request = std::make_unique<Core::FMTYieldRequest>(m_development);
		// The first use of the request locates its yield data; its value then goes to the cache.
		const double VALUE = m_perfYields->getYields().get(*m_request, CACHED_YIELD);
		Core::FMTYieldsCache cache;
		cache.set(VALUE, *m_request, CACHED_YIELD);
	}

	double CachedComplexYieldBenchmark::run()
	{
		return m_perfYields->getYields().get(*m_request, CACHED_YIELD);
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
		p_suite.add(std::make_unique<CachedComplexYieldBenchmark>(PERF_YIELDS));
	}
}
