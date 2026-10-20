/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "ParserRoundTrip.h"
#include "FMTScheduleParser.h"
#include "FMTModelParser.h"
#include "FMTModel.h"
#include "FMTSchedule.h"
#include "FMTConstants.h"

#include <gtest/gtest.h>

#include <filesystem>
#include <string>
#include <vector>

// FMTScheduleParser::write with the schedule files it writes, read back by FMTScheduleParser::read. The schedules
// come from the schedule section of the scenario LP of TWD_land, which needs no solver.
namespace
{
    // The schedules of the scenario LP, one per period, and the model that reads them.
    struct ScenarioSchedules
    {
        Models::FMTModel model;
        std::vector<Core::FMTSchedule> schedules;
    };

    ScenarioSchedules readScenarioSchedules()
    {
        ScenarioSchedules scenarioSchedules;
        scenarioSchedules.model = Testing::readScenario("LP");
        Parser::FMTModelParser parser;
        parser.setDefaultExceptionHandler();
        scenarioSchedules.schedules = parser.readSchedules(Testing::TWD_LAND_PRIMARY_FILE,
            std::vector<Models::FMTModel>(1, scenarioSchedules.model)).at(0);
        return scenarioSchedules;
    }

    std::vector<Core::FMTSchedule> readSchedules(const Models::FMTModel& p_model, const std::filesystem::path& p_file)
    {
        Parser::FMTScheduleParser parser;
        return parser.read(p_model.getThemes(), Core::FMTConstants(), p_model.getActions(), p_file.string());
    }

    void expectSameSchedules(const std::vector<Core::FMTSchedule>& p_expected, const std::vector<Core::FMTSchedule>& p_actual)
    {
        ASSERT_EQ(p_actual.size(), p_expected.size());
        for (size_t schedule = 0; schedule < p_expected.size(); ++schedule)
        {
            EXPECT_TRUE(p_actual.at(schedule) == p_expected.at(schedule)) << "the schedules of period "
                << p_expected.at(schedule).getPeriod() << " differ";
        }
    }
}

TEST(FMTScheduleParser, GivenTheSchedulesOfAScenarioWhenWriteThenRereadsAndRewritesTheSameSchedules)
{
    // Arrange
    const ScenarioSchedules scenarioSchedules = readScenarioSchedules();
    ASSERT_GT(scenarioSchedules.schedules.size(), 1u);
    const std::filesystem::path folder = Testing::getCaseFolder();
    const std::filesystem::path firstFile = folder / "first.seq";
    const std::filesystem::path secondFile = folder / "second.seq";
    Parser::FMTScheduleParser parser;

    // Act
    parser.write(scenarioSchedules.schedules, firstFile.string());
    const std::vector<Core::FMTSchedule> reread = readSchedules(scenarioSchedules.model, firstFile);
    parser.write(reread, secondFile.string());

    // Assert
    expectSameSchedules(scenarioSchedules.schedules, reread);
    EXPECT_EQ(Testing::getFirstDifference(firstFile, secondFile), "");
}

TEST(FMTScheduleParser, GivenAScheduleFileWhenWriteWithAppendThenRereadsTheSchedulesOfBothWrites)
{
    // Arrange
    const ScenarioSchedules scenarioSchedules = readScenarioSchedules();
    ASSERT_GT(scenarioSchedules.schedules.size(), 1u);
    const std::vector<Core::FMTSchedule> firstPeriod(scenarioSchedules.schedules.begin(), scenarioSchedules.schedules.begin() + 1);
    const std::vector<Core::FMTSchedule> laterPeriods(scenarioSchedules.schedules.begin() + 1, scenarioSchedules.schedules.end());
    const std::filesystem::path file = Testing::getCaseFolder() / "appended.seq";
    Parser::FMTScheduleParser parser;
    parser.write(firstPeriod, file.string());

    // Act
    parser.write(laterPeriods, file.string(), true);

    // Assert
    expectSameSchedules(scenarioSchedules.schedules, readSchedules(scenarioSchedules.model, file));
}
