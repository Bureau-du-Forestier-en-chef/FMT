/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include <filesystem>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>
#ifdef FMTWITHGDAL
	#include <boost/algorithm/string.hpp>
	#include "FMTSesModel.h"
	#include "FMTModelParser.h"
	#include "FMTAreaParser.h"
	#include "FMTForest.h"
	#include "FMTSpatialSchedule.h"
	#include "FMTGCBMTransition.h"
	#include "FMTTransition.h"
	#include "FMTSchedule.h"
	#include "FMTTheme.h"
#endif
#include "FMTDefaultLogger.h"
#include "TestTools.h"

// Checks FMTSpatialSchedule::getGCBMtransitions (#369): for each GCBM key and each classifier,
// the final attribute is the one found on most cells of the key.
//
// argv[1]: primary file
// argv[2]: <scenario of the schedule>|<scenario of the transitions>
// argv[3]: number of periods to simulate
//
// The expected attributes are counted cell by cell: the key of a cell comes from the stacked
// actions layer filled by getGCBMtransitions, its final attributes from getForestPeriod.
// The transitions scenario must give, for at least one key, a most present attribute that is
// not the first in alphabetical order: otherwise the test cannot see the bug of #369.

#ifdef FMTWITHGDAL
namespace
{
	// Number of cells by key, classifier name and final attribute.
	typedef std::map<std::string, std::map<std::string, std::map<std::string, int>>> AttributesCount;

	AttributesCount countFinalAttributes(const Spatial::FMTSpatialSchedule& p_schedule,
		const Spatial::FMTLayer<std::string>& p_keys,
		const std::vector<Core::FMTTheme>& p_classifiers, int p_period)
	{
		AttributesCount counts;
		const std::vector<Spatial::FMTLayer<std::string>> FINAL_THEMES = p_schedule.getForestPeriod(p_period).getThemes(p_classifiers);
		for (Spatial::FMTLayer<std::string>::const_iterator keyIt = p_keys.begin(); keyIt != p_keys.end(); ++keyIt)
		{
			for (size_t themeId = 0; themeId < p_classifiers.size(); ++themeId)
			{
				const std::string THEME_NAME = "THEME" + std::to_string(p_classifiers.at(themeId).getId() + 1);
				++counts[keyIt->second][THEME_NAME][FINAL_THEMES.at(themeId).at(keyIt->first)];
			}
		}
		return counts;
	}

	// Returns the cells of each attribute, as "UTR1=3 UTR3=5", when a key ends with several.
	std::string describeCounts(const std::map<std::string, int>& p_counts)
	{
		std::string description;
		if (p_counts.size() > 1)
		{
			for (const std::pair<const std::string, int>& count : p_counts)
			{
				description += " " + count.first + "=" + std::to_string(count.second);
			}
			description = ", cells:" + description;
		}
		return description;
	}

	// Returns the most present attribute; on a tie, the first one in alphabetical order.
	std::string getMostPresent(const std::map<std::string, int>& p_counts)
	{
		std::string mostPresent;
		int maximum = 0;
		for (const std::pair<const std::string, int>& count : p_counts)
		{
			if (count.second > maximum)
			{
				maximum = count.second;
				mostPresent = count.first;
			}
		}
		return mostPresent;
	}
}
#endif

int main(int argc, char* argv[])
{
#ifdef FMTWITHGDAL
	Logging::FMTDefaultLogger().logStamp();
	return Testing::runTest([&]()
		{
		if (argc < 4)
		{
			throw std::invalid_argument("usage: GetGCBMTransitions <primary file> <schedule scenario>|<transitions scenario> <periods>");
		}
		const std::string PRIMARY_LOCATION = argv[1];
		const std::string SCENARIOS_ARGUMENT = argv[2];
		std::vector<std::string> scenarios;
		boost::split(scenarios, SCENARIOS_ARGUMENT, boost::is_any_of("|"));
		const int PERIODS = std::stoi(argv[3]);
		Parser::FMTModelParser modelParser;
		std::vector<Exception::FMTexc> errors;
		errors.push_back(Exception::FMTexc::FMTmissingyield);
		errors.push_back(Exception::FMTexc::FMToutput_missing_operator);
		errors.push_back(Exception::FMTexc::FMToutput_too_much_operator);
		errors.push_back(Exception::FMTexc::FMTinvalidyield_number);
		errors.push_back(Exception::FMTexc::FMTundefinedoutput_attribute);
		errors.push_back(Exception::FMTexc::FMToveridedyield);
		errors.push_back(Exception::FMTexc::FMTsourcetotarget_transition);
		errors.push_back(Exception::FMTexc::FMTsame_transitiontargets);
		errors.push_back(Exception::FMTexc::FMTunclosedforloop);
		modelParser.setErrorsToWarnings(errors);
		const std::vector<Models::FMTModel> MODELS = modelParser.readproject(PRIMARY_LOCATION, scenarios);
		const std::vector<Core::FMTSchedule> SCHEDULE = modelParser.readSchedules(PRIMARY_LOCATION,
			std::vector<Models::FMTModel>(1, MODELS.at(0))).at(0);
		//The schedule scenario gives the actions to spatialize, the transitions scenario what they do
		Models::FMTSesModel simulation(MODELS.at(0));
		std::vector<Core::FMTTransition> singleTransitions;
		for (const Core::FMTTransition& transition : MODELS.at(1).getTransitions())
		{
			singleTransitions.push_back(transition.single());
		}
		simulation.setTransitions(singleTransitions);
		const std::filesystem::path RASTERS = std::filesystem::path(PRIMARY_LOCATION).parent_path() / "rasters";
		std::vector<std::string> themeRasters;
		for (size_t themeId = 1; themeId <= simulation.getThemes().size(); ++themeId)
		{
			themeRasters.push_back((RASTERS / ("THEME" + std::to_string(themeId) + ".tif")).string());
		}
		Parser::FMTAreaParser areaParser;
		simulation.setInitialMapping(areaParser.readRasters(simulation.getThemes(), themeRasters,
			(RASTERS / "AGE.tif").string(), 1, 0.0001));
		const size_t GREEDY_ITERATIONS = 10;
		for (int period = 0; period < PERIODS; ++period)
		{
			simulation.greedyReferenceBuild(SCHEDULE.at(period), GREEDY_ITERATIONS);
		}
		const Spatial::FMTSpatialSchedule SOLUTION = simulation.getSpSchedule();
		const std::vector<Core::FMTAction> ACTIONS = simulation.getActions();
		Testing::Checker checker;
		bool bugVisible = false;
		//One classifier at a time, as BFECGCBM does with THEME5: the key then ignores the other themes
		for (const Core::FMTTheme& theme : simulation.getThemes())
		{
			const std::vector<Core::FMTTheme> CLASSIFIERS(1, theme);
			for (int period = 1; period <= PERIODS; ++period)
			{
				const std::string CASE_NAME = "THEME" + std::to_string(theme.getId() + 1) + " period " + std::to_string(period);
				Spatial::FMTLayer<std::string> keys(SOLUTION.copyExtent<std::string>());
				const std::vector<Core::FMTGCBMTransition> TRANSITIONS = SOLUTION.getGCBMtransitions(keys, ACTIONS, CLASSIFIERS, period);
				const AttributesCount COUNTS = countFinalAttributes(SOLUTION, keys, CLASSIFIERS, period);
				checker.check(TRANSITIONS.size() == COUNTS.size(), CASE_NAME + ": one transition by key");
				for (const Core::FMTGCBMTransition& transition : TRANSITIONS)
				{
					const AttributesCount::const_iterator KEY_COUNTS = COUNTS.find(transition.name);
					if (!checker.check(KEY_COUNTS != COUNTS.end(), CASE_NAME + " " + transition.name + ": key found on the cells"))
					{
						continue;
					}
					for (const std::pair<const std::string, std::map<std::string, int>>& themeCounts : KEY_COUNTS->second)
					{
						const std::string EXPECTED = getMostPresent(themeCounts.second);
						const std::map<std::string, std::string>::const_iterator ACTUAL = transition.themes.find(themeCounts.first);
						const std::string ACTUAL_VALUE = ACTUAL == transition.themes.end() ? "<missing>" : ACTUAL->second;
						checker.check(ACTUAL_VALUE == EXPECTED, CASE_NAME + " " + transition.name + ": "
							+ ACTUAL_VALUE + " (expected " + EXPECTED + describeCounts(themeCounts.second) + ")");
						bugVisible = bugVisible || EXPECTED != themeCounts.second.begin()->first;
					}
				}
			}
		}
		checker.check(bugVisible, "a key whose most present final attribute is not the first in alphabetical order");
		return checker.exitCode();
		});
#else
	return Testing::skip("FMT is compiled without GDAL");
#endif
}