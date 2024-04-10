#ifdef FMTWITHONNXR
	#ifdef FMTWITHOSI
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
	#endif
#endif

int main()
{
#ifdef FMTWITHONNXR
#ifdef FMTWITHOSI
	Logging::FMTlogger().logstamp();
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
	mparser.seterrorstowarnings(errors);
	const std::vector<std::string>scenarios(1, "Predictors");
	const std::vector<Models::FMTmodel> models = mparser.readproject(primarylocation, scenarios);
	//mparser.write(models.at(0), "E:/Projects/BFEC_MachineLearning/test/");
	Models::FMTsesmodel simulationmodel(models.at(0));
	const std::vector<std::vector<Core::FMTschedule>> schedules = mparser.readschedules(primarylocation, models);
	std::vector<Core::FMTtransition> strans;
	for (const auto& tran : simulationmodel.gettransitions())
	{
		strans.push_back(tran.single());
	}
	simulationmodel.settransitions(strans);
	std::vector<Core::FMTconstraint>newconstraints;
	size_t id = 0;
	for (const Core::FMTconstraint& cnt : simulationmodel.getconstraints())
	{
		if (id<2)
		{
			newconstraints.push_back(cnt);
		}
		++id;
	}
	simulationmodel.setconstraints(newconstraints);
	Parser::FMTareaparser areaparser;
	const std::string rastpath =  modellocation + "rasters/";
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
	std::vector<Core::FMToutput> spatialoutput;
	for (const Core::FMToutput& output : simulationmodel.getoutputs())
	{
		if (output.getname().find("_") != std::string::npos)
		{
			spatialoutput.push_back(output);
		}
	}

	mparser.writeresults(simulationmodel, spatialoutput, 1, 10, "../../tests/testyieldmodel/", Core::FMToutputlevel::totalonly, "CSV");

	//simulationmodel.solve();
	for (const Core::FMToutput& output : spatialoutput)
	{
		for (int period = 1; period < 11; ++period)
		{
			Logging::FMTlogger() << "output value " << output.getname() << " " << simulationmodel.getoutput(output, period, Core::FMToutputlevel::totalonly).at("Total")/ 1814 << " at period " << period << "\n";
		}
	}
#endif
#endif
	return 0;
}

