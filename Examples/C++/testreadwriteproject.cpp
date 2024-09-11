#include "FMTmodelparser.h"
#include "FMTfreeexceptionhandler.h"
#ifdef FMTWITHOSI
	#include "FMTlpmodel.h"
#endif
#include <boost/algorithm/string.hpp>

int main(int argc, char* argv[])
{
#ifdef FMTWITHOSI
	//const std::string primarylocation = std::string(argv[1]); 
	std::string primarylocation = std::string("../../../../Examples/Models/TWD_land/TWD_land.pri"); // changer ici pour pointer sur un modèle avec le bug
	Parser::FMTmodelparser modelparser;
	modelparser.setdefaultexceptionhandler();
	const std::string outdir = "../../tests/testreadwriteproject/";
	//const std::string outdir = "D:/FMT/build/release/tests/testreadwriteproject/";
	
	std::vector<Exception::FMTexc> errors;
	errors.push_back(Exception::FMTexc::FMTmissingyield);
	//errors.push_back(Exception::FMTexc::FMToutput_too_much_operator); // tester en relecture
	//errors.push_back(Exception::FMTexc::FMTinvalidyield_number); // 
	errors.push_back(Exception::FMTexc::FMToveridedyield);
	//modelparser.seterrorstowarnings(errors);
	
	std::vector<Exception::FMTexc> readErrors(errors);
	readErrors.push_back(Exception::FMTexc::FMToutput_too_much_operator);
	readErrors.push_back(Exception::FMTexc::FMTinvalidyield_number);
	modelparser.seterrorstowarnings(readErrors);

	std::vector<std::string>scenarios;
	//const std::string scenariostr =  std::string(argv[2]); 
	const std::string scenariostr = std::string("operator_separator");// changer ici pour avoir le nom souhaité du scénario
	boost::split(scenarios, scenariostr,boost::is_any_of("|"));
	const std::vector<Models::FMTmodel> READ_MODELS = modelparser.readproject(primarylocation, scenarios);
	for (const Models::FMTmodel& model: READ_MODELS)
	{
		Models::FMTlpmodel lpmodel(model,Models::FMTsolverinterface::CLP);
		lpmodel.doplanning(true);
		modelparser.writetoproject(outdir + "test.pri", lpmodel);
	}
	scenarios.erase(scenarios.begin());
	modelparser.seterrorstowarnings(errors);
	scenarios.insert(scenarios.begin(), "ROOT");
	const std::vector<Models::FMTmodel> newmodels = modelparser.readproject(outdir + "test.pri", scenarios);
	
#endif
	return 0;
}