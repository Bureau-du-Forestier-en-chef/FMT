#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#ifdef FMTWITHOSI
	#include "FMTModelParser.h"
	#include "FMTModel.h"
	#include "FMTLpModel.h"
	#include "FMTSchedule.h"
	#include "FMTOutput.h"
	#include "FMTGraphStats.h"
#endif
#include "TestTools.h"

// Checks what building and optimizing a scenario gives: one query per CSV row.
//
// argv[1]: <primary file>|<scenario>|<query>|<length>
// argv[2]: expected value, for the queries that use one.
//
// Queries:
//   STATS|<length>    <vertices>|<edges>|<rows>|<columns> of the matrix once the scenario is
//                     optimized, from FMTSrModel::getStats. The expected value is measured
//                     once and protects the graph and matrix building from silent changes.
//   REPLAY|<length>   optimizes, then replays the optimal schedule in a fresh model with
//                     doPlanning(false, schedules) and compares every output of every period.
//                     Nothing to maintain: the model is its own reference. argv[2] then names
//                     another scenario to receive the schedule, which must make the test fail.
//
// The solver is MOSEK when FMT is built with it, CLP otherwise; both must give the same
// answer. Without OSI the test returns 77 (skipped).
#ifdef FMTWITHOSI
namespace
{
	// Tolerance of the replayed schedule, as in FMTsetsolution.
	const double REPLAY_TOLERANCE = 0.01;

	std::vector<std::string> split(const std::string& p_value)
	{
		std::vector<std::string> items;
		boost::split(items, p_value, boost::is_any_of("|"));
		return items;
	}

	Models::FMTLpModel getOptimizationModel(const Models::FMTModel& p_model, int p_length)
	{
		#ifdef FMTWITHMOSEK
			Models::FMTLpModel model(p_model, Models::FMTSolverInterface::MOSEK);
		#else
			Models::FMTLpModel model(p_model, Models::FMTSolverInterface::CLP);
		#endif
		model.setParameter(Models::FMTintmodelparameters::LENGTH, p_length);
		return model;
	}

	// Compares every output of every period between the optimized model and the replayed one.
	int checkReplay(const Models::FMTModel& p_base, Models::FMTLpModel& p_optimized, int p_length)
	{
		std::vector<Core::FMTSchedule> schedules;
		for (int period = 1; period <= p_length; ++period)
		{
			schedules.push_back(p_optimized.getSolution(period));
		}
		Models::FMTLpModel replayed = getOptimizationModel(p_base, p_length);
		replayed.FMTModel::setParameter(Models::FMTdblmodelparameters::TOLERANCE, REPLAY_TOLERANCE);
		replayed.setParameter(Models::FMTboolmodelparameters::FORCE_PARTIAL_BUILD, true);
		replayed.doPlanning(false, schedules);
		Testing::Checker checker;
		size_t comparisons = 0;
		for (const Core::FMTOutput& output : p_optimized.getOutputs())
		{
			for (int period = 1; period <= p_length; ++period)
			{
				const double OPTIMIZED = p_optimized.getOutput(output, period, Core::FMToutputlevel::totalonly).at("Total");
				const double REPLAYED = replayed.getOutput(output, period, Core::FMToutputlevel::totalonly).at("Total");
				const std::string DESCRIPTION = output.getName() + " at period " + std::to_string(period);
				if ((std::isnan(OPTIMIZED) && std::isnan(REPLAYED))
					|| (std::isinf(OPTIMIZED) && OPTIMIZED == REPLAYED))
				{
					// An output without a finite value, such as the ratio of two null outputs
					// DIVIDEZERO of TWD_land, is compared as is: the replay must give the same
					// non-finite value.
					checker.check(true, DESCRIPTION + ": same non-finite value on both sides");
				}
				else
				{
					checker.checkNear(REPLAYED, OPTIMIZED, 0.01 * (std::max)(1.0, std::abs(OPTIMIZED)), DESCRIPTION);
				}
				++comparisons;
			}
		}
		checker.check(comparisons > 0, "outputs compared: " + std::to_string(comparisons));
		return checker.exitCode();
	}
}
#endif

int main(int argc, char* argv[])
{
	return Testing::runTest([&]()
		{
		#ifdef FMTWITHOSI
		if (argc < 2)
		{
			throw std::invalid_argument("Usage: testScenarioPlanning <primary file>|<scenario>|<query>|<length> [expected value]");
		}
		const std::vector<std::string> ARGUMENTS = split(argv[1]);
		if (ARGUMENTS.size() != 4)
		{
			throw std::invalid_argument("The first argument needs a primary file, a scenario, a query and a length");
		}
		const std::string SCENARIO = ARGUMENTS.at(1);
		const std::string QUERY = ARGUMENTS.at(2);
		const int LENGTH = std::stoi(ARGUMENTS.at(3));

		Parser::FMTModelParser parser;
		parser.setDefaultExceptionHandler();
		const std::vector<Models::FMTModel> MODELS = parser.readproject(ARGUMENTS.at(0), std::vector<std::string>(1, SCENARIO));

		Models::FMTLpModel optimized = getOptimizationModel(MODELS.at(0), LENGTH);
		optimized.doPlanning(true);

		if (QUERY == "STATS")
		{
			if (argc < 3)
			{
				throw std::invalid_argument("STATS needs an expected value");
			}
			const Graph::FMTGraphStats STATS = optimized.getStats();
			const std::string ACTUAL = std::to_string(STATS.vertices) + "|" + std::to_string(STATS.edges)
				+ "|" + std::to_string(STATS.rows) + "|" + std::to_string(STATS.cols);
			Testing::Checker checker;
			checker.check(ACTUAL == argv[2], SCENARIO + " vertices|edges|rows|columns: " + ACTUAL
				+ " (expected " + argv[2] + ")");
			return checker.exitCode();
		}
		if (QUERY == "REPLAY")
		{
			if (argc > 2)
			{
				const std::vector<Models::FMTModel> OTHERS = parser.readproject(ARGUMENTS.at(0),
					std::vector<std::string>(1, argv[2]));
				return checkReplay(OTHERS.at(0), optimized, LENGTH);
			}
			return checkReplay(MODELS.at(0), optimized, LENGTH);
		}
		throw std::invalid_argument("Unknown query: " + QUERY);
		#else
		static_cast<void>(argc);
		static_cast<void>(argv);
		return Testing::skip("FMT is built without OSI");
		#endif
		});
}
