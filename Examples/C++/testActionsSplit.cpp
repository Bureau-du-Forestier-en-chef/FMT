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
#endif
#include "FMTdefaultlogger.h"


int main(int argc, char* argv[])
{
#ifdef FMTWITHOSI
	Logging::FMTDefaultLogger().logStamp();

	if (Version::FMTversion().hasFeature("OSI"))
	{
		const std::string vals = argv[1];
		std::vector<std::string>results;
		boost::split(results, vals, boost::is_any_of("|"));
		const std::string PRIMARYm_location =  results.at(0);
		const std::string CARTE = results.at(1);
		const std::string SCENARIO = results.at(2);
		std::vector<std::string>SPLITTED;
		boost::split(SPLITTED, argv[2], boost::is_any_of("|"));
		std::vector<std::string>SPLITTED_MASK;
		boost::split(SPLITTED_MASK, argv[3], boost::is_any_of("|"));

		if (argc>1)
		{

		}
		else {

		}
		/*
		const std::string PRIMARYm_location = "T:/Donnees/02_Courant/07_Outil_moyen_methode/01_Entretien_developpement/09_FMT/Modeles_test/Prototype_Dec2023/PC_7002071_UA08152_FINAL.pri";
		const std::string CARTE = "Carte/PC_7002071_UA_U08152.shp";
		const std::string SCENARIO = "01_Regl_prov_apsp_1_6";
		const std::vector<std::string>SPLITTED = { "ACT","AEC"};
		const std::vector<std::string>SPLITTED_MASK = { "? ? ? ? ? ? ? ? ? ? ? ? ? UTA11 ? ? ? ? ? ?",
														"? ? ? ? ? ? ? ? ? ? ? ? ? !UTA11 ? ? ? ? ? ?" };*/
		//const std::string OUTPUT_DIRECTORY = "outputs/";
		const std::string OUTPUT_DIRECTORY = "../../tests/testActionsSplit/";
		Parser::FMTModelParser ModelParser;
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
		ModelParser.setErrorsToWarnings(errors);
		const std::vector<std::string>SCENARIOS(1, SCENARIO);
		const std::vector<Models::FMTModel> MODELS = ModelParser.readproject(PRIMARYm_location, SCENARIOS);
		const std::vector<Core::FMTSchedule>SCHEDULES = ModelParser.readschedules(PRIMARYm_location, MODELS).at(0);
		Models::FMTLpModel Optimization1(MODELS.at(0), Models::FMTsolverinterface::CLP);
		Optimization1.FMTModel::setParameter(Models::FMTdblmodelparameters::TOLERANCE, 0.01);
		Optimization1.FMTModel::setParameter(Models::FMTintmodelparameters::PRESOLVE_ITERATIONS, 10);
		Optimization1.FMTModel::setParameter(Models::FMTintmodelparameters::LENGTH, std::min(static_cast<int>(SCHEDULES.size()), 3));
		Optimization1.doPlanning(false, SCHEDULES);
		const Models::FMTModel SPLITTED_MODEL = MODELS.at(0).splitActions(SPLITTED, SPLITTED_MASK);
		ModelParser.writeToProject(OUTPUT_DIRECTORY + SCENARIO +".pri", SPLITTED_MODEL);
		if (!SCHEDULES.empty())
			{
			Parser::FMTScheduleParser SCHEDULE_PARSER;
			const std::vector<Core::FMTSchedule>NEWSCHEDULE = SPLITTED_MODEL.splitSchedules(SCHEDULES);
			SCHEDULE_PARSER.write(NEWSCHEDULE, OUTPUT_DIRECTORY + SCENARIO + ".seq");
			}
		const std::vector<std::string>ROOT(1,"ROOT");
		const std::vector<Models::FMTModel> READMODELS = ModelParser.readproject(OUTPUT_DIRECTORY + SCENARIO + ".pri", ROOT);
		const std::vector<Core::FMTSchedule>READSCHEDULE = ModelParser.readschedules(OUTPUT_DIRECTORY + SCENARIO + ".pri", READMODELS).at(0);
		Models::FMTSesModel Simulation(READMODELS.at(0));
		Parser::FMTAreaParser areaParser;
		const boost::filesystem::path BASE_PATH = boost::filesystem::path(PRIMARYm_location).parent_path();
		const std::string MAPm_location = (BASE_PATH / boost::filesystem::path(CARTE)).string();
		const int SIZE = 2000;
		const Spatial::FMTforest FOREST = areaParser.vectormaptoFMTforest(MAPm_location, SIZE, Simulation.getThemes(), "AGE", "SUPERFICIE", 1, 0.0001, "STANLOCK");
		Simulation.setInitialMapping(FOREST);
		std::vector<Core::FMTTransition>NewTransitions;
		for (const Core::FMTTransition& TRANSITION : Simulation.getTransitions())
			{
			NewTransitions.push_back(TRANSITION.single());
			}
		Simulation.setTransitions(NewTransitions);
		Simulation.FMTModel::setParameter(Models::FMTintmodelparameters::PRESOLVE_ITERATIONS, 10);
		Simulation.FMTModel::setParameter(Models::FMTintmodelparameters::NUMBER_OF_ITERATIONS, 1);
		Simulation.FMTModel::setParameter(Models::FMTintmodelparameters::LENGTH, std::min(static_cast<int>(READSCHEDULE.size()),3));
		Simulation.doPlanning(true, READSCHEDULE);
	}else {
		Logging::FMTDefaultLogger() << "FMT needs to be compiled with OSI" << "\n";
	}
#endif 
	return 0;
}

