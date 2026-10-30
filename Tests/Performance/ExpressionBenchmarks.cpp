/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

// Evaluation of expressions, as FMT does for the _EQUATION yields and for the outputs that combine
// their sources. Expression.ShuntingYard evaluates an expression with the values of its variables;
// Expression.Operator calls an operator and a function that are already built. FMTExpression keeps
// its expression as text and converts it again at every evaluation: it has nothing to prepare, so
// that its first call costs as much as the next ones.

#include "ExpressionBenchmarks.h"

#include "FMTExpression.h"
#include "FMTFunctionCall.h"
#include "FMTOperator.h"

#include "Benchmark.h"
#include "BenchmarkSuite.h"

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace
{
	const std::string VARIABLE = "VOLUMETOTAL";
	// The value of VOLUMETOTAL for the development of the yield benchmarks, UNITE1 PEUPLEMENT1 UTR1 at
	// age 7.
	constexpr double VALUE = 120.0;

	// Evaluates VOLUMETOTAL + VOLUMETOTAL * 2 with VOLUMETOTAL at 120: 120 + 240 = 360.
	class ShuntingYardBenchmark final : public Performance::Benchmark
	{
	public:
		std::string getName() const override;
		std::string getDataset() const override;
		void prepare() override;
		double run() override;

	private:
		Core::FMTExpression m_expression;
		std::map<std::string, double> m_values;
	};

	std::string ShuntingYardBenchmark::getName() const
	{
		return "Expression.ShuntingYard";
	}

	std::string ShuntingYardBenchmark::getDataset() const
	{
		return "in code";
	}

	void ShuntingYardBenchmark::prepare()
	{
		m_expression = Core::FMTExpression(std::vector<std::string>{ VARIABLE, "+", VARIABLE, "*", "2" });
		m_values[VARIABLE] = VALUE;
	}

	double ShuntingYardBenchmark::run()
	{
		return m_expression.shuntingYard(m_values);
	}

	// Multiplies 120 by 2 with the operator *, and adds the natural logarithm of 1 with the function
	// LN: 240 + 0 = 240.
	class OperatorBenchmark final : public Performance::Benchmark
	{
	public:
		std::string getName() const override;
		std::string getDataset() const override;
		void prepare() override;
		double run() override;

	private:
		Core::FMTOperator m_multiply;
		Core::FMTFunctionCall m_logarithm;
		double m_factor = 2.0;
		double m_one = 1.0;
	};

	std::string OperatorBenchmark::getName() const
	{
		return "Expression.Operator";
	}

	std::string OperatorBenchmark::getDataset() const
	{
		return "in code";
	}

	void OperatorBenchmark::prepare()
	{
		m_multiply = Core::FMTOperator("*");
		m_logarithm = Core::FMTFunctionCall("LN");
	}

	double OperatorBenchmark::run()
	{
		return m_multiply.call(VALUE, m_factor) + m_logarithm.call(m_one);
	}
}

namespace Performance
{
	void addExpressionBenchmarks(BenchmarkSuite& p_suite)
	{
		p_suite.add(std::make_unique<ShuntingYardBenchmark>());
		p_suite.add(std::make_unique<OperatorBenchmark>());
	}
}
