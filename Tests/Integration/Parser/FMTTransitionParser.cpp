/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "ParserRoundTrip.h"
#include "FMTDevelopment.h"
#include "FMTFork.h"
#include "FMTMask.h"
#include "FMTModel.h"
#include "FMTTransition.h"
#include "FMTYields.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <string>
#include <vector>

// FMTTransitionParser, through the transition section of a scenario of TWD_land read by FMTModelParser.

// Characterizes a known behavior (issue #358): FMTParser::_setSpec reads only @AGE(min..max) and @YLD(yield,min..max)
// in a transition source; it drops the action form _AGE >= 10 without a message, and the source then applies to every
// development of its mask. The scenario INVALID_sourcecondition has such a source, which the disabled row of
// ReadInvalidScenario in Tests/System/Cpp/knownbugs.csv expects to be refused.
TEST(FMTTransitionParser, GivenASourceConditionInTheActionFormWhenReadThenAppliesTheSourceToEveryAge)
{
    // Arrange: *CASE arecup, *SOURCE ? feu ? _AGE >= 10
    const Models::FMTModel model = Testing::readScenario("INVALID_sourcecondition");
    const std::vector<Core::FMTTransition> transitions = model.getTransitions();
    const std::vector<Core::FMTTransition>::const_iterator salvage = std::find_if(transitions.begin(), transitions.end(),
        [](const Core::FMTTransition& p_transition) { return p_transition.getName() == "ARECUP"; });
    ASSERT_NE(salvage, transitions.end());
    const Core::FMTYields yields = model.getYields();
    const Core::FMTDevelopment youngBurnt(Core::FMTMask("UNITE1 FEU UTR1", model.getThemes()), 5, 0, 1);

    // Act
    const Core::FMTFork* fork = salvage->getFork(youngBurnt, yields);

    // Assert
    ASSERT_NE(fork, nullptr);
    EXPECT_TRUE(fork->emptyAge());
}
