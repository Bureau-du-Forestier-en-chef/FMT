#include <vector>
#ifdef FMTWITHGDAL
	#include "FMTsesmodel.h"
	#include "FMTmodelparser.h"
	#include "FMTareaparser.h"
	#include "FMTdefaultlogger.h"
	#include "FMTforest.h"
	#include "FMToutput.h"
	#include "FMTfreeexceptionhandler.h"
	#include "FMTGCBMtransition.h"
#endif

void setMapping(const std::string& rastpath, Models::FMTsesmodel& model)
{
	const std::string agerast = rastpath + "AGE.tif";
	std::vector<std::string> themesrast;
	for (int i = 1; i <= model.getthemes().size(); i++)
	{
		themesrast.push_back(rastpath + "THEME" + std::to_string(i) + ".tif");
	}
	Parser::FMTareaparser areaparser;
	const Spatial::FMTforest initialforestmap = areaparser.readrasters(model.getthemes(), themesrast, agerast, 1, 0.0001);
	model.setinitialmapping(initialforestmap);
}

int main(int argc, char* argv[])
{
#ifdef FMTWITHGDAL
	Logging::FMTdefaultlogger().logstamp();
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
		modellocation = "T:/Donnees/02_Courant/07_Outil_moyen_methode/01_Entretien_developpement/Interne/FMT/Entretien/Modeles_test/Weight/";
		primarylocation = modellocation + "PC_9964_U08451_2028_MODB01.pri";
		scenario = "120_RegProv_apsp_AGG";
		length = 3;
		spatialOutputs = { "OVOLTOTREC"};
		/*modellocation = "T:/Donnees/02_Courant/07_Outil_moyen_methode/01_Entretien_developpement/Interne/FMT/Entretien/Modeles_test/CC_V2/20251016/";
		primarylocation = modellocation + "Mod_cc_v2.pri";
		scenario = "ROOT";
		length = 5;
		spatialOutputs = { "OVOLTOTREC" };*/
	}
	const std::string outdir = "../../tests/Spatial_doplanning/";
	Parser::FMTmodelparser mparser;
	mparser.setdebuglogger();
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
	mparser.seterrorstowarnings(errors);
	const std::vector<std::string>scenarios(1, scenario);
	const std::vector<Models::FMTmodel> models = mparser.readproject(primarylocation, scenarios);
	Models::FMTsesmodel simulationmodel(models.at(0));
	const std::vector<std::vector<Core::FMTschedule>> schedules = mparser.readschedules(primarylocation, models);
	std::vector<Core::FMTtransition> strans;
	for (const auto& tran : simulationmodel.gettransitions())
	{
		strans.push_back(tran.single());
	}
	simulationmodel.settransitions(strans);
	const std::string rastpath = modellocation + "rasters/";
	setMapping(rastpath, simulationmodel);
	simulationmodel.setparameter(Models::FMTintmodelparameters::LENGTH, length);
	simulationmodel.setparameter(Models::FMTintmodelparameters::NUMBER_OF_ITERATIONS, 1);
	simulationmodel.setparameter(Models::FMTboolmodelparameters::FORCE_PARTIAL_BUILD, true);
	simulationmodel.setparameter(Models::FMTboolmodelparameters::POSTSOLVE, true);
	simulationmodel.doplanning(false, schedules.at(0));
	simulationmodel.LogConstraintsInfeasibilities();
	const Spatial::FMTSpatialSchedule& SPATIAL_SCHEDULE = simulationmodel.getspschedule();
	/*Parser::FMTareaparser areaParser;
	for (int period = 1; period <= length; ++period)
	{
		const std::string NAME = outdir + "PERIOD_" + std::to_string(period) + "_";
		std::vector<std::string> themesrast;
		for (int i = 1; i <= simulationmodel.getthemes().size(); i++)
		{
			themesrast.push_back(NAME + "THEME" + std::to_string(i) + ".tif");
		}
		areaParser.writeforest(SPATIAL_SCHEDULE.getforestperiod(period), simulationmodel.getthemes(), themesrast, NAME + "AGE.tif", NAME + "LOCK.tif");
	}*/
	Parser::FMTareaparser areaParser;
	std::vector<Core::FMTtheme>selected(1, simulationmodel.getthemes().at(7));
	areaParser.writedisturbances(outdir, SPATIAL_SCHEDULE, simulationmodel.getactions(), selected, 1);
	for (const Core::FMToutput& OUTOUT : simulationmodel.getoutputs())
		{
		if (std::find(spatialOutputs.begin(), spatialOutputs.end(), OUTOUT.getname())!= spatialOutputs.end())
			{
			for (int period = 1; period <= length;++period)
				{
				const std::string NAME = outdir + "PERIOD_"+std::to_string(period)+"_" + OUTOUT.getname() + ".tif";
				const double TOTAL_VALUE = simulationmodel.getoutput(OUTOUT, period, Core::FMToutputlevel::totalonly)["Total"];
				double FULL_VALUE = 0;
				for (const auto& STANDARD : simulationmodel.getoutput(OUTOUT, period, Core::FMToutputlevel::standard))
				{
					std::cout << STANDARD.first << " " << STANDARD.second << "\n";
					if (STANDARD.first=="Total")
					{
						FULL_VALUE = STANDARD.second;
					}
				}
				
				const Spatial::FMTlayer<double> RESULT = simulationmodel.getspatialoutput(OUTOUT, period);
				double spatialResult = 0;
				for (const auto& CELL : RESULT)
					{
					spatialResult += (CELL.second);
					}
				std::cout <<"NON SPATIAL TOTAL "<< TOTAL_VALUE<<" NON SPATIAL THEMATIC "<< FULL_VALUE << " SPATIAL " << spatialResult << "\n";
				if (std::abs(spatialResult - FULL_VALUE)>1)
					{
					Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed,
						"On output "+ NAME+" "+std::to_string(FULL_VALUE)+" vs "+ std::to_string(spatialResult),
						"Spatial_doplanning", __LINE__, primarylocation);
					}
				areaParser.writelayer(RESULT, NAME);
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


