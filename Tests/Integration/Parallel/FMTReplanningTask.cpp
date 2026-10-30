/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifdef FMTWITHOSI
	#include "FMTLpModel.h"
	#include "FMTModel.h"
	#include "FMTNssModel.h"
	#include "FMTOutput.h"
	#include "FMTReplanningTask.h"
	#include "FMTTaskHandler.h"
#endif
#include "ImportRedirect.h"
#include "ParallelRuns.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

// FMTReplanningTask run by FMTTaskHandler, as the interface runs it (FMTWrapperCore/Source/Planning.cpp): the
// replicates are replanned on several threads, and every thread copies, at every period, the global, stochastic and
// local models that the copies of the task share. Each case replans 10 replicates of 5 periods of the replanning
// scenarios of TWD_land, as ReplanConcurrently and Flow.Replanning.Threads<N> do, on one thread first, then REPETITIONS
// times on 2 and on 5 threads: each run must write the same rows as the run on one thread. The replanned values depend
// on which optimal solution the solver returns (Documentation/PerformanceTesting.md, Model flows), so that the run on one
// thread is the reference, and only its number of rows is known by hand: 10 replicates x 5 periods x 2 outputs for the
// local model. Each model solves on one thread of its solver (rule 7 of Tests/ETAT.md).
//
// The disabled case observes the copies of the MOSEK tasks, through the import table of the interface of COIN-OR to
// MOSEK (ImportRedirect.h): it fails on a defect of FMTlib, named above it.

#ifdef FMTWITHOSI
namespace
{
	const std::vector<std::string> SCENARIOS = { "Globalreplanning", "Globalfire", "Localreplanning" };
	const std::vector<std::string> OUTPUTS = { "OVOLREC", "BURNEDAREA" };
	constexpr int GLOBAL_LENGTH = 10;
	constexpr int REPLICATES = 10;
	constexpr int PERIODS = 5;
	// Seed of the stochastic model and minimal drift of a replanned output, as in the replanning examples.
	constexpr unsigned int SEED = 0;
	constexpr double MINIMAL_DRIFT = 0.5;
	// Each run replans the ten replicates again: enough runs for a race between the threads to have its chance.
	constexpr int REPETITIONS = 10;
	const std::vector<unsigned int> THREADS = { 2, 5 };

	// Replans the replicates on p_threads threads, by onDemandRun or conccurentRun, and returns the rows written in
	// p_folder.
	Testing::WrittenRows replan(const std::vector<Models::FMTModel>& p_models, unsigned int p_threads, bool p_onDemand,
		const std::filesystem::path& p_folder)
	{
		std::filesystem::create_directories(p_folder);
		{
			Models::FMTLpModel global(p_models.at(0), Testing::getSolver());
			global.setParameter(Models::FMTintmodelparameters::LENGTH, GLOBAL_LENGTH);
			global.setParameter(Models::FMTintmodelparameters::NUMBER_OF_THREADS, 1);
			global.setParameter(Models::FMTboolmodelparameters::PRESOLVE_CAN_REMOVE_STATIC_THEMES, true);
			global.setParameter(Models::FMTboolmodelparameters::QUIET_LOGGING, true);
			Models::FMTNssModel stochastic(p_models.at(1), SEED);
			stochastic.setParameter(Models::FMTintmodelparameters::LENGTH, 1);
			stochastic.setParameter(Models::FMTboolmodelparameters::QUIET_LOGGING, true);
			Models::FMTLpModel local(p_models.at(2), Testing::getSolver());
			local.setParameter(Models::FMTintmodelparameters::LENGTH, 1);
			local.setParameter(Models::FMTintmodelparameters::NUMBER_OF_THREADS, 1);
			local.setParameter(Models::FMTboolmodelparameters::QUIET_LOGGING, true);
			std::vector<Core::FMTOutput> outputs;
			for (const Core::FMTOutput& OUTPUT : global.getOutputs())
			{
				if (std::find(OUTPUTS.begin(), OUTPUTS.end(), OUTPUT.getName()) != OUTPUTS.end())
				{
					outputs.push_back(OUTPUT);
				}
			}
			const std::unique_ptr<Parallel::FMTTask> TASK(new Parallel::FMTReplanningTask(global, stochastic, local, outputs,
				p_folder.string(), "CSV", std::vector<std::string>(1, "SEPARATOR=SEMICOLON"), REPLICATES, PERIODS,
				MINIMAL_DRIFT, Core::FMToutputlevel::totalonly, false));
			Parallel::FMTTaskHandler handler(TASK, p_threads);
			if (p_onDemand)
			{
				handler.onDemandRun();
			}
			else
			{
				handler.conccurentRun();
			}
		}
		return Testing::readWrittenRows(p_folder);
	}

	// Replans the replicates REPETITIONS times on each number of THREADS.
	std::vector<Testing::WrittenRows> replanRepeatedly(const std::vector<Models::FMTModel>& p_models, bool p_onDemand,
		const std::filesystem::path& p_folder)
	{
		std::vector<Testing::WrittenRows> runs;
		for (const unsigned int THREAD_COUNT : THREADS)
		{
			for (int repetition = 1; repetition <= REPETITIONS; ++repetition)
			{
				const std::string RUN = "threads" + std::to_string(THREAD_COUNT) + "_" + std::to_string(repetition);
				runs.push_back(replan(p_models, THREAD_COUNT, p_onDemand, p_folder / RUN));
			}
		}
		return runs;
	}

#if defined(FMTWITHMOSEK) && defined(_WIN32)
	// The calls of MSK_clonetask, observed by the disabled case.
	Testing::ObjectCalls* cloneCalls = nullptr;
	using CloneFunction = int (*)(void*, void**);
	CloneFunction originalClone = nullptr;

	int observeClone(void* p_task, void** p_clone)
	{
		Testing::ObjectCalls* const CALLS = cloneCalls;
		if (CALLS != nullptr)
		{
			CALLS->enter(p_task);
		}
		const int RESULT = originalClone(p_task, p_clone);
		if (CALLS != nullptr)
		{
			CALLS->leave(p_task);
		}
		return RESULT;
	}
#endif
}
#endif

TEST(FMTReplanningTask, GivenTheReplanningScenariosWhenReplannedOnSeveralThreadsByOnDemandRunThenWritesTheRowsOfOneThread)
{
#ifdef FMTWITHOSI
	// Arrange
	const std::filesystem::path FOLDER = Testing::getCaseFolder("ReplanningOnDemandRun");
	const std::vector<Models::FMTModel> MODELS = Testing::readScenarios(SCENARIOS);
	const Testing::WrittenRows ONE_THREAD = replan(MODELS, 1, false, FOLDER / "threads1");

	// Act
	const std::vector<Testing::WrittenRows> RUNS = replanRepeatedly(MODELS, true, FOLDER);

	// Assert
	EXPECT_EQ(Testing::countRows(ONE_THREAD, "Localreplanning.csv"), static_cast<std::size_t>(REPLICATES * PERIODS * 2));
	for (std::size_t run = 0; run < RUNS.size(); ++run)
	{
		EXPECT_EQ(RUNS.at(run), ONE_THREAD) << "run " << run;
	}
#else
	GTEST_SKIP() << "FMT is built without OSI";
#endif
}

TEST(FMTReplanningTask, GivenTheReplanningScenariosWhenReplannedOnSeveralThreadsByConcurrentRunThenWritesTheRowsOfOneThread)
{
#ifdef FMTWITHOSI
	// Arrange
	const std::filesystem::path FOLDER = Testing::getCaseFolder("ReplanningConcurrentRun");
	const std::vector<Models::FMTModel> MODELS = Testing::readScenarios(SCENARIOS);
	const Testing::WrittenRows ONE_THREAD = replan(MODELS, 1, false, FOLDER / "threads1");

	// Act
	const std::vector<Testing::WrittenRows> RUNS = replanRepeatedly(MODELS, false, FOLDER);

	// Assert
	EXPECT_EQ(Testing::countRows(ONE_THREAD, "Localreplanning.csv"), static_cast<std::size_t>(REPLICATES * PERIODS * 2));
	for (std::size_t run = 0; run < RUNS.size(); ++run)
	{
		EXPECT_EQ(RUNS.at(run), ONE_THREAD) << "run " << run;
	}
#else
	GTEST_SKIP() << "FMT is built without OSI";
#endif
}

// Disabled: the copies of a replanning task share its global, stochastic and local models, and every thread copies them
// at every period without a lock, so that several threads copy the same MOSEK task at once (MSK_clonetask), which MOSEK
// forbids: a task must not be "accessed from more than one thread at a time" (defect 37 of section 2.7 of
// Tests/ETAT.md). Two threads that copy the same task within 20 ms of each other are let meet, so that the copies that
// can run at the same time do.
TEST(FMTReplanningTask, DISABLED_GivenTheReplanningScenariosWhenReplannedOnSeveralThreadsThenNoTwoThreadsCopyTheSameMosekTaskAtOnce)
{
#if defined(FMTWITHMOSEK) && defined(_WIN32)
	// Arrange
	const std::filesystem::path FOLDER = Testing::getCaseFolder("ReplanningMosekCopies");
	const std::vector<Models::FMTModel> MODELS = Testing::readScenarios(SCENARIOS);
	const Testing::RedirectedImport CLONE("OsiMsk-0.dll", "mosek64_10_1.dll", "MSK_clonetask", reinterpret_cast<void*>(&observeClone));
	if (CLONE.getOriginal() == nullptr)
	{
		GTEST_SKIP() << "OsiMsk-0.dll does not import MSK_clonetask of mosek64_10_1.dll";
	}
	originalClone = reinterpret_cast<CloneFunction>(CLONE.getOriginal());
	Testing::ObjectCalls calls(std::chrono::milliseconds(20));
	cloneCalls = &calls;

	// Act
	replan(MODELS, 5, true, FOLDER / "threads5");
	cloneCalls = nullptr;

	// Assert
	ASSERT_FALSE(calls.empty());
	EXPECT_EQ(calls.getMostThreadsAtOnce(), 1);
#else
	GTEST_SKIP() << "FMT is built without MOSEK, or not for Windows";
#endif
}
