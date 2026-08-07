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
			
		}else{
			primarylocation = "T:/Donnees/02_Courant/07_Outil_moyen_methode/01_Entretien_developpement/Interne/FMT/Entretien/Modeles_test/WS_08751_REG/Feux_2023_ouest_V01.pri";
			scenarios = std::vector<std::string>(1, "2001_StratReg_2028_apFeu_avsp");
			ovoltotrecvalue = 2252907.601621;
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
		errors.push_back(Exception::FMTexc::FMToutofrangeyield);
		errors.push_back(Exception::FMTexc::FMTdeathwithlock);
		errors.push_back(Exception::FMTexc::FMTsame_transitiontargets);
		modelparser.setErrorsToWarnings(errors);
		const std::vector<Models::FMTModel> models = modelparser.readproject(primarylocation, scenarios);
		//Models::FMTLpModel optimizationmodel(models.at(0), Models::FMTSolverInterface::CLP);
		Models::FMTLpModel optimizationmodel(models.at(0), Models::FMTSolverInterface::MOSEK);
		const std::vector<Core::FMTSchedule>schedules = modelparser.readSchedules(primarylocation,models).at(0);
		const double tolerance = 0.01;
		//optimizationmodel.setParameter(Models::FMTintmodelparameters::PRESOLVE_ITERATIONS, 0);
		optimizationmodel.setParameter(Models::FMTboolmodelparameters::FORCE_PARTIAL_BUILD, true);
		optimizationmodel.FMTModel::setParameter(Models::FMTdblmodelparameters::TOLERANCE, tolerance);
		//modelparser.write(*optimizationmodel.preSolve(), "D:/test/");
		optimizationmodel.doPlanning(false, schedules);
		/*for (size_t period = 1; period <= 6; ++period)
			{
			optimizationmodel.buildperiod(schedules.at(period - 1));
			}
		for (size_t period = 1; period <= 6; ++period)
			{
			optimizationmodel.setSolution(period,schedules.at(period-1), tolerance);
			}*/
		if (argc>3)//Got the double for validation!
			{
			bool gotovoltotrec = false;
			std::vector<Core::FMTOutput>selected;
			for (const Core::FMTOutput& output : optimizationmodel.getOutputs())
				{
				if (output.getName() == "OVOLTOTREC")
					{
					gotovoltotrec = true;
					const double returnedvalue = optimizationmodel.getOutput(output, 2, Core::FMToutputlevel::totalonly).at("Total");
					std::cout << "OVOLTOTREC " +std::to_string(returnedvalue) << "\n";
					if ((returnedvalue < (ovoltotrecvalue - tolerance)) || (returnedvalue >(ovoltotrecvalue + tolerance)))
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
			//modelparser.writeResults(optimizationmodel, selected, 1, 10, "D:/test/out", Core::FMToutputlevel::totalonly);
			}
			//
	}else {
		Logging::FMTDefaultLogger() << "FMT needs to be compiled with OSI" << "\n";
		}
	#endif
	return 0;
	}

