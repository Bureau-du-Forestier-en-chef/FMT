#include <vector>
#ifdef FMTWITHGDAL
	#include "FMTsesmodel.h"
	#include "FMTmodelparser.h"
	#include "FMTareaparser.h"
	#include "FMTlogger.h"
	#include "FMTforest.h"
	#include "FMToutput.h"
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
	Logging::FMTlogger().logstamp();
	const std::string vals = argv[1];
	std::vector<std::string>results;
	boost::split(results, vals, boost::is_any_of("|"));
	const std::string modellocation = results.at(0);
	const std::string primarylocation = results.at(1);
	const std::string scenario = results.at(2);
	const int length = std::stoi(argv[2]);
	/*const std::string modellocation = "T:/Donnees/02_Courant/07_Outil_moyen_methode/01_Entretien_developpement/09_FMT/Modeles_test/08152/";
	const std::string primarylocation = modellocation + "PC_7002071_UA08152_FINAL.pri";
	const std::string scenario = "02_Sc0_Rend_sout";
	const int length = 5;*/
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
#endif
	return 0;
}


