/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTSpec.h"
#include "FMTAgeBounds.h"
#include "FMTutility.h"

#include <gtest/gtest.h>

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
