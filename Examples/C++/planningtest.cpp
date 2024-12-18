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
	/*const std::string folder = "D:/CC_modele_feu/WS_CC/";
	const std::string primlocation = folder + "Feux_2023_ouest_V01.pri";
	std::vector<bool>playback;
	std::vector<std::string>allscenarios;
	playback.push_back(true);
	allscenarios.push_back("PlayBack");*/
	Parser::FMTmodelparser modelparser;
	modelparser.setdefaultexceptionhandler();
	std::vector<Exception::FMTexc>errors;
	errors.push_back(Exception::FMTexc::FMTempty_schedules);
	errors.push_back(Exception::FMTexc::FMTmissingyield);
	modelparser.seterrorstowarnings(errors);
	std::string outputlocation = "../../tests/planning";
	std::vector<std::string>layersoptions;
	layersoptions.push_back("SEPARATOR=SEMICOLON");
	//Si on fournit la localisation du fichier primaire � la task il �crira la schedule pour tous les sc�narios.
	Parallel::FMTplanningtask newplanningtask(1, 7, outputlocation, "CSV", layersoptions, Core::FMToutputlevel::standard, primlocation);
	const std::vector<Models::FMTmodel> models = modelparser.readproject(primlocation, allscenarios);
	const std::vector<std::vector<Core::FMTschedule>> schedules = modelparser.readschedules(primlocation, models);
	for (size_t modelid = 0;modelid<models.size();++modelid)
		{
		Models::FMTlpmodel lpmodel(models.at(modelid), Models::FMTsolverinterface::MOSEK);
		lpmodel.setparameter(Models::FMTintmodelparameters::LENGTH,7);
		lpmodel.setparameter(Models::FMTintmodelparameters::NUMBER_OF_THREADS, 1);
		lpmodel.FMTmodel::setparameter(Models::FMTdblmodelparameters::TOLERANCE, 0.01);
		std::vector<Core::FMToutput>selectedoutputs;
		for (const Core::FMToutput& output : lpmodel.getoutputs())
		{
			if (output.getname() == "OVOLREC" || output.getname() == "OSUPBRULER_ORI")
			{
				selectedoutputs.push_back(output);
			}
		}
		lpmodel.setparameter(Models::FMTboolmodelparameters::FORCE_PARTIAL_BUILD, playback.at(modelid));
		//lpmodel.setoutputs(selectedoutputs);
		newplanningtask.push_back(lpmodel,schedules.at(modelid), selectedoutputs);
		}
	Parallel::FMTtaskhandler handler(newplanningtask,3);
	//handler.setquietlogger();
	//handler.ondemandrun();
	handler.conccurentrun();
	#endif
	return 0;
	}

