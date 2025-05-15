#include <string>
#include <vector>
#include "FMTexception.h"
#include "FMTmodel.h"
#include "FMTmodelparser.h"
#include "TransformationCore.h"
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

int main(int argc, char* argv[])
{

	std::string primary_path;
	std::vector<std::string> splitted;
	std::vector<std::string> splittedMask;
	std::string scenarioName;
	if (argc < 2)
	{
		primary_path = "D:/01_Valide_ServiceOuest/01_Valide_ServiceOuest/PC_9949_U08251_2028_MODB01.pri";
		splitted = { "AEC", "ACT" };
		splittedMask = { "? ? ? ? ? ? ? ? ? ? ? ? ? UTA11 ? ? ? ? ? ?", "? ? ? ? ? ? ? ? ? ? ? ? ? !UTA11 ? ? ? ? ? ?" };
		scenarioName = "1201_RegProv_avsp";
		
	}
	else 
	{
		std::vector<std::string> result;
		boost::split(result, argv[1], boost::is_any_of("|"));
		primary_path = result.at(0);
		scenarioName = result.at(1);
		boost::split(splitted, argv[2], boost::is_any_of("|"));
		boost::split(splittedMask, argv[3], boost::is_any_of("|"));
	}
	std::string output_scenario_name = scenarioName + "_test";
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


	const std::vector<Models::FMTmodel> MODELS = ModelParser.readproject(primary_path, { scenarioName });
	const Models::FMTmodel SPLITTED_MODEL = FMTWrapperCore::Transformation::splitActions(MODELS.at(0), primary_path, splitted, splittedMask, output_scenario_name);
	
	// On vérifie si on a plus d'actions dans le nouveau model
	if (SPLITTED_MODEL.getactions().size() <= MODELS.at(0).getactions().size())
	{
		throw Exception::FMTexc::FMTinvalid_action;
	}
	// On vérifie si on a bien écrit le modèle
	const boost::filesystem::path SCENARIO_PATH = boost::filesystem::path(primary_path).parent_path() / boost::filesystem::path("Scenarios") / boost::filesystem::path(output_scenario_name);
	if (!boost::filesystem::is_directory(SCENARIO_PATH))
	{
		throw Exception::FMTexc::FMTfunctionfailed;
	}
	return 0;
}