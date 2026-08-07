#include <vector>
#ifdef FMTWITHOSI
#include "FMTLpModel.h"
#include "FMTModelParser.h"
#include "FMTScheduleParser.h"
#include "FMTVersion.h"
#include "FMTDefaultLogger.h"
#include "FMTConstraint.h"
#include "FMTFreeExceptionHandler.h"
#include <boost/algorithm/string.hpp>
#include "FMTAreaParser.h"
#endif


int main(int argc, char* argv[])
{
#ifdef FMTWITHOSI
	Logging::FMTDefaultLogger().logStamp();

	if (Version::FMTVersion().hasFeature("OSI"))
	{
		std::string primarylocation;
		std::string scenario;
		int length;
		double objectivevalue;
		if (argc>1)
		{
			const std::string vals = argv[1];
			std::vector<std::string>results;
			boost::split(results, vals, boost::is_any_of("|"));
			primarylocation = results.at(0);
			scenario = results.at(1);
			length = std::stoi(argv[2]);
			objectivevalue = std::stod(argv[3]);
		}
		else {
			primarylocation = "D://CC_modele_feu//CC_V3//Mod_cc_v3.pri";
			scenario = "stochastique_Histo_02";
			//primarylocation = "T:/Donnees/02_Courant/01_CPF/01_UA/_2028_2033/02571/22_Phase1_Corr_Volume/24_ModeleWS/02_Travail/2026-04-13/PC_10067_U02571_2028_Corr_volume/PC_10067_U02571_2028_MODB01.pri";
			//scenario = "160_Equi_apsp";
			length = 20; 
			objectivevalue = 14406932.651803;
		}
		Parser::FMTModelParser modelparser;
		//modelparser.setDebugExceptionHandler();
		std::vector<Exception::FMTexc>errors;
		errors.push_back(Exception::FMTexc::FMTmissingyield);
		errors.push_back(Exception::FMTexc::FMToutput_missing_operator);
		errors.push_back(Exception::FMTexc::FMToutput_too_much_operator);
		errors.push_back(Exception::FMTexc::FMTinvalidyield_number);
		errors.push_back(Exception::FMTexc::FMTundefinedoutput_attribute);
		errors.push_back(Exception::FMTexc::FMToveridedyield);
		errors.push_back(Exception::FMTexc::FMTsourcetotarget_transition);
		errors.push_back(Exception::FMTexc::FMTsame_transitiontargets);
		errors.push_back(Exception::FMTexc::FMTunclosedforloop);
		errors.push_back(Exception::FMTexc::FMToutofrangeyield);
		errors.push_back(Exception::FMTexc::FMTdeathwithlock);
		modelparser.setErrorsToWarnings(errors);
		const std::vector<std::string>scenarios(1, scenario);
		const std::vector<Models::FMTModel> models = modelparser.readproject(primarylocation, scenarios);
		#ifdef FMTWITHMOSEK
			Models::FMTLpModel optimizationmodel(models.at(0), Models::FMTSolverInterface::MOSEK);
		#else
			Models::FMTLpModel optimizationmodel(models.at(0), Models::FMTSolverInterface::CLP);
		#endif
		//modelparser.write(optimizationmodel, "D:/test/");
		optimizationmodel.setParameter(Models::FMTintmodelparameters::LENGTH, length);
		optimizationmodel.FMTModel::setParameter(Models::FMTboolmodelparameters::STRICTLY_POSITIVE,true);
		optimizationmodel.setParameter(Models::FMTintmodelparameters::PRESOLVE_ITERATIONS,10);
		optimizationmodel.setParameter(Models::FMTintmodelparameters::NUMBER_OF_THREADS, 1);
		optimizationmodel.setParameter(Models::FMTintmodelparameters::NUMBER_OF_ITERATIONS, 1);

		optimizationmodel.FMTModel::setParameter(Models::FMTboolmodelparameters::DEBUG_MATRIX,true);
		//modelparser.write(optimizationmodel, "D:/test/");
		if (optimizationmodel.doPlanning(true)) {
			
			std::cout << std::to_string(optimizationmodel.getObjValue()) << std::endl;
			//Parser::FMTAreaParser arepars;
			//arepars.write(optimizationmodel.getArea(1), "D:/test/test.are");
			
			std::vector<Core::FMTOutput>outputs;
			/*
			for (const Core::FMTOutput& output : optimizationmodel.getoutputs())
			{
				if (output.getName() == "OVOLTOTREC")
				{
					for (int period = 1; period < 10; ++period)
					{
					const std::map<std::string, double>outs = optimizationmodel.getOutput(output, period, Core::FMToutputlevel::totalonly);
					for (const auto& val : outs)
						{
						std::cout << val.first << " " << val.second << "\n";
	
						}
					}
				}
			}
			*/

			/*for (const auto& solution : optimizationmodel.getSolution(1))
			{
				for (const auto& val : solution.second)
				{
					std::cout << solution.first.getName() << " " << std::string(val.first) << "\n";
				}
				
			}*/
			/*for (const Core::FMTOutput& output : optimizationmodel.getoutputs())
			{
				if (output.getName()=="OYOUVERT")
				{
					std::cout << output.getName() << " ";
					std::map<std::string, std::vector<double>>alls;
					for (int period =1 ; period <=5; ++period)
						{
						const std::map<std::string, double>outs = optimizationmodel.getOutput(output, period, Core::FMToutputlevel::standard);
						for (const auto& val : outs)
						{
							if (alls.find(val.first)== alls.end())
							{
								alls[val.first] = std::vector<double>();
							}
							alls[val.first].push_back(val.second);
							
						}
						}
					for (const auto& val : alls)
					{
						std::cout << val.first << " ";
						for (const auto& out : val.second) 
						{
							if (out > 0)
							{
								std::cout << 1 << " ";
							}
							else {
								std::cout << 0 << " ";
							}
							
						}
						std::cout << "\n";
					}
					//std::cout << "\n";
					outputs.push_back(output);
				}

			}*/
			//modelparser.writeResults(optimizationmodel, outputs, 10, 10, "D:/test/out", Core::FMToutputlevel::totalonly);
			//optimizationmodel.writeLp("D:/test/"+ scenario+".lp");
		}
		
		
		/*Parser::FMTScheduleParser schparser;
		std::vector<Core::FMTSchedule>returnschedule;
		for (int id = 0; id < length+1;++id)
		{
			returnschedule.push_back(optimizationmodel.getSolution(id+1, true));
		}
		schparser.write(returnschedule, "../../tests/doPlanning/schedule.seq");*/
		std::cout << " Objective: " << int(optimizationmodel.getObjValue())<<"\n";
		if ((std::abs(optimizationmodel.getObjValue() - objectivevalue)) >= 1)
		{
			Exception::FMTFreeExceptionHandler().raise(Exception::FMTexc::FMTfunctionfailed, "Wrong value",
				"presolvetest", __LINE__, primarylocation);
		}
	}
	else {
		Logging::FMTDefaultLogger() << "FMT needs to be compiled with OSI" << "\n";
	}
#endif 
	return 0;
}

