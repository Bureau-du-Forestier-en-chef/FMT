/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTAgeBounds.h"
#include "FMTutility.h"

#include <gtest/gtest.h>

#include <limits>
#include <string>

// The bounds of an age, as the parsers build them: in an action (FMTActionParser, open bounds at the lowest or the
// highest int), in the source or the target of a transition (FMTParser::_setSpec, FMTTransitionParser) and in an
// output source. Their string is the condition written in the section of the bounds.

TEST(FMTAgeBounds, GivenDefaultBoundsWhenConvertedToStringThenWritesNothing)
{
    // Arrange
    const Core::FMTAgeBounds bounds;

    // Act
    const std::string written = std::string(bounds);

    // Assert
    EXPECT_TRUE(bounds.empty());
    EXPECT_EQ(written, "");
}

TEST(FMTAgeBounds, GivenAgeBoundsWhenCopiedThenTheCopyIsEqual)
{
    // Arrange
    const Core::FMTAgeBounds original(Core::FMTsection::Transition, Core::FMTkwor::Source, 20, 10);

    // Act
    const Core::FMTAgeBounds copy(original);
    Core::FMTAgeBounds assigned;
    assigned = original;

    // Assert
    EXPECT_TRUE(copy == original);
    EXPECT_TRUE(assigned == original);
    EXPECT_EQ(assigned.getLower(), 10);
    EXPECT_EQ(assigned.getUpper(), 20);
}

TEST(FMTAgeBounds, GivenASourceAndATargetWithTheSameAgesWhenComparedThenTheyDiffer)
{
    // Arrange
    const Core::FMTAgeBounds source(Core::FMTsection::Transition, Core::FMTkwor::Source, 10, 10);
    const Core::FMTAgeBounds target(Core::FMTsection::Transition, Core::FMTkwor::Target, 10, 10);

    // Act
    const bool equal = (source == target);

    // Assert
    EXPECT_FALSE(equal);
}

TEST(FMTAgeBounds, GivenAnAgeRangeOfAnActionWhenConvertedToStringThenWritesBothConditions)
{
    // Arrange
    const Core::FMTAgeBounds bounds(Core::FMTsection::Action, 20, 10);

    // Act
    const std::string written = std::string(bounds);

    // Assert
    EXPECT_EQ(written, "_AGE >= 10 AND _AGE <= 20");
}

TEST(FMTAgeBounds, GivenAnActionBoundOpenAboveWhenConvertedToStringThenWritesTheLowerConditionOnly)
{
    // Arrange
    const Core::FMTAgeBounds bounds(Core::FMTsection::Action, (std::numeric_limits<int>::max)(), 10);

    // Act
    const std::string written = std::string(bounds);

    // Assert
    EXPECT_EQ(written, "_AGE >= 10");
}

TEST(FMTAgeBounds, GivenAnActionBoundOpenBelowWhenConvertedToStringThenWritesTheUpperConditionOnly)
{
    // Arrange
    const Core::FMTAgeBounds bounds(Core::FMTsection::Action, 20, std::numeric_limits<int>::lowest());

    // Act
    const std::string written = std::string(bounds);

    // Assert
    EXPECT_EQ(written, "_AGE <= 20");
}

TEST(FMTAgeBounds, GivenASingleAgeOfAnActionWhenConvertedToStringThenWritesAnEquality)
{
    // Arrange
    const Core::FMTAgeBounds bounds(Core::FMTsection::Action, 10, 10);

    // Act
    const std::string written = std::string(bounds);

    // Assert
    EXPECT_EQ(written, "_AGE = 10");
}

TEST(FMTAgeBounds, GivenATransitionSourceOpenAboveWhenConvertedToStringThenWritesMaxAgeForTheUpperBound)
{
    // Arrange
    const Core::FMTAgeBounds bounds(Core::FMTsection::Transition, Core::FMTkwor::Source,
        (std::numeric_limits<int>::max)(), 10);

    // Act
    const std::string written = std::string(bounds);

    // Assert
    EXPECT_EQ(written, "@AGE(10.._MAXAGE)");
}

// FMT writes a source of a single age @AGE(10), a form that FMTParser::_setSpec does not read back: the transition
// read back applies to every age (known behavior, issue #358).
TEST(FMTAgeBounds, GivenATransitionSourceOfASingleAgeWhenConvertedToStringThenWritesTheAgeAlone)
{
    // Arrange
    const Core::FMTAgeBounds bounds(Core::FMTsection::Transition, Core::FMTkwor::Source, 10, 10);

    // Act
    const std::string written = std::string(bounds);

    // Assert
    EXPECT_EQ(written, "@AGE(10)");
}

TEST(FMTAgeBounds, GivenATransitionTargetWhenConvertedToStringThenWritesTheAgeAfterTheKeyword)
{
    // Arrange
    const Core::FMTAgeBounds bounds(Core::FMTsection::Transition, Core::FMTkwor::Target, 5, 5);

    // Act
    const std::string written = std::string(bounds);

    // Assert
    EXPECT_EQ(written, "_AGE 5");
}

TEST(FMTAgeBounds, GivenAnOutputSourceWhenConvertedToStringThenWritesTheAgeRange)
{
    // Arrange
    const Core::FMTAgeBounds bounds(Core::FMTsection::Outputs, Core::FMTkwor::Source, 20, 10);

    // Act
    const std::string written = std::string(bounds);

    // Assert
    EXPECT_EQ(written, "@AGE(10..20)");
}
