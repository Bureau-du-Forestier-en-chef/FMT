#include <vector>
#ifdef FMTWITHGDAL
	#include "FMTsesmodel.h"
	#include "FMTmodelparser.h"
	#include "FMTareaparser.h"
	#include "FMTdefaultlogger.h"
	#include "FMTforest.h"
	#include "FMToutput.h"
	#include "FMTfreeexceptionhandler.h"
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
	const std::string vals = argv[1];
	std::vector<std::string>results;
	boost::split(results, vals, boost::is_any_of("|"));
	const std::string modellocation = results.at(0);
	const std::string primarylocation = modellocation + results.at(1);
	const std::string scenario = results.at(2);
	std::vector<std::string>spatialOutputs;
	boost::split(spatialOutputs, std::string(argv[1]), boost::is_any_of("|"));
	const int length = std::stoi(argv[3]);
	/*const std::string modellocation = "T:/Donnees/02_Courant/07_Outil_moyen_methode/01_Entretien_developpement/09_FMT/Modeles_test/Prototype_Dec2023_TBE/";
	const std::string primarylocation = modellocation + "PC_7002071_UA08152_FINAL.pri";
	const std::string scenario = "20_Regl_prov";
	const int length = 3;
	std::vector<std::string>spatialOutputs = {"OVOLTOTREC"};*/
	const std::string outdir = "../../tests/Spatial_doplanning/";
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
	simulationmodel.setparameter(Models::FMTintmodelparameters::NUMBER_OF_ITERATIONS, 10);
	simulationmodel.setparameter(Models::FMTboolmodelparameters::FORCE_PARTIAL_BUILD, true);
	simulationmodel.setparameter(Models::FMTboolmodelparameters::POSTSOLVE, true);
	simulationmodel.doplanning(false, schedules.at(0));
	//Parser::FMTareaparser areaParser;
	for (const Core::FMToutput& OUTOUT : simulationmodel.getoutputs())
		{
		if (std::find(spatialOutputs.begin(), spatialOutputs.end(), OUTOUT.getname())!= spatialOutputs.end())
			{
			for (int period = 1; period <= length;++period)
				{
				const std::string NAME = outdir + OUTOUT.getname() + "_" + std::to_string(period) + ".tif";
				const double FULL_VALUE = simulationmodel.getoutput(OUTOUT, period, Core::FMToutputlevel::totalonly)["Total"];
				std::cout << NAME << " " << FULL_VALUE << "\n";
				const Spatial::FMTlayer<double> RESULT = simulationmodel.getspatialoutput(OUTOUT, period);
				double spatialResult = 0;
				for (const auto& CELL : RESULT)
					{
					spatialResult += (CELL.second);
					}
				if (std::abs(spatialResult - FULL_VALUE)>1)
					{
					Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed,
						"On output "+ NAME+" "+std::to_string(FULL_VALUE)+" vs "+ std::to_string(spatialResult),
						"Spatial_doplanning", __LINE__, primarylocation);
					}
				//areaParser.writelayer(RESULT, NAME);
				}
			
			}
		}


#endif
	return 0;
}


