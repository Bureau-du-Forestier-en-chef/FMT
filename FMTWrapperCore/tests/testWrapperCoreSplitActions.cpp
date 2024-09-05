#include <string>
#include <vector>
#include "FMTexception.h"
#include "FMTmodel.h"
#include "FMTmodelparser.h"
#include "TransformationCore.h"
#include <boost/algorithm/string.hpp>

int main(int argc, char* argv[])
{

	std::string pathPri;
	std::vector<std::string> splitted;
	std::vector<std::string> splittedMask;
	const std::string OUTPUT_DIRECTORY = "../../tests/testWrapperCoreSplitActions";
	std::string scenarioName;
	if (argc < 2)
	{
		pathPri = "T:/Donnees/02_Courant/07_Outil_moyen_methode/01_Entretien_developpement/09_FMT/Modeles_test/Prototype_Dec2023/PC_7002071_UA08152_FINAL.pri";
		splitted = { "AEC", "ACT" };
		splittedMask = { "? ? ? ? ? ? ? ? ? ? ? ? ? UTA11 ? ? ? ? ? ?", "? ? ? ? ? ? ? ? ? ? ? ? ? !UTA11 ? ? ? ? ? ?" };
		scenarioName = "01_Regl_prov_apsp_1_6";
		
	}
	else 
	{
		std::vector<std::string> result;
		boost::split(result, argv[1], boost::is_any_of("|"));
		pathPri = result.at(0);
		scenarioName = result.at(1);
		boost::split(splitted, argv[2], boost::is_any_of("|"));
		boost::split(splittedMask, argv[3], boost::is_any_of("|"));
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
	const Models::FMTmodel SPLITTED_MODEL = FMTWrapperCore::Transformation::splitActions(MODELS.at(0), pathPri, splitted, splittedMask, OUTPUT_DIRECTORY, scenarioName);
	
	// On vérifie si on a plus d'actions dans le nouveau model
	int splittedCount = SPLITTED_MODEL.getactions().size();
	int startCount = MODELS.at(0).getactions().size();
	std::cout << "startCount: " << startCount << std::endl;
	std::cout << "splittedCount: " << splittedCount << std::endl;
	if (SPLITTED_MODEL.getactions().size() <= MODELS.at(0).getactions().size())
	{
		throw Exception::FMTexc::FMTinvalid_action;
	}
	return 0;
}