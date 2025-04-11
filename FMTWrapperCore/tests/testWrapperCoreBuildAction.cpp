#include <vector>
#include <string>
#include "FMTmodel.h"
#include "FMTmodelparser.h"
#include "TransformationCore.h"
#include <boost/filesystem.hpp>

int main(int argc, char* argv[])
{
	//const std::string OUTPUT_DIRECTORY = "../../tests/testWrapperCoreBuildAction";
	const std::string NEW_SCENARIO_NAME = "Test_Output_scenario_name";

	const std::string OUTPUT_DIRECTORY = "D:/01_Valide_ServiceOuest/01_Valide_ServiceOuest/Scenarios/" + NEW_SCENARIO_NAME;
	std::string pathPri;
	std::string actionName;
	std::string targetYield;
	std::string scenarioName;
	std::string primaryName;
	if (argc > 1)
	{
		std::vector<std::string> result;
		boost::split(result, argv[1], boost::is_any_of("|"));
		pathPri = result.at(0);
		scenarioName = result.at(1);
		actionName = argv[2];
		targetYield = argv[3];
	}
	else 
	{
		//pathPri = "T:/Donnees/02_Courant/07_Outil_moyen_methode/01_Entretien_developpement/Interne/FMT/Entretien/Modeles_test/Prototype_Dec2023_TBE/PC_7002071_UA08152_FINAL.pri";
		pathPri = "D:/01_Valide_ServiceOuest/01_Valide_ServiceOuest/PC_9949_U08251_2028_MODB01.pri";
		actionName = "ATBEMORT";
		targetYield = "YV_S_MORT";
		//scenarioName = "20_Regl_prov";
		scenarioName = "1201_RegProv_avsp";
		primaryName = "PC_9949_U08251_2028_MODB01";

	}


	Parser::FMTmodelparser ModelParser;
	std::vector<Exception::FMTexc>errors;
	errors.push_back(Exception::FMTexc::FMTmissingyield);
	errors.push_back(Exception::FMTexc::FMToutput_missing_operator);
	errors.push_back(Exception::FMTexc::FMToutput_too_much_operator);
	errors.push_back(Exception::FMTexc::FMTinvalidyield_number);
	errors.push_back(Exception::FMTexc::FMTundefinedoutput_attribute);
	errors.push_back(Exception::FMTexc::FMToveridedyield);
	errors.push_back(Exception::FMTexc::FMTsourcetotarget_transition);
	errors.push_back(Exception::FMTexc::FMTsame_transitiontargets);
	errors.push_back(Exception::FMTexc::FMTunclosedforloop);
	errors.push_back(Exception::FMTexc::FMToutofrangeyield);
	errors.push_back(Exception::FMTexc::FMTdeathwithlock);
	errors.push_back(Exception::FMTexc::FMTempty_schedules);
	errors.push_back(Exception::FMTexc::FMTinvalid_geometry);
	ModelParser.seterrorstowarnings(errors);

	const std::vector<Models::FMTmodel> MODELS = ModelParser.readproject(pathPri, { scenarioName });
	const Models::FMTmodel BUILDED_MODEL = FMTWrapperCore::Transformation::buildAction(MODELS.at(0), actionName, targetYield, pathPri, OUTPUT_DIRECTORY, NEW_SCENARIO_NAME, primaryName);

	// on fait des v�rifications sur le model construit
	if (MODELS.at(0) == BUILDED_MODEL)
	{
		throw Exception::FMTexc::FMTfunctionfailed;
	}
	if (!boost::filesystem::exists(OUTPUT_DIRECTORY + "/" + primaryName + "._are") &&
        !boost::filesystem::exists(OUTPUT_DIRECTORY + "/" + primaryName + "._yld") &&
        !boost::filesystem::exists(OUTPUT_DIRECTORY + "/" + primaryName + "._act") &&
        !boost::filesystem::exists(OUTPUT_DIRECTORY + "/" + primaryName + "._trn") &&
        !boost::filesystem::exists(OUTPUT_DIRECTORY + "/" + primaryName + "._out") &&
        !boost::filesystem::exists(OUTPUT_DIRECTORY + "/" + primaryName + "._opt") &&
        !boost::filesystem::exists(OUTPUT_DIRECTORY + "/" + primaryName + "._lif") &&
        !boost::filesystem::exists(OUTPUT_DIRECTORY + "/" + primaryName + "._seq") &&
		!boost::filesystem::exists(OUTPUT_DIRECTORY + "/" + primaryName + "._lan")
		)
	{
		throw Exception::FMTexc::FMTfunctionfailed;
	}

	return 0;
}

