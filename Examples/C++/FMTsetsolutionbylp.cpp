#ifdef FMTWITHOSI
	#include <vector>
	#include "FMTLpModel.h"
	#include "FMTModelParser.h"
	#include "FMTVersion.h"
	#include "FMTDefaultLogger.h"
	#include "FMTScheduleParser.h"
	#include "FMTSchedule.h"
	#include "FMTOutputNode.h"
	#include "FMTFreeExceptionHandler.h"
#endif

int main(int argc, char *argv[])
	{
	#ifdef FMTWITHOSI
	Logging::FMTDefaultLogger().logStamp();
	if (Version::FMTVersion().hasFeature("OSI"))
		{
		std::string primarylocation;
		std::vector<std::string>scenarios;
		double ovoltotrecvalue;
		if (argc>1)
		{
			primarylocation = std::string(argv[1]);
			scenarios = std::vector<std::string>(1, std::string(argv[2]));
			if (argc > 3)
				{
				ovoltotrecvalue = std::stod(argv[3]);
				}
		}else {
			primarylocation = "T:/Donnees/Usagers/CYRGU3/WS_08751_REG - Copie/Feux_2023_ouest_V01.pri";
			scenarios = std::vector<std::string>(1, "2001_StratReg_2028_apFeu_avsp");
			ovoltotrecvalue = 2085200.026;
			argc = 4;
		}

		Parser::FMTModelParser modelparser;
		modelparser.setDefaultExceptionHandler();
		std::vector<Exception::FMTexc>errors;
		errors.push_back(Exception::FMTexc::FMTmissingyield);
		errors.push_back(Exception::FMTexc::FMToutput_missing_operator);
		errors.push_back(Exception::FMTexc::FMToutput_too_much_operator);
		errors.push_back(Exception::FMTexc::FMTinvalidyield_number);
		errors.push_back(Exception::FMTexc::FMTundefinedoutput_attribute);
		errors.push_back(Exception::FMTexc::FMToveridedyield);
        errors.push_back(Exception::FMTexc::FMTdeathwithlock);
		modelparser.setErrorsToWarnings(errors);
		const std::vector<Models::FMTModel> models = modelparser.readproject(primarylocation, scenarios);
		Models::FMTLpModel optimizationmodel(models.at(0), Models::FMTsolverinterface::CLP);
		const std::vector<Core::FMTSchedule>schedules = modelparser.readschedules(primarylocation,models).at(0);
		const double tolerance = 0.0001;
		for (size_t period = 1; period <= 6; ++period)
			{
			optimizationmodel.buildPeriod(schedules.at(period-1),true);
			}
		for (size_t period = 1; period <= 6; ++period)
			{
			optimizationmodel.setSolutionByLp(period,schedules.at(period-1), tolerance);
			}
		if (argc>3)//Got the double for validation!
			{
			bool gotovoltotrec = false;
			for (const Core::FMTOutput& output : optimizationmodel.getOutputs())
				{
				if (output.getName()=="OVOLTOTREC")
					{
					gotovoltotrec = true;
					const double returnedvalue = optimizationmodel.getOutput(output, 2, Core::FMToutputlevel::totalonly).at("Total");
                    Logging::FMTDefaultLogger() << "returned value : "+std::to_string(returnedvalue)+ "\nValue : "+std::to_string(ovoltotrecvalue) << "\n";
					if ((returnedvalue < (ovoltotrecvalue - 1))||(returnedvalue > (ovoltotrecvalue + 1)))
						{
						Exception::FMTFreeExceptionHandler().raise(Exception::FMTexc::FMTfunctionfailed, "Wrong value",
							"FMTsetsolutionbylp", __LINE__, primarylocation);
						}
					break;
					}
				}
			if (!gotovoltotrec)
				{
				Exception::FMTFreeExceptionHandler().raise(Exception::FMTexc::FMTfunctionfailed, "No OVOLTOTREC OUPUT",
					"FMTsetsolution", __LINE__, primarylocation);
				}
			}

	}else {
		Logging::FMTDefaultLogger() << "FMT needs to be compiled with OSI" << "\n";
		}
	#endif
	return 0;
	}