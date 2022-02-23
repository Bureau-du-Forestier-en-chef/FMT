#include <vector>
#include <string>
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
	const std::string primlocation = argv[1];
	const int length = std::stoi(argv[2]);
	const int replicate = std::stoi(argv[3]);
	std::vector<std::string>allscenarios;
	allscenarios.push_back("Globalreplanning");
	allscenarios.push_back("Globalfire");
	allscenarios.push_back("Localreplanning");
	Parser::FMTmodelparser modelparser;
	modelparser.setdefaultexceptionhandler();
	std::vector<Models::FMTmodel> models = modelparser.readproject(primlocation, allscenarios);
	Models::FMTlpmodel global(models.at(0), Models::FMTsolverinterface::MOSEK);
	global.setparameter(Models::FMTintmodelparameters::LENGTH, length);
	global.setparameter(Models::FMTintmodelparameters::NUMBER_OF_THREADS,1);
	global.setparameter(Models::FMTboolmodelparameters::PRESOLVE_CAN_REMOVE_STATIC_THEMES, true);
	Models::FMTnssmodel stochastic(models.at(1), 0);
	stochastic.setparameter(Models::FMTintmodelparameters::LENGTH, 1);
	Models::FMTlpmodel local(models.at(2), Models::FMTsolverinterface::MOSEK);
	local.setparameter(Models::FMTintmodelparameters::LENGTH, 1);
	local.setparameter(Models::FMTintmodelparameters::NUMBER_OF_THREADS,1);
	std::vector<Core::FMToutput>selectedoutputs;
	for (const Core::FMToutput& output : global.getoutputs())
	{
		if (output.getname() == "OVOLTOTREC"|| output.getname() == "OVOL_UA_TOTREC")
		{
			selectedoutputs.push_back(output);
		}
	}
	std::size_t lastslash = primlocation.find_last_of("/\\");
	const std::string locname = primlocation.substr(lastslash+1,(primlocation.size()-lastslash)-4);
	const std::string outputlocation = "tests/"+ locname;
	std::vector<std::string>layersoptions;
	layersoptions.push_back("SEPARATOR=SEMICOLON");
	std::unique_ptr<Parallel::FMTtask> maintaskptr(new Parallel::FMTreplanningtask(global, stochastic, local, selectedoutputs, outputlocation, "CSV", layersoptions, replicate,length,0.5, Core::FMToutputlevel::totalonly));
	Parallel::FMTtaskhandler handler(maintaskptr,2);
	handler.setquietlogger();
	//handler.ondemandrun();
	handler.conccurentrun();
	#endif
	return 0;
	}

