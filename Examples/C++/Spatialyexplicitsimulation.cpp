#include <vector>
#include "FMTsesmodel.h"
#include "FMTmodelparser.h"
#include "FMTversion.h"
#include "FMTlogger.h"

int main()
{
	#if defined _MSC_VER
		const std::string modellocation = "../Examples/Models/Jesus_land/";
	#else
		const std::string modellocation = "Examples/Models/Jesus_land/";
	#endif
	const std::string	primarylocation = modellocation + "Jesus_land.pri";
	Parser::FMTmodelparser mparser;
	const std::vector<std::string>scenarios(1, "LP");
	const std::vector<Models::FMTmodel> models = mparser.readproject(primarylocation, scenarios);
	Models::FMTsesmodel simulationmodel(models.at(0));
	const std::vector<std::vector<Core::FMTschedule>> schedules = mparser.readschedules(primarylocation, models);
	std::vector<Spatial::FMTspatialaction> spactions;
	for (const auto action : simulationmodel.getactions())
		{
		Spatial::FMTspatialaction spaction(action);
		spaction.neighbors = std::vector<std::string>(1, "coupetotale");
		spaction.green_up = 1;
		spaction.adjacency = 1;
		spaction.minimal_size = 1;
		spaction.maximal_size = 2;
		spaction.neighbors_size = 4;
		spactions.push_back(spaction);
		}
	simulationmodel.setspactions(spactions);
	std::vector<Core::FMTtransition> strans;
	for (const auto& tran : simulationmodel.gettransitions())
		{
			strans.push_back(tran.single());
		}
	simulationmodel.settransitions(strans);
	Parser::FMTareaparser areaparser;
	const std::string rastpath = modellocation + "rasters/";
	const std::string agerast = rastpath + "AGE.tif";
	std::vector<std::string> themesrast;
	for (int i = 1; i <= simulationmodel.getthemes().size(); i++)
		{
		themesrast.push_back(rastpath + "THEME" + std::to_string(i) + ".tif");
		}
	Spatial::FMTforest initialforestmap = areaparser.readrasters(simulationmodel.getthemes(), themesrast, agerast, 1, 0.0001);
	simulationmodel.setinitialmapping(initialforestmap);
	for (int period = 0; period < 10; ++period)
		{
		for (const auto& t : simulationmodel.simulate(schedules.at(0).at(period), false, 1))
			{
			Logging::FMTlogger() << t.first << " " << t.second << " ";
			}
		Logging::FMTlogger() << "\n";
		}
	return 0;
}
        
