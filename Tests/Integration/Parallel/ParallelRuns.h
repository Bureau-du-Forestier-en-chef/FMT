/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef TESTING_PARALLELRUNS_H_INCLUDED
#define TESTING_PARALLELRUNS_H_INCLUDED

#ifdef FMTWITHOSI
	#include "FMTModel.h"
	#include "FMTModelParser.h"
	#include "FMTsolverinterface.h"
#endif

#include <gtest/gtest.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <map>
#include <string>
#include <vector>

// The tools of the integration tests of the parallel tasks: the scenarios of Examples/Models/TWD_land, an empty
// folder for each run of a test case, and the rows that a run wrote.
namespace Testing
{
	// The primary file of TWD_land, from bin/<build type>, the working directory of the tests.
	const std::string TWD_LAND_PRIMARY_FILE = "../../../../Examples/Models/TWD_land/TWD_land.pri";

	// The rows of the CSV files written in a folder, by file name, their header left out and sorted: the order of the
	// rows of a file depends on which thread writes first, their content must not.
	using WrittenRows = std::map<std::string, std::vector<std::string>>;

	// Returns the folder of a test case, tests/IntegrationParallel/<p_name> in the build folder, emptied. The name is
	// short, not the name of the case: a case writes a folder for each of its runs, and the paths of their files must
	// stay below the 260 characters that Windows allows.
	inline std::filesystem::path getCaseFolder(const std::string& p_name)
	{
		const std::filesystem::path folder = std::filesystem::path("../../tests/IntegrationParallel") / p_name;
		std::filesystem::remove_all(folder);
		std::filesystem::create_directories(folder);
		return folder;
	}

	inline WrittenRows readWrittenRows(const std::filesystem::path& p_folder)
	{
		WrittenRows rows;
		for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(p_folder))
		{
			if (entry.path().extension() != ".csv")
			{
				continue;
			}
			std::vector<std::string>& fileRows = rows[entry.path().filename().string()];
			std::ifstream stream(entry.path());
			std::string line;
			std::getline(stream, line);
			while (std::getline(stream, line))
			{
				if (!line.empty())
				{
					fileRows.push_back(line);
				}
			}
			std::sort(fileRows.begin(), fileRows.end());
		}
		return rows;
	}

	// Number of rows of a file of p_rows, 0 when the file was not written.
	inline std::size_t countRows(const WrittenRows& p_rows, const std::string& p_file)
	{
		const WrittenRows::const_iterator FILE_ROWS = p_rows.find(p_file);
		return FILE_ROWS == p_rows.end() ? 0 : FILE_ROWS->second.size();
	}

#ifdef FMTWITHOSI
	// MOSEK when FMT is built with it, CLP otherwise (rule 6 of Tests/ETAT.md).
	inline Models::FMTSolverInterface getSolver()
	{
		#ifdef FMTWITHMOSEK
			return Models::FMTSolverInterface::MOSEK;
		#else
			return Models::FMTSolverInterface::CLP;
		#endif
	}

	// Reads scenarios of TWD_land, in their order.
	inline std::vector<Models::FMTModel> readScenarios(const std::vector<std::string>& p_scenarios)
	{
		Parser::FMTModelParser parser;
		parser.setDefaultExceptionHandler();
		return parser.readproject(TWD_LAND_PRIMARY_FILE, p_scenarios);
	}
#endif
}

#endif
