#ifdef FMTWITHOSI
	#include "FMTmodelparser.h"
	#include "FMTmodel.h"
	#include "FMTversion.h"
#endif
#include "FMTdefaultlogger.h"
#include "TransformationCore.h"

int main()
{
#ifdef FMTWITHOSI
	Logging::FMTdefaultlogger().logstamp();

	if (Version::FMTversion().hasfeature("OSI"))
	{
		std::vector<std::string> aggregates = { "REGAFIN", "REGAPAR", "REGAEDU", "REGAREG", "ATBEMORT", "ATBERETARD", "ASNAT", "AECHEC" };
		std::vector<std::string> order = { "REGAFIN", "REGAPAR", "REGAEDU", "REGAREG", "ATBEMORT", "ATBERETARD", "ASNAT", "AECHEC" };
		const int index = 0;
		const std::string pathPri = "T:/Donnees/02_Courant/07_Outil_moyen_methode/01_Entretien_developpement/09_FMT/Modeles_test/Prototype_Dec2023_TBE/PC_7002071_UA08152_FINAL.pri";
		const std::string outputDir = "../../tests/aggregateAllActions";
		const std::string SCENARIO = "20_Regl_prov";
		const std::vector<std::string>SCENARIOS(1, SCENARIO);
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
		Models::FMTmodel aggregatedModel = FMTWrapperCore::Transformation::aggregateAllActions(MODELS.at(0), aggregates, order, pathPri, outputDir, SCENARIO);

	}
	else {
	Logging::FMTdefaultlogger() << "FMT needs to be compiled with OSI" << "\n";
	}
#endif
	return 0;

}
