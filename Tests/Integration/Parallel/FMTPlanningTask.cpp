/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifdef FMTWITHOSI
	#include "FMTLpModel.h"
	#include "FMTModel.h"
	#include "FMTOutput.h"
	#include "FMTPlanningTask.h"
	#include "FMTSchedule.h"
	#include "FMTTaskHandler.h"
#endif
#include "ImportRedirect.h"
#include "ParallelRuns.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <map>
#include <memory>
#include <string>
#include <thread>
#include <vector>

// FMTPlanningTask run by FMTTaskHandler, as the interface runs it (FMTWrapperCore/Source/Planning.cpp): the scenarios
// of a task are planned on several threads, each on its own copy of its model, and their outputs are written by the
// FMTParallelWriter that the threads share. Each case plans five scenarios of TWD_land whose optimum DoPlanning knows
// (Tests/System/Cpp/basetests.csv), on one thread first, then REPETITIONS times on 2 and on 4 threads: each run must
// find the same objectives and write the same rows as the run on one thread. Each model solves on one thread of its
// solver, so that only the tasks run in parallel and the optimal solution does not depend on the threads (rule 7 of
// Tests/ETAT.md).
//
// The task keeps its models (setKeepModels), so that the objectives of conccurentRun are read from the tasks of the
// handler; onDemandRun gives its tasks to workers that destroy them, and only its rows are compared.
//
// The disabled case observes the message handlers of the solvers, through the import tables of the interfaces of
// COIN-OR (ImportRedirect.h): it fails on a defect of FMTlib, named above it.

#ifdef FMTWITHOSI
namespace
{
	// A scenario, its number of periods and its optimum, from the DoPlanning rows of basetests.csv.
	struct Scenario
	{
		std::string name;
		int length;
		double optimum;
	};

	const std::vector<Scenario> SCENARIOS = {
		{ "NOT_MASK", 5, 90738 },
		{ "division", 5, 349540 },
		{ "ExcludeArea", 5, 16849 },
		{ "replanification_tactique", 5, 201360 },
		{ "equation", 2, 26665 } };
	// The tolerance of DoPlanning on an optimum.
	constexpr double OPTIMUM_TOLERANCE = 1.0;
	// Each run plans the five scenarios again: enough runs for a race between the threads to have its chance.
	constexpr int REPETITIONS = 20;
	const std::vector<unsigned int> THREADS = { 2, 4 };

	struct PlanningRun
	{
		std::map<std::string, double> objectives;
		Testing::WrittenRows rows;
	};

	const Scenario& getScenario(const std::string& p_name)
	{
		return *std::find_if(SCENARIOS.begin(), SCENARIOS.end(), [&p_name](const Scenario& p_scenario)
			{
				return p_scenario.name == p_name;
			});
	}

	std::vector<std::string> getScenarioNames()
	{
		std::vector<std::string> names;
		for (const Scenario& SCENARIO : SCENARIOS)
		{
			names.push_back(SCENARIO.name);
		}
		return names;
	}

	// Plans p_models with p_solver on p_threads threads, by onDemandRun or conccurentRun, and writes their outputs in
	// p_folder.
	PlanningRun plan(const std::vector<Models::FMTModel>& p_models, unsigned int p_threads, bool p_onDemand,
		const std::filesystem::path& p_folder, Models::FMTSolverInterface p_solver = Testing::getSolver())
	{
		PlanningRun run;
		std::filesystem::create_directories(p_folder);
		{
			Parallel::FMTPlanningTask task(1, 5, p_folder.string(), "CSV", std::vector<std::string>(1, "SEPARATOR=SEMICOLON"),
				Core::FMToutputlevel::totalonly);
			task.setKeepModels();
			for (const Models::FMTModel& MODEL : p_models)
			{
				Models::FMTLpModel model(MODEL, p_solver);
				model.setParameter(Models::FMTintmodelparameters::LENGTH, getScenario(MODEL.getName()).length);
				model.setParameter(Models::FMTintmodelparameters::NUMBER_OF_THREADS, 1);
				model.setParameter(Models::FMTboolmodelparameters::QUIET_LOGGING, true);
				task.push_back(model, std::vector<Core::FMTSchedule>(), model.getOutputs());
			}
			Parallel::FMTTaskHandler handler(task, p_threads);
			if (p_onDemand)
			{
				handler.onDemandRun();
			}
			else
			{
				handler.conccurentRun();
				for (const std::unique_ptr<Parallel::FMTTask>& TASK : handler.getTasks())
				{
					const Parallel::FMTPlanningTask& PLANNING = dynamic_cast<const Parallel::FMTPlanningTask&>(*TASK);
					for (const Models::FMTLpModel* MODEL : PLANNING.getModelsFromDynamicCast<Models::FMTLpModel>())
					{
						run.objectives[MODEL->getName()] = MODEL->getObjValue();
					}
				}
			}
		}
		// The writer closes its files when the last task that shares it is destroyed.
		run.rows = Testing::readWrittenRows(p_folder);
		return run;
	}

	// Plans the scenarios on one thread, then REPETITIONS times on each number of THREADS.
	std::vector<PlanningRun> planRepeatedly(const std::vector<Models::FMTModel>& p_models, bool p_onDemand,
		const std::filesystem::path& p_folder)
	{
		std::vector<PlanningRun> runs;
		for (const unsigned int THREAD_COUNT : THREADS)
		{
			for (int repetition = 1; repetition <= REPETITIONS; ++repetition)
			{
				const std::string RUN = "threads" + std::to_string(THREAD_COUNT) + "_" + std::to_string(repetition);
				runs.push_back(plan(p_models, THREAD_COUNT, p_onDemand, p_folder / RUN));
			}
		}
		return runs;
	}

#ifdef _WIN32
	// The calls of CoinMessageHandler::message(int, const CoinMessages&), observed by the disabled case.
	Testing::ObjectCalls* messageCalls = nullptr;
	using MessageFunction = void* (*)(void*, int, const void*);
	MessageFunction originalMessage = nullptr;

	void* observeMessage(void* p_handler, int p_number, const void* p_messages)
	{
		Testing::ObjectCalls* const CALLS = messageCalls;
		if (CALLS != nullptr)
		{
			CALLS->enter(p_handler);
		}
		void* const RESULT = originalMessage(p_handler, p_number, p_messages);
		if (CALLS != nullptr)
		{
			CALLS->leave(p_handler);
		}
		return RESULT;
	}
#endif
}
#endif

TEST(FMTPlanningTask, GivenScenariosWithKnownOptimaWhenPlannedOnSeveralThreadsByConcurrentRunThenFindsTheOptimaAndWritesTheRowsOfOneThread)
{
#ifdef FMTWITHOSI
	// Arrange
	const std::filesystem::path FOLDER = Testing::getCaseFolder("PlanningConcurrentRun");
	const std::vector<Models::FMTModel> MODELS = Testing::readScenarios(getScenarioNames());
	const PlanningRun ONE_THREAD = plan(MODELS, 1, false, FOLDER / "threads1");

	// Act
	const std::vector<PlanningRun> RUNS = planRepeatedly(MODELS, false, FOLDER);

	// Assert
	ASSERT_EQ(ONE_THREAD.objectives.size(), SCENARIOS.size());
	for (const Scenario& SCENARIO : SCENARIOS)
	{
		EXPECT_NEAR(ONE_THREAD.objectives.at(SCENARIO.name), SCENARIO.optimum, OPTIMUM_TOLERANCE) << SCENARIO.name;
	}
	EXPECT_EQ(ONE_THREAD.rows.size(), SCENARIOS.size());
	for (std::size_t run = 0; run < RUNS.size(); ++run)
	{
		EXPECT_EQ(RUNS.at(run).objectives, ONE_THREAD.objectives) << "run " << run;
		EXPECT_EQ(RUNS.at(run).rows, ONE_THREAD.rows) << "run " << run;
	}
#else
	GTEST_SKIP() << "FMT is built without OSI";
#endif
}

TEST(FMTPlanningTask, GivenScenariosWithKnownOptimaWhenPlannedOnSeveralThreadsByOnDemandRunThenWritesTheRowsOfOneThread)
{
#ifdef FMTWITHOSI
	// Arrange
	const std::filesystem::path FOLDER = Testing::getCaseFolder("PlanningOnDemandRun");
	const std::vector<Models::FMTModel> MODELS = Testing::readScenarios(getScenarioNames());
	const PlanningRun ONE_THREAD = plan(MODELS, 1, true, FOLDER / "threads1");

	// Act
	const std::vector<PlanningRun> RUNS = planRepeatedly(MODELS, true, FOLDER);

	// Assert
	EXPECT_EQ(ONE_THREAD.rows.size(), SCENARIOS.size());
	for (std::size_t run = 0; run < RUNS.size(); ++run)
	{
		EXPECT_EQ(RUNS.at(run).rows, ONE_THREAD.rows) << "run " << run;
	}
#else
	GTEST_SKIP() << "FMT is built without OSI";
#endif
}

// Disabled: a planning task gives every model pushed to it the logger of the task, and the copies that split and spawn
// make for the threads keep its message handler, so that the solvers of every thread log into the same
// CoinMessageHandler, which has no lock: with CLP, the threads write in it at once (defect 36 of section 2.7 of
// Tests/ETAT.md). The case plans with CLP, whose solver logs through the handler; MOSEK does not, at the log level of
// the task.
TEST(FMTPlanningTask, DISABLED_GivenClpModelsWhenPlannedOnSeveralThreadsThenEachThreadLogsIntoItsOwnMessageHandler)
{
#if defined(FMTWITHOSI) && defined(_WIN32)
	// Arrange
	const std::filesystem::path FOLDER = Testing::getCaseFolder("PlanningMessageHandlers");
	const std::vector<Models::FMTModel> MODELS = Testing::readScenarios(getScenarioNames());
	const char* const MESSAGE = "?message@CoinMessageHandler@@QEAAAEAV1@HAEBVCoinMessages@@@Z";
	const Testing::RedirectedImport FROM_OSI_CLP("OsiClp-0.dll", "CoinUtils-0.dll", MESSAGE, reinterpret_cast<void*>(&observeMessage));
	const Testing::RedirectedImport FROM_CLP("Clp-0.dll", "CoinUtils-0.dll", MESSAGE, reinterpret_cast<void*>(&observeMessage));
	const Testing::RedirectedImport FROM_OSI("Osi-0.dll", "CoinUtils-0.dll", MESSAGE, reinterpret_cast<void*>(&observeMessage));
	for (const Testing::RedirectedImport* IMPORT : { &FROM_OSI_CLP, &FROM_CLP, &FROM_OSI })
	{
		if (IMPORT->getOriginal() != nullptr)
		{
			originalMessage = reinterpret_cast<MessageFunction>(IMPORT->getOriginal());
		}
	}
	if (originalMessage == nullptr)
	{
		GTEST_SKIP() << "No interface of COIN-OR imports CoinMessageHandler::message";
	}
	Testing::ObjectCalls calls;
	messageCalls = &calls;

	// Act
	plan(MODELS, 4, false, FOLDER / "threads4", Models::FMTSolverInterface::CLP);
	messageCalls = nullptr;

	// Assert
	ASSERT_FALSE(calls.empty());
	EXPECT_EQ(calls.getMostThreadsOfAnObject(std::this_thread::get_id()), static_cast<std::size_t>(1));
#else
	GTEST_SKIP() << "FMT is built without OSI, or not for Windows";
#endif
}
