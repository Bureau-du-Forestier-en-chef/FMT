#include <vector>
#include "FMTsesmodel.hpp"
#include "FMTmodelparser.hpp"
#include "FMTareaparser.hpp"
#include "FMTversion.hpp"
#include "FMTlogger.hpp"
#include "FMTforest.hpp"
#include "FMTspatialschedule.hpp"
#include "FMToutput.hpp"
#include "FMTyieldmodel.hpp"

int main()
{
	Logging::FMTlogger().logstamp();
	const std::string modellocation = "E:/Projects/BFEC_MachineLearning/FMT/Examples/Models/TWD_land/";
	const std::string	primarylocation = modellocation + "TWD_land.pri";
	Parser::FMTmodelparser mparser;
	const std::vector<std::string>scenarios(1, "Predictors");
	const std::vector<Models::FMTmodel> models = mparser.readproject(primarylocation, scenarios);
	Models::FMTsesmodel simulationmodel(models.at(0));
	const std::vector<std::vector<Core::FMTschedule>> schedules = mparser.readschedules(primarylocation, models);
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
	const size_t greedysearch = 10;
	for (int period = 0; period < 10; ++period)
	{
		for (const auto& t : simulationmodel.greedyreferencebuild(schedules.at(0).at(period), greedysearch))
		{
			Logging::FMTlogger() << t.first << " " << t.second << " ";
		}
		Logging::FMTlogger() << "\n";
	}
	Core::FMToutput spatialoutput;
	for (const Core::FMToutput& output : simulationmodel.getoutputs())
	{
		if (output.getname() == "TEST")
		{
			spatialoutput = output;
			break;
		}
	}
	//simulationmodel.solve();
	for (int period = 1; period < 11; ++period)
	{
		Logging::FMTlogger() << "output value " << simulationmodel.getoutput(spatialoutput, period, Core::FMToutputlevel::totalonly).at("Total") << " at period " << period << "\n";
	}
	return 0;
}

