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
#include "FMTfreeexceptionhandler.h"
#include "FMToutput.h"

#endif

int main()
{
#ifdef FMTWITHGDAL
	Logging::FMTdefaultlogger().logStamp();
	const std::string modellocation = "../../../../Examples/Models/TWD_land/";
	const std::string	primarylocation = modellocation + "TWD_land.pri";
	const std::string outdir = "../../tests/Spatialyexplicitsimulation_doplanning/";
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
	errors.push_back(Exception::FMTexc::FMTinvalid_geometry);
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
	const std::string shpfile = modellocation + "Carte/TWD_land.shp";
	Spatial::FMTforest initialforestmap = areaparser.vectormaptoFMTforest(shpfile,380,simulationmodel.getThemes(),"AGE","SUPERFICIE", 1, 0.0001);
	simulationmodel.setInitialMapping(initialforestmap);
	simulationmodel.setParameter(Models::FMTintmodelparameters::LENGTH,10);
	simulationmodel.setParameter(Models::FMTintmodelparameters::NUMBER_OF_ITERATIONS, 10);
	simulationmodel.setParameter(Models::FMTboolmodelparameters::FORCE_PARTIAL_BUILD, true);
	simulationmodel.setParameter(Models::FMTboolmodelparameters::POSTSOLVE, true);
	simulationmodel.doPlanning(false,schedules.at(0));
	Core::FMToutput spatialoutput;
	Core::FMToutput sumoutputs;
	std::vector<Core::FMToutput>outputs;
	for (const Core::FMToutput& output : simulationmodel.getOutputs())
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
	Logging::FMTdefaultlogger() << "outvalues " << thevalue << "\n";
	Logging::FMTdefaultlogger() << "outglobalvalues " << globalvalue << "\n";
	if (thevalue < globalvalue)
	{
		Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed, "Wrong value",
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
			Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed, "Wrong Patch size",
				"", __LINE__, primarylocation);
			}
	}

	Logging::FMTdefaultlogger() <<"xsize : "<< spatialsolution.getXSize() << "\n";
	Logging::FMTdefaultlogger() << "ysize : " << spatialsolution.getYSize() << "\n";
	for (int period = 1; period <= 10; ++period)
		{
		for (const std::pair<Spatial::FMTcoordinate,double>& value : spatialsolution.getOutputByCoordinate(simulationmodel, spatialoutput, period))
				{
				Logging::FMTdefaultlogger() << "period: " << period << " X: " << value.first.getX() << " Y: " << value.first.getY() << " value: " << value.second << "\n";
				}
		}
	

#endif
	return 0;
}
        
