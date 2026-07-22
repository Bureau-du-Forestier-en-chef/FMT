#include <vector>
#ifdef FMTWITHGDAL
	#include "FMTSesModel.h"
	#include "FMTModelParser.h"
	#include "FMTAreaParser.h"
	#include "FMTDefaultLogger.h"
	#include "FMTForest.h"
	#include "FMTOutput.h"
	#include "FMTFreeExceptionHandler.h"
	#include "FMTGCBMTransition.h"
	#include "FMTScheduleParser.h"
#endif

void setMapping(const std::string& rastpath, Models::FMTSesModel& model)
{
	const std::string agerast = rastpath + "AGE.tif";
	std::vector<std::string> themesrast;
	for (int i = 1; i <= model.getThemes().size(); i++)
	{
		themesrast.push_back(rastpath + "THEME" + std::to_string(i) + ".tif");
	}
	Parser::FMTAreaParser areaparser;
	const Spatial::FMTForest initialforestmap = areaparser.readRasters(model.getThemes(), themesrast, agerast, 1, 0.0001);
	model.setInitialMapping(initialforestmap);
}

int main(int argc, char* argv[])
{
#ifdef FMTWITHGDAL
	Logging::FMTDefaultLogger().logStamp();
	std::string modellocation;
	std::string primarylocation;
	std::string scenario;
	std::vector<std::string>spatialOutputs;
	int length;
	if (argc>1)
	{
		const std::string vals = argv[1];
		std::vector<std::string>results;
		boost::split(results, vals, boost::is_any_of("|"));
		modellocation = results.at(0);
		primarylocation = modellocation + results.at(1);
		scenario = results.at(2);
		std::vector<std::string>spatialOutputs;
		boost::split(spatialOutputs, std::string(argv[1]), boost::is_any_of("|"));
		length = std::stoi(argv[3]);
	}else {
		modellocation = "C:\\Users\\admlocal\\Desktop\\06471\\";
		primarylocation = modellocation + "PC_9981_U06471_2028_MODB01.pri";
		scenario = "TBE_V3";
		length = 30;
		spatialOutputs = { "OVOLTOTREC"};
		/*modellocation = "T:/Donnees/02_Courant/07_Outil_moyen_methode/01_Entretien_developpement/Interne/FMT/Entretien/Modeles_test/CC_V2/20251016/";
		primarylocation = modellocation + "Mod_cc_v2.pri";
		scenario = "ROOT";
		length = 5;
		spatialOutputs = { "OVOLTOTREC" };*/
	}
	const std::string outdir = "../../tests/Spatial_doplanning/";
	Parser::FMTModelParser mparser;
	mparser.setDebugLogger();
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
	errors.push_back(Exception::FMTexc::FMToutofrangeyield);
	errors.push_back(Exception::FMTexc::FMTdeathwithlock);
	errors.push_back(Exception::FMTexc::FMTempty_schedules);
	mparser.setErrorsToWarnings(errors);
	mparser.setMaxWarningsBeforeSilenced(10000000);
	const std::vector<std::string>scenarios(1, scenario);
	const std::vector<Models::FMTModel> models = mparser.readproject(primarylocation, scenarios);
	Models::FMTSesModel simulationmodel(models.at(0));
	const std::vector<std::vector<Core::FMTSchedule>> schedules = mparser.readschedules(primarylocation, models);
	std::vector<Core::FMTTransition> strans;
	for (const auto& tran : simulationmodel.getTransitions())
	{
		strans.push_back(tran.single());
	}
	simulationmodel.setTransitions(strans);
	const std::string rastpath = modellocation + "rasters/";
	setMapping(rastpath, simulationmodel);
	//mparser.write(simulationmodel, "D:/test/");
	simulationmodel.setParameter(Models::FMTintmodelparameters::LENGTH, length);
	simulationmodel.setParameter(Models::FMTintmodelparameters::NUMBER_OF_ITERATIONS, 30);
	simulationmodel.setParameter(Models::FMTboolmodelparameters::FORCE_PARTIAL_BUILD, true);
	simulationmodel.setParameter(Models::FMTboolmodelparameters::POSTSOLVE, true);
	simulationmodel.doPlanning(false, schedules.at(0));
	simulationmodel.logConstraintsInfeasibilities();
	const Spatial::FMTSpatialSchedule& SPATIAL_SCHEDULE = simulationmodel.getSpSchedule();
	const auto test = simulationmodel.getSchedule(false);
	Parser::FMTScheduleParser scheduleParser;
	scheduleParser.write(test, outdir + "schedules.seq");

	/*Parser::FMTAreaParser areaParser;
	for (int period = 1; period <= length; ++period)
	{
		const std::string NAME = outdir + "PERIOD_" + std::to_string(period) + "_";
		std::vector<std::string> themesrast;
		for (int i = 1; i <= simulationmodel.getthemes().size(); i++)
		{
			themesrast.push_back(NAME + "THEME" + std::to_string(i) + ".tif");
		}
		areaParser.writeForest(SPATIAL_SCHEDULE.getForestPeriod(period), simulationmodel.getthemes(), themesrast, NAME + "AGE.tif", NAME + "LOCK.tif");
	}*/
	Parser::FMTAreaParser areaParser;
	std::vector<Core::FMTTheme>selected(1, simulationmodel.getThemes().at(2));
	areaParser.writeDisturbances(outdir, SPATIAL_SCHEDULE, simulationmodel.getactions(), selected, length);
	for (const Core::FMTOutput& OUTOUT : simulationmodel.getOutputs())
		{
		if (std::find(spatialOutputs.begin(), spatialOutputs.end(), OUTOUT.getName())!= spatialOutputs.end())
			{
			for (int period = 1; period <= length;++period)
				{
				const std::string NAME = outdir + "PERIOD_"+std::to_string(period)+"_" + OUTOUT.getName() + ".tif";
				const double TOTAL_VALUE = simulationmodel.getOutput(OUTOUT, period, Core::FMToutputlevel::totalonly)["Total"];
				double FULL_VALUE = 0;
				for (const auto& STANDARD : simulationmodel.getOutput(OUTOUT, period, Core::FMToutputlevel::standard))
				{
					std::cout << STANDARD.first << " " << STANDARD.second << "\n";
					if (STANDARD.first=="Total")
					{
						FULL_VALUE = STANDARD.second;
					}
				}
				
				const Spatial::FMTLayer<double> RESULT = simulationmodel.getSpatialOutput(OUTOUT, period);
				double spatialResult = 0;
				for (const auto& CELL : RESULT)
					{
					spatialResult += (CELL.second);
					}
				std::cout <<"NON SPATIAL TOTAL "<< TOTAL_VALUE<<" NON SPATIAL THEMATIC "<< FULL_VALUE << " SPATIAL " << spatialResult << "\n";
				if (std::abs(spatialResult - FULL_VALUE)>1)
					{
					Exception::FMTFreeExceptionHandler().raise(Exception::FMTexc::FMTfunctionfailed,
						"On output "+ NAME+" "+std::to_string(FULL_VALUE)+" vs "+ std::to_string(spatialResult),
						"Spatial_doplanning", __LINE__, primarylocation);
					}
				areaParser.writeLayer(RESULT, NAME);
				/*const std::map<std::string, double>SAFE = SPATIAL_SCHEDULE.getSafeOutput(simulationmodel, OUTOUT, period, Core::FMToutputlevel::totalonly);
				for (const auto& OUTPUT : SAFE)
				{
					std::cout << "SAFE " << OUTPUT.first << " " << OUTPUT.second << "\n";
				}*/
				}
			
			}
		}


#endif
	return 0;
}


