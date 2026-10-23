/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTPerBounds.h"
#include "FMTutility.h"

#include <gtest/gtest.h>

#include <limits>
#include <string>

// The bounds of a period, as the parsers build them: _CP in an action (FMTActionParser), and the periods of a
// constraint or of an output source (FMTParser::_getPerBound, in the optimization section).

TEST(FMTPerBounds, GivenPeriodBoundsWhenCopiedThenTheCopyIsEqual)
{
    // Arrange
    const Core::FMTPerBounds original(Core::FMTsection::Optimize, 5, 1);

    // Act
    const Core::FMTPerBounds copy(original);
    Core::FMTPerBounds assigned;
    assigned = original;

    // Assert
    EXPECT_TRUE(copy == original);
    EXPECT_TRUE(assigned == original);
    EXPECT_EQ(assigned.getLower(), 1);
    EXPECT_EQ(assigned.getUpper(), 5);
}

TEST(FMTPerBounds, GivenPeriodsThatDifferWhenComparedThenTheyDiffer)
{
    // Arrange
    const Core::FMTPerBounds first(Core::FMTsection::Optimize, 5, 1);
    const Core::FMTPerBounds second(Core::FMTsection::Optimize, 5, 2);

    // Act
    const bool equal = (first == second);

    // Assert
    EXPECT_FALSE(equal);
}

TEST(FMTPerBounds, GivenAPeriodRangeOfAnActionWhenConvertedToStringThenWritesBothConditionsOnCp)
{
    // Arrange
    const Core::FMTPerBounds bounds(Core::FMTsection::Action, 5, 2);

    // Act
    const std::string written = std::string(bounds);

    // Assert
    EXPECT_EQ(written, "_CP >= 2 AND _CP <= 5");
}

TEST(FMTPerBounds, GivenASinglePeriodOfAnActionWhenConvertedToStringThenWritesAnEqualityOnCp)
{
    // Arrange
    const Core::FMTPerBounds bounds(Core::FMTsection::Action, 3, 3);

    // Act
    const std::string written = std::string(bounds);

    // Assert
    EXPECT_EQ(written, "_CP = 3");
}

// The periods of a constraint are written by FMTConstraint, not by its bounds.
TEST(FMTPerBounds, GivenThePeriodsOfAConstraintWhenConvertedToStringThenWritesNothing)
{
    // Arrange
    const Core::FMTPerBounds bounds(Core::FMTsection::Optimize, (std::numeric_limits<int>::max)(), 1);

    // Act
    const std::string written = std::string(bounds);

    // Assert
    EXPECT_EQ(written, "");
}

TEST(FMTPerBounds, GivenPeriodsFromOneWithoutEndWhenInThenAcceptsEveryPeriodFromOne)
{
    // Arrange
    const Core::FMTPerBounds bounds(Core::FMTsection::Optimize, (std::numeric_limits<int>::max)(), 1);

    // Act
    const bool acceptsZero = bounds.in(0);
    const bool acceptsOne = bounds.in(1);
    const bool acceptsLast = bounds.in((std::numeric_limits<int>::max)());

    // Assert
    EXPECT_FALSE(acceptsZero);
    EXPECT_TRUE(acceptsOne);
    EXPECT_TRUE(acceptsLast);
}
