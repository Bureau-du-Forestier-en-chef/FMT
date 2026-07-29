
#include <vector>
#ifdef FMTWITHOSI
	#include "FMTTaskHandler.h"
	#include "FMTPlanningTask.h"
	#include "FMTLpModel.h"
	#include "FMTNssModel.h"
	#include "FMTFreeExceptionHandler.h"
	#include "FMTModelParser.h"
#include "FMTDefaultLogger.h"
#endif

int main(int argc, char *argv[])
	{
	
	#ifdef FMTWITHOSI
	Logging::FMTDefaultLogger().logStamp();
	std::vector<bool> playback;
	std::vector<std::string> allscenarios;
	std::string primlocation;
	std::string outputlocation;
	std::string scenario_name;
	int length;
	unsigned int n_threads = 1;
	// Test arg : planningtest;prim_location|scenario_name;outputm_location;length
	if (argc > 1) {
		const std::string vals = argv[1];
		std::vector<std::string> results;
		boost::split(results, vals, boost::is_any_of("|"));
		primlocation = results[0];
		for (std::size_t i = 1; i < results.size(); ++i) {
			allscenarios.push_back(results[i]);
			playback.push_back(true);
		}
		outputlocation = argv[2];
		length = std::stoi(argv[3]);
	}else
	{
		primlocation = "C:\\Users\\Admlocal\\Documents\\issues\\C2_02031521\\PC_10067_U02571_2028_Corr_Vol_Phase2\\PC_10067_U02571_2028_MODB01.pri";
		outputlocation = "C:\\Users\\Admlocal\\Documents\\issues\\C2_02031521\\output";
		length = 1;
		scenario_name = "200_StratReg_apsp";
		playback.push_back(true);
		allscenarios.push_back(scenario_name);
	}

	Parser::FMTModelParser modelparser;
	modelparser.setDefaultExceptionHandler();
	std::vector<Exception::FMTexc> errors;
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
	errors.push_back(Exception::FMTexc::FMTempty_schedules);
	modelparser.setErrorsToWarnings(errors);
	std::vector<std::string> layersoptions;
	layersoptions.push_back("SEPARATOR=SEMICOLON");
	//Si on fournit la localisation du fichier primaire à la task il écrira la schedule pour tous les scénarios.
	Parallel::FMTPlanningTask newplanningtask(0, length, outputlocation, "CSV", layersoptions, Core::FMToutputlevel::totalonly, primlocation);
	const std::vector<Models::FMTModel> models = modelparser.readproject(primlocation, allscenarios);
	const std::vector<std::vector<Core::FMTSchedule>> schedules = modelparser.readSchedules(primlocation, models);
	for (size_t modelid = 0; modelid<models.size(); ++modelid)
		{
		Models::FMTLpModel lpmodel(models.at(modelid), Models::FMTsolverinterface::MOSEK);
		lpmodel.setParameter(Models::FMTintmodelparameters::LENGTH, length);
		lpmodel.setParameter(Models::FMTintmodelparameters::NUMBER_OF_THREADS, n_threads);
		lpmodel.FMTModel::setParameter(Models::FMTdblmodelparameters::TOLERANCE, 0.01);
		std::vector<Core::FMTOutput> selectedoutputs;
		for (const Core::FMTOutput& output : lpmodel.getOutputs())
		{
			if (
				output.getName() == "OVOLTOTREC" 
				//|| output.getName() == "OVOLREC" 
				//|| output.getName() == "OSUPBRULER_ORI"
				//|| output.getName() == "OSUPREGECO_HARTIF25UTR"
				//|| output.getName() == "OSUPREGECO_HARAT100UTR"
				//|| output.getName() == "OSUPJEUNEALERTEREGECO"
				)
			{
				selectedoutputs.push_back(output);
			}
		}
		lpmodel.setParameter(Models::FMTboolmodelparameters::FORCE_PARTIAL_BUILD, playback.at(modelid));
		//lpmodel.setOutputs(selectedoutputs);
		newplanningtask.push_back(lpmodel, schedules.at(modelid), selectedoutputs);
		}
	Parallel::FMTTaskHandler handler(newplanningtask, 1);
	//handler.setQuietLogger();
	//handler.onDemandRun();
	handler.conccurentRun();
	
	#endif
	return 0;
	}

