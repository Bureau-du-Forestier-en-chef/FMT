#include <vector>
#include <string>
#include "FMTmodel.h"
#include "FMTmodelparser.h"
#include "TransformationCore.h"
#include <boost/filesystem.hpp>

int main(int argc, char* argv[])
{

	std::string primary_path;
	std::string actionName;
	std::string targetYield;
	std::string scenarioName;
	if (argc > 1)
	{
		std::vector<std::string> result;
		boost::split(result, argv[1], boost::is_any_of("|"));
		primary_path = result.at(0);
		scenarioName = result.at(1);
		actionName = argv[2];
		targetYield = argv[3];

	}
	else 
	{
		primary_path = "D:/01_Valide_ServiceOuest/01_Valide_ServiceOuest/PC_9949_U08251_2028_MODB01.pri";
		actionName = "ATBEMORT";
		targetYield = "YV_S_MORT";
		scenarioName = "1201_RegProv_avsp";

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
	const Models::FMTmodel BUILDED_MODEL = FMTWrapperCore::Transformation::buildAction(MODELS.at(0), actionName, targetYield, primary_path, output_scenario_name);

	// on fait des v�rifications sur le model construit
	if (MODELS.at(0) == BUILDED_MODEL)
	{
		throw Exception::FMTexc::FMTfunctionfailed;
	}
	// on v�rifie s'il y a bien �crit le mod�le
	const boost::filesystem::path SCENARIO_PATH = boost::filesystem::path(primary_path).parent_path() / boost::filesystem::path("Scenarios") / boost::filesystem::path(output_scenario_name);
	if (!boost::filesystem::is_directory(SCENARIO_PATH))
	{
		throw Exception::FMTexc::FMTinvalid_path;
	}

	return 0;
}

