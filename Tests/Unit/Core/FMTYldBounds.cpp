/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTYldBounds.h"
#include "FMTutility.h"

#include <gtest/gtest.h>

#include <limits>
#include <string>

// The bounds of a yield, as the parsers build them: in an action, open bounds at the lowest or the highest double;
// in a transition source or target. Their string writes the yield with its bounds in std::to_string form.

TEST(FMTYldBounds, GivenYieldBoundsWhenCopiedThenTheCopyIsEqual)
{
    // Arrange
    const Core::FMTYldBounds original(Core::FMTsection::Action, "VOLUME", 200.0, 100.0);

    // Act
    const Core::FMTYldBounds copy(original);
    Core::FMTYldBounds assigned;
    assigned = original;

    // Assert
    EXPECT_TRUE(copy == original);
    EXPECT_TRUE(assigned == original);
    EXPECT_EQ(std::string(assigned), "VOLUME >= 100.000000 AND VOLUME <= 200.000000");
}

TEST(FMTYldBounds, GivenTheSameBoundsOfTwoYieldsWhenComparedThenTheyDiffer)
{
    // Arrange
    const Core::FMTYldBounds volume(Core::FMTsection::Action, "VOLUME", 200.0, 100.0);
    const Core::FMTYldBounds stems(Core::FMTsection::Action, "STEMS", 200.0, 100.0);

    // Act
    const bool equal = (volume == stems);

    // Assert
    EXPECT_FALSE(equal);
}

TEST(FMTYldBounds, GivenARealBoundWhenConstructedWithAYieldThenKeepsTheBoundsOfTheReal)
{
    // Arrange
    const Core::FMTYldBounds bound(Core::FMTsection::Action, "VOLUME", 200.0, 100.0);

    // Act
    const Core::FMTYldBounds renamed("STEMS", bound);

    // Assert
    EXPECT_EQ(renamed.getLower(), 100.0);
    EXPECT_EQ(renamed.getUpper(), 200.0);
    EXPECT_EQ(std::string(renamed), "STEMS >= 100.000000 AND STEMS <= 200.000000");
}

TEST(FMTYldBounds, GivenAnActionBoundOpenAboveWhenConvertedToStringThenWritesTheLowerConditionOnly)
{
    // Arrange
    const Core::FMTYldBounds bounds(Core::FMTsection::Action, "VOLUME", (std::numeric_limits<double>::max)(), 100.0);

    // Act
    const std::string written = std::string(bounds);

    // Assert
    EXPECT_EQ(written, "VOLUME >= 100.000000");
}

TEST(FMTYldBounds, GivenAnActionBoundOpenBelowWhenConvertedToStringThenWritesTheUpperConditionOnly)
{
    // Arrange
    const Core::FMTYldBounds bounds(Core::FMTsection::Action, "VOLUME", 200.0, std::numeric_limits<double>::lowest());

    // Act
    const std::string written = std::string(bounds);

    // Assert
    EXPECT_EQ(written, "VOLUME <= 200.000000");
}

TEST(FMTYldBounds, GivenATransitionSourceOpenAboveWhenConvertedToStringThenWritesMaxAgeForTheUpperBound)
{
    // Arrange
    const Core::FMTYldBounds bounds(Core::FMTsection::Transition, Core::FMTkwor::Source, "VOLUME",
        (std::numeric_limits<double>::max)(), 100.0);

    // Act
    const std::string written = std::string(bounds);

    // Assert
    EXPECT_EQ(written, "@YLD(VOLUME,100.000000.._MAXAGE)");
}

TEST(FMTYldBounds, GivenATransitionTargetWhenConvertedToStringThenWritesTheYieldAndItsLowerBound)
{
    // Arrange
    const Core::FMTYldBounds bounds(Core::FMTsection::Transition, Core::FMTkwor::Target, "VOLUME", 150.0, 150.0);

    // Act
    const std::string written = std::string(bounds);

    // Assert
    EXPECT_EQ(written, "VOLUME 150.000000");
}
