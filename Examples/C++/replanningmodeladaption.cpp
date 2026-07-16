#include <vector>
#ifdef FMTWITHOSI
	#include "FMTtaskhandler.h"
	#include "FMTreplanningtask.h"
	#include "FMTlpmodel.h"
	#include "FMTnssmodel.h"
	#include "FMTfreeexceptionhandler.h"
	#include "FMTmodelparser.h"
#endif
#include "FMTdefaultlogger.h"

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
	Parser::FMTmodelparser modelparser;
	modelparser.setDefaultExceptionHandler();
	std::vector<Models::FMTmodel> models = modelparser.readproject(primlocation, allscenarios);
	Models::FMTlpmodel global(models.at(0), Models::FMTsolverinterface::CLP);
	global.setParameter(Models::FMTintmodelparameters::LENGTH, 10);
	global.setParameter(Models::FMTintmodelparameters::NUMBER_OF_THREADS,1);
	global.setParameter(Models::FMTboolmodelparameters::PRESOLVE_CAN_REMOVE_STATIC_THEMES, true);
	Models::FMTnssmodel stochastic(models.at(1), 0);
	stochastic.setParameter(Models::FMTintmodelparameters::LENGTH, 1);
	Models::FMTlpmodel local(models.at(2), Models::FMTsolverinterface::CLP);
	local.setParameter(Models::FMTintmodelparameters::LENGTH, 1);
	local.setParameter(Models::FMTintmodelparameters::NUMBER_OF_THREADS,1);
	std::vector<Core::FMToutput>selectedoutputs;
	for (const Core::FMToutput& output : global.getOutputs())
	{
		if (output.getName() == "OVOLREC"|| output.getName() == "VOLINVENT" || output.getName()=="BURNEDAREA")
		{
			selectedoutputs.push_back(output);
		}
	}
	const std::string outputlocation = "../../tests/replanningmodeladaption/replanning";
	std::vector<std::string>layersoptions;
	layersoptions.push_back("SEPARATOR=SEMICOLON");
	std::unique_ptr<Parallel::FMTtask> maintaskptr(new Parallel::FMTreplanningtask(global, stochastic, local, selectedoutputs, outputlocation, "CSV", layersoptions,10,10,0.5, Core::FMToutputlevel::totalonly));
	Parallel::FMTtaskhandler handler(maintaskptr,10);
	//handler.setQuietLogger();
	//handler.onDemandRun();
	handler.conccurentRun();
	#endif
	return 0;
	}

