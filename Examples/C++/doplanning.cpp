#include <vector>
#ifdef FMTWITHOSI
	#include "FMTlpmodel.hpp"
	#include "FMTmodelparser.hpp"
	#include "FMTversion.hpp"
	#include "FMTlogger.hpp"
	#include "FMTconstraint.hpp"
	#include "FMTfreeexceptionhandler.hpp"
	#include <boost/algorithm/string.hpp>
#endif


int main(int argc, char *argv[])
{
#ifdef FMTWITHOSI
	Logging::FMTlogger().logstamp();
	
	if (Version::FMTversion().hasfeature("OSI"))
	{
		const std::string vals = argv[1];
		std::vector<std::string>results;
		boost::split(results, vals,boost::is_any_of("|"));
		const std::string primarylocation = results.at(0);
		const std::string scenario = results.at(1);
		const int length = std::stoi(argv[2]);
		const double objectivevalue = std::stod(argv[3]);
		Parser::FMTmodelparser modelparser;
		std::vector<Exception::FMTexc>errors;
		errors.push_back(Exception::FMTexc::FMTmissingyield);
		errors.push_back(Exception::FMTexc::FMToutput_missing_operator);
		errors.push_back(Exception::FMTexc::FMToutput_too_much_operator);
		errors.push_back(Exception::FMTexc::FMTinvalidyield_number);
		errors.push_back(Exception::FMTexc::FMTundefinedoutput_attribute);
		errors.push_back(Exception::FMTexc::FMToveridedyield);
		modelparser.seterrorstowarnings(errors);
		const std::vector<std::string>scenarios(1,scenario);
		const std::vector<Models::FMTmodel> models = modelparser.readproject(primarylocation, scenarios);
		Models::FMTlpmodel optimizationmodel(models.at(0),Models::FMTsolverinterface::MOSEK);
		optimizationmodel.setparameter(Models::FMTintmodelparameters::LENGTH,length);
		optimizationmodel.setparameter(Models::FMTintmodelparameters::PRESOLVE_ITERATIONS,10);
		optimizationmodel.doplanning(true);
		if ((std::abs(optimizationmodel.getObjValue()-objectivevalue))>=0.1)
			{
			Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed, "Wrong value",
				"presolvetest", __LINE__, primarylocation);
			}
		
	}
	else {
		Logging::FMTlogger() << "FMT needs to be compiled with OSI" << "\n";
	}
#endif 
	return 0;
}
