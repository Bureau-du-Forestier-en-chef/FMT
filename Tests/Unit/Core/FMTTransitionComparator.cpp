/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTTransition.h"
#include "FMTFork.h"
#include "FMTMask.h"
#include "FMTTheme.h"
#include "FMTTransitionMask.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <iterator>
#include <string>
#include <vector>

// FMTTransitionComparator finds the transition of an action by its name, as FMTModel pairs each action with its
// transition (*CASE of the transition section). The transitions are built on one theme, as FMTTransitionParser builds
// them.

namespace
{
    std::vector<Core::FMTTheme> getThemes()
    {
        return { Core::FMTTheme({ "A1", "A2" }, 0, 0, "THEME1") };
    }

    // *CASE <name>, *SOURCE ?, *TARGET A2 100
    Core::FMTTransition getTransition(const std::string& p_name)
    {
        Core::FMTFork fork;
        fork.add(Core::FMTTransitionMask("A2", getThemes(), 100.0));
        Core::FMTTransition transition(p_name);
        transition.push_back(Core::FMTMask("?", getThemes()), fork);
        transition.update();
        return transition;
    }
}

TEST(FMTTransitionComparator, GivenANameWhenComparedThenMatchesTheTransitionOfThatNameOnly)
{
    // Arrange
    const Core::FMTTransitionComparator comparator("AFIRE");

    // Act
    const bool matchesFire = comparator(getTransition("AFIRE"));
    const bool matchesSalvage = comparator(getTransition("ARECUP"));
    const bool matchesLowerCase = Core::FMTTransitionComparator("afire")(getTransition("AFIRE"));

    // Assert
    EXPECT_TRUE(matchesFire);
    EXPECT_FALSE(matchesSalvage);
    EXPECT_FALSE(matchesLowerCase);
}

TEST(FMTTransitionComparator, GivenTransitionsWhenFindIfWithTheNameOfAnActionThenFindsItsTransition)
{
    // Arrange
    const std::vector<Core::FMTTransition> transitions = { getTransition("COUPETOTALE"), getTransition("ACARIBOU"),
        getTransition("AFIRE") };

    // Act
    const std::vector<Core::FMTTransition>::const_iterator fire =
        std::find_if(transitions.begin(), transitions.end(), Core::FMTTransitionComparator("AFIRE"));
    const std::vector<Core::FMTTransition>::const_iterator salvage =
        std::find_if(transitions.begin(), transitions.end(), Core::FMTTransitionComparator("ARECUP"));

    // Assert
    EXPECT_EQ(std::distance(transitions.begin(), fire), 2);
    EXPECT_TRUE(salvage == transitions.end());
}
