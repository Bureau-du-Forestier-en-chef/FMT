/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTAgeBounds.h"
#include "FMTutility.h"

#include <gtest/gtest.h>

#include <limits>

// FMTBounds<int> is reached through FMTAgeBounds, the exported bounds of an age: used directly, it does not
// link next to the tests of FMTSpec (Documentation/Architecture.md, Known Architectural Debt).

TEST(FMTBounds, GivenBoundsFromTenToTwentyWhenInThenAcceptsBothBounds)
{
    // Arrange
    const int lower = 10;
    const int upper = 20;
    const Core::FMTAgeBounds bounds(Core::FMTsection::Action, upper, lower);

    // Act
    const bool acceptsLower = bounds.in(lower);
    const bool acceptsUpper = bounds.in(upper);

    // Assert
    EXPECT_TRUE(acceptsLower);
    EXPECT_TRUE(acceptsUpper);
}

TEST(FMTBounds, GivenBoundsFromTenToTwentyWhenInThenRejectsTheValuesJustOutside)
{
    // Arrange
    const int lower = 10;
    const int upper = 20;
    const Core::FMTAgeBounds bounds(Core::FMTsection::Action, upper, lower);

    // Act
    const bool acceptsBelowLower = bounds.in(lower - 1);
    const bool acceptsAboveUpper = bounds.in(upper + 1);

    // Assert
    EXPECT_FALSE(acceptsBelowLower);
    EXPECT_FALSE(acceptsAboveUpper);
}

TEST(FMTBounds, GivenUnusedBoundsWhenInThenAcceptsAnyValue)
{
    // Arrange
    const Core::FMTAgeBounds bounds;

    // Act
    const bool acceptsLowest = bounds.in(std::numeric_limits<int>::lowest());
    const bool acceptsHighest = bounds.in((std::numeric_limits<int>::max)());

    // Assert
    EXPECT_TRUE(acceptsLowest);
    EXPECT_TRUE(acceptsHighest);
}
