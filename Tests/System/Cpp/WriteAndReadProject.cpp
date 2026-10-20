/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "TestTools.h"
#ifdef FMTWITHOSI
	#include <cmath>
	#include <filesystem>
	#include <memory>
	#include <stdexcept>
	#include <string>
	#include <vector>
	#include <boost/algorithm/string.hpp>
	#include "FMTModelParser.h"
	#include "FMTLpModel.h"
#endif

// Checks FMTModelParser::writeToProject on solved models: the first one becomes the root of a new project, each
// other one a scenario of it, written with the sections that differ from the root. Each model read back from the
// project, solved again, must reach the optimum of the model written.
//
// argv[1]: the primary file.
// argv[2]: <scenario>[|<scenario>...], the scenarios solved and written, in this order.
// argv[3]: optional, the same scenarios in the order in which they are read back and compared with those of
//          argv[2], argv[2] by default: giving them in another order must make the test fail.
int main(int argc, char* argv[])
{
#ifdef FMTWITHOSI
	return Testing::runTest([&]()
		{
		// Arrange
		std::string primarylocation = "../../../../Examples/Models/TWD_land/TWD_land.pri";
		std::vector<std::string> scenarios(1, "operator_separator");
		if (argc > 2)
		{
			primarylocation = std::string(argv[1]);
			boost::split(scenarios, std::string(argv[2]), boost::is_any_of("|"));
		}
		std::vector<std::string> scenariosReadBack = scenarios;
		if (argc > 3)
		{
			boost::split(scenariosReadBack, std::string(argv[3]), boost::is_any_of("|"));
		}
		if (scenariosReadBack.size() != scenarios.size())
		{
			throw std::invalid_argument("argv[3] must give as many scenarios as argv[2]");
		}
		Parser::FMTModelParser modelparser;
		modelparser.setDefaultExceptionHandler();
		const std::filesystem::path OUTPUT_FOLDER = "../../tests/WriteAndReadProject/";
		std::filesystem::remove_all(OUTPUT_FOLDER);
		std::filesystem::create_directories(OUTPUT_FOLDER);
		const std::string PROJECT_FILE = (OUTPUT_FOLDER / "test.pri").string();

		std::vector<Exception::FMTexc> errors;
		errors.push_back(Exception::FMTexc::FMTmissingyield);
		errors.push_back(Exception::FMTexc::FMToveridedyield);
		errors.push_back(Exception::FMTexc::FMTdeathwithlock);
		std::vector<Exception::FMTexc> readErrors(errors);
		readErrors.push_back(Exception::FMTexc::FMToutput_too_much_operator);
		readErrors.push_back(Exception::FMTexc::FMTinvalidyield_number);
		modelparser.setErrorsToWarnings(readErrors);

		const std::vector<Models::FMTModel> READ_MODELS = modelparser.readproject(primarylocation, scenarios);
		std::vector<std::unique_ptr<Models::FMTLpModel>> solvedModels;
		for (const Models::FMTModel& model : READ_MODELS)
		{
			solvedModels.emplace_back(new Models::FMTLpModel(model, Models::FMTSolverInterface::CLP));
			solvedModels.back()->doPlanning(true);
		}
		// The first scenario written is the root of the project.
		for (std::string& scenario : scenariosReadBack)
		{
			if (scenario == scenarios.at(0))
			{
				scenario = "ROOT";
			}
		}

		// Act
		for (const std::unique_ptr<Models::FMTLpModel>& solvedModel : solvedModels)
		{
			modelparser.writeToProject(PROJECT_FILE, *solvedModel);
		}
		modelparser.setErrorsToWarnings(errors);
		const std::vector<Models::FMTModel> REREAD_MODELS = modelparser.readproject(PROJECT_FILE, scenariosReadBack);

		// Assert
		Testing::Checker checker;
		if (!checker.check(REREAD_MODELS.size() == solvedModels.size(), std::to_string(REREAD_MODELS.size())
			+ " models read back from the project, " + std::to_string(solvedModels.size()) + " written"))
		{
			return checker.exitCode();
		}
		for (size_t model = 0; model < solvedModels.size(); ++model)
		{
			Models::FMTLpModel rereadModel(REREAD_MODELS.at(model), Models::FMTSolverInterface::CLP);
			const bool SOLVED = rereadModel.doPlanning(true);
			const double WRITTEN_OPTIMUM = solvedModels.at(model)->getObjValue();
			checker.check(SOLVED && solvedModels.at(model)->isOptimal(), scenarios.at(model) + " and "
				+ scenariosReadBack.at(model) + " read back are solved");
			checker.checkNear(rereadModel.getObjValue(), WRITTEN_OPTIMUM, 1e-6 * (std::max)(1.0, std::abs(WRITTEN_OPTIMUM)),
				scenariosReadBack.at(model) + " read back reaches the optimum of " + scenarios.at(model));
		}
		return checker.exitCode();
		});
#else
	return Testing::skip("FMT is compiled without OSI");
#endif
}
