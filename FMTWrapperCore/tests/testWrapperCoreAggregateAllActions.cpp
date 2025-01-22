#ifdef FMTWITHOSI
	#include "FMTmodelparser.h"
	#include "FMTmodel.h"
	#include "FMTversion.h"
#endif
#include "FMTdefaultlogger.h"
#include "TransformationCore.h"
#include <boost/filesystem.hpp>

int main(int argc, char* argv[])
{
#ifdef FMTWITHOSI
	Logging::FMTdefaultlogger().logstamp();

	if (Version::FMTversion().hasfeature("OSI"))
	{
		const std::string OUTPUT_DIRECTORY = "../../tests/testWrapperCoreaggregateAllActions";
		std::string pathPri;
		std::string priName;
		std::vector<std::string> aggregates;
		std::vector<std::string> order;
		std::string scenarioName;
		if (argc < 2)
		{
			pathPri = "T:/Donnees/02_Courant/07_Outil_moyen_methode/01_Entretien_developpement/09_FMT/Modeles_test/Prototype_Dec2023_TBE/PC_7002071_UA08152_FINAL.pri";
			priName = "PC_7002071_UA08152_FINAL";
			scenarioName = "20_Regl_prov";
			aggregates = { "REGAFIN", "REGAPAR", "REGAEDU", "REGAREG", "ATBEMORT", "ATBERETARD", "ASNAT", "AECHEC" };
			order = { "REGAFIN", "REGAPAR", "REGAEDU", "REGAREG", "ATBEMORT", "ATBERETARD", "ASNAT", "AECHEC" };
		}
		else 
		{
			std::vector<std::string> result;
			boost::split(result, argv[1], boost::is_any_of("|"));
			pathPri = result.at(0);
			scenarioName = result.at(1);
			boost::split(aggregates, argv[2], boost::is_any_of("|"));
			//boost::split(order, argv[3], boost::is_any_of("|"));

		}

		const std::vector<std::string>SCENARIOS(1, scenarioName);
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

		const std::vector<Models::FMTmodel> MODELS =ModelParser.readproject(pathPri, SCENARIOS);
		Models::FMTmodel aggregatedModel = FMTWrapperCore::Transformation::aggregateAllActions(MODELS.at(0), aggregates, order, pathPri, OUTPUT_DIRECTORY, scenarioName, priName);

		// On vérifie si on a moins d'action que dans le nouveau
		std::vector<Core::FMTaction> actions = MODELS.at(0).getactions();
		std::vector<Core::FMTaction> aggregatedActions = aggregatedModel.getactions();
		if (actions.size() <= aggregatedActions.size())
		{
			throw Exception::FMTexc::FMTinvalid_aggregate;
		}
		// on vérifie s'il y a bien écrit le modèle
		if (!boost::filesystem::exists(OUTPUT_DIRECTORY + "/" + scenarioName + ".pri"))
		{
			throw Exception::FMTexc::FMTinvalid_aggregate;
		}
	}
	else {
	Logging::FMTdefaultlogger() << "FMT needs to be compiled with OSI" << "\n";
	}
#endif
	return 0;

}
