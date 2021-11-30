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
	global.setparameter(Models::FMTintmodelparameters::LENGTH, 20);
	Models::FMTnssmodel stochastic(models.at(1), 0);
	stochastic.setparameter(Models::FMTintmodelparameters::LENGTH, 1);
	Models::FMTlpmodel local(models.at(0), Models::FMTsolverinterface::CLP);
	local.setparameter(Models::FMTintmodelparameters::LENGTH, 1);
	std::vector<Core::FMToutput>selectedoutputs;
	for (const Core::FMToutput& output : global.getoutputs())
	{
		if (output.getname() == "HARVESTEDAREA")
		{
			selectedoutputs.push_back(output);
		}
	}
	const std::string outputlocation = "tests/replanning.csv";
	std::unique_ptr<Parallel::FMTtask> maintaskptr(new Parallel::FMTreplanningtask(global, stochastic, local, selectedoutputs, outputlocation, "CSV", 10, 20, 100));
	Parallel::FMTtaskhandler handler(maintaskptr, 2);
	handler.conccurentrun();
	#endif
	return 0;
	}

