#include <algorithm>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include "FMTModelParser.h"
#include "FMTModel.h"
#include "TestTools.h"

// Checks that scenarios read together by one FMTModelParser::readproject call give the same
// models as the same scenarios read one at a time.
//
// When several scenarios are read at once, FMTModelParser::referenceRead reuses the sections
// they have in common. A section taken from the wrong scenario would change a model without
// any message: each model read together and its counterpart read alone are written with
// FMTModelParser::write, then their files are compared line by line.
//
// argv[1]: <primary file>|<scenario>|<scenario>[|<scenario>...]
// argv[2]: optional, the scenarios read one at a time, compared in the order of argv[1].
//          It defaults to the scenarios of argv[1]: giving them in another order must make
//          the test fail.
namespace
{
	std::vector<std::string> split(const std::string& p_value)
	{
		std::vector<std::string> items;
		boost::split(items, p_value, boost::is_any_of("|"));
		return items;
	}

	// Writes a model in an empty folder and returns its files by extension.
	std::map<std::string, std::filesystem::path> writeModel(const Models::FMTModel& p_model,
		const std::filesystem::path& p_folder)
	{
		std::filesystem::create_directories(p_folder);
		Parser::FMTModelParser parser;
		parser.write(p_model, p_folder.string() + "/");
		std::map<std::string, std::filesystem::path> files;
		for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(p_folder))
		{
			if (entry.is_regular_file())
			{
				files[entry.path().extension().string()] = entry.path();
			}
		}
		return files;
	}

	std::vector<std::string> readLines(const std::filesystem::path& p_file)
	{
		std::vector<std::string> lines;
		std::ifstream stream(p_file);
		std::string line;
		while (std::getline(stream, line))
		{
			lines.push_back(line);
		}
		return lines;
	}

	// Returns an empty string when both files have the same lines, else the first difference.
	std::string firstDifference(const std::filesystem::path& p_joint, const std::filesystem::path& p_single)
	{
		const std::vector<std::string> JOINT = readLines(p_joint);
		const std::vector<std::string> SINGLE = readLines(p_single);
		const size_t LINES = (std::max)(JOINT.size(), SINGLE.size());
		for (size_t line = 0; line < LINES; ++line)
		{
			const std::string JOINT_LINE = (line < JOINT.size()) ? JOINT.at(line) : "<end of file>";
			const std::string SINGLE_LINE = (line < SINGLE.size()) ? SINGLE.at(line) : "<end of file>";
			if (JOINT_LINE != SINGLE_LINE)
			{
				return "line " + std::to_string(line + 1) + ": \"" + JOINT_LINE + "\" read together, \""
					+ SINGLE_LINE + "\" read alone";
			}
		}
		return std::string();
	}
}

int main(int argc, char* argv[])
{
	return Testing::runTest([&]()
		{
		if (argc < 2)
		{
			throw std::invalid_argument("Usage: testJointScenarioReading <primary file>|<scenario>|<scenario>... [<scenarios read alone>]");
		}
		const std::vector<std::string> ARGUMENTS = split(argv[1]);
		const std::string PRIMARY_FILE = ARGUMENTS.at(0);
		const std::vector<std::string> SCENARIOS(ARGUMENTS.begin() + 1, ARGUMENTS.end());
		const std::vector<std::string> ALONE = (argc > 2) ? split(argv[2]) : SCENARIOS;
		if (SCENARIOS.size() < 2 || ALONE.size() != SCENARIOS.size())
		{
			throw std::invalid_argument("At least two scenarios are needed, and as many read alone");
		}
		// One folder per CSV row, named by a hash: scenario names would exceed the Windows path limit.
		std::ostringstream folderName;
		folderName << std::hex << std::hash<std::string>()(boost::join(SCENARIOS, "|") + "/" + boost::join(ALONE, "|"));
		const std::filesystem::path FOLDER = std::filesystem::path("../../tests/testJointScenarioReading") / folderName.str();
		std::filesystem::remove_all(FOLDER);
		std::cout << "Models written in " << FOLDER.string() << std::endl;

		Parser::FMTModelParser parser;
		parser.setDefaultExceptionHandler();
		const std::vector<Models::FMTModel> TOGETHER = parser.readproject(PRIMARY_FILE, SCENARIOS);

		Testing::Checker checker;
		if (!checker.check(TOGETHER.size() == SCENARIOS.size(), std::to_string(TOGETHER.size()) + " models read together"))
		{
			return checker.exitCode();
		}
		for (size_t scenario = 0; scenario < SCENARIOS.size(); ++scenario)
		{
			Parser::FMTModelParser aloneParser;
			aloneParser.setDefaultExceptionHandler();
			const Models::FMTModel ALONE_MODEL =
				aloneParser.readproject(PRIMARY_FILE, std::vector<std::string>(1, ALONE.at(scenario))).at(0);
			const std::map<std::string, std::filesystem::path> JOINT_FILES =
				writeModel(TOGETHER.at(scenario), FOLDER / "together" / std::to_string(scenario));
			const std::map<std::string, std::filesystem::path> ALONE_FILES =
				writeModel(ALONE_MODEL, FOLDER / "alone" / std::to_string(scenario));
			const std::string NAME = SCENARIOS.at(scenario) + " (compared with " + ALONE.at(scenario) + " read alone)";
			if (!checker.check(!JOINT_FILES.empty() && JOINT_FILES.size() == ALONE_FILES.size(),
				NAME + ": " + std::to_string(JOINT_FILES.size()) + " and " + std::to_string(ALONE_FILES.size()) + " files written"))
			{
				continue;
			}
			for (const std::pair<const std::string, std::filesystem::path>& JOINT_FILE : JOINT_FILES)
			{
				const std::map<std::string, std::filesystem::path>::const_iterator ALONE_FILE = ALONE_FILES.find(JOINT_FILE.first);
				if (!checker.check(ALONE_FILE != ALONE_FILES.end(), NAME + ": both write a " + JOINT_FILE.first + " file"))
				{
					continue;
				}
				const std::string FIRST_DIFFERENCE = firstDifference(JOINT_FILE.second, ALONE_FILE->second);
				checker.check(FIRST_DIFFERENCE.empty(), NAME + ", " + JOINT_FILE.first + " section"
					+ (FIRST_DIFFERENCE.empty() ? " identical" : " differs at " + FIRST_DIFFERENCE));
			}
		}
		return checker.exitCode();
		});
}
