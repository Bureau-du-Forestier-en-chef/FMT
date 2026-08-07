#include <vector>
#ifdef FMTWITHOSI
	#include "FMTTaskHandler.h"
	#include "FMTReplanningTask.h"
	#include "FMTLpModel.h"
	#include "FMTNssModel.h"
	#include "FMTFreeExceptionHandler.h"
	#include "FMTModelParser.h"
#endif
#include "FMTDefaultLogger.h"

int main(int argc, char *argv[])
	{
	#ifdef FMTWITHOSI
	Logging::FMTDefaultLogger().logStamp();
	const std::string folder = "../../../../Examples/Models/TWD_land/";
	const std::string primlocation = folder + "TWD_land.pri";
	std::vector<std::string>allscenarios;
	allscenarios.push_back("Globalreplanning");
	allscenarios.push_back("Globalfire");
	allscenarios.push_back("Localreplanning");
	Parser::FMTModelParser modelparser;
	modelparser.setDefaultExceptionHandler();
	std::vector<Models::FMTModel> models = modelparser.readproject(primlocation, allscenarios);
	Models::FMTLpModel global(models.at(0), Models::FMTSolverInterface::CLP);
	global.setParameter(Models::FMTintmodelparameters::LENGTH, 10);
	global.setParameter(Models::FMTintmodelparameters::NUMBER_OF_THREADS,1);
	global.setParameter(Models::FMTboolmodelparameters::PRESOLVE_CAN_REMOVE_STATIC_THEMES, true);
	Models::FMTNssModel stochastic(models.at(1), 0);
	stochastic.setParameter(Models::FMTintmodelparameters::LENGTH, 1);
	Models::FMTLpModel local(models.at(2), Models::FMTSolverInterface::CLP);
	local.setParameter(Models::FMTintmodelparameters::LENGTH, 1);
	local.setParameter(Models::FMTintmodelparameters::NUMBER_OF_THREADS,1);
	std::vector<Core::FMTOutput>selectedoutputs;
	for (const Core::FMTOutput& output : global.getOutputs())
	{
		if (output.getName() == "OVOLREC"|| output.getName() == "VOLINVENT" || output.getName()=="BURNEDAREA")
		{
			selectedoutputs.push_back(output);
		}
	}
	const std::string outputlocation = "../../tests/replanningmodeladaption/replanning";
	std::vector<std::string>layersoptions;
	layersoptions.push_back("SEPARATOR=SEMICOLON");
	std::unique_ptr<Parallel::FMTTask> maintaskptr(new Parallel::FMTReplanningTask(global, stochastic, local, selectedoutputs, outputlocation, "CSV", layersoptions,10,10,0.5, Core::FMToutputlevel::totalonly));
	Parallel::FMTTaskHandler handler(maintaskptr,10);
	//handler.setQuietLogger();
	//handler.onDemandRun();
	handler.conccurentRun();
	#endif
	return 0;
	}

