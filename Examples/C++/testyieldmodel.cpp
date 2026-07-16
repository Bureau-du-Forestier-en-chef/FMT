#ifdef FMTWITHONNXR
	#ifdef FMTWITHOSI
		#include <vector>
		#include "FMTsesmodel.h"
		#include "FMTmodelparser.h"
		#include "FMTareaparser.h"
		#include "FMTversion.h"
		#include "FMTdefaultlogger.h"
		#include "FMTforest.h"
		#include "FMTspatialschedule.h"
		#include "FMToutput.h"
		#include "FMTyieldmodel.h"
	#endif
#endif

int main()
{
#ifdef FMTWITHONNXR
#ifdef FMTWITHOSI
	Logging::FMTDefaultLogger().logStamp();
	const std::string modellocation = "../../../../Examples/Models/TWD_land/";
	const std::string	primarylocation =  modellocation + "TWD_land.pri";
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
	const std::vector<std::string>scenarios(1, "Predictors");
	const std::vector<Models::FMTmodel> models = mparser.readproject(primarylocation, scenarios);
	//mparser.write(models.at(0), "E:/Projects/BFEC_MachineLearning/test/");
	Models::FMTsesmodel simulationmodel(models.at(0));
	const std::vector<std::vector<Core::FMTSchedule>> schedules = mparser.readschedules(primarylocation, models);
	std::vector<Core::FMTTransition> strans;
	for (const auto& tran : simulationmodel.getTransitions())
	{
		strans.push_back(tran.single());
	}
	simulationmodel.setTransitions(strans);
	std::vector<Core::FMTConstraint>newconstraints;
	size_t id = 0;
	for (const Core::FMTConstraint& cnt : simulationmodel.getconstraints())
	{
		if (id<2)
		{
			newconstraints.push_back(cnt);
		}
		++id;
	}
	simulationmodel.setConstraints(newconstraints);
	Parser::FMTareaparser areaparser;
	const std::string rastpath =  modellocation + "rasters/";
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
		for (const auto& t : simulationmodel.greedyReferenceBuild(schedules.at(0).at(period), greedysearch))
		{
			Logging::FMTDefaultLogger() << t.first << " " << t.second << " ";
		}
		Logging::FMTDefaultLogger() << "\n";
	}
	std::vector<Core::FMTOutput> spatialoutput;
	for (const Core::FMTOutput& output : simulationmodel.getOutputs())
	{
		if (output.getName().find("_") != std::string::npos)
		{
			spatialoutput.push_back(output);
		}
	}

	mparser.writeResults(simulationmodel, spatialoutput, 1, 10, "../../tests/testyieldmodel/", Core::FMToutputlevel::totalonly, "CSV");

	//simulationmodel.solve();
	for (const Core::FMTOutput& output : spatialoutput)
	{
		for (int period = 1; period < 11; ++period)
		{
			Logging::FMTDefaultLogger() << "output value " << output.getName() << " " << simulationmodel.getOutput(output, period, Core::FMToutputlevel::totalonly).at("Total")/ 1814 << " at period " << period << "\n";
		}
	}
#endif
#endif
	return 0;
}

