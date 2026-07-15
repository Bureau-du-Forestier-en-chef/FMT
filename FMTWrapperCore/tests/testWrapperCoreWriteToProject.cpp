#include <vector>
#include <string>
#include "FMTmodel.h"
#include "FMTmodelparser.h"
#include "Tools.h"
#include <boost/filesystem.hpp>

int main(int argc, char* argv[])
{

	std::string primary_path;
	std::string scenarioName;
	std::string output_directory;
	if (argc > 1)
	{
		std::vector<std::string> result;
		boost::split(result, argv[1], boost::is_any_of("|"));
		primary_path = result.at(0);
		scenarioName = result.at(1);
		output_directory = argv[2];
	}
	else
	{
		primary_path = "C:\\Users\\Admlocal\\Documents\\issues\\C2_02020265\\02_Travail_Realisme\\PC_9943_U08651_2028_MODB01.pri";
		scenarioName = "323_TYFSansPre_avsp";
		output_directory = "C:\\Users\\Admlocal\\Documents\\FMT\\SCRAP\\";
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
	ModelParser.setErrorsToWarnings(errors);

	// On lit le projet puis on fabrique plusieurs models pour simuler une cache
	// contenant un model de base + des sc�narios d�riv�s (noms distincts).
	const Models::FMTmodel BASE_MODEL = ModelParser.readproject(primary_path, { scenarioName }).at(0);
	const std::string SCENARIO_A = "writetoproject_test_A";
	const std::string SCENARIO_B = "writetoproject_test_B";
	Models::FMTmodel scenarioModelA = BASE_MODEL;
	scenarioModelA.setName(SCENARIO_A);
	Models::FMTmodel scenarioModelB = BASE_MODEL;
	scenarioModelB.setName(SCENARIO_B);

	std::vector<Models::FMTmodel> models;
	models.push_back(BASE_MODEL);
	models.push_back(scenarioModelA);
	models.push_back(scenarioModelB);

	if (!boost::filesystem::is_directory(output_directory))
	{
		boost::filesystem::create_directories(output_directory);
	}
	const boost::filesystem::path OUTPUT_PRI = boost::filesystem::path(output_directory) / boost::filesystem::path("writetoproject_test.pri");

	// FMTForm::writetoprojectfromcache en bouclant sur la cache.
	for (const Models::FMTmodel& MODEL : models)
	{
		FMTWrapperCore::Tools::writeToProject(MODEL, OUTPUT_PRI.string());
	}

	std::vector<std::string> extensions = { ".pri", ".lan", ".are", ".yld", ".act", ".trn" };
	for (const std::string& EXT : extensions)
	{
		const boost::filesystem::path SECTION_PATH = OUTPUT_PRI.parent_path() / boost::filesystem::path(OUTPUT_PRI.stem().string() + EXT);
		if (!boost::filesystem::is_regular_file(SECTION_PATH))
		{
			throw Exception::FMTexc::FMTinvalid_path;
		}
	}

	const boost::filesystem::path SCENARIOS_DIR = OUTPUT_PRI.parent_path() / boost::filesystem::path("Scenarios");
	for (const std::string& SCENARIO : { SCENARIO_A, SCENARIO_B })
	{
		if (!boost::filesystem::is_directory(SCENARIOS_DIR / boost::filesystem::path(SCENARIO)))
		{
			throw Exception::FMTexc::FMTinvalid_path;
		}
	}

	return 0;
}
