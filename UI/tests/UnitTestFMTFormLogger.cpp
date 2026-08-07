#include <vector>
#include <string>
#include <iostream>
#include <memory>

#ifdef FMTWITHMOSEK
	#include "FMTModelParser.h"
	#include "FMTVersion.h"
	#include "FMTFormLogger.h"
	#include "FMTFreeExceptionHandler.h"
	#include "FMTDefaultLogger.h"
	#include "FMTLpModel.h"
	#include "FMTNssModel.h"
	#include "FMTTaskHandler.h"
	#include "FMTReplanningTask.h"
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
					// std::unique_ptr<Logging::FMTLogger> logger(new Wrapper::FMTFormLogger(filename, (logfunc)(void*)intptrptr));
					m_logger = Wrapper::FMTFormLogger(outFile, (Wrapper::logfunc)(void*)&out);
					m_logger.settasklogginglevel(1);
					}
				void testReplanning()
				{
					Logging::FMTDefaultLogger().logStamp();
					const std::string folder = "../../../../Examples/Models/TWD_land/";
					const std::string primlocation = folder + "TWD_land.pri";
					std::vector<std::string>allscenarios;
					allscenarios.push_back("Globalreplanning");
					allscenarios.push_back("Globalfire");
					allscenarios.push_back("Localreplanning");
					Parser::FMTModelParser modelparser;
					std::unique_ptr<Logging::FMTLogger> logger(new Wrapper::FMTFormLogger(m_logger));
					Parser::FMTModelParser Modelparser;
					Modelparser.passInLogger(logger);
					std::vector<Models::FMTModel> models = modelparser.readproject(primlocation, allscenarios);
					Models::FMTLpModel global(models.at(0), Models::FMTSolverInterface::CLP);
					global.setParameter(Models::FMTintmodelparameters::LENGTH, 10);
					global.setParameter(Models::FMTintmodelparameters::NUMBER_OF_THREADS, 1);
					global.setParameter(Models::FMTboolmodelparameters::PRESOLVE_CAN_REMOVE_STATIC_THEMES, true);
					Models::FMTNssModel stochastic(models.at(1), 0);
					stochastic.setParameter(Models::FMTintmodelparameters::LENGTH, 1);
					Models::FMTLpModel local(models.at(2), Models::FMTSolverInterface::CLP);
					local.setParameter(Models::FMTintmodelparameters::LENGTH, 1);
					local.setParameter(Models::FMTintmodelparameters::NUMBER_OF_THREADS, 1);
					std::vector<Core::FMTOutput>selectedoutputs;
					for (const Core::FMTOutput& output : global.getOutputs())
					{
						if (output.getName() == "OVOLREC" || output.getName() == "VOLINVENT" || output.getName() == "BURNEDAREA")
						{
							selectedoutputs.push_back(output);
						}
					}
				
					std::vector<std::string>layersoptions;
					layersoptions.push_back("SEPARATOR=SEMICOLON");
					std::unique_ptr<Parallel::FMTTask> maintaskptr(new Parallel::FMTReplanningTask(global, stochastic, local, selectedoutputs, m_OutLocation, "CSV", layersoptions, 10, 10, 0.5, Core::FMToutputlevel::totalonly));
					Parallel::FMTTaskHandler handler(maintaskptr, 2);
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

