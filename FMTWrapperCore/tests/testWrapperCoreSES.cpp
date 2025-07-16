#ifdef FMTWITHOSI
	#include "FMTmodelparser.h"
	#include "FMTmodel.h"
	#include "FMTversion.h"
#endif
#include "FMTdefaultlogger.h"
#include "SES.h"
#include <boost/filesystem.hpp>

int main(int argc, char* argv[])
{
#ifdef FMTWITHOSI
	Logging::FMTdefaultlogger().logstamp();

	if (Version::FMTversion().hasfeature("OSI"))
	{
		if (argc < 2)
		{
			std::string primary_path = "//Artemis/fecgeo/Donnees/02_Courant/07_Outil_moyen_methode/01_Entretien_developpement/Interne/FMT/Entretien/Modeles_test/11272/PC_9940_U11272_2028_MODB01.pri";
			std::string rasters_path = "path";
			std::string scenario_name = "121_RegProv_apsp_sansTBE";
			int num_of_iterations = 5;
			int length = 5;
			std::vector<std::string> constraints = {"REGAFIN", "REGAPAR", "REGAEDU", "REGAREG", "REGASNAT", "AECHEC"};
			bool stanlock = true;
			std::vector<std::string> outputs = {""};
			int outputs_level = 0;
			int output_min = 0;
			int output_max = length
			std::string output_name = "C:/FMT/Output/Fmtoutput.csv";
			bool event = true;
			bool spatial_output = true;
			std::vector<std::string>& predictoryields, 
			std::vector<int>& growththemes
		}
		else 
		{
			//TODO
		}

		const std::vector<std::string> SCENARIOS(1, scenarioName);
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

		const std::vector<Models::FMTmodel> MODELS = ModelParser.readproject(primary_path, SCENARIOS);
		Models::FMTmodel Model = FMTWrapperCore::SES::SimulationSpatialeExplicite(MODELS.at(0), aggregates, order, primary_path, output_scenario_name);
	}
	else {
	Logging::FMTdefaultlogger() << "FMT needs to be compiled with OSI" << "\n";
	}
#endif
	return 0;

}
