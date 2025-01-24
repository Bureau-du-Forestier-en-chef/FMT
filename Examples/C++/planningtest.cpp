
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
	/*const std::string folder = "../../../../Examples/Models/TWD_land/";
	const std::string primlocation = folder + "TWD_land.pri";
	std::vector<bool>playback;
	std::vector<std::string>allscenarios;
	playback.push_back(true);
	allscenarios.push_back("LP3");
	playback.push_back(true);
	allscenarios.push_back("LP4");
	playback.push_back(false);
	allscenarios.push_back("LP5");*/
	const std::string folder = "D:/CC_modele_feu/WS_CC/";
	const std::string primlocation = folder + "Feux_2023_ouest_V01.pri";
	std::vector<bool>playback;
	std::vector<std::string>allscenarios;
	//playback.push_back(true);
	playback.push_back(true);
	/*playback.push_back(true);
	playback.push_back(true);
	playback.push_back(true);
	playback.push_back(true);*/
	//allscenarios.push_back("0");
	allscenarios.push_back("1");
	/*allscenarios.push_back("2");
	allscenarios.push_back("3");
	allscenarios.push_back("4");
	allscenarios.push_back("5");*/
	Parser::FMTmodelparser modelparser;
	modelparser.setdefaultexceptionhandler();
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
	std::string outputlocation = "../../tests/planning";
	std::vector<std::string>layersoptions;
	layersoptions.push_back("SEPARATOR=SEMICOLON");
	//Si on fournit la localisation du fichier primaire à la task il écrira la schedule pour tous les scénarios.
	Parallel::FMTplanningtask newplanningtask(1, 5, outputlocation, "CSV", layersoptions, Core::FMToutputlevel::standard, primlocation);
	const std::vector<Models::FMTmodel> models = modelparser.readproject(primlocation, allscenarios);
	const std::vector<std::vector<Core::FMTschedule>> schedules = modelparser.readschedules(primlocation, models);
	for (size_t modelid = 0;modelid<models.size();++modelid)
		{
		Models::FMTlpmodel lpmodel(models.at(modelid), Models::FMTsolverinterface::MOSEK);
		lpmodel.setparameter(Models::FMTintmodelparameters::LENGTH,5);
		lpmodel.setparameter(Models::FMTintmodelparameters::NUMBER_OF_THREADS, 1);
		lpmodel.FMTmodel::setparameter(Models::FMTdblmodelparameters::TOLERANCE, 0.01);
		std::vector<Core::FMToutput>selectedoutputs;
		for (const Core::FMToutput& output : lpmodel.getoutputs())
		{
			if (output.getname() == "OVOLTOTREC" || output.getname() == "OVOLREC" || output.getname() == "OSUPBRULER_ORI")
			{
				selectedoutputs.push_back(output);
			}
		}
		lpmodel.setparameter(Models::FMTboolmodelparameters::FORCE_PARTIAL_BUILD, playback.at(modelid));
		//lpmodel.setoutputs(selectedoutputs);
		newplanningtask.push_back(lpmodel,schedules.at(modelid), selectedoutputs);
		}
	Parallel::FMTtaskhandler handler(newplanningtask,1);
	//handler.setquietlogger();
	//handler.ondemandrun();
	handler.conccurentrun();
	
	#endif
	return 0;
	}

