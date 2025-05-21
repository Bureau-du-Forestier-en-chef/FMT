#include <vector>
#include <string>
#include "FMTmodel.h"
#include "FMTlpmodel.h"
#include "FMTmodelparser.h"
#include "Tools.h"
#include "FMTfreeexceptionhandler.h"

int main(int argc, char* argv[])
{
	std::string pathPri;
	std::string scenarioName;
	std::string mask;
	std::string yieldName;
	int age = 0;
	double result = 0;
	if (argc > 1)
	{
		std::vector<std::string>primaryScenario;
		const std::string vals1 = argv[1];
		boost::split(primaryScenario, vals1, boost::is_any_of("|"));
		pathPri = primaryScenario.at(0);
		scenarioName = primaryScenario.at(1);
		yieldName = primaryScenario.at(2);
		mask = primaryScenario.at(3);

		std::vector<std::string>ageResult;
		const std::string vals2 = argv[2];
		boost::split(ageResult, vals2, boost::is_any_of("|"));
		age = std::stoi(ageResult.at(0));
		result = std::stoi(ageResult.at(1));
	}
	else
	{
		pathPri = "//Artemis/fecgeo/Donnees/02_Courant/07_Outil_moyen_methode/01_Entretien_developpement/Interne/FMT/Entretien/Modeles_test/TEST_TBE_CourbesHorsHorizon/PC_9949_U08251_2028_MODB01.pri";
		scenarioName = "TBE_TEST_CORRECTION";
		mask = "? ? ? ? FC2582 ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? POST";
		yieldName = "YV_E_SAB";
		age = 16;
		result = 54.53;
	}


	Parser::FMTmodelparser ModelParser;
	std::vector<Exception::FMTexc>errors;
	//errors.push_back(Exception::FMTexc::FMTmissingyield);
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

	const double yield = FMTWrapperCore::Tools::getYield(MODELS.at(0), mask, yieldName, age);
	std::cout << "Yield: " << yield << std::endl;

	// on fait des v�rifications sur le nombre renvoyer
	if (yield != result) {
		Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed, "Nombre de yield n'est pas égal au résultat attendu",
			"TestWrapperCoreGetYield", __LINE__, __FILE__);
	}


	return 0;
}

