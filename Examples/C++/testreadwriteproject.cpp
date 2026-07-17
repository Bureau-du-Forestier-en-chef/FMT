#include "FMTModelParser.h"
#include "FMTFreeExceptionHandler.h"
#ifdef FMTWITHOSI
	#include "FMTLpModel.h"
#endif
#include <boost/algorithm/string.hpp>

int main(int argc, char* argv[])
{
#ifdef FMTWITHOSI
	std::string primarylocation;
	std::vector<std::string>scenarios;
	if (argc>1)
	{
		primarylocation = std::string(argv[1]);
		const std::string scenariostr = std::string(argv[2]);
		//const std::string scenariostr = std::string("operator_separator");// changer ici pour avoir le nom souhaité du scénario
		boost::split(scenarios, scenariostr, boost::is_any_of("|"));
	}
	else {
		scenarios.push_back("operator_separator");
		primarylocation = "../../../../Examples/Models/TWD_land/TWD_land.pri";
		//scenarios.push_back("120_RegProv_apsp");
		//primarylocation = "C:/Users/Admlocal/Documents/issues/290/01_Valide/PC_9967_U06271_2028_MODB01.pri";
	}
	//std::string primarylocation = std::string("../../../../Examples/Models/TWD_land/TWD_land.pri"); // changer ici pour pointer sur un modèle avec le bug
	Parser::FMTModelParser modelparser;
	modelparser.setDefaultExceptionHandler();
	const std::string outdir = "../../tests/testreadwriteproject/";
	//const std::string outdir = "D:/FMT/build/release/tests/testreadwriteproject/";
	
	std::vector<Exception::FMTexc> errors;
	errors.push_back(Exception::FMTexc::FMTmissingyield);
	//errors.push_back(Exception::FMTexc::FMToutput_too_much_operator); // tester en relecture
	//errors.push_back(Exception::FMTexc::FMTinvalidyield_number); // 
	errors.push_back(Exception::FMTexc::FMToveridedyield);
	errors.push_back(Exception::FMTexc::FMTdeathwithlock);
	//modelparser.setErrorsToWarnings(errors);
	
	std::vector<Exception::FMTexc> readErrors(errors);
	readErrors.push_back(Exception::FMTexc::FMToutput_too_much_operator);
	readErrors.push_back(Exception::FMTexc::FMTinvalidyield_number);
	modelparser.setErrorsToWarnings(readErrors);

	
	
	const std::vector<Models::FMTModel> READ_MODELS = modelparser.readproject(primarylocation, scenarios);
	for (const Models::FMTModel& model: READ_MODELS)
	{
		Models::FMTLpModel lpmodel(model,Models::FMTsolverinterface::CLP);
		lpmodel.doPlanning(true);
		modelparser.writeToProject(outdir + "test.pri", lpmodel);
	}
	scenarios.erase(scenarios.begin());
	modelparser.setErrorsToWarnings(errors);
	scenarios.insert(scenarios.begin(), "ROOT");
	const std::vector<Models::FMTModel> newmodels = modelparser.readproject(outdir + "test.pri", scenarios);
	
#endif
	return 0;
}