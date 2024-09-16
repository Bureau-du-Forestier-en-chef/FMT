#include <vector>
#include <string>
#include "FMTmodel.h"
#include "FMTmodelparser.h"
#include "Tools.h"

int main(int argc, char* argv[])
{
	std::string pathPri;
	std::string scenarioName;
	if (argc > 1)
	{
		pathPri = argv[1];
		scenarioName = argv[2];
	}
	else
	{
		pathPri = "../../../../Examples/Models/TWD_land/TWD_land.pri";
		scenarioName = "equation";
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
	const double maxAge = FMTWrapperCore::Tools::getMaxAge(MODELS.at(0));

	// on fait des vérifications sur le nombre renvoyer
	if (maxAge < 1)
	{
		throw "Error: The age is empty or negative";
	}
	if (maxAge > 254) {
		throw "Error: The maxAge is too high";
	}


	return 0;
}

