/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTGCBMTransition.h"

#include <gtest/gtest.h>

#include <map>
#include <string>
#include <vector>

// A GCBM transition is what FMTSpatialSchedule::getGCBMTransitions and FMTAreaParser give for each stack of a
// disturbance: the average age after it, the attribute that most cells get for each theme, named THEME<n>, and the
// name of the stack, the action followed by the attributes before it. The values are of that form.

namespace
{
    Core::FMTGCBMTransition getTransition()
    {
        return Core::FMTGCBMTransition(0, { { "THEME2", "PEUPLEMENT2" }, { "THEME3", "UTR1" } },
            "COUPETOTALE-PEUPLEMENT1-UTR1");
    }
}

TEST(FMTGCBMTransition, GivenNothingWhenDefaultConstructedThenHasAgeZeroNoThemeAndNoName)
{
    // Arrange and Act
    const Core::FMTGCBMTransition transition;

    // Assert
    EXPECT_EQ(transition.ageafter, 0);
    EXPECT_TRUE(transition.themes.empty());
    EXPECT_TRUE(transition.name.empty());
}

TEST(FMTGCBMTransition, GivenAnAgeThemesAndANameWhenConstructedThenKeepsThem)
{
    // Arrange
    const std::map<std::string, std::string> themes = { { "THEME2", "FEU" } };

    // Act
    const Core::FMTGCBMTransition transition(5, themes, "AFEU-PEUPLEMENT1");

    // Assert
    EXPECT_EQ(transition.ageafter, 5);
    EXPECT_EQ(transition.themes, themes);
    EXPECT_EQ(transition.name, "AFEU-PEUPLEMENT1");
}

TEST(FMTGCBMTransition, GivenATransitionWhenCopyConstructedThenTheCopyEqualsItAndChangesApartFromIt)
{
    // Arrange
    const Core::FMTGCBMTransition transition = getTransition();

    // Act
    Core::FMTGCBMTransition copy(transition);
    const bool equalAfterCopy = (copy == transition);
    copy.themes["THEME2"] = "FEU";

    // Assert
    EXPECT_TRUE(equalAfterCopy);
    EXPECT_EQ(transition.themes.at("THEME2"), "PEUPLEMENT2");
    EXPECT_EQ(copy.themes.at("THEME2"), "FEU");
}

TEST(FMTGCBMTransition, GivenATransitionWhenAssignedThenTheTargetEqualsIt)
{
    // Arrange
    const Core::FMTGCBMTransition transition = getTransition();
    Core::FMTGCBMTransition target(10, { { "THEME1", "UNITE1" } }, "ARECUP-FEU");
    Core::FMTGCBMTransition self = getTransition();

    // Act
    target = transition;
    self = self;

    // Assert
    EXPECT_TRUE(target == transition);
    EXPECT_EQ(target.themes.size(), 2u);
    EXPECT_EQ(target.name, "COUPETOTALE-PEUPLEMENT1-UTR1");
    EXPECT_TRUE(self == transition);
}

TEST(FMTGCBMTransition, GivenTwoTransitionsOfTheSameValuesWhenComparedThenTheyAreEqual)
{
    // Arrange
    const Core::FMTGCBMTransition first = getTransition();
    const Core::FMTGCBMTransition second = getTransition();

    // Act
    const bool equal = (first == second);
    const bool different = (first != second);

    // Assert
    EXPECT_TRUE(equal);
    EXPECT_FALSE(different);
}

// operator== compares the age, the name and every theme with its attribute.
TEST(FMTGCBMTransition, GivenTransitionsThatDifferByOneValueWhenComparedThenTheyAreDifferent)
{
    // Arrange
    const Core::FMTGCBMTransition transition = getTransition();
    Core::FMTGCBMTransition otherAge = getTransition();
    otherAge.ageafter = 1;
    Core::FMTGCBMTransition otherName = getTransition();
    otherName.name = "COUPETOTALE-PEUPLEMENT1-UTR2";
    Core::FMTGCBMTransition otherAttribute = getTransition();
    otherAttribute.themes["THEME3"] = "UTR2";
    Core::FMTGCBMTransition otherTheme(0, { { "THEME1", "PEUPLEMENT2" }, { "THEME3", "UTR1" } },
        "COUPETOTALE-PEUPLEMENT1-UTR1");
    Core::FMTGCBMTransition moreThemes = getTransition();
    moreThemes.themes["THEME1"] = "UNITE1";

    // Act
    const std::vector<bool> different = { transition != otherAge, transition != otherName,
        transition != otherAttribute, transition != otherTheme, transition != moreThemes };
    const std::vector<bool> equal = { transition == otherAge, transition == otherName,
        transition == otherAttribute, transition == otherTheme, transition == moreThemes };

    // Assert
    EXPECT_EQ(different, std::vector<bool>(5, true));
    EXPECT_EQ(equal, std::vector<bool>(5, false));
}
