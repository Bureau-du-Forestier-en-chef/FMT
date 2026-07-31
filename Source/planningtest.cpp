#include <vector>
#ifdef FMTWITHOSI
	#include "FMTTaskHandler.h"
	#include "FMTPlanningTask.h"
	#include "FMTLpModel.h"
	#include "FMTNssModel.h"
	#include "FMTFreeExceptionHandler.h"
	#include "FMTModelParser.h"
#endif

int main(int argc, char *argv[])
	{
	#ifdef FMTWITHOSI
	Logging::FMTLogger().logStamp();
	const std::string folder = "../../../../Examples/Models/TWD_land/";
	const std::string primlocation = folder + "TWD_land.pri";
	std::vector<bool>playback;
	std::vector<std::string>allscenarios;
	playback.push_back(true);
	allscenarios.push_back("LP3");
	playback.push_back(true);
	allscenarios.push_back("LP4");
	playback.push_back(false);
	allscenarios.push_back("LP5");
	Parser::FMTModelParser modelparser;
	modelparser.setDefaultExceptionHandler();
	std::vector<Exception::FMTexc>errors;
	errors.push_back(Exception::FMTexc::FMTempty_schedules);
	modelparser.setErrorsToWarnings(errors);
	const std::string outputlocation = "tests/planning";
	std::vector<std::string>layersoptions;
	layersoptions.push_back("SEPARATOR=SEMICOLON");
	//Si on fournit la localisation du fichier primaire à la task il écrira la schedule pour tous les scénarios.
	Parallel::FMTPlanningTask newplanningtask(1,7, outputlocation, "CSV", layersoptions, Core::FMToutputlevel::totalonly/*, primlocation*/);
	const std::vector<Models::FMTModel> models = modelparser.readproject(primlocation, allscenarios);
	const std::vector<std::vector<Core::FMTSchedule>> schedules = modelparser.readSchedules(primlocation, models);
	for (size_t modelid = 0;modelid<models.size();++modelid)
		{
		Models::FMTLpModel lpmodel(models.at(modelid), Models::FMTsolverinterface::CLP);
		lpmodel.setParameter(Models::FMTintmodelparameters::LENGTH,7);
		lpmodel.setParameter(Models::FMTintmodelparameters::NUMBER_OF_THREADS, 1);
		std::vector<Core::FMTOutput>selectedoutputs;
		for (const Core::FMTOutput& output : lpmodel.getOutputs())
		{
			if (output.getName() == "OVOLREC")
			{
				selectedoutputs.push_back(output);
			}
		}
		lpmodel.setParameter(Models::FMTboolmodelparameters::FORCE_PARTIAL_BUILD, playback.at(modelid));
		lpmodel.setOutputs(selectedoutputs);
		newplanningtask.push_back(lpmodel,schedules.at(modelid));
		}
	Parallel::FMTTaskHandler handler(newplanningtask,3);
	handler.setQuietLogger();
	//handler.onDemandRun();
	handler.conccurentRun();
	#endif
	return 0;
	}

