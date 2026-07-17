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

// comparaison de output qui sort d'un doPlanning

int main(int argc, char *argv[])
	{
	#ifdef FMTWITHOSI
	Logging::FMTDefaultLogger().logStamp();
	if (Version::FMTVersion().hasFeature("OSI"))
		{
		Parser::FMTModelParser modelparser;
		modelparser.setDefaultExceptionHandler();
		std::vector<Exception::FMTexc>errors;
		errors.push_back(Exception::FMTexc::FMTmissingyield);
		errors.push_back(Exception::FMTexc::FMToutput_missing_operator);
		errors.push_back(Exception::FMTexc::FMToutput_too_much_operator);
		errors.push_back(Exception::FMTexc::FMTinvalidyield_number);
		errors.push_back(Exception::FMTexc::FMTundefinedoutput_attribute);
		errors.push_back(Exception::FMTexc::FMToveridedyield);
		errors.push_back(Exception::FMTexc::FMToutofrangeyield);
		errors.push_back(Exception::FMTexc::FMTdeathwithlock);
		modelparser.setErrorsToWarnings(errors);
		std::string primarylocation;
		std::string scenario;
		std::string outputname;
		int period = 2;
		double outputvalue;
		if (argc > 1)
		{
			const std::string vals = argv[1];
			std::vector<std::string>results;
			boost::split(results, vals, boost::is_any_of("|"));
			primarylocation = results.at(0);
			scenario = results.at(1);
			outputname = argv[2];
			outputvalue = std::stod(argv[3]);
		}
		else 
		{
			primarylocation = "C:/Users/Admlocal/Documents/issues/280/02_Travail/PC_9943_U08651_2028_MODB01.pri";
			scenario = "120_RegProv_apsp";
			outputname = "OSUPREGECOCOS";
			outputvalue = 230.364845;
			period = 14;
		}
		const std::vector<std::string>scenarios(1, scenario);
		const std::vector<Models::FMTModel> models = modelparser.readproject(primarylocation, scenarios);
		Models::FMTLpModel optimizationmodel(models.at(0), Models::FMTsolverinterface::CLP);
		const std::vector<Core::FMTSchedule>schedules = modelparser.readschedules(primarylocation,models).at(0);
		const double tolerance = 0.01;
		optimizationmodel.setParameter(Models::FMTboolmodelparameters::FORCE_PARTIAL_BUILD, true);
		optimizationmodel.FMTModel::setParameter(Models::FMTdblmodelparameters::TOLERANCE, tolerance);
		optimizationmodel.doPlanning(false, schedules);
		bool gotovoltotrec = false;
		for (const Core::FMTOutput& output : optimizationmodel.getOutputs())
			{
			if (output.getName() == outputname)
				{
				gotovoltotrec = true;
				const double returnedvalue = optimizationmodel.getOutput(output, period, Core::FMToutputlevel::totalonly).at("Total");
				std::cout << returnedvalue << "\n";
				if ((returnedvalue < (outputvalue - tolerance)) || (returnedvalue >(outputvalue + tolerance)))
					{
					Exception::FMTFreeExceptionHandler().raise(Exception::FMTexc::FMTfunctionfailed, "Wrong value",
						"FMTsetsolution", __LINE__, primarylocation);
					}
				break;
				}
			}
		if (!gotovoltotrec)
			{
			Exception::FMTFreeExceptionHandler().raise(Exception::FMTexc::FMTfunctionfailed, "No OVOLTOTREC OUPUT",
				"FMTsetsolution", __LINE__, primarylocation);
			}
	}else {
		Logging::FMTDefaultLogger() << "FMT needs to be compiled with OSI" << "\n";
		}
	#endif
	return 0;
	}

