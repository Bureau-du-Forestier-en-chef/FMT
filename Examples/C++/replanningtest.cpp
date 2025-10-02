#include <vector>
#ifdef FMTWITHOSI
	#include "FMTtaskhandler.h"
	#include "FMTreplanningtask.h"
	#include "FMTlpmodel.h"
	#include "FMTnssmodel.h"
	#include "FMTfreeexceptionhandler.h"
	#include "FMTmodelparser.h"
	#include "FMTscheduleparser.h"
#include "FMTdefaultlogger.h"
#include "boost/filesystem.hpp"
#endif

int main(int argc, char *argv[])
	{
	#ifdef FMTWITHOSI
	Logging::FMTdefaultlogger().logstamp();
	const std::string folder = "../../../../Examples/Models/TWD_land/";
	const std::string outputlocation = "../../tests/replanningtest/replanning";
	const std::string scheduleLocation = "../../tests/replanningtest/replanning/scenarios/replicat1/replanning._seq";
	//const std::string folder = "../../Examples/Models/TWD_land/";
	const std::string primlocation = folder + "TWD_land.pri";
	std::vector<std::string>allscenarios;
	allscenarios.push_back("Globalreplanning");
	allscenarios.push_back("Globalfire");
	allscenarios.push_back("Localreplanning");
	Parser::FMTmodelparser modelparser;
	Parser::FMTscheduleparser scheduleParser;
	modelparser.setdefaultexceptionhandler();
	std::vector<Models::FMTmodel> models = modelparser.readproject(primlocation, allscenarios);
	Models::FMTlpmodel global(models.at(0), Models::FMTsolverinterface::CLP);
	global.setparameter(Models::FMTintmodelparameters::LENGTH, 10);
	global.setparameter(Models::FMTintmodelparameters::NUMBER_OF_THREADS,1);
	global.setparameter(Models::FMTboolmodelparameters::PRESOLVE_CAN_REMOVE_STATIC_THEMES, true);
	Models::FMTnssmodel stochastic(models.at(1), 0);
	stochastic.setparameter(Models::FMTintmodelparameters::LENGTH, 1);
	Models::FMTlpmodel local(models.at(2), Models::FMTsolverinterface::CLP);
	local.setparameter(Models::FMTintmodelparameters::LENGTH, 1);
	local.setparameter(Models::FMTintmodelparameters::NUMBER_OF_THREADS,1);
	std::vector<Core::FMToutput>selectedoutputs;
	for (const Core::FMToutput& output : global.getoutputs())
	{
		if (
			output.getname() == "OVOLREC" ||
			output.getname() == "VOLINVENT" ||
			output.getname()=="BURNEDAREA" || 
			output.getname() == "DIVIDEZERO"
			)
		{
			selectedoutputs.push_back(output);
		}
	}
	std::vector<std::string>layersoptions;
	layersoptions.push_back("SEPARATOR=SEMICOLON");
	std::unique_ptr<Parallel::FMTtask> maintaskptr(new Parallel::FMTreplanningtask(
		global, stochastic, local, selectedoutputs, outputlocation, "CSV", layersoptions, 10, 10, 0.5, Core::FMToutputlevel::standard, true));
	Parallel::FMTtaskhandler handler(maintaskptr,10);
	//handler.setquietlogger();
	//handler.ondemandrun();
	handler.conccurentrun();

	//On lis les schédules
	const std::vector<Core::FMTtheme> THEMES = models.at(0).getthemes();
	const std::vector<Core::FMTaction> ACTIONS = models.at(0).getactions();
	scheduleParser.read(THEMES, ACTIONS, scheduleLocation);

	#endif
	/*#ifdef FMTWITHOSI
	Logging::FMTlogger().logstamp();
	const std::string folder = "../../../../Examples/Models/TWD_land/";
	const std::string primlocation = "D:/08762/PC_9429_U08762_4_Vg1_2023_vSSP03.pri";
	std::vector<std::string>allscenarios;
	allscenarios.push_back("13_Sc5a_Determin_avsp_Global");
	allscenarios.push_back("feux");
	allscenarios.push_back("13_Sc5a_Determin_avsp_Local");
	Parser::FMTmodelparser modelparser;
	modelparser.setdefaultexceptionhandler();
	std::vector<Exception::FMTexc>errors;
	errors.push_back(Exception::FMTexc::FMTmissingyield);
	errors.push_back(Exception::FMTexc::FMToutput_missing_operator);
	errors.push_back(Exception::FMTexc::FMToutput_too_much_operator);
	errors.push_back(Exception::FMTexc::FMTinvalidyield_number);
	errors.push_back(Exception::FMTexc::FMTundefinedoutput_attribute);
	errors.push_back(Exception::FMTexc::FMToveridedyield);
	errors.push_back(Exception::FMTexc::FMToutofrangeyield);
	errors.push_back(Exception::FMTexc::FMTdeathwithlock);
	modelparser.seterrorstowarnings(errors);
	std::vector<Models::FMTmodel> models = modelparser.readproject(primlocation, allscenarios);
	Models::FMTlpmodel global(models.at(0), Models::FMTsolverinterface::MOSEK);
	global.setparameter(Models::FMTintmodelparameters::LENGTH, 30);
	global.setparameter(Models::FMTintmodelparameters::NUMBER_OF_THREADS,2);
	global.setparameter(Models::FMTboolmodelparameters::PRESOLVE_CAN_REMOVE_STATIC_THEMES, true);
	Models::FMTnssmodel stochastic(models.at(1), 0);
	stochastic.setparameter(Models::FMTintmodelparameters::LENGTH, 1);
	Models::FMTlpmodel local(models.at(2), Models::FMTsolverinterface::MOSEK);
	local.setparameter(Models::FMTintmodelparameters::LENGTH, 1);
	local.setparameter(Models::FMTintmodelparameters::NUMBER_OF_THREADS,2);
	std::vector<Core::FMToutput>selectedoutputs;
	for (const Core::FMToutput& output : global.getoutputs())
	{
		if (output.getname() == "OVOLTOTREC"|| output.getname() == "OSUPFEUX" || output.getname().find("OSUPRECTOT")!=std::string::npos)
		{
			selectedoutputs.push_back(output);
		}
	}
	const std::string outputlocation = "../../tests/replanningtest/replanning";
	std::vector<std::string>layersoptions;
	layersoptions.push_back("SEPARATOR=SEMICOLON");
	std::unique_ptr<Parallel::FMTtask> maintaskptr(new Parallel::FMTreplanningtask(global, stochastic, local, selectedoutputs, outputlocation, "CSV", layersoptions,10,10,0.5, Core::FMToutputlevel::totalonly));
	Parallel::FMTtaskhandler handler(maintaskptr,2);
	//handler.setquietlogger();
	//handler.ondemandrun();
	handler.conccurentrun();
	#endif*/
	return 0;
	}

