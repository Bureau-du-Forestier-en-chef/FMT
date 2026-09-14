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

// Test manuel et privé de FMTWrapperCore::OperatingArea : aucun modèle public n'a de
// yield YOUVERT. Il reprend le scénario et les réglages de l'exemple testOAschedulertask.
//
// Arguments, même forme que testOAschedulertask dans BFECtests.csv :
//   argv[1] = fichier .pri
//   argv[2] = "<scénario>|<fichier de paramètres>", ce dernier étant cherché dans
//             <dossier du .pri>/Scenarios/<scénario>/
//   argv[3] = fichier vectoriel des aires d'opération
// Sans argument : première ligne testOAschedulertask de BFECtests.csv.
//
// Vérifie que la planification réussit, que la tâche écrit son modèle final dans le
// dossier de résultat, puis que ce modèle se relit et rejoue sa cédule.
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
	// getOperatingArea transmet ce numéro tel quel à readOaSchedulerParameters, qui attend
	// un indice 0-based : 13 désigne le 14e thème, celui qu'utilise testOAschedulertask.
	params.themeNumber = 13;
	params.maximumTime = 9000;
	params.numberOfIterations = 10;
	params.ageField = "AGE";
	params.areaField = "SUPERFICIE";
	params.lockField = "STANLOCK";
	params.parametersFilePath =
		(PRIMARY_FOLDER / "Scenarios" / scenario / parametersFile).string();
	params.resultFolder = "../../tests/testWrapperCoreOperatingArea";
	// 2 est la valeur par défaut de UPDATE dans FMT, celle que garde l'exemple.
	params.updatePeriod = 2;
	params.returnTimeOutputName = "OATTEINTE7M";

	// Les sorties de temps de retour sont écrites sous <dossier>/Retour, qui doit exister.
	std::filesystem::create_directories(std::filesystem::path(params.resultFolder) / "Retour");

	// La tâche écrit son modèle final dans le dossier de résultat, sous le nom du scénario.
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
			std::cerr << "Planification refusee : " << RESULTS.errorMessage << std::endl;
			return 1;
		}

		if (!std::filesystem::exists(FINAL_MODEL))
		{
			std::cerr << "Modele final absent : " << FINAL_MODEL.string() << std::endl;
			return 1;
		}

		// Comme testOAschedulertask : le modèle final doit se relire et rejouer sa cédule.
		const std::vector<Models::FMTModel> WRITTEN =
			modelParser.readproject(FINAL_MODEL.string(), std::vector<std::string>(1, "ROOT"));
		const std::vector<Core::FMTSchedule> SCHEDULES =
			modelParser.readSchedules(FINAL_MODEL.string(), WRITTEN).at(0);

		if (SCHEDULES.empty())
		{
			std::cerr << "Le modele final n'a pas de cedule" << std::endl;
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
