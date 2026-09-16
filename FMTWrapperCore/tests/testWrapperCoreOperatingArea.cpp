#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>

#include "FMTException.h"
#include "FMTLpModel.h"
#include "FMTModel.h"
#include "FMTModelParser.h"
#include "FMTSchedule.h"
#include "OperatingArea.h"

// Private test of FMTWrapperCore::OperatingArea, registered in BFECtests.csv: no public
// model has a YOUVERT yield. It reuses the scenario and the settings of the
// testOAschedulertask example.
//
// Arguments, same form as testOAschedulertask in BFECtests.csv:
//   argv[1] = .pri file
//   argv[2] = "<scenario>|<parameters file>", the latter being looked for in
//             <.pri folder>/Scenarios/<scenario>/
//   argv[3] = vector file of the operating areas
// Without arguments: first testOAschedulertask line of BFECtests.csv.
//
// Checks that the scheduling succeeds, that the task writes its final model into the
// result folder, then that this model is read back and replays its schedule.
int main(int argc, char* argv[])
{
	std::string primaryLocation;
	std::string scenario;
	std::string parametersFile;
	std::string vectorFile;

	if (argc > 3)
	{
		primaryLocation = argv[1];
		std::vector<std::string> values;
		const std::string ARGUMENT = argv[2];
		boost::split(values, ARGUMENT, boost::is_any_of("|"));
		scenario = values.at(0);
		parametersFile = values.at(1);
		vectorFile = argv[3];
	}
	else
	{
		const std::string FOLDER =
			"T:/Donnees/02_Courant/07_Outil_moyen_methode/01_Entretien_developpement/Interne/FMT/Entretien/Modeles_test/08551_det/";
		primaryLocation = FOLDER + "PC_9424_U08551_4_Vg1_2023_vSSP03.pri";
		scenario = "13_Sc5a_Determin_avsp_CLE_PESSIERE";
		parametersFile = "parameters8551_rigide.csv";
		vectorFile = FOLDER + "Carte/PC_9424_UA_U08551.shp";
	}

	const std::filesystem::path PRIMARY_FOLDER =
		std::filesystem::path(primaryLocation).parent_path();

	FMTWrapperCore::OperatingAreaParameters params;
	params.vectorFilePath = vectorFile;
	params.solver = static_cast<int>(Models::FMTSolverInterface::MOSEK);
	params.numberOfPeriods = 5;
	params.numberOfThreads = 1;
	// getOperatingArea passes this number as is to readOaSchedulerParameters, which expects
	// a 0-based index: 13 designates the 14th theme, the one testOAschedulertask uses.
	params.themeNumber = 13;
	params.maximumTime = 9000;
	params.numberOfIterations = 10;
	params.ageField = "AGE";
	params.areaField = "SUPERFICIE";
	params.lockField = "STANLOCK";
	params.parametersFilePath =
		(PRIMARY_FOLDER / "Scenarios" / scenario / parametersFile).string();
	params.resultFolder = "../../tests/testWrapperCoreOperatingArea";
	// 2 is the default UPDATE value in FMT, the one the example keeps.
	params.updatePeriod = 2;
	params.returnTimeOutputName = "OATTEINTE7M";

	// The return time outputs are written under <folder>/Retour, which must exist.
	std::filesystem::create_directories(std::filesystem::path(params.resultFolder) / "Retour");

	// The task writes its final model into the result folder, under the scenario name.
	const std::filesystem::path FINAL_MODEL =
		std::filesystem::path(params.resultFolder) / (scenario + ".pri");
	std::filesystem::remove(FINAL_MODEL);

	Parser::FMTModelParser modelParser;
	modelParser.setDefaultExceptionHandler();
	modelParser.setDefaultLogger();

	std::vector<Exception::FMTexc> errors;
	errors.push_back(Exception::FMTexc::FMTmissingyield);
	errors.push_back(Exception::FMTexc::FMToutput_missing_operator);
	errors.push_back(Exception::FMTexc::FMToutput_too_much_operator);
	errors.push_back(Exception::FMTexc::FMTinvalidyield_number);
	errors.push_back(Exception::FMTexc::FMTundefinedoutput_attribute);
	errors.push_back(Exception::FMTexc::FMToveridedyield);
	errors.push_back(Exception::FMTexc::FMToutofrangeyield);
	errors.push_back(Exception::FMTexc::FMTsame_transitiontargets);
	errors.push_back(Exception::FMTexc::FMTEmpty_OA);
	errors.push_back(Exception::FMTexc::FMTdeathwithlock);
	modelParser.setErrorsToWarnings(errors);

	try
	{
		const std::vector<Models::FMTModel> MODELS =
			modelParser.readproject(primaryLocation, std::vector<std::string>(1, scenario));

		const FMTWrapperCore::OperatingAreaResults RESULTS =
			FMTWrapperCore::OperatingArea::schedule(params, MODELS.at(0));

		if (!RESULTS.success)
		{
			std::cerr << "Scheduling refused: " << RESULTS.errorMessage << std::endl;
			return 1;
		}

		if (!std::filesystem::exists(FINAL_MODEL))
		{
			std::cerr << "Missing final model: " << FINAL_MODEL.string() << std::endl;
			return 1;
		}

		// As in testOAschedulertask: the final model must be read back and replay its schedule.
		const std::vector<Models::FMTModel> WRITTEN =
			modelParser.readproject(FINAL_MODEL.string(), std::vector<std::string>(1, "ROOT"));
		const std::vector<Core::FMTSchedule> SCHEDULES =
			modelParser.readSchedules(FINAL_MODEL.string(), WRITTEN).at(0);

		if (SCHEDULES.empty())
		{
			std::cerr << "The final model has no schedule" << std::endl;
			return 1;
		}

		Models::FMTLpModel replay(WRITTEN.at(0), Models::FMTSolverInterface::CLP);
		replay.setParameter(Models::FMTintmodelparameters::LENGTH, params.numberOfPeriods);
		replay.setParameter(Models::FMTboolmodelparameters::STRICTLY_POSITIVE, true);
		replay.Models::FMTModel::setParameter(Models::FMTdblmodelparameters::TOLERANCE, 0.01);
		replay.doPlanning(false, SCHEDULES);
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		try { std::rethrow_if_nested(e); }
		catch (const std::exception& nested) { std::cerr << nested.what() << std::endl; }
		return 1;
	}
	catch (...)
	{
		std::cerr << "Unknown error" << std::endl;
		return 1;
	}

	std::cout << "test OperatingArea passed!" << std::endl;

	return 0;
}
