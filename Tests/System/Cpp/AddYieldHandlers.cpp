/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include <stdexcept>
#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include "FMTModelParser.h"
#include "FMTModel.h"
#include "FMTAgeYieldHandler.h"
#include "FMTMask.h"
#include "FMTDevelopment.h"
#include "TestTools.h"

// Checks FMTModel::addYieldHandlers: an age yield handler is added for each stand type of the second theme, the
// attributes PEUPLEMENT*, and its yield VOLUMETOTALNEXTAGE gives, at each of its base ages, the VOLUMETOTAL of the
// stand five years older. The model must then give this yield to each stand type at each base age, and to one
// development at an age between two bases, by interpolation.
//
// argv[1]: the primary file, TWD_land by default.
// argv[2]: <scenario>|<development mask>|<age>, the development whose VOLUMETOTALNEXTAGE argv[3] gives.
// argv[3]: that VOLUMETOTALNEXTAGE, computed by hand from the yields of the scenario.
int main(int argc, char* argv[])
{
	return Testing::runTest([&]()
		{
		// Arrange
		const std::string PRIMARY_FILE = (argc > 1) ? argv[1] : "../../../../Examples/Models/TWD_land/TWD_land.pri";
		std::vector<std::string> query;
		boost::split(query, (argc > 2) ? std::string(argv[2]) : std::string("LP|UNITE1 PEUPLEMENT1 UTR1|2"),
			boost::is_any_of("|"));
		if (query.size() != 3 || argc == 3)
		{
			throw std::invalid_argument("Usage: AddYieldHandlers <primary file> <scenario>|<development mask>|<age> <expected yield>");
		}
		const double EXPECTED_YIELD = (argc > 3) ? std::stod(argv[3]) : 120.0;
		const std::string NEW_YIELD = "VOLUMETOTALNEXTAGE";
		const std::vector<int> BASES = { 0, 5, 10, 15, 20, 25, 30 };
		const int AGE_DIFFERENCE = 5;
		Parser::FMTModelParser modelparser;
		Models::FMTModel model = modelparser.readproject(PRIMARY_FILE, std::vector<std::string>(1, query.at(0))).at(0);
		const std::vector<Core::FMTTheme> THEMES = model.getThemes();
		std::vector<Core::FMTMask> standMasks;
		std::vector<Core::FMTAgeYieldHandler> newHandlers;
		for (const std::string& forestAttribute : THEMES.at(1).getAttributes("?"))
		{
			if (forestAttribute.find("PEUPLEMENT") == 0)
			{
				const Core::FMTMask STAND_MASK("? " + forestAttribute + " ?", THEMES);
				std::vector<double> olderVolumes;
				for (const int& age : BASES)
				{
					const Core::FMTDevelopment OLDER_STAND(STAND_MASK, age + AGE_DIFFERENCE, 0, 1);
					olderVolumes.push_back(model.getYields().get(OLDER_STAND.getYieldRequest(), "VOLUMETOTAL"));
				}
				Core::FMTAgeYieldHandler handler(STAND_MASK);
				handler.setYieldValues(NEW_YIELD, BASES, olderVolumes);
				newHandlers.push_back(handler);
				standMasks.push_back(STAND_MASK);
			}
		}

		// Act
		model.addYieldHandlers(newHandlers);

		// Assert
		Testing::Checker checker;
		const Core::FMTYields YIELDS = model.getYields();
		checker.check(!standMasks.empty(), std::to_string(standMasks.size()) + " stand types get a new yield");
		checker.check(YIELDS.isYld(NEW_YIELD), "the model has the yield " + NEW_YIELD);
		for (const Core::FMTMask& standMask : standMasks)
		{
			std::string firstDifference;
			for (const int& age : BASES)
			{
				const Core::FMTDevelopment STAND(standMask, age, 0, 1);
				const Core::FMTDevelopment OLDER_STAND(standMask, age + AGE_DIFFERENCE, 0, 1);
				const double NEW_VALUE = YIELDS.get(STAND.getYieldRequest(), NEW_YIELD);
				const double OLDER_VOLUME = YIELDS.get(OLDER_STAND.getYieldRequest(), "VOLUMETOTAL");
				if (firstDifference.empty() && NEW_VALUE != OLDER_VOLUME)
				{
					firstDifference = " (age " + std::to_string(age) + ": " + std::to_string(NEW_VALUE) + " and "
						+ std::to_string(OLDER_VOLUME) + ")";
				}
			}
			checker.check(firstDifference.empty(), std::string(standMask) + ": " + NEW_YIELD + " is the VOLUMETOTAL "
				+ std::to_string(AGE_DIFFERENCE) + " years older at each base age" + firstDifference);
		}
		const Core::FMTDevelopment DEVELOPMENT(Core::FMTMask(query.at(1), THEMES), std::stoi(query.at(2)), 0, 1);
		checker.checkNear(YIELDS.get(DEVELOPMENT.getYieldRequest(), NEW_YIELD), EXPECTED_YIELD, 1e-9,
			NEW_YIELD + " of " + query.at(1) + " at age " + query.at(2));
		return checker.exitCode();
		});
}
