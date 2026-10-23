/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTSpec.h"
#include "FMTAgeBounds.h"
#include "FMTLockBounds.h"
#include "FMTPerBounds.h"
#include "FMTYldBounds.h"
#include "FMTutility.h"

#include <gtest/gtest.h>

#include <boost/functional/hash.hpp>

#include <limits>
#include <string>
#include <vector>

// The operability of a development, as an action or a transition reads it: the bounds of its period, age, lock and
// yields. The bounds are built as the parsers build them (see Tests/Unit/Core/FMTAgeBounds.cpp).

namespace
{
    const int NO_UPPER_BOUND = (std::numeric_limits<int>::max)();

    // The spec of the condition _AGE >= p_lower AND _AGE <= p_upper of an action.
    Core::FMTSpec getAgeSpec(int p_lower, int p_upper)
    {
        Core::FMTSpec spec;
        spec.addBounds(Core::FMTAgeBounds(Core::FMTsection::Action, p_upper, p_lower));
        return spec;
    }

    // The spec of the condition p_yield >= p_lower of an action.
    Core::FMTSpec getYieldSpec(const std::string& p_yield, double p_lower)
    {
        Core::FMTSpec spec;
        spec.addBounds(Core::FMTYldBounds(Core::FMTsection::Action, p_yield, (std::numeric_limits<double>::max)(),
            p_lower));
        return spec;
    }
}

TEST(FMTSpec, GivenAnAgeRangeWhenAllowWithoutYieldThenAllowsBothAgeBounds)
{
    // Arrange
    const int lowerAge = 10;
    const int upperAge = 20;
    const int period = 1;
    const int lock = 0;
    Core::FMTSpec spec;
    spec.addBounds(Core::FMTAgeBounds(Core::FMTsection::Action, upperAge, lowerAge));

    // Act
    const bool allowsLowerAge = spec.allowWithoutYield(period, lowerAge, lock);
    const bool allowsUpperAge = spec.allowWithoutYield(period, upperAge, lock);

    // Assert
    EXPECT_TRUE(allowsLowerAge);
    EXPECT_TRUE(allowsUpperAge);
}

TEST(FMTSpec, GivenAnAgeRangeWhenAllowWithoutYieldThenRefusesTheAgesJustOutside)
{
    // Arrange
    const int lowerAge = 10;
    const int upperAge = 20;
    const int period = 1;
    const int lock = 0;
    Core::FMTSpec spec;
    spec.addBounds(Core::FMTAgeBounds(Core::FMTsection::Action, upperAge, lowerAge));

    // Act
    const bool allowsYoungerAge = spec.allowWithoutYield(period, lowerAge - 1, lock);
    const bool allowsOlderAge = spec.allowWithoutYield(period, upperAge + 1, lock);

    // Assert
    EXPECT_FALSE(allowsYoungerAge);
    EXPECT_FALSE(allowsOlderAge);
}

TEST(FMTSpec, GivenASpecWithoutBoundsWhenAllowThenAllowsAnyPeriodAgeAndLock)
{
    // Arrange
    const Core::FMTSpec spec;

    // Act
    const bool allowsFirstPeriod = spec.allow(1, 0, 0, {});
    const bool allowsOldLockedDevelopment = spec.allow(30, 200, 5, {});

    // Assert
    EXPECT_TRUE(spec.empty());
    EXPECT_TRUE(allowsFirstPeriod);
    EXPECT_TRUE(allowsOldLockedDevelopment);
}

TEST(FMTSpec, GivenAPeriodRangeWhenAllowWithoutYieldThenAllowsItsPeriodsOnly)
{
    // Arrange: _CP >= 2 AND _CP <= 4
    Core::FMTSpec spec;
    spec.setBounds(Core::FMTPerBounds(Core::FMTsection::Action, 4, 2));

    // Act
    const bool allowsPeriodOne = spec.allowWithoutYield(1, 10, 0);
    const bool allowsPeriodTwo = spec.allowWithoutYield(2, 10, 0);
    const bool allowsPeriodFour = spec.allowWithoutYield(4, 10, 0);
    const bool allowsPeriodFive = spec.allowWithoutYield(5, 10, 0);

    // Assert
    EXPECT_FALSE(allowsPeriodOne);
    EXPECT_TRUE(allowsPeriodTwo);
    EXPECT_TRUE(allowsPeriodFour);
    EXPECT_FALSE(allowsPeriodFive);
}

// A lock bound allows the locks from its lower bound up, whatever its upper bound: _INVLOCK, the lock bound that the
// parsers build for a source, is open above.
TEST(FMTSpec, GivenALockBoundWhenAllowWithoutYieldThenAllowsTheLocksFromItsLowerBound)
{
    // Arrange: _INVLOCK, from lock 1
    Core::FMTSpec spec;
    spec.addBounds(Core::FMTLockBounds(Core::FMTsection::Outputs, Core::FMTkwor::Source, 2, 1));

    // Act
    const bool allowsUnlocked = spec.allowWithoutYield(1, 10, 0);
    const bool allowsLockOne = spec.allowWithoutYield(1, 10, 1);
    const bool allowsLockFive = spec.allowWithoutYield(1, 10, 5);

    // Assert
    EXPECT_FALSE(allowsUnlocked);
    EXPECT_TRUE(allowsLockOne);
    EXPECT_TRUE(allowsLockFive);
}

TEST(FMTSpec, GivenYieldBoundsWhenAllowThenChecksEachYieldValueInTheOrderOfTheYields)
{
    // Arrange: VOLUME >= 100 AND STEMS <= 50
    Core::FMTSpec spec = getYieldSpec("VOLUME", 100.0);
    spec.addBounds(Core::FMTYldBounds(Core::FMTsection::Action, "STEMS", 50.0, std::numeric_limits<double>::lowest()));

    // Act
    const bool allowsBothInBounds = spec.allow(1, 10, 0, { 150.0, 40.0 });
    const bool allowsLowVolume = spec.allow(1, 10, 0, { 99.0, 40.0 });
    const bool allowsManyStems = spec.allow(1, 10, 0, { 150.0, 51.0 });

    // Assert
    EXPECT_EQ(spec.getYlds(), std::vector<std::string>({ "VOLUME", "STEMS" }));
    EXPECT_TRUE(allowsBothInBounds);
    EXPECT_FALSE(allowsLowVolume);
    EXPECT_FALSE(allowsManyStems);
}

TEST(FMTSpec, GivenYieldBoundsWhenAllowYieldsThenIgnoresThePeriodAgeAndLockBounds)
{
    // Arrange: _AGE >= 10 AND _AGE <= 20 AND VOLUME >= 100
    Core::FMTSpec spec = getAgeSpec(10, 20);
    spec.addBounds(Core::FMTYldBounds(Core::FMTsection::Action, "VOLUME", (std::numeric_limits<double>::max)(), 100.0));

    // Act
    const bool allowsVolume = spec.allowYields({ 150.0 });
    const bool allowsWithoutYield = spec.allowWithoutYield(1, 30, 0);

    // Assert
    EXPECT_TRUE(allowsVolume);
    EXPECT_FALSE(allowsWithoutYield);
}

TEST(FMTSpec, GivenAYieldBoundWhenAddBoundsOfTheSameYieldThenMergesThemInOneBound)
{
    // Arrange: VOLUME >= 100, then VOLUME <= 200
    Core::FMTSpec spec = getYieldSpec("VOLUME", 100.0);

    // Act
    spec.addBounds(Core::FMTYldBounds(Core::FMTsection::Action, "VOLUME", 200.0, std::numeric_limits<double>::lowest()));

    // Assert
    ASSERT_EQ(spec.getYlds(), std::vector<std::string>({ "VOLUME" }));
    EXPECT_EQ(spec.getYieldBound("VOLUME").getLower(), 100.0);
    EXPECT_EQ(spec.getYieldBound("VOLUME").getUpper(), 200.0);
}

TEST(FMTSpec, GivenAnAgeBoundWhenEmptyOfEachKindThenOnlyTheAgeIsNotEmpty)
{
    // Arrange
    const Core::FMTSpec spec = getAgeSpec(10, 20);

    // Act
    const bool empty = spec.empty();
    const bool emptyAge = spec.emptyAge();
    const bool emptyPeriod = spec.emptyPeriod();
    const bool emptyLock = spec.emptyLock();
    const bool emptyYields = spec.emptyYlds();

    // Assert
    EXPECT_FALSE(empty);
    EXPECT_FALSE(emptyAge);
    EXPECT_TRUE(emptyPeriod);
    EXPECT_TRUE(emptyLock);
    EXPECT_TRUE(emptyYields);
}

TEST(FMTSpec, GivenPeriodAgeAndLockBoundsWhenGetTheirBoundsThenReturnsThem)
{
    // Arrange
    Core::FMTSpec spec = getAgeSpec(10, 20);
    spec.setBounds(Core::FMTPerBounds(Core::FMTsection::Action, 4, 2));
    spec.addBounds(Core::FMTLockBounds(Core::FMTsection::Transition, Core::FMTkwor::Target, 3, 3));

    // Act
    const std::vector<int> bounds = { spec.getPeriodLowerBound(), spec.getPeriodUpperBound(), spec.getAgeLowerBound(),
        spec.getAgeUpperBound(), spec.getLockLowerBound(), spec.getLockUpperBound() };

    // Assert
    EXPECT_EQ(bounds, std::vector<int>({ 2, 4, 10, 20, 3, 3 }));
}

TEST(FMTSpec, GivenTwoSpecsWhenAddThenTakesTheAgeOfTheAddedSpecAndMergesTheYields)
{
    // Arrange
    Core::FMTSpec spec = getAgeSpec(10, 20);
    spec.addBounds(Core::FMTYldBounds(Core::FMTsection::Action, "VOLUME", 200.0, 100.0));
    Core::FMTSpec added = getAgeSpec(30, 40);
    added.addBounds(Core::FMTYldBounds(Core::FMTsection::Action, "VOLUME", 300.0, 250.0));
    added.addBounds(Core::FMTYldBounds(Core::FMTsection::Action, "STEMS", 60.0, 50.0));

    // Act
    spec.add(added);

    // Assert
    EXPECT_EQ(spec.getAgeLowerBound(), 30);
    EXPECT_EQ(spec.getAgeUpperBound(), 40);
    ASSERT_EQ(spec.getYlds(), std::vector<std::string>({ "VOLUME", "STEMS" }));
    EXPECT_EQ(spec.getYieldBound("VOLUME").getLower(), 250.0);
    EXPECT_EQ(spec.getYieldBound("STEMS").getUpper(), 60.0);
}

TEST(FMTSpec, GivenASpecWhenCopiedThenTheCopyIsEqual)
{
    // Arrange
    Core::FMTSpec original = getAgeSpec(10, 20);
    original.addBounds(Core::FMTYldBounds(Core::FMTsection::Action, "VOLUME", 200.0, 100.0));

    // Act
    const Core::FMTSpec copy(original);
    Core::FMTSpec assigned;
    assigned = original;

    // Assert
    EXPECT_TRUE(copy == original);
    EXPECT_TRUE(assigned == original);
}

TEST(FMTSpec, GivenSpecsThatDifferByTheirAgeBoundsWhenComparedThenTheyDifferAndOneComesFirst)
{
    // Arrange
    const Core::FMTSpec younger = getAgeSpec(10, 20);
    const Core::FMTSpec older = getAgeSpec(10, 30);

    // Act
    const bool equal = (younger == older);
    const bool youngerFirst = (younger < older);
    const bool olderFirst = (older < younger);

    // Assert
    EXPECT_FALSE(equal);
    EXPECT_TRUE(youngerFirst);
    EXPECT_FALSE(olderFirst);
}

TEST(FMTSpec, GivenEqualSpecsWhenHashThenTheyHaveTheSameHash)
{
    // Arrange
    Core::FMTSpec first = getAgeSpec(10, 20);
    first.addBounds(Core::FMTYldBounds(Core::FMTsection::Action, "VOLUME", 200.0, 100.0));
    const Core::FMTSpec second(first);

    // Act
    const size_t firstHash = first.hash();
    const size_t secondHash = boost::hash<Core::FMTSpec>()(second);

    // Assert
    EXPECT_EQ(firstHash, secondHash);
}

TEST(FMTSpec, GivenSpecsThatDifferByAYieldBoundWhenHashThenTheirHashesDiffer)
{
    // Arrange
    const Core::FMTSpec lowVolume = getYieldSpec("VOLUME", 100.0);
    const Core::FMTSpec highVolume = getYieldSpec("VOLUME", 150.0);

    // Act
    const size_t lowHash = lowVolume.hash();
    const size_t highHash = highVolume.hash();

    // Assert
    EXPECT_NE(lowHash, highHash);
}

TEST(FMTSpec, GivenAgeAndYieldBoundsOfAnActionWhenConvertedToStringThenJoinsTheConditionsWithAnd)
{
    // Arrange
    Core::FMTSpec spec = getAgeSpec(10, 20);
    spec.addBounds(Core::FMTYldBounds(Core::FMTsection::Action, "VOLUME", (std::numeric_limits<double>::max)(), 100.0));

    // Act
    const std::string written = std::string(spec);

    // Assert
    EXPECT_EQ(written, "_AGE >= 10 AND _AGE <= 20 AND VOLUME >= 100.000000 ");
}

TEST(FMTSpec, GivenAgeBoundsInsideOtherAgeBoundsWhenIsSubsetOfThenReturnsTrueOnlyForTheInnerBounds)
{
    // Arrange
    const Core::FMTSpec inner = getAgeSpec(12, 18);
    const Core::FMTSpec outer = getAgeSpec(10, 20);

    // Act
    const bool innerInOuter = inner.isSubsetOf(outer);
    const bool outerInInner = outer.isSubsetOf(inner);

    // Assert
    EXPECT_TRUE(innerInOuter);
    EXPECT_FALSE(outerInInner);
}

TEST(FMTSpec, GivenASpecWithoutAgeBoundsWhenIsSubsetOfASpecWithAgeBoundsThenReturnsFalse)
{
    // Arrange
    const Core::FMTSpec unbounded;
    const Core::FMTSpec bounded = getAgeSpec(10, 20);

    // Act
    const bool unboundedInBounded = unbounded.isSubsetOf(bounded);
    const bool boundedInUnbounded = bounded.isSubsetOf(unbounded);

    // Assert
    EXPECT_FALSE(unboundedInBounded);
    EXPECT_TRUE(boundedInUnbounded);
}

// Characterizes the current behavior: for the yields, isSubsetOf compares the lower bounds the other way around from
// the periods and the ages. VOLUME 100..200 is not a subset of VOLUME 50..300, and VOLUME 10..200 is. Its only caller,
// FMTOutputSource::isSubsetOf, serves the cache of the output nodes (FMTOutputNodeCache), which uses a node it finds
// only when its spec equals that of the source it looks for: no value computed by FMT depends on it (Tests/ETAT.md,
// section 5.10).
TEST(FMTSpec, GivenYieldBoundsWhenIsSubsetOfThenComparesTheLowerBoundsTheOtherWayAround)
{
    // Arrange
    Core::FMTSpec inner;
    inner.addBounds(Core::FMTYldBounds(Core::FMTsection::Outputs, "VOLUME", 200.0, 100.0));
    Core::FMTSpec lowerInner;
    lowerInner.addBounds(Core::FMTYldBounds(Core::FMTsection::Outputs, "VOLUME", 200.0, 10.0));
    Core::FMTSpec outer;
    outer.addBounds(Core::FMTYldBounds(Core::FMTsection::Outputs, "VOLUME", 300.0, 50.0));

    // Act
    const bool innerInOuter = inner.isSubsetOf(outer);
    const bool lowerInnerInOuter = lowerInner.isSubsetOf(outer);

    // Assert
    EXPECT_FALSE(innerInOuter);
    EXPECT_TRUE(lowerInnerInOuter);
}
