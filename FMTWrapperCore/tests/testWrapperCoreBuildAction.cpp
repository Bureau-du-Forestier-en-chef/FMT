#include <vector>
#include <string>
#include "FMTmodel.h"
#include "FMTmodelparser.h"
#include "TransformationCore.h"


int main(int argc, char* argv[])
{
	const std::string OUTPUT_DIRECTORY = "../../tests/testWrapperCoreBuildAction";
	const std::string NEW_SCENARIO_NAME = "Test_Output_scenario_name";
	std::string pathPri;
	std::string actionName;
	std::string targetYield;
	std::string scenarioName;
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
		pathPri = "D:/Inputs_Prototype/Prototype_Dec2023_MethodeTBE/PC_7002071_UA08152_FINAL.pri";
		actionName = "A_TBE";
		targetYield = "YV_S_MORT";
		scenarioName = "30_ReglProv_avsp";
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
	const Models::FMTmodel BUILDED_MODEL = FMTWrapperCore::Transformation::buildAction(MODELS.at(0), actionName, targetYield, pathPri, OUTPUT_DIRECTORY, NEW_SCENARIO_NAME);
	return 0;
}

