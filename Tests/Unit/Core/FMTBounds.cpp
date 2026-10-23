/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTAgeBounds.h"
#include "FMTYldBounds.h"
#include "FMTutility.h"

#include <gtest/gtest.h>

#include <limits>

// FMTBounds<int> is reached through FMTAgeBounds, the exported bounds of an age: used directly, it does not
// link next to the tests of FMTSpec (Documentation/Architecture.md, Known Architectural Debt). FMTBounds<double> is
// reached through FMTYldBounds. A bound left open is built as FMTParser::bounds builds it: the lowest or the
// highest value of its type.

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

TEST(FMTBounds, GivenBoundsFromTenToTwentyWhenOutThenRejectsOnlyTheValuesOutside)
{
    // Arrange
    const Core::FMTAgeBounds bounds(Core::FMTsection::Action, 20, 10);

    // Act
    const bool lowerIsOut = bounds.out(10);
    const bool upperIsOut = bounds.out(20);
    const bool belowIsOut = bounds.out(9);
    const bool aboveIsOut = bounds.out(21);

    // Assert
    EXPECT_FALSE(lowerIsOut);
    EXPECT_FALSE(upperIsOut);
    EXPECT_TRUE(belowIsOut);
    EXPECT_TRUE(aboveIsOut);
}

// Unlike in, out does not look at whether the bounds are used: unused bounds are 0 to 0. FMT calls out only on bounds
// that are used, the yield bounds of a spec and of a transition.
TEST(FMTBounds, GivenUnusedBoundsWhenOutThenTakesThemForBoundsFromZeroToZero)
{
    // Arrange
    const Core::FMTAgeBounds bounds;

    // Act
    const bool zeroIsOut = bounds.out(0);
    const bool fiveIsOut = bounds.out(5);

    // Assert
    EXPECT_FALSE(zeroIsOut);
    EXPECT_TRUE(fiveIsOut);
}

TEST(FMTBounds, GivenBoundsWhenEmptyThenReturnsTrueOnlyForUnusedBounds)
{
    // Arrange
    const Core::FMTAgeBounds unused;
    const Core::FMTAgeBounds used(Core::FMTsection::Action, 20, 10);

    // Act
    const bool unusedIsEmpty = unused.empty();
    const bool usedIsEmpty = used.empty();

    // Assert
    EXPECT_TRUE(unusedIsEmpty);
    EXPECT_FALSE(usedIsEmpty);
}

TEST(FMTBounds, GivenUnusedBoundsWhenAddThenTakesTheAddedBounds)
{
    // Arrange
    Core::FMTAgeBounds bounds;
    const Core::FMTAgeBounds added(Core::FMTsection::Action, 20, 10);

    // Act
    bounds.add(added);

    // Assert
    EXPECT_FALSE(bounds.empty());
    EXPECT_EQ(bounds.getLower(), 10);
    EXPECT_EQ(bounds.getUpper(), 20);
}

// _AGE >= 10 AND _AGE <= 20 in an action: FMTActionParser adds a bound open above, then one open below.
TEST(FMTBounds, GivenALowerBoundWhenAddAnUpperBoundThenKeepsBoth)
{
    // Arrange
    Core::FMTAgeBounds bounds(Core::FMTsection::Action, (std::numeric_limits<int>::max)(), 10);
    const Core::FMTAgeBounds upperBound(Core::FMTsection::Action, 20, std::numeric_limits<int>::lowest());

    // Act
    bounds.add(upperBound);

    // Assert
    EXPECT_EQ(bounds.getLower(), 10);
    EXPECT_EQ(bounds.getUpper(), 20);
}

TEST(FMTBounds, GivenBoundsWhenAddClosedBoundsThenReplacesBothBounds)
{
    // Arrange
    Core::FMTAgeBounds bounds(Core::FMTsection::Action, 20, 10);
    const Core::FMTAgeBounds added(Core::FMTsection::Action, 40, 30);

    // Act
    bounds.add(added);

    // Assert
    EXPECT_EQ(bounds.getLower(), 30);
    EXPECT_EQ(bounds.getUpper(), 40);
}

TEST(FMTBounds, GivenBoundsWithTheSameLimitsWhenComparedThenTheyAreEqual)
{
    // Arrange
    const Core::FMTAgeBounds first(Core::FMTsection::Action, 20, 10);
    const Core::FMTAgeBounds second(Core::FMTsection::Action, 20, 10);
    const Core::FMTAgeBounds otherUpper(Core::FMTsection::Action, 30, 10);
    const Core::FMTAgeBounds otherSection(Core::FMTsection::Outputs, 20, 10);

    // Act
    const bool sameLimits = (first == second);
    const bool differentUpper = (first == otherUpper);
    const bool differentSection = (first == otherSection);

    // Assert
    EXPECT_TRUE(sameLimits);
    EXPECT_FALSE(differentUpper);
    EXPECT_FALSE(differentSection);
}

TEST(FMTBounds, GivenBoundsThatDifferByTheirUpperLimitWhenOrderedThenTheSmallerUpperLimitComesFirst)
{
    // Arrange
    const Core::FMTAgeBounds smaller(Core::FMTsection::Action, 20, 10);
    const Core::FMTAgeBounds larger(Core::FMTsection::Action, 30, 10);

    // Act
    const bool smallerFirst = (smaller < larger);
    const bool largerFirst = (larger < smaller);
    const bool beforeItself = (smaller < smaller);

    // Assert
    EXPECT_TRUE(smallerFirst);
    EXPECT_FALSE(largerFirst);
    EXPECT_FALSE(beforeItself);
}

TEST(FMTBounds, GivenAndBoundsWhenSetOrBoundThenTheyNoLongerEqualTheAndBounds)
{
    // Arrange
    const Core::FMTAgeBounds andBounds(Core::FMTsection::Action, 20, 10);
    Core::FMTAgeBounds orBounds(andBounds);

    // Act
    orBounds.setOrBound();

    // Assert
    EXPECT_FALSE(orBounds == andBounds);
    EXPECT_TRUE(orBounds.in(15));
}

TEST(FMTBounds, GivenRealBoundsWhenInThenComparesTheValuesAsReals)
{
    // Arrange
    const Core::FMTYldBounds bounds(Core::FMTsection::Action, "VOLUME", 200.5, 100.25);

    // Act
    const bool acceptsLower = bounds.in(100.25);
    const bool acceptsJustBelowLower = bounds.in(100.24);
    const bool acceptsUpper = bounds.in(200.5);
    const bool acceptsJustAboveUpper = bounds.in(200.51);

    // Assert
    EXPECT_TRUE(acceptsLower);
    EXPECT_FALSE(acceptsJustBelowLower);
    EXPECT_TRUE(acceptsUpper);
    EXPECT_FALSE(acceptsJustAboveUpper);
}

// The bounds are not checked: a lower bound above the upper bound gives bounds that no value is in.
TEST(FMTBounds, GivenALowerBoundAboveTheUpperBoundWhenInThenRejectsEveryValue)
{
    // Arrange
    const Core::FMTAgeBounds bounds(Core::FMTsection::Action, 10, 20);

    // Act
    const bool acceptsLower = bounds.in(20);
    const bool acceptsBetween = bounds.in(15);
    const bool acceptsUpper = bounds.in(10);

    // Assert
    EXPECT_FALSE(bounds.empty());
    EXPECT_FALSE(acceptsLower);
    EXPECT_FALSE(acceptsBetween);
    EXPECT_FALSE(acceptsUpper);
}
