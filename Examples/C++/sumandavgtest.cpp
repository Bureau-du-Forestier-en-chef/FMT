#include <vector>
#ifdef FMTWITHOSI
	#include "FMTtaskhandler.h"
	#include "FMTplanningtask.h"
	#include "FMTlpmodel.h"
	#include "FMTnssmodel.h"
	#include "FMTfreeexceptionhandler.h"
	#include "FMTmodelparser.h"
#endif
#include "FMTdefaultlogger.h"

int main(int argc, char *argv[])
	{
	#ifdef FMTWITHOSI
	Logging::FMTdefaultlogger().logstamp();
	const std::string folder = "../../../../Examples/Models/TWD_land/";
	const std::string primlocation = folder + "TWD_land.pri";
	std::vector<bool>playback;
	std::vector<std::string>allscenarios;
	playback.push_back(false);
	allscenarios.push_back("sumavg");
	Parser::FMTmodelparser modelparser;
	modelparser.setdefaultexceptionhandler();
	std::vector<Exception::FMTexc>errors;
	errors.push_back(Exception::FMTexc::FMTempty_schedules);
	modelparser.seterrorstowarnings(errors);
	const std::string outputlocation = "../../tests/sumandavgtest";
	std::vector<std::string>layersoptions;
	layersoptions.push_back("SEPARATOR=SEMICOLON");
	//Si on fournit la localisation du fichier primaire à la task il écrira la schedule pour tous les scénarios.
	Parallel::FMTplanningtask newplanningtask(1,10, outputlocation, "CSV", layersoptions, Core::FMToutputlevel::totalonly, primlocation);
	newplanningtask.setkeepmodels();
	const std::vector<Models::FMTmodel> models = modelparser.readproject(primlocation, allscenarios);
	const std::vector<std::vector<Core::FMTschedule>> schedules = modelparser.readschedules(primlocation, models);
	std::vector<Core::FMToutput>selectedoutputs;
	for (size_t modelid = 0;modelid<models.size();++modelid)
		{
		Models::FMTlpmodel lpmodel(models.at(modelid), Models::FMTsolverinterface::CLP);
		lpmodel.setparameter(Models::FMTintmodelparameters::LENGTH,7);
		lpmodel.setparameter(Models::FMTintmodelparameters::NUMBER_OF_THREADS, 1);
		for (const Core::FMToutput& output : lpmodel.getoutputs())
		{
			if (output.getname() == "OSUPREC"||
				output.getname() == "SUMOSUPREC"|| 
				output.getname() == "AVGOSUPREC")
			{
				selectedoutputs.push_back(output);
			}
		}
		lpmodel.setparameter(Models::FMTboolmodelparameters::FORCE_PARTIAL_BUILD, playback.at(modelid));
		lpmodel.setoutputs(selectedoutputs);
		newplanningtask.push_back(lpmodel,schedules.at(modelid));
		}
	Parallel::FMTtaskhandler handler(newplanningtask,1);
	handler.setquietlogger();
	//handler.ondemandrun();
	handler.conccurentrun();
	const std::vector<const Parallel::FMTplanningtask*> tasks= handler.gettasksfromdynamiccast<Parallel::FMTplanningtask>();
	const std::vector<const Models::FMTlpmodel*> modelsresults =  tasks.at(0)->getmodelsfromdynamiccast<Models::FMTlpmodel>();
	const double sumvalue = modelsresults.at(0)->getoutput(selectedoutputs.at(1), 5, Core::FMToutputlevel::totalonly).at("Total");
	const double avgvalue = modelsresults.at(0)->getoutput(selectedoutputs.at(2), 5, Core::FMToutputlevel::totalonly).at("Total");
	if (std::abs(sumvalue-1498.60684)>0.1)
		{
		Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed, "Wrong value for sum "+std::to_string(sumvalue),
			"sumandavgtest", __LINE__,"");
		}
	if (std::abs(avgvalue - 299.721368) > 0.1)
		{
		Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed, "Wrong value for average " + std::to_string(avgvalue),
			"sumandavgtest", __LINE__, "");
		}

	#endif
	return 0;
	}

