/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "BenchmarkOptions.h"

#include <cctype>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace
{
	// Paths of the source tree and of the build seen from build/release/bin/Release, where ctest
	// runs the benchmarks. The ctest rows pass absolute paths instead.
	const std::string DEFAULT_MODEL_FILE = "../../../../Examples/Models/TWD_land/TWD_land.pri";
	const std::string DEFAULT_EXPECTATIONS_FILE = "../../../../Tests/Performance/performance.csv";
	const std::string DEFAULT_OUTPUT_FOLDER = "../../tests/performance";
	const char* const MODE_VARIABLE = "FMT_BENCHMARK_MODE";

	Performance::BenchmarkMode toMode(const std::string& p_text)
	{
		if (p_text == "full")
		{
			return Performance::BenchmarkMode::Full;
		}
		if (p_text == "smoke")
		{
			return Performance::BenchmarkMode::Smoke;
		}
		throw std::invalid_argument("Unknown benchmark mode \"" + p_text + "\": use smoke or full");
	}

	Performance::BenchmarkMode modeFromEnvironment()
	{
		const char* const VALUE = std::getenv(MODE_VARIABLE);
		if (VALUE == nullptr || std::string(VALUE).empty())
		{
			return Performance::BenchmarkMode::Smoke;
		}
		return toMode(VALUE);
	}

	// Returns true when p_text matches p_pattern, where * matches any text, including none.
	bool matches(const std::string& p_pattern, const std::string& p_text)
	{
		std::size_t patternIndex = 0;
		std::size_t textIndex = 0;
		std::size_t starIndex = std::string::npos;
		std::size_t textIndexAtStar = 0;
		while (textIndex < p_text.size())
		{
			if (patternIndex < p_pattern.size() && p_pattern[patternIndex] == '*')
			{
				starIndex = patternIndex;
				++patternIndex;
				textIndexAtStar = textIndex;
			}
			else if (patternIndex < p_pattern.size() && p_pattern[patternIndex] == p_text[textIndex])
			{
				++patternIndex;
				++textIndex;
			}
			else if (starIndex != std::string::npos)
			{
				// The last star absorbs one more character, and the match resumes after it.
				patternIndex = starIndex + 1;
				++textIndexAtStar;
				textIndex = textIndexAtStar;
			}
			else
			{
				return false;
			}
		}
		while (patternIndex < p_pattern.size() && p_pattern[patternIndex] == '*')
		{
			++patternIndex;
		}
		return patternIndex == p_pattern.size();
	}

	std::vector<std::string> splitRow(const std::string& p_row)
	{
		std::vector<std::string> fields;
		std::stringstream stream(p_row);
		std::string field;
		while (std::getline(stream, field, ';'))
		{
			fields.push_back(field);
		}
		return fields;
	}

	double toNumber(const std::string& p_text, const std::string& p_context)
	{
		char* end = nullptr;
		const double NUMBER = std::strtod(p_text.c_str(), &end);
		if (p_text.empty() || end != p_text.c_str() + p_text.size())
		{
			throw std::invalid_argument(p_context + ": \"" + p_text + "\" is not a number");
		}
		return NUMBER;
	}

	std::int64_t toCount(const std::string& p_text, const std::string& p_context)
	{
		char* end = nullptr;
		const long long COUNT = std::strtoll(p_text.c_str(), &end, 10);
		if (p_text.empty() || end != p_text.c_str() + p_text.size() || COUNT < 0)
		{
			throw std::invalid_argument(p_context + ": \"" + p_text + "\" is not a count");
		}
		return static_cast<std::int64_t>(COUNT);
	}

	// Name of the default results file: the selection, with * written "all".
	std::string fileNameOf(const std::string& p_pattern)
	{
		std::string name;
		for (const char CHARACTER : p_pattern)
		{
			if (CHARACTER == '*')
			{
				name += "all";
			}
			else if (std::isalnum(static_cast<unsigned char>(CHARACTER)) != 0
				|| CHARACTER == '.' || CHARACTER == '-' || CHARACTER == '_')
			{
				name += CHARACTER;
			}
			else
			{
				name += '_';
			}
		}
		return name + ".json";
	}
}

namespace Performance
{
	std::string toString(BenchmarkMode p_mode)
	{
		return p_mode == BenchmarkMode::Full ? "full" : "smoke";
	}

	BenchmarkOptions BenchmarkOptions::parse(int p_argc, const char* const p_argv[], const std::string& p_executable)
	{
		BenchmarkOptions options;
		for (int index = 1; index < p_argc; ++index)
		{
			const std::string OPTION = p_argv[index];
			if (OPTION == "--list")
			{
				options.m_listing = true;
			}
			else if (index + 1 < p_argc)
			{
				++index;
				options._set(OPTION, p_argv[index]);
			}
			else
			{
				throw std::invalid_argument("Option " + OPTION + " needs a value");
			}
		}
		if (options.m_outputFile.empty())
		{
			options.m_outputFile = std::filesystem::path(DEFAULT_OUTPUT_FOLDER) / fileNameOf(options.m_pattern);
		}
		if (!options.m_listing)
		{
			options._readExpectations(p_executable);
			options._applyOverrides();
		}
		return options;
	}

	bool BenchmarkOptions::isListing() const
	{
		return m_listing;
	}

	const std::string& BenchmarkOptions::getSelection() const
	{
		return m_pattern;
	}

	bool BenchmarkOptions::selects(const std::string& p_benchmark) const
	{
		if (m_exactName)
		{
			return p_benchmark == m_pattern;
		}
		return matches(m_pattern, p_benchmark);
	}

	BenchmarkMode BenchmarkOptions::getMode() const
	{
		return m_mode;
	}

	const std::string& BenchmarkOptions::getModelFile() const
	{
		return m_modelFile;
	}

	const std::filesystem::path& BenchmarkOptions::getOutputFile() const
	{
		return m_outputFile;
	}

	std::optional<Expectation> BenchmarkOptions::getExpectation(const std::string& p_benchmark) const
	{
		const auto FOUND = m_expectations.find(p_benchmark);
		if (FOUND == m_expectations.end())
		{
			return std::nullopt;
		}
		return FOUND->second;
	}

	BenchmarkOptions::BenchmarkOptions() :
		m_mode(modeFromEnvironment()),
		m_modelFile(DEFAULT_MODEL_FILE),
		m_expectationsFile(DEFAULT_EXPECTATIONS_FILE)
	{
	}

	void BenchmarkOptions::_set(const std::string& p_option, const std::string& p_value)
	{
		if (p_option == "--benchmark")
		{
			m_pattern = p_value;
			m_exactName = true;
		}
		else if (p_option == "--filter")
		{
			m_pattern = p_value;
			m_exactName = false;
		}
		else if (p_option == "--mode")
		{
			m_mode = toMode(p_value);
		}
		else if (p_option == "--model")
		{
			m_modelFile = p_value;
		}
		else if (p_option == "--expectations")
		{
			m_expectationsFile = p_value;
		}
		else if (p_option == "--expected")
		{
			m_expectedOverride = toNumber(p_value, "--expected");
		}
		else if (p_option == "--max-allocations")
		{
			m_boundOverride = toCount(p_value, "--max-allocations");
		}
		else if (p_option == "--max-retained-bytes")
		{
			m_retainedBoundOverride = toCount(p_value, "--max-retained-bytes");
		}
		else if (p_option == "--output")
		{
			m_outputFile = p_value;
		}
		else
		{
			throw std::invalid_argument("Unknown option " + p_option);
		}
	}

	void BenchmarkOptions::_readExpectations(const std::string& p_executable)
	{
		std::ifstream file(m_expectationsFile);
		if (!file)
		{
			throw std::runtime_error("Cannot read the expectations file " + m_expectationsFile.string());
		}
		std::string row;
		std::size_t rowNumber = 0;
		while (std::getline(file, row))
		{
			++rowNumber;
			if (!row.empty() && row.back() == '\r')
			{
				row.pop_back();
			}
			const std::vector<std::string> FIELDS = splitRow(row);
			if (FIELDS.empty() || FIELDS.at(0) != p_executable)
			{
				continue;
			}
			const std::string CONTEXT = m_expectationsFile.string() + ", row " + std::to_string(rowNumber);
			if (FIELDS.size() < 3)
			{
				throw std::invalid_argument(CONTEXT + ": expected <executable>;<benchmark>;<result>[;<allocations>[;<retained bytes>]]");
			}
			Expectation expectation;
			expectation.result = toNumber(FIELDS.at(2), CONTEXT);
			if (FIELDS.size() > 3 && !FIELDS.at(3).empty())
			{
				expectation.maximumAllocations = toCount(FIELDS.at(3), CONTEXT);
			}
			if (FIELDS.size() > 4 && !FIELDS.at(4).empty())
			{
				expectation.maximumRetainedBytes = toCount(FIELDS.at(4), CONTEXT);
			}
			m_expectations[FIELDS.at(1)] = expectation;
		}
	}

	// --expected, --max-allocations and --max-retained-bytes replace the expectation of one
	// benchmark, so that a check can be seen failing without editing the expectations file.
	void BenchmarkOptions::_applyOverrides()
	{
		if (!m_expectedOverride.has_value() && !m_boundOverride.has_value() && !m_retainedBoundOverride.has_value())
		{
			return;
		}
		if (!m_exactName)
		{
			throw std::invalid_argument("--expected, --max-allocations and --max-retained-bytes apply to the benchmark named by --benchmark");
		}
		Expectation& expectation = m_expectations[m_pattern];
		if (m_expectedOverride.has_value())
		{
			expectation.result = *m_expectedOverride;
		}
		if (m_boundOverride.has_value())
		{
			expectation.maximumAllocations = m_boundOverride;
		}
		if (m_retainedBoundOverride.has_value())
		{
			expectation.maximumRetainedBytes = m_retainedBoundOverride;
		}
	}
}
