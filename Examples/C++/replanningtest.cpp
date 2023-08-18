#include <vector>
#ifdef FMTWITHOSI
	#include "FMTtaskhandler.hpp"
	#include "FMTreplanningtask.hpp"
	#include "FMTlpmodel.hpp"
	#include "FMTnssmodel.hpp"
	#include "FMTfreeexceptionhandler.hpp"
	#include "FMTmodelparser.hpp"
#endif

int main(int argc, char *argv[])
	{
	#ifdef FMTWITHOSI
	Logging::FMTlogger().logstamp();
	const std::string folder = "../../../../Examples/Models/TWD_land/";
	const std::string primlocation = folder + "TWD_land.pri";
	std::vector<std::string>allscenarios;
	allscenarios.push_back("Globalreplanning");
	allscenarios.push_back("Globalfire");
	allscenarios.push_back("Localreplanning");
	Parser::FMTmodelparser modelparser;
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
		if (output.getname() == "OVOLREC"|| output.getname() == "VOLINVENT" || output.getname()=="BURNEDAREA")
		{
			selectedoutputs.push_back(output);
		}
	}
	const std::string outputlocation = "../../tests/replanningtest/replanning";
	std::vector<std::string>layersoptions;
	layersoptions.push_back("SEPARATOR=SEMICOLON");
	std::unique_ptr<Parallel::FMTtask> maintaskptr(new Parallel::FMTreplanningtask(global, stochastic, local, selectedoutputs, outputlocation, "CSV", layersoptions,10,10,0.5, Core::FMToutputlevel::totalonly));
	Parallel::FMTtaskhandler handler(maintaskptr,10);
	//handler.setquietlogger();
	//handler.ondemandrun();
	handler.conccurentrun();
	#endif
	
	}

