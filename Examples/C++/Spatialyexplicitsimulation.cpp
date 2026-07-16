#include <vector>
#ifdef FMTWITHGDAL
#include "FMTsesmodel.h"
#include "FMTmodelparser.h"
#include "FMTareaparser.h"
#include "FMTversion.h"
#include "FMTdefaultlogger.h"
#include "FMTforest.h"
#include "FMTspatialschedule.h"
#include "FMTtransitionparser.h"
#include "FMTGCBMtransition.h"
#include "FMToutput.h"
#endif

int main()
{
#ifdef FMTWITHGDAL
	Logging::FMTDefaultLogger().logStamp();
	const std::string modellocation =  "../../../../Examples/Models/TWD_land/";
	const std::string	primarylocation = modellocation + "TWD_land.pri";
	const std::string outdir = "../../tests/Spatialyexplicitsimulation/";
	Parser::FMTmodelparser mparser;
	std::vector<Exception::FMTexc>errors;
	errors.push_back(Exception::FMTexc::FMTmissingyield);
	errors.push_back(Exception::FMTexc::FMToutput_missing_operator);
	errors.push_back(Exception::FMTexc::FMToutput_too_much_operator);
	errors.push_back(Exception::FMTexc::FMTinvalidyield_number);
	errors.push_back(Exception::FMTexc::FMTundefinedoutput_attribute);
	errors.push_back(Exception::FMTexc::FMToveridedyield);
	errors.push_back(Exception::FMTexc::FMTsourcetotarget_transition);
	errors.push_back(Exception::FMTexc::FMTsame_transitiontargets);
	errors.push_back(Exception::FMTexc::FMTunclosedforloop);
	mparser.setErrorsToWarnings(errors);
	const std::vector<std::string>scenarios(1, "Spatial");
	const std::vector<Models::FMTmodel> models = mparser.readproject(primarylocation, scenarios);
	Models::FMTsesmodel simulationmodel(models.at(0));
	const std::vector<std::vector<Core::FMTschedule>> schedules = mparser.readschedules(primarylocation, models);
	std::vector<Core::FMTtransition> strans;
	for (const auto& tran : simulationmodel.getTransitions())
		{
			strans.push_back(tran.single());
		}
	simulationmodel.setTransitions(strans);
	Parser::FMTareaparser areaparser;
	const std::string rastpath = modellocation + "rasters/";
	const std::string agerast = rastpath + "AGE.tif";
	std::vector<std::string> themesrast;
	for (int i = 1; i <= simulationmodel.getThemes().size(); i++)
		{
		themesrast.push_back(rastpath + "THEME" + std::to_string(i) + ".tif");
		}
	Spatial::FMTforest initialforestmap = areaparser.readRasters(simulationmodel.getThemes(), themesrast, agerast, 1, 0.0001);
	simulationmodel.setInitialMapping(initialforestmap);
	const size_t greedysearch = 10;
	for (int period = 0; period < 10; ++period)
		{
		Core::FMTschedule sche = schedules.at(0).at(period);
		for (const auto& t : simulationmodel.greedyReferenceBuild(sche, greedysearch))
			{
			Logging::FMTDefaultLogger()<< t.first << " " << t.second << " ";
			}
		Logging::FMTDefaultLogger() << "\n";
		}
	Core::FMToutput spatialoutput;
	for (const Core::FMToutput& output : simulationmodel.getOutputs())
	{
		if (output.getName() == "OSUPREC")
		{
			spatialoutput = output;
		}
	}
	const Spatial::FMTSpatialSchedule spatialsolution = simulationmodel.getSpSchedule();
	Logging::FMTDefaultLogger() << "xsize : " << spatialsolution.getXSize() << "\n";
	Logging::FMTDefaultLogger() << "ysize : " << spatialsolution.getYSize() << "\n";
	for (int period = 1; period <=30; ++period)
		{
		for (const std::pair<Spatial::FMTcoordinate, double>& value : spatialsolution.getOutputByCoordinate(simulationmodel, spatialoutput, period))
				{
				Logging::FMTDefaultLogger() << "period: " << period << " X: " << value.first.getX() << " Y: " << value.first.getY() << " value: " << value.second << "\n";
				}
		Logging::FMTDefaultLogger() << std::to_string(period) << " "<<simulationmodel.getOutput(spatialoutput, period, Core::FMToutputlevel::totalonly).at("Total")  <<"\n";
		}
	const std::vector<Core::FMTaction>actions = simulationmodel.getactions();
	const std::vector<Core::FMTtheme>growththeme(1,simulationmodel.getThemes().at(1));
	Parser::FMTtransitionparser transitionparser;
	for (int period = 1; period <= 10; ++period)
		{
		const std::vector<Core::FMTGCBMtransition>transitions = areaparser.writeDisturbances(outdir,
			spatialsolution,
			actions,
			growththeme, period);
		transitionparser.writeGCBM(transitions, outdir + "transition" + std::to_string(period) + ".xml");
		}
#endif
	return 0;
}


