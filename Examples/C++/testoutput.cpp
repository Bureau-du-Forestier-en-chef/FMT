#ifdef FMTWITHOSI
	#include <vector>
	#include "FMTlpmodel.h"
	#include "FMTmodelparser.h"
	#include "FMTversion.h"
	#include "FMTdefaultlogger.h"
	#include "FMTscheduleparser.h"
	#include "FMTschedule.h"
	#include "FMToutputnode.h"
	#include "FMTfreeexceptionhandler.h"
#endif

int main(int argc, char *argv[])
	{
	#ifdef FMTWITHOSI
	Logging::FMTdefaultlogger().logstamp();
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
		const std::string scenario =  results.at(1);
		const std::string outputname =  argv[2];
		const double outputvalue = std::stod(argv[3]);
		const std::vector<std::string>scenarios(1, scenario);
		const std::vector<Models::FMTmodel> models = modelparser.readproject(primarylocation, scenarios);
		Models::FMTlpmodel optimizationmodel(models.at(0), Models::FMTsolverinterface::CLP);
		const std::vector<Core::FMTschedule>schedules = modelparser.readschedules(primarylocation,models).at(0);
		const double tolerance = 0.01;
		optimizationmodel.setparameter(Models::FMTboolmodelparameters::FORCE_PARTIAL_BUILD, true);
		optimizationmodel.FMTmodel::setparameter(Models::FMTdblmodelparameters::TOLERANCE, tolerance);
		optimizationmodel.doplanning(false, schedules);
		bool gotovoltotrec = false;
		for (const Core::FMToutput& output : optimizationmodel.getoutputs())
			{
			if (output.getname() == outputname)
				{
				gotovoltotrec = true;
				const double returnedvalue = optimizationmodel.getoutput(output, 2, Core::FMToutputlevel::totalonly).at("Total");
				std::cout << returnedvalue << "\n";
				if ((returnedvalue < (outputvalue - tolerance)) || (returnedvalue >(outputvalue + tolerance)))
					{
					Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed, "Wrong value",
						"FMTsetsolution", __LINE__, primarylocation);
					}
				break;
				}
			}
		if (!gotovoltotrec)
			{
			Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed, "No OVOLTOTREC OUPUT",
				"FMTsetsolution", __LINE__, primarylocation);
			}
	}else {
		Logging::FMTdefaultlogger() << "FMT needs to be compiled with OSI" << "\n";
		}
	#endif
	return 0;
	}

