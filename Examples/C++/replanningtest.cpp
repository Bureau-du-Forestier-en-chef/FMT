#include <vector>
#ifdef FMTWITHOSI
	#include "FMTTaskHandler.h"
	#include "FMTReplanningTask.h"
	#include "FMTLpModel.h"
	#include "FMTNssModel.h"
	#include "FMTFreeExceptionHandler.h"
	#include "FMTModelParser.h"
	#include "FMTScheduleParser.h"
#include "FMTDefaultLogger.h"
#include "boost/filesystem.hpp"
#endif

int main(int argc, char *argv[])
	{
	#ifdef FMTWITHOSI
	Logging::FMTDefaultLogger().logStamp();
	const std::string folder = "../../../../Examples/Models/TWD_land/";
	const std::string outputlocation = "../../tests/replanningtest/replanning";
	const std::string scheduleLocation = "../../tests/replanningtest/replanning/scenarios/replicat1/replanning._seq";
	//const std::string folder = "../../Examples/Models/TWD_land/";
	const std::string primlocation = folder + "TWD_land.pri";
	std::vector<std::string>allscenarios;
	allscenarios.push_back("Globalreplanning");
	allscenarios.push_back("Globalfire");
	allscenarios.push_back("Localreplanning");
	Parser::FMTModelParser modelparser;
	Parser::FMTScheduleParser scheduleParser;
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
		if (
			output.getName() == "OVOLREC" ||
			output.getName() == "VOLINVENT" ||
			output.getName()=="BURNEDAREA" || 
			output.getName() == "DIVIDEZERO"
			)
		{
			selectedoutputs.push_back(output);
		}
	}
	std::vector<std::string>layersoptions;
	layersoptions.push_back("SEPARATOR=SEMICOLON");
	std::unique_ptr<Parallel::FMTTask> maintaskptr(new Parallel::FMTReplanningTask(
		global, stochastic, local, selectedoutputs, outputlocation, "CSV", layersoptions, 10, 10, 0.5, Core::FMToutputlevel::standard, true));
	Parallel::FMTTaskHandler handler(maintaskptr,10);
	//handler.setQuietLogger();
	//handler.onDemandRun();
	handler.conccurentRun();

	//On lis les schédules
	const std::vector<Core::FMTTheme> THEMES = models.at(0).getThemes();
	const std::vector<Core::FMTAction> ACTIONS = models.at(0).getActions();
	scheduleParser.read(THEMES, ACTIONS, scheduleLocation);

	#endif
	/*#ifdef FMTWITHOSI
	Logging::FMTLogger().logstamp();
	const std::string folder = "../../../../Examples/Models/TWD_land/";
	const std::string primlocation = "D:/08762/PC_9429_U08762_4_Vg1_2023_vSSP03.pri";
	std::vector<std::string>allscenarios;
	allscenarios.push_back("13_Sc5a_Determin_avsp_Global");
	allscenarios.push_back("feux");
	allscenarios.push_back("13_Sc5a_Determin_avsp_Local");
	Parser::FMTModelParser modelparser;
	modelparser.setDefaultExceptionHandler();
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
	std::vector<Models::FMTModel> models = modelparser.readproject(primlocation, allscenarios);
	Models::FMTLpModel global(models.at(0), Models::FMTSolverInterface::MOSEK);
	global.setparameter(Models::FMTintmodelparameters::LENGTH, 30);
	global.setparameter(Models::FMTintmodelparameters::NUMBER_OF_THREADS,2);
	global.setparameter(Models::FMTboolmodelparameters::PRESOLVE_CAN_REMOVE_STATIC_THEMES, true);
	Models::FMTNssModel stochastic(models.at(1), 0);
	stochastic.setparameter(Models::FMTintmodelparameters::LENGTH, 1);
	Models::FMTLpModel local(models.at(2), Models::FMTSolverInterface::MOSEK);
	local.setparameter(Models::FMTintmodelparameters::LENGTH, 1);
	local.setparameter(Models::FMTintmodelparameters::NUMBER_OF_THREADS,2);
	std::vector<Core::FMTOutput>selectedoutputs;
	for (const Core::FMTOutput& output : global.getoutputs())
	{
		if (output.getName() == "OVOLTOTREC"|| output.getName() == "OSUPFEUX" || output.getName().find("OSUPRECTOT")!=std::string::npos)
		{
			selectedoutputs.push_back(output);
		}
	}
	const std::string outputlocation = "../../tests/replanningtest/replanning";
	std::vector<std::string>layersoptions;
	layersoptions.push_back("SEPARATOR=SEMICOLON");
	std::unique_ptr<Parallel::FMTTask> maintaskptr(new Parallel::FMTReplanningTask(global, stochastic, local, selectedoutputs, outputlocation, "CSV", layersoptions,10,10,0.5, Core::FMToutputlevel::totalonly));
	Parallel::FMTTaskHandler handler(maintaskptr,2);
	//handler.setQuietLogger();
	//handler.ondemandrun();
	handler.conccurentrun();
	#endif*/
	return 0;
	}

