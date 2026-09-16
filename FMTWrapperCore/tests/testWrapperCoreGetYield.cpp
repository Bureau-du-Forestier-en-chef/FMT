#include <algorithm>
#include <cmath>
#include <vector>
#include <string>
#include "FMTModel.h"
#include "FMTLpModel.h"
#include "FMTModelParser.h"
#include "ModelQuery.h"
#include "FMTFreeExceptionHandler.h"

// Test of FMTWrapperCore::ModelQuery::getYield, registered in basetests.csv (TWD_land)
// and BFECtests.csv.
//   argv[1] = "<.pri file>|<scenario>|<yield>|<mask>"
//   argv[2] = "<age>|<expected value>"
// Without arguments, it falls back on a private model.
int main(int argc, char* argv[])
{
	std::string pathPri;
	std::string scenarioName;
	std::string mask;
	std::string yieldName;
	int age = 0;
	double result = 0;
	if (argc > 1)
	{
		std::vector<std::string>primaryScenario;
		const std::string vals1 = argv[1];
		boost::split(primaryScenario, vals1, boost::is_any_of("|"));
		pathPri = primaryScenario.at(0);
		scenarioName = primaryScenario.at(1);
		yieldName = primaryScenario.at(2);
		mask = primaryScenario.at(3);

		std::vector<std::string>ageResult;
		const std::string vals2 = argv[2];
		boost::split(ageResult, vals2, boost::is_any_of("|"));
		age = std::stoi(ageResult.at(0));
		result = std::stod(ageResult.at(1));
	}
	else
	{
		pathPri = "D:/01_Valide_ServiceOuest_TBE/PC_9949_U08251_2028_MODB01.pri";
		scenarioName = "TBE_TEST_CORRECTION";
		//mask = "? ? 1 ? FC2551 ? ? ? ? ? ? NA EL7 S01555 ? ? ? ? ? ? POST";
		//mask = "? ? 1 ? FC2452 ? ? ? ? ? ? NA EL9 S01727 ? ? ? ? ? ? POST";
		mask = "? ? 1 ? FC2530 ? ? ? ? ? ? NA EL11 S01640 ? ? ? ? ? ? PRE";
		yieldName = "YV_E_SAB";
		age = 19;
		result = 54.53;
	}


	Parser::FMTModelParser ModelParser;
	std::vector<Exception::FMTexc>errors;
	//errors.push_back(Exception::FMTexc::FMTmissingyield);
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


	const std::vector<Models::FMTModel> MODELS = ModelParser.readproject(pathPri, { scenarioName });

	const double yield = FMTWrapperCore::ModelQuery::getYield(MODELS.at(0), mask, yieldName, age);
	std::cout << "Yield: " << yield << std::endl;

	// Checks on the returned number, within a relative tolerance: equation yields are
	// computed, not read.
	if (std::abs(yield - result) > 1e-6 * std::max(1.0, std::abs(result))) {
		Exception::FMTFreeExceptionHandler().raise(Exception::FMTexc::FMTfunctionfailed, "The yield does not match the expected result",
			"TestWrapperCoreGetYield", __LINE__, __FILE__);
	}


	return 0;
}

