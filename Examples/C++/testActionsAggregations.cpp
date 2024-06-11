#include <vector>
#ifdef FMTWITHOSI
	#include "FMTmodel.h"
	#include "FMTlpmodel.h"
	#include "FMTsesmodel.h"
	#include "FMTmodelparser.h"
	#include "FMTareaparser.h"
	#include "FMTscheduleparser.h"
	#include <boost/algorithm/string.hpp>
	#include "FMTversion.h"
	#include <boost/filesystem.hpp>
	#include "FMTforest.h"
	#include "FMTtransition.h"
	#include "FMTGCBMtransition.h"
#endif
#include "FMTdefaultlogger.h"


int main(int argc, char* argv[])
{
#ifdef FMTWITHOSI
	Logging::FMTdefaultlogger().logstamp();

	if (Version::FMTversion().hasfeature("OSI"))
	{
		const std::string vals = argv[1];
		std::vector<std::string>results;
		boost::split(results, vals, boost::is_any_of("|"));
		const std::string PRIMARY_LOCATION =  results.at(0);
		const std::string CARTE = results.at(1);
		const std::string SCENARIO = results.at(2);
		std::vector<std::string>AGGREGATES;
		boost::split(AGGREGATES, argv[2], boost::is_any_of("|"));
		/*const std::string PRIMARY_LOCATION = "T:/Donnees/02_Courant/07_Outil_moyen_methode/01_Entretien_developpement/09_FMT/Modeles_test/Prototype_Dec2023_TBE/PC_7002071_UA08152_FINAL.pri";
		const std::string CARTE = "Carte/PC_7002071_UA_U08152.shp";
		const std::string SCENARIO = "20_Regl_prov";
		const std::vector<std::string>AGGREGATES = { "REGAFIN","REGAPAR","REGAEDU","REGAREG","ATBEMORT","ATBERETARD","ASNAT","AECHEC"};*/
		//const std::string OUTPUT_DIRECTORY = "outputs/";
		const std::string OUTPUT_DIRECTORY = "../../tests/testActionsAggregations/";
		Parser::FMTmodelparser ModelParser;
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
		errors.push_back(Exception::FMTexc::FMTinvalid_geometry);
		ModelParser.seterrorstowarnings(errors);
		const std::vector<std::string>SCENARIOS(1, SCENARIO);
		const std::vector<Models::FMTmodel> MODELS = ModelParser.readproject(PRIMARY_LOCATION, SCENARIOS);
		const std::vector<Core::FMTschedule>SCHEDULES = ModelParser.readschedules(PRIMARY_LOCATION, MODELS).at(0);
		Models::FMTlpmodel Optimization1(MODELS.at(0), Models::FMTsolverinterface::CLP);

		//ModelParser.writetoproject(OUTPUT_DIRECTORY + "test.pri", Optimization1);


		Optimization1.FMTmodel::setparameter(Models::FMTdblmodelparameters::TOLERANCE, 0.01);
		Optimization1.FMTmodel::setparameter(Models::FMTintmodelparameters::PRESOLVE_ITERATIONS, 10);
		Optimization1.FMTmodel::setparameter(Models::FMTintmodelparameters::LENGTH, std::min(static_cast<int>(SCHEDULES.size()), 3));
		Optimization1.doplanning(false, SCHEDULES);
		const Models::FMTmodel AGGREGATED_MODEL = MODELS.at(0).aggregateAllActions(AGGREGATES);
		ModelParser.writetoproject(OUTPUT_DIRECTORY + SCENARIO +".pri", AGGREGATED_MODEL);
		if (!SCHEDULES.empty())
			{
			Parser::FMTscheduleparser SCHEDULE_PARSER;
			const std::vector<Core::FMTschedule>NEWSCHEDULE = AGGREGATED_MODEL.aggregateSchedules(SCHEDULES);
			SCHEDULE_PARSER.write(NEWSCHEDULE, OUTPUT_DIRECTORY + SCENARIO + ".seq");
			}
		const std::vector<std::string>ROOT(1,"ROOT");
		const std::vector<Models::FMTmodel> READMODELS = ModelParser.readproject(OUTPUT_DIRECTORY + SCENARIO + ".pri", ROOT);
		const std::vector<Core::FMTschedule>READSCHEDULE = ModelParser.readschedules(OUTPUT_DIRECTORY + SCENARIO + ".pri", READMODELS).at(0);
		Models::FMTsesmodel Simulation(READMODELS.at(0));
		Parser::FMTareaparser areaParser;
		const boost::filesystem::path BASE_PATH = boost::filesystem::path(PRIMARY_LOCATION).parent_path();
		const std::string MAP_LOCATION = (BASE_PATH / boost::filesystem::path(CARTE)).string();
		const int SIZE = 2000;
		const Spatial::FMTforest FOREST = areaParser.vectormaptoFMTforest(MAP_LOCATION, SIZE, Simulation.getthemes(), "AGE", "SUPERFICIE", 1, 0.0001, "STANLOCK");
		Simulation.setinitialmapping(FOREST);
		std::vector<Core::FMTtransition>NewTransitions;
		for (const Core::FMTtransition& TRANSITION : Simulation.gettransitions())
			{
			NewTransitions.push_back(TRANSITION.single());
			}
		Simulation.settransitions(NewTransitions);
		Simulation.FMTmodel::setparameter(Models::FMTintmodelparameters::PRESOLVE_ITERATIONS, 10);
		Simulation.FMTmodel::setparameter(Models::FMTintmodelparameters::NUMBER_OF_ITERATIONS, 1);
		Simulation.FMTmodel::setparameter(Models::FMTintmodelparameters::LENGTH, std::min(static_cast<int>(READSCHEDULE.size()),3));
		Simulation.doplanning(true, READSCHEDULE);
		areaParser.writedisturbances(OUTPUT_DIRECTORY, Simulation.getspschedule(),
			Simulation.getactions(), Simulation.getthemes(), 1);

		
	}else {
		Logging::FMTdefaultlogger() << "FMT needs to be compiled with OSI" << "\n";
	}
#endif 
	return 0;
}

