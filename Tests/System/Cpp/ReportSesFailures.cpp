/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include <algorithm>
#include <cstddef>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include "TestTools.h"
#ifdef FMTWITHGDAL
#include "FMTException.h"
#include "FMTSesModel.h"
#include "FMTModelParser.h"
#include "FMTAreaParser.h"
#include "FMTForest.h"
#include "FMTConstraint.h"
#include "FMTOutput.h"
#endif

// Checks that an error of a spatial simulation describes the graphs where it happens (issue vector too long): a
// yield missing for most of the stands raises FMTmissingyield in FMTGraph::getSource, which gives the node and
// the period, then FMTSpatialGraphs adds the constraint or the output and reports the graphs in error, once.
//
// argv[1]: <primary file>|<scenario>|<case>, the case being
//          constraint: the build of the first period evaluates a constraint on the yield
//                      (FMTSpatialGraphs::_buildConstraintsValues);
//          output:     without that constraint, the output on the yield is asked after the build
//                      (FMTSpatialGraphs::getOutput).

namespace
{
	//FMT reads the names in uppercase
	const std::string FAILING_OUTPUT = "OVOLPARTIEL";

	std::string getMessages(const std::exception& p_exception)
	{
		std::string messages;
		Testing::visitNested(p_exception, [&messages](const std::exception& p_nested, std::size_t)
			{
			messages += std::string(p_nested.what()) + "\n";
			});
		return messages;
	}

	size_t countOccurrences(const std::string& p_text, const std::string& p_searched)
	{
		size_t count = 0;
		for (size_t position = p_text.find(p_searched); position != std::string::npos;
			position = p_text.find(p_searched, position + p_searched.size()))
		{
			++count;
		}
		return count;
	}

	// Returns the number of the texts of p_expected missing in p_messages, after writing each of them.
	int countMissing(const std::string& p_messages, const std::vector<std::string>& p_expected)
	{
		int missing = 0;
		for (const std::string& EXPECTED : p_expected)
		{
			if (p_messages.find(EXPECTED) == std::string::npos)
			{
				std::cerr << "Missing in the error: " << EXPECTED << std::endl;
				++missing;
			}
		}
		return missing;
	}
}

int main(int argc, char* argv[])
{
	return Testing::runTest([&]()
		{
#ifdef FMTWITHGDAL
		if (argc < 2)
		{
			throw std::invalid_argument("Usage: ReportSesFailures <primary file>|<scenario>|<constraint or output>");
		}
		const std::string FIRST_ARGUMENT(argv[1]);
		std::vector<std::string> arguments;
		boost::split(arguments, FIRST_ARGUMENT, boost::is_any_of("|"));
		if (arguments.size() != 3 || (arguments.at(2) != "constraint" && arguments.at(2) != "output"))
		{
			throw std::invalid_argument("The first argument needs a primary file, a scenario and constraint or output");
		}
		const std::string PRIMARY = arguments.at(0);
		const bool ON_CONSTRAINT = (arguments.at(2) == "constraint");
		const std::string MODEL_LOCATION = PRIMARY.substr(0, PRIMARY.find_last_of("/\\") + 1);

		//As SimulateSpatially, but FMTmissingyield stays an error
		Parser::FMTModelParser parser;
		std::vector<Exception::FMTexc> errors;
		errors.push_back(Exception::FMTexc::FMToutput_missing_operator);
		errors.push_back(Exception::FMTexc::FMToutput_too_much_operator);
		errors.push_back(Exception::FMTexc::FMTinvalidyield_number);
		errors.push_back(Exception::FMTexc::FMTundefinedoutput_attribute);
		errors.push_back(Exception::FMTexc::FMToveridedyield);
		errors.push_back(Exception::FMTexc::FMTsourcetotarget_transition);
		errors.push_back(Exception::FMTexc::FMTsame_transitiontargets);
		errors.push_back(Exception::FMTexc::FMTunclosedforloop);
		parser.setErrorsToWarnings(errors);
		const std::vector<Models::FMTModel> MODELS = parser.readproject(PRIMARY, std::vector<std::string>(1, arguments.at(1)));
		const std::vector<std::vector<Core::FMTSchedule>> SCHEDULES = parser.readSchedules(PRIMARY, MODELS);
		Models::FMTSesModel model(MODELS.at(0));
		std::vector<Core::FMTTransition> singleTransitions;
		for (const Core::FMTTransition& transition : model.getTransitions())
		{
			singleTransitions.push_back(transition.single());
		}
		model.setTransitions(singleTransitions);
		Core::FMTOutput failingOutput;
		for (const Core::FMTOutput& output : model.getOutputs())
		{
			if (output.getName() == FAILING_OUTPUT)
			{
				failingOutput = output;
			}
		}
		if (!ON_CONSTRAINT)
		{
			std::vector<Core::FMTConstraint> constraints;
			for (const Core::FMTConstraint& constraint : model.getConstraints())
			{
				if (std::string(constraint).find(FAILING_OUTPUT) == std::string::npos)
				{
					constraints.push_back(constraint);
				}
			}
			model.setConstraints(constraints);
		}
		const std::string RASTERS = MODEL_LOCATION + "rasters/";
		std::vector<std::string> themeRasters;
		for (size_t theme = 1; theme <= model.getThemes().size(); ++theme)
		{
			themeRasters.push_back(RASTERS + "THEME" + std::to_string(theme) + ".tif");
		}
		Parser::FMTAreaParser areaparser;
		model.setInitialMapping(areaparser.readRasters(model.getThemes(), themeRasters, RASTERS + "AGE.tif", 1, 0.0001));

		std::string messages;
		std::vector<Exception::FMTexc> types;
		try
		{
			model.greedyReferenceBuild(SCHEDULES.at(0).at(0), 1);
			if (!ON_CONSTRAINT)
			{
				model.getOutput(failingOutput, 1, Core::FMToutputlevel::totalonly);
			}
		}
		catch (const std::exception& error)
		{
			messages = getMessages(error);
			Testing::visitNested(error, [&types](const std::exception& p_nested, std::size_t)
				{
				const Exception::FMTException* FMT_EXCEPTION = dynamic_cast<const Exception::FMTException*>(&p_nested);
				if (FMT_EXCEPTION != nullptr)
				{
					types.push_back(FMT_EXCEPTION->getType());
				}
				});
		}
		if (messages.empty())
		{
			std::cerr << "No error raised" << std::endl;
			return 1;
		}
		std::vector<std::string> expected = { "For node", "at period", "Graphs in error: 0 of" };
		if (ON_CONSTRAINT)
		{
			expected.insert(expected.end(), { "For graph id", "of family", "constraint", "bounds" });
		}else {
			expected.push_back("For output " + FAILING_OUTPUT + " at period 1");
		}
		int missing = countMissing(messages, expected);
		const size_t REPORTS = countOccurrences(messages, "Graphs in error:");
		if (REPORTS != 1)
		{
			std::cerr << "The graphs in error are reported " << REPORTS << " times" << std::endl;
			++missing;
		}
		if (std::find(types.begin(), types.end(), Exception::FMTexc::FMTmissingyield) == types.end())
		{
			std::cerr << "The error is not FMTmissingyield" << std::endl;
			++missing;
		}
		if (missing > 0)
		{
			std::cerr << messages << std::endl;
			return 1;
		}
#endif
		return 0;
		});
}
