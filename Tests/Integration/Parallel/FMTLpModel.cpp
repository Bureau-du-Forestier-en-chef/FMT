/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifdef FMTWITHOSI
	#include "FMTLpModel.h"
	#include "FMTModel.h"
#endif
#include "ParallelRuns.h"

#include <gtest/gtest.h>

#include <string>
#include <vector>

// The threads of the solver of FMTLpModel, set by its parameter NUMBER_OF_THREADS (FMTLpSolver::setNumberOfThreads):
// MOSEK solves on that many threads when FMT is built with it; CLP ignores the parameter. A scenario of TWD_land
// solved REPETITIONS times on 4 threads must find the optimum it finds on one, which DoPlanning knows
// (Tests/System/Cpp/basetests.csv).

#ifdef FMTWITHOSI
namespace
{
	const std::string SCENARIO = "NOT_MASK";
	constexpr int LENGTH = 5;
	constexpr double OPTIMUM = 90738;
	// The tolerance of DoPlanning on an optimum.
	constexpr double OPTIMUM_TOLERANCE = 1.0;
	constexpr int REPETITIONS = 10;

	double solve(const Models::FMTModel& p_model, int p_threads)
	{
		Models::FMTLpModel model(p_model, Testing::getSolver());
		model.setParameter(Models::FMTintmodelparameters::LENGTH, LENGTH);
		model.setParameter(Models::FMTintmodelparameters::NUMBER_OF_THREADS, p_threads);
		model.setParameter(Models::FMTboolmodelparameters::QUIET_LOGGING, true);
		model.doPlanning(true);
		return model.getObjValue();
	}
}
#endif

TEST(FMTLpModel, GivenAScenarioSolvedOnOneThreadWhenSolvedOnFourThreadsThenFindsTheSameOptimum)
{
#ifdef FMTWITHOSI
	// Arrange
	const Models::FMTModel MODEL = Testing::readScenarios(std::vector<std::string>(1, SCENARIO)).at(0);
	const double ONE_THREAD = solve(MODEL, 1);

	// Act
	std::vector<double> optima;
	for (int repetition = 0; repetition < REPETITIONS; ++repetition)
	{
		optima.push_back(solve(MODEL, 4));
	}

	// Assert
	EXPECT_NEAR(ONE_THREAD, OPTIMUM, OPTIMUM_TOLERANCE);
	for (const double OPTIMUM_ON_FOUR_THREADS : optima)
	{
		EXPECT_NEAR(OPTIMUM_ON_FOUR_THREADS, ONE_THREAD, OPTIMUM_TOLERANCE);
	}
#else
	GTEST_SKIP() << "FMT is built without OSI";
#endif
}
