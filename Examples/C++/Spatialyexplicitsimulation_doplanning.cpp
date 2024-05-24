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
	Logging::FMTdefaultlogger().logstamp();
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
	mparser.seterrorstowarnings(errors);
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
	Core::FMToutput sumoutputs;
	std::vector<Core::FMToutput>outputs;
	for (const Core::FMToutput& output : simulationmodel.getoutputs())
	{
		if (output.getname() == "OSUPREC")
		{
			spatialoutput = output;
			outputs.push_back(output);
		}else if (output.getname() =="COUPE2PEU")
		{
			sumoutputs = output;
		}
	}
	const double thevalue = simulationmodel.getoutput(sumoutputs,1, Core::FMToutputlevel::totalonly).at("Total");
	const double globalvalue = simulationmodel.getoutput(spatialoutput, 1, Core::FMToutputlevel::totalonly).at("Total");
	Logging::FMTdefaultlogger() << "outvalues " << thevalue << "\n";
	Logging::FMTdefaultlogger() << "outglobalvalues " << globalvalue << "\n";
	if (thevalue < globalvalue)
	{
		Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed, "Wrong value",
			"presolvetest", __LINE__, primarylocation);
	}
	mparser.writeresults(simulationmodel, outputs, 1, 10, outdir + "test.csv", Core::FMToutputlevel::totalonly);
	const Spatial::FMTspatialschedule spatialsolution = simulationmodel.getspschedule();
	const std::string stats = spatialsolution.getpatchstats(simulationmodel.getactions());
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

	Logging::FMTdefaultlogger() <<"xsize : "<< spatialsolution.GetXSize() << "\n";
	Logging::FMTdefaultlogger() << "ysize : " << spatialsolution.GetYSize() << "\n";
	for (int period = 1; period <= 10; ++period)
		{
		for (const std::pair<Spatial::FMTcoordinate,double>& value : spatialsolution.getoutputbycoordinate(simulationmodel, spatialoutput, period))
				{
				Logging::FMTdefaultlogger() << "period: " << period << " X: " << value.first.getx() << " Y: " << value.first.gety() << " value: " << value.second << "\n";
				}
		}
	

#endif
	return 0;
}
        
