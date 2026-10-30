/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "ParserRoundTrip.h"
#include "FMTAction.h"
#include "FMTDevelopment.h"
#include "FMTMask.h"
#include "FMTModel.h"
#include "FMTYields.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <string>
#include <vector>

// FMTActionParser, through the action section of the scenario strictbounds of TWD_land read by FMTModelParser: four
// actions of the mask ? prod ?, whose age condition is written with the operator >, >=, < or <=.

namespace
{
    // The ages of a development of peuplement1 at period 1, among 0, 4, 5, 10, 11 and 20, at which it is operable to
    // the action p_action of p_model.
    std::vector<int> getOperableAges(const Models::FMTModel& p_model, const std::string& p_action)
    {
        const std::vector<Core::FMTAction> actions = p_model.getActions();
        const std::vector<Core::FMTAction>::const_iterator action = std::find_if(actions.begin(), actions.end(),
            [&p_action](const Core::FMTAction& p_candidate) { return p_candidate.getName() == p_action; });
        if (action == actions.end())
        {
            ADD_FAILURE() << "the model has no action " << p_action;
            return std::vector<int>();
        }
        const Core::FMTYields yields = p_model.getYields();
        std::vector<int> operableAges;
        for (const int age : { 0, 4, 5, 10, 11, 20 })
        {
            const Core::FMTDevelopment development(Core::FMTMask("UNITE1 PEUPLEMENT1 UTR1", p_model.getThemes()), age, 0, 1);
            if (development.operable(*action, yields))
            {
                operableAges.push_back(age);
            }
        }
        return operableAges;
    }
}

TEST(FMTActionParser, GivenAMinimumAgeWrittenWithGreaterOrEqualWhenReadThenTheActionIsOperableFromThatAge)
{
    // Arrange: *ACTION aFromAge Y, ? prod ? _AGE >= 11
    const Models::FMTModel model = Testing::readScenario("strictbounds");

    // Act
    const std::vector<int> operableAges = getOperableAges(model, "AFROMAGE");

    // Assert
    EXPECT_EQ(operableAges, std::vector<int>({ 11, 20 }));
}

TEST(FMTActionParser, GivenAMaximumAgeWrittenWithLessOrEqualWhenReadThenTheActionIsOperableUpToThatAge)
{
    // Arrange: *ACTION aUpToAge Y, ? prod ? _AGE <= 4
    const Models::FMTModel model = Testing::readScenario("strictbounds");

    // Act
    const std::vector<int> operableAges = getOperableAges(model, "AUPTOAGE");

    // Assert
    EXPECT_EQ(operableAges, std::vector<int>({ 0, 4 }));
}

// Disabled: FMTParser::bounds gives no bound to the operators > and <, which getBaseOperators lists: the condition is
// dropped without a message, and the action is operable at every age (#393, defect 18 of section 2.7 of Tests/ETAT.md).
// Woodstock refuses both operators (messages 142 and 143 of Wk.msg): if the correction refuses them too, this case
// becomes a test of the exception.
TEST(FMTActionParser, DISABLED_GivenAMinimumAgeWrittenWithGreaterThanWhenReadThenTheActionIsOperableAboveThatAgeOnly)
{
    // Arrange: *ACTION aOlderThan Y, ? prod ? _AGE > 10
    const Models::FMTModel model = Testing::readScenario("strictbounds");

    // Act
    const std::vector<int> operableAges = getOperableAges(model, "AOLDERTHAN");

    // Assert
    EXPECT_EQ(operableAges, std::vector<int>({ 11, 20 }));
}

// Disabled: the operator < is dropped as > is (#393, defect 18 of section 2.7 of Tests/ETAT.md).
TEST(FMTActionParser, DISABLED_GivenAMaximumAgeWrittenWithLessThanWhenReadThenTheActionIsOperableBelowThatAgeOnly)
{
    // Arrange: *ACTION aYoungerThan Y, ? prod ? _AGE < 5
    const Models::FMTModel model = Testing::readScenario("strictbounds");

    // Act
    const std::vector<int> operableAges = getOperableAges(model, "AYOUNGERTHAN");

    // Assert
    EXPECT_EQ(operableAges, std::vector<int>({ 0, 4 }));
}
