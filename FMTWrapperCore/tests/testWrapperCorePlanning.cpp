#include <exception>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "FMTException.h"
#include "FMTLpModel.h"
#include "FMTModel.h"
#include "FMTModelParser.h"
#include "Planning.h"

// Test of FMTWrapper::Backend::Planning, registered in basetests.csv, on the public
// TWD_land data.
//
// Planning writes the schedule of the optimized scenarios into the project: the test
// therefore works on a copy of TWD_land, never on the repository example.
//
// - Planning: LP is optimized and its schedule rewritten; LP3 is played back from its
//   schedule (planningtest scenario); Globalreplanning, which has no schedule, checks
//   that a failed schedule read is reported without interrupting the planning.
// - Replanning: the replanningtest scenarios, with the replicate schedules written.
namespace
{
	int failures = 0;

	void check(bool p_condition, const std::string& p_description)
	{
		std::cout << (p_condition ? "  ok      " : "  FAILED  ") << p_description << std::endl;

		if (!p_condition)
		{
			++failures;
		}
	}

	const Models::FMTModel& findModel(
		const std::vector<Models::FMTModel>& p_models,
		const std::string& p_name)
	{
		for (const Models::FMTModel& MODEL : p_models)
		{
			if (MODEL.getName() == p_name)
			{
				return MODEL;
			}
		}

		throw std::runtime_error("Scenario not found: " + p_name);
	}

	bool containsFile(const std::filesystem::path& p_folder, const std::string& p_extension)
	{
		if (!std::filesystem::is_directory(p_folder))
		{
			return false;
		}

		for (const std::filesystem::directory_entry& ENTRY :
			std::filesystem::recursive_directory_iterator(p_folder))
		{
			if (ENTRY.is_regular_file() && ENTRY.path().extension() == p_extension)
			{
				return true;
			}
		}

		return false;
	}

	void report(const std::exception& p_exception)
	{
		std::cerr << p_exception.what() << std::endl;

		try
		{
			std::rethrow_if_nested(p_exception);
		}
		catch (const std::exception& nested)
		{
			std::cerr << nested.what() << std::endl;
		}
	}
}

int main()
{
	const std::filesystem::path SOURCE_PROJECT = "../../../../Examples/Models/TWD_land";
	const std::filesystem::path TEST_FOLDER = "../../tests/testWrapperCorePlanning";
	const std::filesystem::path PROJECT = TEST_FOLDER / "TWD_land";
	const std::filesystem::path PRIMARY_FILE = PROJECT / "TWD_land.pri";
	const std::filesystem::path OPTIMIZED_SCHEDULE = PROJECT / "Scenarios" / "LP" / "TWD_land._seq";
	const std::filesystem::path PLANNING_OUTPUT = TEST_FOLDER / "planning";
	const std::filesystem::path REPLANNING_OUTPUT = TEST_FOLDER / "replanning";

	try
	{
		std::filesystem::remove_all(TEST_FOLDER);
		std::filesystem::create_directories(TEST_FOLDER);
		std::filesystem::copy(SOURCE_PROJECT, PROJECT, std::filesystem::copy_options::recursive);

		// Planning must rewrite the schedule of LP, which it optimizes.
		std::filesystem::remove(OPTIMIZED_SCHEDULE);

		Parser::FMTModelParser modelParser;
		modelParser.setDefaultExceptionHandler();
		modelParser.setDefaultLogger();

		const std::vector<Models::FMTModel> MODELS = modelParser.readproject(
			PRIMARY_FILE.string(),
			{ "LP", "LP3", "Globalreplanning", "Globalfire", "Localreplanning" });

		FMTWrapper::Backend::PlanningParameters planning;
		planning.primaryFilePath = PRIMARY_FILE.string();
		planning.solver = static_cast<int>(Models::FMTSolverInterface::CLP);
		planning.numberOfPeriods = 5;
		planning.numberOfThreads = 2;
		planning.outputNames = { "OVOLREC" };
		planning.outputLevel = static_cast<int>(Core::FMToutputlevel::totalonly);
		planning.outputMinPeriod = 1;
		planning.outputMaxPeriod = 5;
		planning.outputPath = PLANNING_OUTPUT.string();
		planning.gdalProvider = "CSV";

		const std::vector<const Models::FMTModel*> PLANNED = {
			&findModel(MODELS, "LP"),
			&findModel(MODELS, "LP3"),
			&findModel(MODELS, "Globalreplanning") };
		const std::vector<bool> PLAYBACK = { false, true, true };

		int reportedScheduleErrors = 0;

		std::cout << "Planning" << std::endl;
		FMTWrapper::Backend::Planning::plan(planning, PLANNED, PLAYBACK,
			[&reportedScheduleErrors](const std::string& p_method, int, const std::string&)
			{
				++reportedScheduleErrors;
				std::cout << "  failed schedule read, reported from " << p_method << std::endl;
			});

		const std::filesystem::path LP_RESULTS = PLANNING_OUTPUT / "LP.csv";

		check(reportedScheduleErrors == 1, "the failed schedule read of Globalreplanning is reported once");
		check(std::filesystem::exists(OPTIMIZED_SCHEDULE), "the schedule of LP, optimized, is rewritten in the project");
		check(std::filesystem::exists(LP_RESULTS) && std::filesystem::file_size(LP_RESULTS) > 0,
			"the outputs of LP are written");

		FMTWrapper::Backend::ReplanningParameters replanning;
		replanning.solver = static_cast<int>(Models::FMTSolverInterface::CLP);
		replanning.numberOfPeriods = 10;
		replanning.replanningPeriods = 3;
		replanning.minimalDrift = 0.5;
		replanning.numberOfThreads = 2;
		replanning.minimumReplicates = 2;
		replanning.maximumReplicates = 2;
		replanning.outputNames = { "OVOLREC", "VOLINVENT", "BURNEDAREA" };
		replanning.outputLevel = static_cast<int>(Core::FMToutputlevel::totalonly);
		replanning.outputPath = REPLANNING_OUTPUT.string();
		replanning.gdalProvider = "CSV";
		replanning.taskLogLevel = 1;
		replanning.writeSchedules = true;

		std::cout << "Replanning" << std::endl;
		FMTWrapper::Backend::Planning::replan(
			replanning,
			findModel(MODELS, "Globalreplanning"),
			findModel(MODELS, "Globalfire"),
			findModel(MODELS, "Localreplanning"));

		check(containsFile(REPLANNING_OUTPUT, ".csv"), "the replanning outputs are written");
		check(containsFile(REPLANNING_OUTPUT, "._seq"), "one schedule is written per replicate");
	}
	catch (const std::exception& e)
	{
		report(e);
		return 1;
	}
	catch (...)
	{
		std::cerr << "Unknown error" << std::endl;
		return 1;
	}

	if (failures > 0)
	{
		std::cerr << failures << " check(s) failed" << std::endl;
		return 1;
	}

	std::cout << "test Planning passed!" << std::endl;

	return 0;
}
