#include <vector>
#include <string>
#include <iostream>
#include <memory>

#ifdef FMTWITHMOSEK
	#include "FMTmodelparser.h"
	#include "FMTversion.h"
	#include "FMTFormLogger.h"
	#include "FMTfreeexceptionhandler.h"
	#include "FMTdefaultlogger.h"
	#include "FMTlpmodel.h"
	#include "FMTnssmodel.h"
	#include "FMTtaskhandler.h"
	#include "FMTreplanningtask.h"
void out(const char* data) {
	std::cout << data;
	}

	namespace Testing
	{
		class UnitTestFMTFormLogger
			{
			public:
				UnitTestFMTFormLogger()
					{
					m_OutLocation = "../../tests/UnitTesteFMTFormLogger";
					// const std::string outFile = m_OutLocation+"/FMTFormLoggertest.log";
					const std::string outFile = "FMTFormLoggertest.log";
					// (Wrapper::logfunc)(void*)intptrptr=&std::cout;
					// std::unique_ptr<Logging::FMTlogger> logger(new Wrapper::FMTFormLogger(filename, (logfunc)(void*)intptrptr));
					m_logger = Wrapper::FMTFormLogger(outFile, (Wrapper::logfunc)(void*)&out);
					m_logger.settasklogginglevel(1);
					}
				void testReplanning()
				{
					Logging::FMTdefaultlogger().logstamp();
					const std::string folder = "../../../../Examples/Models/TWD_land/";
					const std::string primlocation = folder + "TWD_land.pri";
					std::vector<std::string>allscenarios;
					allscenarios.push_back("Globalreplanning");
					allscenarios.push_back("Globalfire");
					allscenarios.push_back("Localreplanning");
					Parser::FMTmodelparser modelparser;
					std::unique_ptr<Logging::FMTlogger> logger(new Wrapper::FMTFormLogger(m_logger));
					Parser::FMTmodelparser Modelparser;
					Modelparser.passinlogger(logger);
					std::vector<Models::FMTmodel> models = modelparser.readproject(primlocation, allscenarios);
					Models::FMTlpmodel global(models.at(0), Models::FMTsolverinterface::CLP);
					global.setparameter(Models::FMTintmodelparameters::LENGTH, 10);
					global.setparameter(Models::FMTintmodelparameters::NUMBER_OF_THREADS, 1);
					global.setparameter(Models::FMTboolmodelparameters::PRESOLVE_CAN_REMOVE_STATIC_THEMES, true);
					Models::FMTnssmodel stochastic(models.at(1), 0);
					stochastic.setparameter(Models::FMTintmodelparameters::LENGTH, 1);
					Models::FMTlpmodel local(models.at(2), Models::FMTsolverinterface::CLP);
					local.setparameter(Models::FMTintmodelparameters::LENGTH, 1);
					local.setparameter(Models::FMTintmodelparameters::NUMBER_OF_THREADS, 1);
					std::vector<Core::FMToutput>selectedoutputs;
					for (const Core::FMToutput& output : global.getoutputs())
					{
						if (output.getname() == "OVOLREC" || output.getname() == "VOLINVENT" || output.getname() == "BURNEDAREA")
						{
							selectedoutputs.push_back(output);
						}
					}
				
					std::vector<std::string>layersoptions;
					layersoptions.push_back("SEPARATOR=SEMICOLON");
					std::unique_ptr<Parallel::FMTtask> maintaskptr(new Parallel::FMTreplanningtask(global, stochastic, local, selectedoutputs, m_OutLocation, "CSV", layersoptions, 10, 10, 0.5, Core::FMToutputlevel::totalonly));
					Parallel::FMTtaskhandler handler(maintaskptr, 2);
				}
			private:
				Wrapper::FMTFormLogger m_logger;
				std::string m_OutLocation;

			};

	}

#endif



int main()
{
	#ifdef FMTWITHMOSEK
		Testing::UnitTestFMTFormLogger test;
		test.testReplanning();
	#endif 
	return 0;
}

