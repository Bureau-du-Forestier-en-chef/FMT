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
		std::string primary_path;
		std::vector<std::string> aggregates;
		std::vector<std::string> order;
		std::string scenarioName;
		std::string output_scenario_name;
		if (argc < 2)
		{
			primary_path = "//Artemis/fecgeo/Donnees/02_Courant/07_Outil_moyen_methode/01_Entretien_developpement/Interne/FMT/Entretien/Modeles_test/11272/PC_9940_U11272_2028_MODB01.pri";
			scenarioName = "121_RegProv_apsp_sansTBE";
			aggregates = { "REGAFIN", "REGAPAR", "REGAEDU", "REGAREG", "REGASNAT", "AECHEC"};
			order = { "ADEGNAT", "AEDUNAT", "ADEGPL", "AEDUPL", "AEPC", "ANET", "AEC", "AEC2", "AEC3", "ACA", "ACA2", "ACPROG", "ACPIL", "ACPIP", "ACPFORT", "ACPFAIBL", "ACP", "ACPJ", "ACJ", "ACRV", "ACRS", "ACFP", "ACPPTM", "ACT", "ACTPL", "ASCA", "APLLI", "APLIN", "APLBA", "APL", "APLCC", "AREG", "ASNAT", "AECHEC", "_DEATH" };
			output_scenario_name = scenarioName + "_test";
		}
		else 
		{
			std::vector<std::string> result;
			boost::split(result, argv[1], boost::is_any_of("|"));
			primary_path = result.at(0);
			scenarioName = result.at(1);
			output_scenario_name = scenarioName + "_test";
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

		const std::vector<Models::FMTmodel> MODELS =ModelParser.readproject(primary_path, SCENARIOS);
		Models::FMTmodel aggregatedModel = FMTWrapperCore::Transformation::aggregateAllActions(MODELS.at(0), aggregates, order, primary_path, output_scenario_name);

		// On v�rifie si on a moins d'action que dans le nouveau
		std::vector<Core::FMTaction> actions = MODELS.at(0).getactions();
		std::vector<Core::FMTaction> aggregatedActions = aggregatedModel.getactions();
		if (actions.size() <= aggregatedActions.size())
		{
			throw Exception::FMTexc::FMTinvalid_aggregate;
		}
		// on v�rifie s'il y a bien �crit le mod�le
		const boost::filesystem::path SCENARIO_PATH = boost::filesystem::path(primary_path).parent_path() / boost::filesystem::path("Scenarios") / boost::filesystem::path(output_scenario_name);
		if (!boost::filesystem::is_directory(SCENARIO_PATH))
		{
			throw Exception::FMTexc::FMTinvalid_path;
		}
	}
	else {
	Logging::FMTdefaultlogger() << "FMT needs to be compiled with OSI" << "\n";
	}
#endif
	return 0;

}
