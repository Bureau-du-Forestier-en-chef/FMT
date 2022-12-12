#include "FMTmodelparser.hpp"
#include "FMTfreeexceptionhandler.hpp"
#ifdef FMTWITHOSI
	#include "FMTlpmodel.hpp"
#endif
#include <boost/algorithm/string.hpp>

int main(int argc, char* argv[])
{
#ifdef FMTWITHOSI
	const std::string primarylocation = std::string(argv[1]);
	Parser::FMTmodelparser modelparser;
	modelparser.setdefaultexceptionhandler();
	const std::string outdir = "../../tests/testreadwriteproject/";
	std::vector<Exception::FMTexc>errors;
	errors.push_back(Exception::FMTexc::FMTmissingyield);
	errors.push_back(Exception::FMTexc::FMToutput_too_much_operator);
	errors.push_back(Exception::FMTexc::FMTinvalidyield_number);
	errors.push_back(Exception::FMTexc::FMToveridedyield);
	modelparser.seterrorstowarnings(errors);
	std::vector<std::string>scenarios;
	const std::string scenariostr = std::string(argv[2]);
	boost::split(scenarios, scenariostr,boost::is_any_of("|"));
	for (const Models::FMTmodel& model : modelparser.readproject(primarylocation, scenarios))
	{
		Models::FMTlpmodel lpmodel(model,Models::FMTsolverinterface::CLP);
		lpmodel.doplanning(true);
		modelparser.writetoproject(outdir + "test.pri", lpmodel);
	}
	scenarios.erase(scenarios.begin());
	scenarios.insert(scenarios.begin(), "ROOT");
	const std::vector<Models::FMTmodel> newmodels = modelparser.readproject(outdir + "test.pri", scenarios);
	
#endif
	return 0;
}