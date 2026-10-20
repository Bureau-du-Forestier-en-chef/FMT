/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef PARSERROUNDTRIP_H_INCLUDED
#define PARSERROUNDTRIP_H_INCLUDED

#include "FMTModelParser.h"
#include "FMTModel.h"
#include "FMTActualDevelopment.h"
#include "FMTDevelopment.h"
#include "FMTYields.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <map>
#include <sstream>
#include <string>
#include <vector>

// The tools of the integration tests of the parsers: the model of Examples/Models/TWD_land, an empty folder for
// each test case, a model written then read back, and the comparisons of written files and of models.
namespace Testing
{
    // The primary file of TWD_land, from bin/<build type>, the working directory of the tests.
    const std::string TWD_LAND_PRIMARY_FILE = "../../../../Examples/Models/TWD_land/TWD_land.pri";

    // The files written in a folder, by extension.
    using WrittenFiles = std::map<std::string, std::filesystem::path>;

    // A model written, read back from what was written, then written again.
    struct ModelRoundTrip
    {
        Models::FMTModel reread;
        WrittenFiles firstFiles;
        WrittenFiles secondFiles;
    };

    // Returns the folder of the running test case, tests/IntegrationParser/<case> in the build folder, emptied.
    inline std::filesystem::path getCaseFolder()
    {
        const ::testing::TestInfo* const testInfo = ::testing::UnitTest::GetInstance()->current_test_info();
        const std::filesystem::path folder = std::filesystem::path("../../tests/IntegrationParser") / testInfo->name();
        std::filesystem::remove_all(folder);
        std::filesystem::create_directories(folder);
        return folder;
    }

    // Reads one scenario of a project, TWD_land by default.
    inline Models::FMTModel readScenario(const std::string& p_scenario,
        const std::string& p_primaryFile = TWD_LAND_PRIMARY_FILE)
    {
        Parser::FMTModelParser parser;
        parser.setDefaultExceptionHandler();
        return parser.readproject(p_primaryFile, std::vector<std::string>(1, p_scenario)).at(0);
    }

    // Returns the files of a folder, by extension.
    inline WrittenFiles getWrittenFiles(const std::filesystem::path& p_folder)
    {
        WrittenFiles files;
        for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(p_folder))
        {
            if (entry.is_regular_file())
            {
                files[entry.path().extension().string()] = entry.path();
            }
        }
        return files;
    }

    // Writes a model with FMTModelParser::write in a new folder and returns its files.
    inline WrittenFiles writeModel(const Models::FMTModel& p_model, const std::filesystem::path& p_folder)
    {
        std::filesystem::create_directories(p_folder);
        Parser::FMTModelParser parser;
        parser.setDefaultExceptionHandler();
        parser.write(p_model, p_folder.string() + "/");
        return getWrittenFiles(p_folder);
    }

    // Writes a model in <folder>/first, reads it back from its primary file, and writes the model read back in
    // <folder>/second.
    inline ModelRoundTrip writeRereadAndRewrite(const Models::FMTModel& p_model, const std::filesystem::path& p_folder)
    {
        ModelRoundTrip roundTrip;
        roundTrip.firstFiles = writeModel(p_model, p_folder / "first");
        const std::filesystem::path primaryFile = p_folder / "first" / (p_model.getName() + ".pri");
        roundTrip.reread = readScenario("ROOT", primaryFile.string());
        // A model read back as the root of its primary file is named ROOT: it takes the name of the model written,
        // so that both writes name their files, and the primary file lists them, the same way.
        roundTrip.reread.setName(p_model.getName());
        roundTrip.secondFiles = writeModel(roundTrip.reread, p_folder / "second");
        return roundTrip;
    }

    inline std::vector<std::string> readLines(const std::filesystem::path& p_file)
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

    // Returns an empty string when both files have the same lines, else their first difference.
    inline std::string getFirstDifference(const std::filesystem::path& p_first, const std::filesystem::path& p_second)
    {
        const std::vector<std::string> firstLines = readLines(p_first);
        const std::vector<std::string> secondLines = readLines(p_second);
        const size_t lineCount = (std::max)(firstLines.size(), secondLines.size());
        for (size_t line = 0; line < lineCount; ++line)
        {
            const std::string firstLine = (line < firstLines.size()) ? firstLines.at(line) : "<end of file>";
            const std::string secondLine = (line < secondLines.size()) ? secondLines.at(line) : "<end of file>";
            if (firstLine != secondLine)
            {
                return "line " + std::to_string(line + 1) + ": \"" + firstLine + "\" in " + p_first.string()
                    + ", \"" + secondLine + "\" in " + p_second.string();
            }
        }
        return std::string();
    }

    inline std::vector<std::string> getExtensions(const WrittenFiles& p_files)
    {
        std::vector<std::string> extensions;
        for (const std::pair<const std::string, std::filesystem::path>& file : p_files)
        {
            extensions.push_back(file.first);
        }
        return extensions;
    }

    // Expects two writes to have the same files, line by line.
    inline void expectSameFiles(const WrittenFiles& p_first, const WrittenFiles& p_second)
    {
        EXPECT_FALSE(p_first.empty());
        EXPECT_EQ(getExtensions(p_first), getExtensions(p_second));
        for (const std::pair<const std::string, std::filesystem::path>& file : p_first)
        {
            const WrittenFiles::const_iterator secondFile = p_second.find(file.first);
            if (secondFile != p_second.end())
            {
                EXPECT_EQ(getFirstDifference(file.second, secondFile->second), "") << "the " << file.first << " files differ";
            }
        }
    }

    // Expects two models to have the same sections, compared by their operator==. The yields are compared by
    // their values (expectSameYieldValues): FMTYields::operator== compares the addresses of their handlers.
    inline void expectSameSections(const Models::FMTModel& p_read, const Models::FMTModel& p_reread)
    {
        EXPECT_TRUE(p_read.getThemes() == p_reread.getThemes()) << "the themes differ";
        EXPECT_TRUE(p_read.getArea() == p_reread.getArea()) << "the area differs";
        EXPECT_TRUE(p_read.getActions() == p_reread.getActions()) << "the actions differ";
        EXPECT_TRUE(p_read.getTransitions() == p_reread.getTransitions()) << "the transitions differ";
        EXPECT_TRUE(p_read.getLifespan() == p_reread.getLifespan()) << "the lifespans differ";
        EXPECT_TRUE(p_read.getOutputs() == p_reread.getOutputs()) << "the outputs differ";
        EXPECT_TRUE(p_read.getConstraints() == p_reread.getConstraints()) << "the constraints differ";
    }

    // The value of a yield for a development, or what FMTYields::get threw.
    struct YieldValue
    {
        bool thrown = false;
        double value = 0.0;
        std::string text;
    };

    // Returns the value of a yield for a development. A request is built for each call: an FMTYieldRequest keeps
    // the handlers of the yields it was first used with.
    inline YieldValue getYieldValue(const Core::FMTYields& p_yields, const Core::FMTDevelopment& p_development,
        const std::string& p_yield)
    {
        YieldValue yieldValue;
        try {
            yieldValue.value = p_yields.get(p_development.getYieldRequest(), p_yield);
            std::ostringstream text;
            text << std::setprecision(12) << yieldValue.value;
            yieldValue.text = text.str();
        }
        catch (const std::exception& error)
        {
            yieldValue.thrown = true;
            yieldValue.text = std::string("throws ") + error.what();
        }
        return yieldValue;
    }

    // True when both values are equal to a relative 1e-9, or both not a number, or when both calls threw.
    inline bool isSameYieldValue(const YieldValue& p_first, const YieldValue& p_second)
    {
        const double RELATIVE_TOLERANCE = 1e-9;
        if (p_first.thrown || p_second.thrown)
        {
            return p_first.thrown == p_second.thrown;
        }
        if (std::isnan(p_first.value) || std::isnan(p_second.value))
        {
            return std::isnan(p_first.value) && std::isnan(p_second.value);
        }
        return std::abs(p_first.value - p_second.value) <= RELATIVE_TOLERANCE * (std::max)(1.0, std::abs(p_first.value));
    }

    // Expects the yields whose name starts with p_prefix to have the same values in both models, for the
    // developments of the area at the ages of AGES and the periods of PERIODS. The names that start with _ or ~,
    // which FMT computes from a graph or keeps for itself, are left out. A yield that throws must throw in both.
    inline void expectSameYieldValues(const Models::FMTModel& p_read, const Models::FMTModel& p_reread,
        const std::string& p_prefix = std::string())
    {
        const int LAST_AGE = 30;
        const int LAST_PERIOD = 3;
        const Core::FMTYields readYields = p_read.getYields();
        const Core::FMTYields rereadYields = p_reread.getYields();
        size_t comparedYields = 0;
        for (const std::string& yield : readYields.getAllYieldNames())
        {
            if (yield.empty() || yield.at(0) == '_' || yield.at(0) == '~' || yield.compare(0, p_prefix.size(), p_prefix) != 0)
            {
                continue;
            }
            ++comparedYields;
            std::string firstDifference;
            for (const Core::FMTActualDevelopment& actual : p_read.getArea())
            {
                Core::FMTDevelopment development(actual);
                for (int period = 1; period <= LAST_PERIOD && firstDifference.empty(); ++period)
                {
                    development.setPeriod(period);
                    for (int age = 0; age <= LAST_AGE && firstDifference.empty(); ++age)
                    {
                        development.setAge(age);
                        const YieldValue readValue = getYieldValue(readYields, development, yield);
                        const YieldValue rereadValue = getYieldValue(rereadYields, development, yield);
                        if (!isSameYieldValue(readValue, rereadValue))
                        {
                            firstDifference = std::string(development.getMask()) + ", age " + std::to_string(age)
                                + ", period " + std::to_string(period) + ": " + readValue.text + " read, "
                                + rereadValue.text + " read back";
                        }
                    }
                }
            }
            EXPECT_EQ(firstDifference, "") << "the values of " << yield << " differ";
        }
        EXPECT_GT(comparedYields, 0u) << "no yield starts with \"" << p_prefix << "\"";
    }

    // Expects a model read back to be the model written: the same sections and the same yield values.
    inline void expectSameModel(const Models::FMTModel& p_read, const Models::FMTModel& p_reread)
    {
        expectSameSections(p_read, p_reread);
        expectSameYieldValues(p_read, p_reread);
    }
}

#endif
