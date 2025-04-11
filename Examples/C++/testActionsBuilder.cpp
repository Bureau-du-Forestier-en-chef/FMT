#include <vector>
#include "FMTmodel.h"
#include "FMTmodelparser.h"
#include "FMTscheduleparser.h"
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include "FMTdefaultlogger.h"
#include "FMTlpmodel.h"


int main(int argc, char* argv[])
{
		Logging::FMTdefaultlogger().logstamp();
		std::string PRIMARY_LOCATION;
		std::string SCENARIO;
		std::string TARGET_YIELD;
		std::string ACTION_NAME;
		const std::string OUTPUT_DIRECTORY = "../../tests/testActionsBuilder/";
		if (argc > 1) {
			const std::string vals = argv[1];
			std::vector<std::string>results;
			boost::split(results, vals, boost::is_any_of("|"));

			PRIMARY_LOCATION =  results.at(0);
			SCENARIO = results.at(1);
			TARGET_YIELD = argv[2];
			ACTION_NAME = argv[3];
			//OUTPUT_DIRECTORY = "../../tests/testActionsBuilder/";
		}
		else {
			PRIMARY_LOCATION = "T:/Donnees/02_Courant/07_Outil_moyen_methode/01_Entretien_developpement/Interne/FMT/Entretien/Modeles_test/Prototype_Dec2023_TBE/PC_7002071_UA08152_FINAL.pri";
			SCENARIO = "20_Regl_prov";
			TARGET_YIELD = "YV_S_MORT";
			ACTION_NAME = "TBE";
			//OUTPUT_DIRECTORY = "../outputs/";

		}
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
		//Models::FMTlpmodel optModel1(MODELS.at(0), Models::FMTsolverinterface::MOSEK);
		//optModel1.doplanning(false, SCHEDULES);
		//Create a addTheme function that add a theme and update all mask taking a vector of string and a default value
		//Detect Yields
		//Split Yields
		//Default mask for first yield
		//Next mask for second yield
		//Add action based on first yield and transition to second yield.
		//Create a schedule based on area detected yield age...
		const Models::FMTmodel BUILDED_MODEL = MODELS.at(0).buildAction(ACTION_NAME, TARGET_YIELD);
		ModelParser.writetoproject(OUTPUT_DIRECTORY + SCENARIO + ".pri", BUILDED_MODEL);
		Parser::FMTscheduleparser SCHEDULE_PARSER;
		const std::vector<Core::FMTschedule> NEW_SCHEDULE = BUILDED_MODEL.buildSchedule(*BUILDED_MODEL.getactions().begin(), SCHEDULES);
		SCHEDULE_PARSER.write(NEW_SCHEDULE, OUTPUT_DIRECTORY + SCENARIO + ".seq");
		const std::vector<std::string>ROOT(1, "ROOT");
		const std::vector<Models::FMTmodel> READMODELS = ModelParser.readproject(OUTPUT_DIRECTORY + SCENARIO + ".pri", ROOT);
		const std::vector<Core::FMTschedule>READSCHEDULE = ModelParser.readschedules(OUTPUT_DIRECTORY + SCENARIO + ".pri", READMODELS).at(0);
		//Models::FMTlpmodel optModel(READMODELS.at(0), Models::FMTsolverinterface::MOSEK);
		//optModel.doplanning(true);
	return 0;
}

