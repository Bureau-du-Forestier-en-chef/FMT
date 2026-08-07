#include <vector>
#ifdef FMTWITHOSI
	#include "FMTTaskHandler.h"
	#include "FMTPlanningTask.h"
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
	std::vector<bool>playback;
	std::vector<std::string>allscenarios;
	playback.push_back(false);
	allscenarios.push_back("sumavg");
	Parser::FMTModelParser modelparser;
	modelparser.setDefaultExceptionHandler();
	std::vector<Exception::FMTexc>errors;
	errors.push_back(Exception::FMTexc::FMTempty_schedules);
	modelparser.setErrorsToWarnings(errors);
	std::string outputlocation = "../../tests/sumandavgtest";
	std::vector<std::string>layersoptions;
	layersoptions.push_back("SEPARATOR=SEMICOLON");
	//Si on fournit la localisation du fichier primaire à la task il écrira la schedule pour tous les scénarios.
	Parallel::FMTPlanningTask newplanningtask(1,10, outputlocation, "CSV", layersoptions, Core::FMToutputlevel::totalonly, primlocation);
	newplanningtask.setKeepModels();
	const std::vector<Models::FMTModel> models = modelparser.readproject(primlocation, allscenarios);
	const std::vector<std::vector<Core::FMTSchedule>> schedules = modelparser.readSchedules(primlocation, models);
	std::vector<Core::FMTOutput>selectedoutputs;
	for (size_t modelid = 0;modelid<models.size();++modelid)
		{
		Models::FMTLpModel lpmodel(models.at(modelid), Models::FMTSolverInterface::CLP);
		lpmodel.setParameter(Models::FMTintmodelparameters::LENGTH,7);
		lpmodel.setParameter(Models::FMTintmodelparameters::NUMBER_OF_THREADS, 1);
		for (const Core::FMTOutput& output : lpmodel.getOutputs())
		{
			if (output.getName() == "OSUPREC"||
				output.getName() == "SUMOSUPREC"|| 
				output.getName() == "AVGOSUPREC")
			{
				selectedoutputs.push_back(output);
			}
		}
		lpmodel.setParameter(Models::FMTboolmodelparameters::FORCE_PARTIAL_BUILD, playback.at(modelid));
		lpmodel.setOutputs(selectedoutputs);
		newplanningtask.push_back(lpmodel,schedules.at(modelid));
		}
	Parallel::FMTTaskHandler handler(newplanningtask,1);
	handler.setQuietLogger();
	//handler.onDemandRun();
	handler.conccurentRun();
	const std::vector<const Parallel::FMTPlanningTask*> tasks= handler.getTasksFromDynamicCast<Parallel::FMTPlanningTask>();
	const std::vector<const Models::FMTLpModel*> modelsresults =  tasks.at(0)->getModelsFromDynamicCast<Models::FMTLpModel>();
	Logging::FMTDefaultLogger() <<"OBJECTIVE "<< modelsresults.at(0)->getObjValue() << "\n";
	const double sumvalue = modelsresults.at(0)->getOutput(selectedoutputs.at(1), 5, Core::FMToutputlevel::totalonly).at("Total");
	const double avgvalue = modelsresults.at(0)->getOutput(selectedoutputs.at(2), 5, Core::FMToutputlevel::totalonly).at("Total");
	if (std::abs(sumvalue-1498.60684)>0.1)
		{
		Exception::FMTFreeExceptionHandler().raise(Exception::FMTexc::FMTfunctionfailed, "Wrong value for sum "+std::to_string(sumvalue),
			"sumandavgtest", __LINE__,"");
		}
	if (std::abs(avgvalue - 299.721368) > 0.1)
		{
		Exception::FMTFreeExceptionHandler().raise(Exception::FMTexc::FMTfunctionfailed, "Wrong value for average " + std::to_string(avgvalue),
			"sumandavgtest", __LINE__, "");
		}

	#endif
	return 0;
	}

