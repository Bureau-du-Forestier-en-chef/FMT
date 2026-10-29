/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "PickleArchive.h"
#include "ParserRoundTrip.h"
#include "FMTFork.h"
#include "FMTModel.h"
#include "FMTTransition.h"

#include <gtest/gtest.h>

#include <string>
#include <vector>

// FMTFork and its FMTTransitionMask through the archive of the pickle of Python: the sources of the transitions of a
// model, which a transition carries through its FMTList (Tests/Integration/Serialization/FMTTransition.cpp).

TEST(FMTFork, GivenTheForksOfTheTransitionsOfTWDLandWhenPickledThenUnpicklesTheSameForks)
{
    // Arrange
    const std::vector<Core::FMTTransition> transitions = Testing::readScenario("ROOT").getTransitions();
    std::vector<Core::FMTFork> forks;
    for (const Core::FMTTransition& transition : transitions)
    {
        for (const auto& maskAndFork : transition)
        {
            forks.push_back(maskAndFork.second);
        }
    }

    // Act
    std::vector<Core::FMTFork> unpickled;
    for (const Core::FMTFork& fork : forks)
    {
        unpickled.push_back(Testing::pickleAndUnpickle(fork));
    }

    // Assert
    ASSERT_FALSE(forks.empty());
    ASSERT_EQ(unpickled.size(), forks.size());
    for (size_t fork = 0; fork < forks.size(); ++fork)
    {
        EXPECT_TRUE(unpickled.at(fork) == forks.at(fork)) << "fork " << fork;
        EXPECT_EQ(unpickled.at(fork).size(), forks.at(fork).size());
        EXPECT_EQ(std::string(unpickled.at(fork)), std::string(forks.at(fork)));
    }
}
