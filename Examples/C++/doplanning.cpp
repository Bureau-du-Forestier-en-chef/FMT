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
		boost::split(results, vals, boost::is_any_of("|"));
		const std::string primarylocation = results.at(0);
		const std::string scenario = results.at(1);
		//const std::string primarylocation = "T:/Donnees/02_Courant/07_Outil_moyen_methode/01_Entretien_developpement/09_FMT/Modeles_test/actionseries/PC_9509_U02751_4_Vg2_2023_vRp2.pri";// results.at(0);
		//const std::string scenario = "14_sc5_determin_apsp_ref_aam_ratioplct_action_series_3";// results.at(1);																				  //const int length = 1;// std::stoi(argv[2]);
		//const int length = 10;// std::stoi(argv[2]);
		//const double objectivevalue = 0;// std::stod(argv[3]);
		const int length =  std::stoi(argv[2]);
		const double objectivevalue =  std::stod(argv[3]);
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
		modelparser.seterrorstowarnings(errors);
		const std::vector<std::string>scenarios(1, scenario);
		const std::vector<Models::FMTmodel> models = modelparser.readproject(primarylocation, scenarios);
		//modelparser.write(models.at(0),"C:/Users/admlocal/Desktop/test2/");
		Models::FMTlpmodel optimizationmodel(models.at(0), Models::FMTsolverinterface::MOSEK);
		//modelparser.write(optimizationmodel, "D:/test/");
		optimizationmodel.setparameter(Models::FMTintmodelparameters::LENGTH, length);
		optimizationmodel.FMTmodel::setparameter(Models::FMTboolmodelparameters::STRICTLY_POSITIVE,true);
		optimizationmodel.setparameter(Models::FMTintmodelparameters::PRESOLVE_ITERATIONS,10);
		optimizationmodel.setparameter(Models::FMTintmodelparameters::NUMBER_OF_THREADS, 1);
		
		//modelparser.write(optimizationmodel, "D:/test/");
		if (optimizationmodel.doplanning(true)) {
			
			std::cout << std::to_string(optimizationmodel.getObjValue()) << std::endl;
			/*std::vector<Core::FMToutput>outputs;
			for (const Core::FMToutput& output : optimizationmodel.getoutputs())
			{
				if (output.getname()=="OAAMINC")
				{
					std::cout << output.getname() << " ";
					for (int period =1 ; period <= 10; ++period)
						{
						const std::map<std::string, double>outs = optimizationmodel.getoutput(output, period, Core::FMToutputlevel::totalonly);
						for (const auto& val : outs)
						{
							if (val.second>0)
							{
								std::cout << period << " " << val.second << "\n";
							}
							
						}
						if (!outs.empty())
							{
							std::cout << outs.at("Total") << " ";
							}
						}
					std::cout << "\n";
					outputs.push_back(output);
				}

			}
			modelparser.writeresults(optimizationmodel, outputs, 10, 10, "D:/test/out", Core::FMToutputlevel::developpement);*/

		}
		//optimizationmodel.writeLP("D:/test/test.lp");
		
		/*Parser::FMTscheduleparser schparser;
		std::vector<Core::FMTschedule>returnschedule;
		for (int id = 0; id < 10;++id)
		{
			returnschedule.push_back(optimizationmodel.getsolution(id+1, false));
		}
		schparser.write(returnschedule, "D:/test/schedule.seq");*/
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

