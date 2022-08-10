#include <vector>
#ifdef FMTWITHOSI
#include "FMTlpmodel.hpp"
#include "FMTmodelparser.hpp"
#include "FMTscheduleparser.hpp"
#include "FMTversion.hpp"
#include "FMTlogger.hpp"
#include "FMTconstraint.hpp"
#include "FMTfreeexceptionhandler.hpp"
#include <boost/algorithm/string.hpp>
#endif


int main(int argc, char* argv[])
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
		const int length =  std::stoi(argv[2]);
		const double objectivevalue = std::stod(argv[3]);
		Parser::FMTmodelparser modelparser;
		std::vector<Exception::FMTexc>errors;
		errors.push_back(Exception::FMTexc::FMTmissingyield);
		errors.push_back(Exception::FMTexc::FMToutput_missing_operator);
		errors.push_back(Exception::FMTexc::FMToutput_too_much_operator);
		errors.push_back(Exception::FMTexc::FMTinvalidyield_number);
		errors.push_back(Exception::FMTexc::FMTundefinedoutput_attribute);
		errors.push_back(Exception::FMTexc::FMToveridedyield);
		errors.push_back(Exception::FMTexc::FMTsourcetotarget_transition);
		errors.push_back(Exception::FMTexc::FMTsame_transitiontargets);
		modelparser.seterrorstowarnings(errors);
		const std::vector<std::string>scenarios(1, scenario);
		const std::vector<Models::FMTmodel> models = modelparser.readproject(primarylocation, scenarios);
		Models::FMTlpmodel optimizationmodel(models.at(0), Models::FMTsolverinterface::MOSEK);
		optimizationmodel.setparameter(Models::FMTintmodelparameters::LENGTH, length);
		optimizationmodel.FMTmodel::setparameter(Models::FMTboolmodelparameters::STRICTLY_POSITIVE, true);
		optimizationmodel.setparameter(Models::FMTintmodelparameters::PRESOLVE_ITERATIONS, 10);
		//modelparser.write(optimizationmodel, "D:/testb/");
		if (optimizationmodel.doplanning(true)) {
			std::cout << std::to_string(optimizationmodel.getObjValue()) << std::endl;
		}
		/*optimizationmodel.writeLP("D:/testb/test.lp");
		Parser::FMTscheduleparser schparser;
		std::vector<Core::FMTschedule>returnschedule;
		returnschedule.push_back(optimizationmodel.getsolution(1, true));
		returnschedule.push_back( optimizationmodel.getsolution(2, true));
		schparser.write(returnschedule, "D:/testb/schedule.seq");*/
		if ((std::abs(optimizationmodel.getObjValue() - objectivevalue)) >= 1)
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

