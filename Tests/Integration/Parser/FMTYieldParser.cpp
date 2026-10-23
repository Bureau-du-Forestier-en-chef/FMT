/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "ParserRoundTrip.h"
#include "FMTDevelopment.h"
#include "FMTMask.h"
#include "FMTModel.h"
#include "FMTYields.h"

#include <gtest/gtest.h>

#include <string>
#include <vector>

// FMTYieldParser, through the yield section of the scenario timeyieldcolumns of TWD_land read by FMTModelParser: a
// block *YT written in columns (_CP CPA CPB), whose lines give the values of CPA and CPB at the periods 1, 3 and 5.

namespace
{
    // The values of p_yield for a development of peuplement1 of age 10, at the periods 1 to 7. A request is built for
    // each period: it keeps the yields of its first use (Tests/ETAT.md, section 7).
    std::vector<double> getValuesByPeriod(const Models::FMTModel& p_model, const std::string& p_yield)
    {
        const Core::FMTYields yields = p_model.getYields();
        Core::FMTDevelopment development(Core::FMTMask("UNITE1 PEUPLEMENT1 UTR1", p_model.getThemes()), 10, 0, 1);
        std::vector<double> values;
        for (int period = 1; period <= 7; ++period)
        {
            development.setPeriod(period);
            values.push_back(yields.get(development.getYieldRequest(), p_yield));
        }
        return values;
    }
}

// Disabled: for each line after the first, FMTYieldParser::read fills the periods from the last one already pushed, which
// it pushes a second time: each line delays the values of the next ones by one period, and CPA is 10, 10, 10, 30, 30,
// 30 and 50 (#387, defect 13 of section 2.7 of Tests/ETAT.md).
TEST(FMTYieldParser, DISABLED_GivenTimeYieldsWrittenInColumnsWhenReadThenEachLineGivesItsValuesFromItsPeriod)
{
    // Arrange: _CP CPA CPB, then the lines 1 10 100, 3 30 300 and 5 50 500
    const Models::FMTModel model = Testing::readScenario("timeyieldcolumns");

    // Act
    const std::vector<double> firstYield = getValuesByPeriod(model, "CPA");
    const std::vector<double> secondYield = getValuesByPeriod(model, "CPB");

    // Assert
    EXPECT_EQ(firstYield, std::vector<double>({ 10.0, 10.0, 30.0, 30.0, 50.0, 50.0, 50.0 }));
    EXPECT_EQ(secondYield, std::vector<double>({ 100.0, 100.0, 300.0, 300.0, 500.0, 500.0, 500.0 }));
}
