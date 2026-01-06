
#include <vector>
#include <string>
#include "FMTmodel.h"
#include "FMTlpmodel.h"
#include "FMTmodelparser.h"
#include "Tools.h"
#include "FMTfreeexceptionhandler.h"


int main(int argc, char* argv[])
{
	std::string pathPri;	
	std::string scenarioName;
	std::string mask;
	std::string yieldName;
	int periods = 0;
	std::vector<int> themesNumbers;
	int resutlSize = 0;
	std::string rastpath;

	if (argc > 1)
	{
		std::vector<std::string>results;
		const std::string vals1 = argv[1];
		boost::split(results, vals1, boost::is_any_of("|"));
		pathPri = results.at(0);
		scenarioName = results.at(1);
		periods = std::stoi(results.at(2));
		resutlSize = std::stoi(results.at(3));

		std::vector<std::string>csvThemesNumber;
		const std::string vals2 = argv[2];
		boost::split(results, vals2, boost::is_any_of("|"));
		for (const std::string& theme : csvThemesNumber)
		{
			themesNumbers.push_back(std::stoi(theme));
		}
	}
	else
	{
		pathPri = "C:\\Users\\Admlocal\\Documents\\issues\\temp_run\\"
			"01_Valide_1272_120_RegProv_apsp_TBE\\01_Valide_1272_120_RegProv_apsp_TBE\\PC_9936_U01272_2028_MODB01.pri";
		scenarioName = "120_RegProv_apsp_TBE";
		periods = 6;
		resutlSize = 32953;
		themesNumbers = {3, 5, 12, 13, 14, 21};
		rastpath = "";
	}

	Parser::FMTmodelparser ModelParser;
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
	ModelParser.seterrorstowarnings(errors);

	Models::FMTmodel model = ModelParser.readproject(pathPri, { scenarioName }).at(0);


	const std::set<std::string> RESULT = FMTWrapperCore::Tools::getAllMasks(
		model, periods, themesNumbers, rastpath);

	std::cout << RESULT.size() << std::endl;

	if (RESULT.size() != resutlSize) {
		Exception::FMTfreeexceptionhandler().raise(Exception::FMTexc::FMTfunctionfailed, "Nombre de masks non valide",
			"TestWrapperCoreGetAllMasks", __LINE__, __FILE__);

	}
	for (const std::string& res : RESULT) {
		std::cout << res << std::endl;
	}

	return 0;
}

