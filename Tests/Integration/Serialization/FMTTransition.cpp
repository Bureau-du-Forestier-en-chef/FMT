/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "PickleArchive.h"
#include "ParserRoundTrip.h"
#include "FMTModel.h"
#include "FMTTransition.h"

#include <gtest/gtest.h>

#include <string>
#include <vector>

// FMTTransition through the archive of the pickle of Python, as Python pickles an FMTtransition. Its forks alone are
// in Tests/Integration/Serialization/FMTFork.cpp.

// Defect 25 of Tests/ETAT.md (section 2.7): FMTList::save writes its base FMTObject, which FMTList::load does not read.
TEST(FMTTransition, DISABLED_GivenTheTransitionsOfTWDLandWhenPickledThenUnpicklesTheSameTransitions)
{
    // Arrange
    const std::vector<Core::FMTTransition> transitions = Testing::readScenario("ROOT").getTransitions();

    // Act
    std::vector<Core::FMTTransition> unpickled;
    for (const Core::FMTTransition& transition : transitions)
    {
        unpickled.push_back(Testing::pickleAndUnpickle(transition));
    }

    // Assert
    ASSERT_FALSE(transitions.empty());
    ASSERT_EQ(unpickled.size(), transitions.size());
    for (size_t transition = 0; transition < transitions.size(); ++transition)
    {
        EXPECT_TRUE(unpickled.at(transition) == transitions.at(transition)) << transitions.at(transition).getName();
        EXPECT_EQ(std::string(unpickled.at(transition)), std::string(transitions.at(transition)));
    }
}
