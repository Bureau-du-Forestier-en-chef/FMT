/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include <vector>
#include "TestTools.h"
#ifdef FMTWITHOSI
#include <cmath>
#include <filesystem>
#include <map>
#include <memory>
#include <string>
#include "FMTLpModel.h"
#include "FMTModelParser.h"
#include "FMTDefaultLogger.h"
#include "FMTConstraint.h"
#include "FMTScheduleParser.h"
#include "FMTSchedule.h"
#include "FMTConstants.h"

// Checks the schedules of an optimal solution written by FMTScheduleParser::write: read back by
// FMTScheduleParser::read, they must give the area of each development of each action of each period of the solution.
//
// argv[1]: the primary file, TWD_land by default.
// argv[2]: the scenario solved, LP by default.
// argv[3]: optional, the scenario whose optimal schedules are expected, argv[2] by default: giving another one must
//          make the test fail.
namespace
{
	const int PERIODS = 10;
	const double AREA_TOLERANCE = 1e-5;

	// Builds a scenario over PERIODS periods, sets its constraints one by one, then its objective, and solves it.
	// Returns the schedules of its solution, one per period, or none when it cannot be solved.
	std::vector<Core::FMTSchedule> solve(Models::FMTLpModel& p_model)
	{
		for (int period = 0; period < PERIODS; ++period)
		{
			p_model.buildPeriod();
		}
		std::vector<Core::FMTConstraint> constraints = p_model.getConstraints();
		const Core::FMTConstraint OBJECTIVE = constraints.at(0);
		constraints.erase(constraints.begin());
		for (const Core::FMTConstraint& constraint : constraints)
		{
			p_model.setConstraint(constraint);
		}
		p_model.setObjective(OBJECTIVE);
		std::vector<Core::FMTSchedule> schedules;
		if (p_model.initialSolve())
		{
			for (int period = 1; period <= PERIODS; ++period)
			{
				schedules.push_back(p_model.getSolution(period));
			}
		}
		return schedules;
	}

	std::unique_ptr<Models::FMTLpModel> readScenario(const std::string& p_primaryFile, const std::string& p_scenario)
	{
		Parser::FMTModelParser modelparser;
		const std::vector<Models::FMTModel> models = modelparser.readproject(p_primaryFile, std::vector<std::string>(1, p_scenario));
		return std::unique_ptr<Models::FMTLpModel>(new Models::FMTLpModel(models.at(0), Models::FMTSolverInterface::CLP));
	}

	// The area of each development of each action of a schedule, by action, mask and age. A schedule without locks is
	// written without them: the areas of the developments that only differ by their lock are added.
	std::map<std::string, double> getAreas(const Core::FMTSchedule& p_schedule)
	{
		std::map<std::string, double> areas;
		for (const auto& action : p_schedule)
		{
			for (const auto& development : action.second)
			{
				std::string key = action.first.getName() + " " + std::string(development.first.getMask()) + " age "
					+ std::to_string(development.first.getAge());
				if (p_schedule.doUseLock())
				{
					key += " lock " + std::to_string(development.first.getLock());
				}
				for (const double& area : development.second)
				{
					areas[key] += area;
				}
			}
		}
		return areas;
	}

	// Returns an empty string when both schedules give the same areas, within AREA_TOLERANCE, else their first
	// difference. An area below the tolerance counts as no area.
	std::string getFirstAreaDifference(const Core::FMTSchedule& p_expected, const Core::FMTSchedule& p_actual)
	{
		std::map<std::string, double> areas = getAreas(p_expected);
		for (const std::pair<const std::string, double>& actualArea : getAreas(p_actual))
		{
			areas[actualArea.first] -= actualArea.second;
		}
		for (const std::pair<const std::string, double>& areaDifference : areas)
		{
			if (std::abs(areaDifference.second) > AREA_TOLERANCE)
			{
				return areaDifference.first + " differs by " + std::to_string(areaDifference.second);
			}
		}
		return std::string();
	}
}
#endif

int main(int argc, char* argv[])
{
#ifdef FMTWITHOSI
	return Testing::runTest([&]()
		{
		// Arrange
		Logging::FMTDefaultLogger().logStamp();
		const std::string PRIMARY_FILE = (argc > 1) ? argv[1] : "../../../../Examples/Models/TWD_land/TWD_land.pri";
		const std::string SCENARIO = (argc > 2) ? argv[2] : "LP";
		const std::string EXPECTED_SCENARIO = (argc > 3) ? argv[3] : SCENARIO;
		const std::filesystem::path OUTPUT_FOLDER = "../../tests/SolveAndWriteSchedules/";
		std::filesystem::remove_all(OUTPUT_FOLDER);
		std::filesystem::create_directories(OUTPUT_FOLDER);
		const std::unique_ptr<Models::FMTLpModel> MODEL = readScenario(PRIMARY_FILE, SCENARIO);
		const std::filesystem::path SCHEDULE_FILE = OUTPUT_FOLDER / (MODEL->getName() + "._seq");
		Parser::FMTScheduleParser scheduleparser;

		// Act
		const std::vector<Core::FMTSchedule> OPTIMAL_SCHEDULES = solve(*MODEL);
		scheduleparser.write(OPTIMAL_SCHEDULES, SCHEDULE_FILE.string());
		const std::vector<Core::FMTSchedule> REREAD_SCHEDULES = scheduleparser.read(MODEL->getThemes(), Core::FMTConstants(),
			MODEL->getActions(), SCHEDULE_FILE.string());

		// Assert
		Testing::Checker checker;
		std::vector<Core::FMTSchedule> expectedSchedules = OPTIMAL_SCHEDULES;
		if (EXPECTED_SCENARIO != SCENARIO)
		{
			expectedSchedules = solve(*readScenario(PRIMARY_FILE, EXPECTED_SCENARIO));
		}
		if (!checker.check(expectedSchedules.size() == static_cast<size_t>(PERIODS),
			EXPECTED_SCENARIO + " is solved over " + std::to_string(PERIODS) + " periods"))
		{
			return checker.exitCode();
		}
		std::map<int, Core::FMTSchedule> rereadByPeriod;
		for (const Core::FMTSchedule& schedule : REREAD_SCHEDULES)
		{
			rereadByPeriod[schedule.getPeriod()] = schedule;
		}
		for (const Core::FMTSchedule& expected : expectedSchedules)
		{
			const std::string FIRST_DIFFERENCE = getFirstAreaDifference(expected, rereadByPeriod[expected.getPeriod()]);
			checker.check(FIRST_DIFFERENCE.empty(), "period " + std::to_string(expected.getPeriod())
				+ ": the schedule read back gives the optimal areas" + (FIRST_DIFFERENCE.empty() ? "" : " (" + FIRST_DIFFERENCE + ")"));
		}
		return checker.exitCode();
		});
#else
	return Testing::skip("FMT is compiled without OSI");
#endif
}
