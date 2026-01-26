#include <vector>
#ifdef FMTWITHOSI
#include "FMTlpmodel.h"
#include "FMTmodelparser.h"
#include "FMTscheduleparser.h"
#include "FMTversion.h"
#include "FMTdefaultlogger.h"
#include "FMTconstraint.h"
#include "FMTfreeexceptionhandler.h"
#include <boost/algorithm/string.hpp>
#include "FMTareaparser.h"
#endif


int main(int argc, char* argv[])
{
#ifdef FMTWITHOSI
	Logging::FMTdefaultlogger().logstamp();

	if (Version::FMTversion().hasfeature("OSI"))
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
			primarylocation = "C:\\Users\\Admlocal\\Documents\\issues\\FMT_305\\CC_V2\\Mod_cc_v2.pri";
			scenario = "strategique_CC_modified";
			length = 5; 
			objectivevalue = -1026547388;
		}
		Parser::FMTmodelparser modelparser;
		//modelparser.setdebugexceptionhandler();
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
		modelparser.seterrorstowarnings(errors);
		const std::vector<std::string>scenarios(1, scenario);
		const std::vector<Models::FMTmodel> models = modelparser.readproject(primarylocation, scenarios);
		#ifdef FMTWITHMOSEK
			Models::FMTlpmodel optimizationmodel(models.at(0), Models::FMTsolverinterface::MOSEK);
		#else
			Models::FMTlpmodel optimizationmodel(models.at(0), Models::FMTsolverinterface::CLP);
		#endif
		//modelparser.write(optimizationmodel, "D:/test/");
		optimizationmodel.setparameter(Models::FMTintmodelparameters::LENGTH, length);
		optimizationmodel.FMTmodel::setparameter(Models::FMTboolmodelparameters::STRICTLY_POSITIVE,true);
		optimizationmodel.setparameter(Models::FMTintmodelparameters::PRESOLVE_ITERATIONS,10);
		optimizationmodel.setparameter(Models::FMTintmodelparameters::NUMBER_OF_THREADS, 1);
		optimizationmodel.setparameter(Models::FMTintmodelparameters::NUMBER_OF_ITERATIONS, 1);

		optimizationmodel.FMTmodel::setparameter(Models::FMTboolmodelparameters::DEBUG_MATRIX,true);
		//modelparser.write(optimizationmodel, "D:/test/");
		if (optimizationmodel.doplanning(true)) {
			
			std::cout << std::to_string(optimizationmodel.getObjValue()) << std::endl;
			//Parser::FMTareaparser arepars;
			//arepars.write(optimizationmodel.getarea(1), "D:/test/test.are");
			
			std::vector<Core::FMToutput>outputs;
			/*
			for (const Core::FMToutput& output : optimizationmodel.getoutputs())
			{
				if (output.getname() == "OVOLTOTREC")
				{
					for (int period = 1; period < 10; ++period)
					{
					const std::map<std::string, double>outs = optimizationmodel.getoutput(output, period, Core::FMToutputlevel::totalonly);
					for (const auto& val : outs)
						{
						std::cout << val.first << " " << val.second << "\n";
	
						}
					}
				}
			}
			*/

			/*for (const auto& solution : optimizationmodel.getsolution(1))
			{
				for (const auto& val : solution.second)
				{
					std::cout << solution.first.getname() << " " << std::string(val.first) << "\n";
				}
				
			}*/
			/*for (const Core::FMToutput& output : optimizationmodel.getoutputs())
			{
				if (output.getname()=="OYOUVERT")
				{
					std::cout << output.getname() << " ";
					std::map<std::string, std::vector<double>>alls;
					for (int period =1 ; period <=5; ++period)
						{
						const std::map<std::string, double>outs = optimizationmodel.getoutput(output, period, Core::FMToutputlevel::standard);
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
			//modelparser.writeresults(optimizationmodel, outputs, 10, 10, "D:/test/out", Core::FMToutputlevel::totalonly);
			//optimizationmodel.writeLP("D:/test/"+ scenario+".lp");
		}
		
		
		/*Parser::FMTscheduleparser schparser;
		std::vector<Core::FMTschedule>returnschedule;
		for (int id = 0; id < length+1;++id)
		{
			returnschedule.push_back(optimizationmodel.getsolution(id+1, false));
		}
		schparser.write(returnschedule, "../../tests/doplanning/schedule.seq");*/
		std::cout << " Objective: " << int(optimizationmodel.getObjValue())<<"\n";
		if ((std::abs(optimizationmodel.getObjValue() - objectivevalue)) >= 1)
		{
			Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed, "Wrong value",
				"presolvetest", __LINE__, primarylocation);
		}
	}
	else {
		Logging::FMTdefaultlogger() << "FMT needs to be compiled with OSI" << "\n";
	}
#endif 
	return 0;
}

