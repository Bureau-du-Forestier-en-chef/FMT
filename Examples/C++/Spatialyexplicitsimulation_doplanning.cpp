#include <vector>
#ifdef FMTWITHGDAL
#include "FMTsesmodel.hpp"
#include "FMTmodelparser.hpp"
#include "FMTareaparser.hpp"
#include "FMTversion.hpp"
#include "FMTlogger.hpp"
#include "FMTforest.hpp"
#include "FMTspatialschedule.hpp"
#include "FMTtransitionparser.hpp"
#include "FMTGCBMtransition.hpp"
#include "FMToutput.hpp"
#endif

int main()
{
#ifdef FMTWITHGDAL
	Logging::FMTlogger().logstamp();
	const std::string modellocation = "../../../../Examples/Models/TWD_land/";
	const std::string	primarylocation = modellocation + "TWD_land.pri";
	const std::string outdir = "tests/";
	Parser::FMTmodelparser mparser;
	const std::vector<std::string>scenarios(1, "Spatial");
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
	const std::string shpfile = modellocation + "Carte/TWD_land.shp";
	Spatial::FMTforest initialforestmap = areaparser.vectormaptoFMTforest(shpfile,380,simulationmodel.getthemes(),"AGE","SUPERFICIE", 1, 0.0001);
	simulationmodel.setinitialmapping(initialforestmap);
	simulationmodel.setparameter(Models::FMTintmodelparameters::LENGTH,10);
	simulationmodel.setparameter(Models::FMTintmodelparameters::NUMBER_OF_ITERATIONS, 10);
	simulationmodel.setparameter(Models::FMTboolmodelparameters::FORCE_PARTIAL_BUILD, true);
	simulationmodel.setparameter(Models::FMTboolmodelparameters::POSTSOLVE, true);
	simulationmodel.doplanning(false,schedules.at(0));
	Core::FMToutput spatialoutput;
	for (const Core::FMToutput& output : simulationmodel.getoutputs())
	{
		if (output.getname() == "OSUPREC")
		{
			spatialoutput = output;
		}
	}
	const Spatial::FMTspatialschedule spatialsolution = simulationmodel.getspschedule();
	std::cout<<"sp"<<std::endl;
	Logging::FMTlogger() <<"xsize : "<< spatialsolution.GetXSize() << "\n";
	Logging::FMTlogger() << "ysize : " << spatialsolution.GetYSize() << "\n";
	for (int period = 1; period <= 10; ++period)
		{
		for (const std::pair<Spatial::FMTcoordinate,double>& value : spatialsolution.getoutputbycoordinate(simulationmodel, spatialoutput, period))
				{
				Logging::FMTlogger() << "period: " << period << " X: " << value.first.getx() << " Y: " << value.first.gety() << " value: " << value.second << "\n";
				}
		}
	const std::vector<Core::FMTaction>actions = simulationmodel.getactions();
	const std::vector<Core::FMTtheme>growththeme(1,simulationmodel.getthemes().at(1));
	Parser::FMTtransitionparser transitionparser;
	for (int period = 1; period <= 10; ++period)
		{
		const std::vector<Core::FMTGCBMtransition>transitions = areaparser.writedisturbances(outdir,
			spatialsolution,
			actions,
			growththeme, period);
		transitionparser.writeGCBM(transitions, outdir + "transition" + std::to_string(period) + ".xml");
		}
#endif
	return 0;
}
        