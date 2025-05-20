// ATTENTION: Ceci est une copie de testWrapperCoreGetYield.cpp on dois l'ajuster

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
	int periods = 0;
	std::vector<int> themesNumbers;
	int resutlSize = 0;

	if (argc > 1)
	{
		std::vector<std::string>results;
		const std::string vals = argv[1];
		boost::split(results, vals, boost::is_any_of("|"));
		pathPri = results.at(0);
		scenarioName = results.at(1);
		periods = std::stoi(results.at(2));
		resutlSize = std::stoi(results.at(3));

		std::vector<std::string>csvThemesNumber;
		const std::string vals = argv[2];
		boost::split(results, vals, boost::is_any_of("|"));
		for (const std::string& theme : csvThemesNumber)
		{
			themesNumbers.push_back(std::stoi(theme));
		}
	}
	else
	{
		pathPri = "//Artemis/fecgeo/Donnees/02_Courant/07_Outil_moyen_methode/01_Entretien_developpement/Interne/FMT/Entretien/Modeles_test/TEST_TBE_CourbesHorsHorizon/PC_9949_U08251_2028_MODB01.pri";
		scenarioName = "TBE_TEST_CORRECTION";
		periods = 5;
		resutlSize = 21821;
		themesNumbers = { 3, 5, 12, 13, 14, 21};
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


	const Models::FMTmodel MODEL = ModelParser.readproject(pathPri, { scenarioName }).at(0);

	const std::set<std::string> RESULT = FMTWrapperCore::Tools::getAllMasks(MODEL, periods, themesNumbers);

	std::cout << RESULT.size() << std::endl;

	if (RESULT.size() != resutlSize) {
		Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed, "Nombre de masks non valide",
			"TestWrapperCoreGetAllMasks", __LINE__, __FILE__);

	}
	//for (const std::string& res : RESULT) {
	//	std::cout << res << std::endl;
	//}

	return 0;
}

