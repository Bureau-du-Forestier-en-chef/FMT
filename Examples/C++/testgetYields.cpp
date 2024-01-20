#ifdef FMTWITHOSI
	#include "FMTlpmodel.hpp"
	#include "FMTmodelparser.hpp"
	#include "FMTscheduleparser.hpp"
	#include "FMTversion.hpp"
	#include "FMTexception.hpp"
	#include "FMTdevelopment.hpp"
	#include "FMTyieldrequest.hpp"
	#include "FMTgraphvertextoyield.hpp"
	#include "FMTfreeexceptionhandler.hpp"
	#include <vector>
#endif

int main(int argc, char *argv[])
	{
	#ifdef FMTWITHOSI
	Logging::FMTlogger().logstamp();
	if (Version::FMTversion().hasfeature("OSI"))
		{
		Parser::FMTmodelparser modelparser;
		modelparser.setdefaultexceptionhandler();
		std::vector<Exception::FMTexc>errors;
		errors.push_back(Exception::FMTexc::FMTmissingyield);
		errors.push_back(Exception::FMTexc::FMToutput_missing_operator);
		errors.push_back(Exception::FMTexc::FMToutput_too_much_operator);
		errors.push_back(Exception::FMTexc::FMTinvalidyield_number);
		errors.push_back(Exception::FMTexc::FMTundefinedoutput_attribute);
		errors.push_back(Exception::FMTexc::FMToveridedyield);
		errors.push_back(Exception::FMTexc::FMToutofrangeyield);
		modelparser.seterrorstowarnings(errors);
		const std::string vals = argv[1];
		std::vector<std::string>results;
		boost::split(results, vals, boost::is_any_of("|"));
		const std::string primarylocation =  results.at(0);
		const std::string scenario = results.at(1);
		const std::string maskstr = results.at(2);
		const int age = std::stoi(results.at(3));
		const int period = std::stoi(results.at(4));
		const std::string yieldname = std::string(argv[2]);
		const double yieldvalue = std::stod(argv[3]);
		const std::vector<std::string>scenarios(1, scenario);
		const std::vector<Models::FMTmodel> models = modelparser.readproject(primarylocation, scenarios);
		Models::FMTlpmodel optimizationmodel(models.at(0), Models::FMTsolverinterface::MOSEK);
		const std::vector<Core::FMTschedule>schedules = modelparser.readschedules(primarylocation,models).at(0);
		
		const double tolerance = 0.01;
		optimizationmodel.setparameter(Models::FMTboolmodelparameters::FORCE_PARTIAL_BUILD, true);
		optimizationmodel.FMTmodel::setparameter(Models::FMTdblmodelparameters::TOLERANCE, tolerance);
		optimizationmodel.FMTmodel::setparameter(Models::FMTintmodelparameters::LENGTH, schedules.back().getperiod());
		
		optimizationmodel.doplanning(false, schedules);
		bool gotovoltotrec = false;
		const Core::FMTdevelopment adev(Core::FMTmask(maskstr,optimizationmodel.getthemes()), age, 0, period);
		const Graph::FMTgraphvertextoyield graph_info = optimizationmodel.getGraphVertexToYield();
		const Core::FMTyieldrequest yieldrequest = adev.getyieldrequest(&graph_info);
		const double returnedvalue = optimizationmodel.getyields().get(yieldrequest,yieldname);
		Logging::FMTlogger() << returnedvalue << "\n";
		if ((returnedvalue < (yieldvalue - tolerance)) || (returnedvalue > (yieldvalue + tolerance)))
			{
			Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed, "Wrong value",
				"FMTsetsolution", __LINE__, primarylocation);
			}
	}else {
		Logging::FMTlogger() << "FMT needs to be compiled with OSI" << "\n";
		}
	#endif
	return 0;
	}

