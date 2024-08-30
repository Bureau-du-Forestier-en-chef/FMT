#include <vector>
#include <string>
#include "FMTmodel.h"
#include "FMTmodelparser.h"
#include "TransformationCore.h"


int main(int argc, char* argv[])
{
	Parser::FMTmodelparser ModelParser;
	const std::string PATHPRI = "D:/Inputs_Prototype/Prototype_Dec2023_MethodeTBE/PC_7002071_UA08152_FINAL.pri";
	const std::string ACTION_NAME = "A_TBE";
	const std::string TARGET_YIELD = "YV_S_MORT";
	const std::string OUTPUT_DIRECTORY = "D:/Test_sortie_FMT";
	const std::string SCENARIO_NAME = "30_ReglProv_avsp";
	const std::string NEW_SCENARIO_NAME = "Output_scenario_name";


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


	const std::vector<Models::FMTmodel> MODELS = ModelParser.readproject(PATHPRI, { SCENARIO_NAME });
	const Models::FMTmodel BUILDED_MODEL = FMTWrapperCore::Transformation::buildAction(MODELS.at(0), ACTION_NAME, TARGET_YIELD, PATHPRI, OUTPUT_DIRECTORY, NEW_SCENARIO_NAME);
	return 0;
}

