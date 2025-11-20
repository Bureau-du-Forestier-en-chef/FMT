
#include <vector>
#ifdef FMTWITHOSI
	#include "FMTtaskhandler.h"
	#include "FMTplanningtask.h"
	#include "FMTlpmodel.h"
	#include "FMTnssmodel.h"
	#include "FMTfreeexceptionhandler.h"
	#include "FMTmodelparser.h"
#include "FMTdefaultlogger.h"
#endif

int main(int argc, char *argv[])
	{
	
	#ifdef FMTWITHOSI
	Logging::FMTdefaultlogger().logstamp();
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
	}
	else
	{
		primlocation = "C:\\Users\\Admlocal\\Documents\\issues\\C2_01009233\\PC_10116_U05151_MODB01_20250912\\PC_10116_U05151_2028_MODB01.pri";
		outputlocation = "C:\\Users\\Admlocal\\Documents\\SCRAP";
		length = 30;
		scenario_name = "Ovoltotrec_complexite";
		playback.push_back(false);
		allscenarios.push_back(scenario_name);
	}

	Parser::FMTmodelparser modelparser;
	modelparser.setdefaultexceptionhandler();
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
	modelparser.seterrorstowarnings(errors);
	std::vector<std::string> layersoptions;
	layersoptions.push_back("SEPARATOR=SEMICOLON");
	//Si on fournit la localisation du fichier primaire à la task il écrira la schedule pour tous les scénarios.
	Parallel::FMTplanningtask newplanningtask(0, length, outputlocation, "CSV", layersoptions, Core::FMToutputlevel::standard, primlocation);
	const std::vector<Models::FMTmodel> models = modelparser.readproject(primlocation, allscenarios);
	const std::vector<std::vector<Core::FMTschedule>> schedules = modelparser.readschedules(primlocation, models);
	for (size_t modelid = 0; modelid<models.size(); ++modelid)
		{
		Models::FMTlpmodel lpmodel(models.at(modelid), Models::FMTsolverinterface::MOSEK);
		lpmodel.setparameter(Models::FMTintmodelparameters::LENGTH, length);
		lpmodel.setparameter(Models::FMTintmodelparameters::NUMBER_OF_THREADS, n_threads);
		lpmodel.FMTmodel::setparameter(Models::FMTdblmodelparameters::TOLERANCE, 0.01);
		std::vector<Core::FMToutput> selectedoutputs;
		for (const Core::FMToutput& output : lpmodel.getoutputs())
		{
			if (
				output.getname() == "OVOLTOTREC" 
				|| output.getname() == "OVOLREC" 
				|| output.getname() == "OSUPBRULER_ORI"
				|| output.getname() == "OSUPREGECO_HARTIF25UTR"
				|| output.getname() == "OSUPREGECO_HARAT100UTR"
				|| output.getname() == "OSUPJEUNEALERTEREGECO"
				)
			{
				selectedoutputs.push_back(output);
			}
		}
		lpmodel.setparameter(Models::FMTboolmodelparameters::FORCE_PARTIAL_BUILD, playback.at(modelid));
		//lpmodel.setoutputs(selectedoutputs);
		newplanningtask.push_back(lpmodel, schedules.at(modelid), selectedoutputs);
		}
	Parallel::FMTtaskhandler handler(newplanningtask, 1);
	//handler.setquietlogger();
	//handler.ondemandrun();
	handler.conccurentrun();
	
	#endif
	return 0;
	}

