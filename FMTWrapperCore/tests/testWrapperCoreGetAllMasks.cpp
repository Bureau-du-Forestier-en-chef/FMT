#include <vector>
#include <string>
#include <fstream>
#include "FMTmodel.h"
#include "FMTlpmodel.h"
#include "FMTmodelparser.h"
#include "Tools.h"
#include "FMTfreeexceptionhandler.h"
#include <set>

int main(int argc, char* argv[])
{
	std::string pathPri;
	std::string scenarioName;
	std::string mask;
	std::string yieldName;
	int periods = 0;
	std::vector<int> themesNumbers;
	int resultSize = 0;
	std::string rastpath;
	std::string outputFilePath;
	bool writeOnDisk;

	if (argc > 1)
	{
		std::vector<std::string> results;
		writeOnDisk = true;
		boost::split(results,argv[1], boost::is_any_of("|"));
		pathPri = results.at(0);
		scenarioName = results.at(1);
		periods = std::stoi(results.at(2));
		resultSize = std::stoi(results.at(3));
		outputFilePath = "..\\..\\tests\\testWrapperCoreGetAllMasks\\" + scenarioName + ".txt";

		std::vector<std::string> csvThemesNumber;
		boost::split(csvThemesNumber, argv[2], boost::is_any_of("|"));
		for (const std::string& theme : csvThemesNumber)
		{
			themesNumbers.push_back(std::stoi(theme));
		}
	}
	else
	{
		pathPri = "T:/Donnees/02_Courant/07_Outil_moyen_methode/01_Entretien_developpement/Interne/FMT/Entretien/Modeles_test/TEST_TBE_CourbesHorsHorizon/PC_9949_U08251_2028_MODB01.pri";
		scenarioName = "TBE_TEST_CORRECTION";
		periods = 5;
		writeOnDisk = true;
		outputFilePath = "..\\..\\tests\\testWrapperCoreGetAllMasks\\get_all_mask_output.txt";
		resultSize = 19607;
		themesNumbers = {3, 5, 12, 13, 14, 21};
		rastpath = "";
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

	Models::FMTmodel model = ModelParser.readproject(pathPri, { scenarioName }).at(0);

	std::cout << pathPri << "\n";
	std::cout << scenarioName << "\n";
	std::cout << periods << "\n";
	std::cout << resultSize << "\n";
	std::cout << outputFilePath << "\n"; 

	for (const int& theme : themesNumbers)
	{
		std::cout << "theme " + std::to_string(theme) << "\n";
	}

	const std::set<std::string> RESULT = FMTWrapperCore::Tools::getAllMasks(
		model, periods, themesNumbers, rastpath);

	// �criture sur le disque en cas de test manuel
	if (writeOnDisk) {
		std::ofstream outputFile(outputFilePath);

		if (outputFile.is_open()) {
			for (const std::string& res : RESULT) {
				outputFile << res << std::endl;
			}
			outputFile.close();
			std::cout << "Les r�sultats ont �t� �crits dans " << outputFilePath << std::endl;
		}
		else {
			std::cerr << "Erreur: Impossible d'ouvrir le fichier " << outputFilePath << " pour l'�criture." << std::endl;
			return 1;
		}

	}

	if (RESULT.size() != resultSize) {
		std::cout << std::to_string(RESULT.size()) + "!=" + std::to_string(resultSize);
		Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed, "Nombre de masks non valide",
			"TestWrapperCoreGetAllMasks", __LINE__, __FILE__);
	}

	return 0;
}