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

// Test manuel de FMTWrapperCore::Planning, sur les données publiques TWD_land.
//
// La planification écrit dans le projet la cédule des scénarios optimisés : le test travaille
// donc sur une copie de TWD_land, jamais sur l'exemple du dépôt.
//
// - Planification : LP est optimisé et sa cédule réécrite ; LP3 est rejoué depuis sa cédule
//   (scénario de planningtest) ; Globalreplanning, qui n'a pas de cédule, vérifie qu'une
//   relecture en échec est signalée sans interrompre la planification.
// - Replanification : les scénarios de replanningtest, avec l'écriture des cédules des
//   réplicats.
namespace
{
	int failures = 0;

	void check(bool p_condition, const std::string& p_description)
	{
		std::cout << (p_condition ? "  ok      " : "  ECHEC   ") << p_description << std::endl;

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

		throw std::runtime_error("Scenario introuvable : " + p_name);
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

		// La planification doit réécrire la cédule de LP, qu'elle optimise.
		std::filesystem::remove(OPTIMIZED_SCHEDULE);

		Parser::FMTModelParser modelParser;
		modelParser.setDefaultExceptionHandler();
		modelParser.setDefaultLogger();

		const std::vector<Models::FMTModel> MODELS = modelParser.readproject(
			PRIMARY_FILE.string(),
			{ "LP", "LP3", "Globalreplanning", "Globalfire", "Localreplanning" });

		FMTWrapperCore::PlanningParameters planning;
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

		std::cout << "Planification" << std::endl;
		FMTWrapperCore::Planning::plan(planning, PLANNED, PLAYBACK,
			[&reportedScheduleErrors](const std::string& p_method, int, const std::string&)
			{
				++reportedScheduleErrors;
				std::cout << "  relecture en echec, signalee depuis " << p_method << std::endl;
			});

		const std::filesystem::path LP_RESULTS = PLANNING_OUTPUT / "LP.csv";

		check(reportedScheduleErrors == 1, "la relecture en echec de Globalreplanning est signalee une fois");
		check(std::filesystem::exists(OPTIMIZED_SCHEDULE), "la cedule de LP, optimise, est reecrite dans le projet");
		check(std::filesystem::exists(LP_RESULTS) && std::filesystem::file_size(LP_RESULTS) > 0,
			"les sorties de LP sont ecrites");

		FMTWrapperCore::ReplanningParameters replanning;
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

		std::cout << "Replanification" << std::endl;
		FMTWrapperCore::Planning::replan(
			replanning,
			findModel(MODELS, "Globalreplanning"),
			findModel(MODELS, "Globalfire"),
			findModel(MODELS, "Localreplanning"));

		check(containsFile(REPLANNING_OUTPUT, ".csv"), "les sorties de la replanification sont ecrites");
		check(containsFile(REPLANNING_OUTPUT, "._seq"), "une cedule est ecrite par replicat");
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
		std::cerr << failures << " verification(s) en echec" << std::endl;
		return 1;
	}

	std::cout << "test Planning passed!" << std::endl;

	return 0;
}
