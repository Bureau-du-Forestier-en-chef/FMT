#include "FMTmodelparser.h"
#include "FMTfreeexceptionhandler.h"
#ifdef FMTWITHOSI
	#include "FMTlpmodel.h"
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
		//const std::string scenariostr = std::string("operator_separator");// changer ici pour avoir le nom souhait� du sc�nario
		boost::split(scenarios, scenariostr, boost::is_any_of("|"));
	}
	else {
		scenarios.push_back("operator_separator");
		primarylocation = "../../../../Examples/Models/TWD_land/TWD_land.pri";
		//scenarios.push_back("120_RegProv_apsp");
		//primarylocation = "C:/Users/Admlocal/Documents/issues/290/01_Valide/PC_9967_U06271_2028_MODB01.pri";
	}
	//std::string primarylocation = std::string("../../../../Examples/Models/TWD_land/TWD_land.pri"); // changer ici pour pointer sur un mod�le avec le bug
	Parser::FMTmodelparser modelparser;
	modelparser.setdefaultexceptionhandler();
	const std::string outdir = "../../tests/testreadwriteproject/";
	//const std::string outdir = "D:/FMT/build/release/tests/testreadwriteproject/";
	
	std::vector<Exception::FMTexc> errors;
	errors.push_back(Exception::FMTexc::FMTmissingyield);
	//errors.push_back(Exception::FMTexc::FMToutput_too_much_operator); // tester en relecture
	//errors.push_back(Exception::FMTexc::FMTinvalidyield_number); // 
	errors.push_back(Exception::FMTexc::FMToveridedyield);
	errors.push_back(Exception::FMTexc::FMTdeathwithlock);
	//modelparser.seterrorstowarnings(errors);
	
	std::vector<Exception::FMTexc> readErrors(errors);
	readErrors.push_back(Exception::FMTexc::FMToutput_too_much_operator);
	readErrors.push_back(Exception::FMTexc::FMTinvalidyield_number);
	modelparser.seterrorstowarnings(readErrors);

	
	
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