#include <vector>
#ifdef FMTWITHGDAL
#include "FMTSesModel.h"
#include "FMTModelParser.h"
#include "FMTAreaParser.h"
#include "FMTVersion.h"
#include "FMTDefaultLogger.h"
#include "FMTForest.h"
#include "FMTSpatialSchedule.h"
#include "FMTTransitionParser.h"
#include "FMTGCBMTransition.h"
#include "FMTFreeExceptionHandler.h"
#include "FMTOutput.h"

#endif

int main()
{
#ifdef FMTWITHGDAL
	Logging::FMTDefaultLogger().logStamp();
	const std::string modellocation = "../../../../Examples/Models/TWD_land/";
	const std::string	primarylocation = modellocation + "TWD_land.pri";
	const std::string outdir = "../../tests/Spatialyexplicitsimulation_doplanning/";
	Parser::FMTModelParser mparser;
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
	errors.push_back(Exception::FMTexc::FMTinvalid_geometry);
	mparser.setErrorsToWarnings(errors);
	const std::vector<std::string>scenarios(1, "Spatial");
	const std::vector<Models::FMTModel> models = mparser.readproject(primarylocation, scenarios);
	Models::FMTSesModel simulationmodel(models.at(0));
	const std::vector<std::vector<Core::FMTSchedule>> schedules = mparser.readschedules(primarylocation, models);
	std::vector<Core::FMTTransition> strans;
	for (const auto& tran : simulationmodel.getTransitions())
		{
			strans.push_back(tran.single());
		}
	simulationmodel.setTransitions(strans);
	Parser::FMTAreaParser areaparser;
	const std::string shpfile = modellocation + "Carte/TWD_land.shp";
	Spatial::FMTForest initialforestmap = areaparser.vectormaptoFMTforest(shpfile,380,simulationmodel.getThemes(),"AGE","SUPERFICIE", 1, 0.0001);
	simulationmodel.setInitialMapping(initialforestmap);
	simulationmodel.setParameter(Models::FMTintmodelparameters::LENGTH,10);
	simulationmodel.setParameter(Models::FMTintmodelparameters::NUMBER_OF_ITERATIONS, 10);
	simulationmodel.setParameter(Models::FMTboolmodelparameters::FORCE_PARTIAL_BUILD, true);
	simulationmodel.setParameter(Models::FMTboolmodelparameters::POSTSOLVE, true);
	simulationmodel.doPlanning(false,schedules.at(0));
	Core::FMTOutput spatialoutput;
	Core::FMTOutput sumoutputs;
	std::vector<Core::FMTOutput>outputs;
	for (const Core::FMTOutput& output : simulationmodel.getOutputs())
	{
		if (output.getName() == "OSUPREC")
		{
			spatialoutput = output;
			outputs.push_back(output);
		}else if (output.getName() =="COUPE2PEU")
		{
			sumoutputs = output;
		}
	}
	const double thevalue = simulationmodel.getOutput(sumoutputs,1, Core::FMToutputlevel::totalonly).at("Total");
	const double globalvalue = simulationmodel.getOutput(spatialoutput, 1, Core::FMToutputlevel::totalonly).at("Total");
	Logging::FMTDefaultLogger() << "outvalues " << thevalue << "\n";
	Logging::FMTDefaultLogger() << "outglobalvalues " << globalvalue << "\n";
	if (thevalue < globalvalue)
	{
		Exception::FMTFreeExceptionHandler().raise(Exception::FMTexc::FMTfunctionfailed, "Wrong value",
			"presolvetest", __LINE__, primarylocation);
	}
	mparser.writeResults(simulationmodel, outputs, 1, 10, outdir + "test.csv", Core::FMToutputlevel::totalonly);
	const Spatial::FMTSpatialSchedule spatialsolution = simulationmodel.getSpSchedule();
	const std::string stats = spatialsolution.getPatchStats(simulationmodel.getactions());
	std::vector<std::string>results;
	boost::split(results, stats, boost::is_any_of("\n"));
	for (const std::string& result : results)
	{
		std::vector<std::string>spresults;
		boost::split(spresults, stats, boost::is_any_of(" "));
		if (spresults.at(1)=="COUPETOTALE" && spresults.at(1) !="2")
			{
			Exception::FMTFreeExceptionHandler().raise(Exception::FMTexc::FMTfunctionfailed, "Wrong Patch size",
				"", __LINE__, primarylocation);
			}
	}

	Logging::FMTDefaultLogger() <<"xsize : "<< spatialsolution.getXSize() << "\n";
	Logging::FMTDefaultLogger() << "ysize : " << spatialsolution.getYSize() << "\n";
	for (int period = 1; period <= 10; ++period)
		{
		for (const std::pair<Spatial::FMTCoordinate,double>& value : spatialsolution.getOutputByCoordinate(simulationmodel, spatialoutput, period))
				{
				Logging::FMTDefaultLogger() << "period: " << period << " X: " << value.first.getX() << " Y: " << value.first.getY() << " value: " << value.second << "\n";
				}
		}
	

#endif
	return 0;
}
        
