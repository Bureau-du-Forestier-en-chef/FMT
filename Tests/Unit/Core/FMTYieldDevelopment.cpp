/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTYieldDevelopment.h"

#include <gtest/gtest.h>

#include <boost/dynamic_bitset.hpp>
#include <boost/unordered_set.hpp>

#include <cstdint>
#include <string>

// FMTYieldDevelopment is the key of the cache of complex yields (Tests/Unit/Core/FMTYieldsCache.cpp): the age and the
// period of a development, its resume mask (Tests/Unit/Core/FMTYieldRequest.cpp) and the name of a yield. The age and
// the period are kept on 8 bits, as FMTDevelopment keeps them.

namespace
{
    // The bits of a mask, written the highest first.
    boost::dynamic_bitset<uint8_t> getBits(const std::string& p_bits)
    {
        return boost::dynamic_bitset<uint8_t>(p_bits);
    }

    Core::FMTYieldDevelopment getKey(int p_age, int p_period, const std::string& p_bits, const std::string& p_yield)
    {
        return Core::FMTYieldDevelopment(p_age, p_period, getBits(p_bits), p_yield);
    }
}

TEST(FMTYieldDevelopment, GivenAnAgeAPeriodAMaskAndAYieldWhenConstructedThenGetPeriodReturnsThePeriod)
{
    // Arrange
    const int period = 3;

    // Act
    const Core::FMTYieldDevelopment key = getKey(20, period, "1111111001", "VOLUME");

    // Assert
    EXPECT_EQ(key.getPeriod(), period);
}

TEST(FMTYieldDevelopment, GivenTwoKeysOfTheSameValuesWhenComparedThenTheyAreEqualAndHaveTheSameHash)
{
    // Arrange
    const Core::FMTYieldDevelopment first = getKey(20, 3, "1111111001", "VOLUME");
    const Core::FMTYieldDevelopment second = getKey(20, 3, "1111111001", "VOLUME");

    // Act
    const bool equal = (first == second);
    const size_t firstHash = first.hash();
    const size_t secondHash = second.hash();

    // Assert
    EXPECT_TRUE(equal);
    EXPECT_EQ(firstHash, secondHash);
    EXPECT_EQ(boost::hash<Core::FMTYieldDevelopment>()(first), firstHash);
}

TEST(FMTYieldDevelopment, GivenKeysThatDifferByOneValueWhenComparedThenTheyDiffer)
{
    // Arrange
    const Core::FMTYieldDevelopment key = getKey(20, 3, "1111111001", "VOLUME");

    // Act
    const bool equalToOtherAge = (key == getKey(21, 3, "1111111001", "VOLUME"));
    const bool equalToOtherPeriod = (key == getKey(20, 4, "1111111001", "VOLUME"));
    const bool equalToOtherMask = (key == getKey(20, 3, "1111111010", "VOLUME"));
    const bool equalToALongerMask = (key == getKey(20, 3, "01111111001", "VOLUME"));
    const bool equalToOtherLastLetter = (key == getKey(20, 3, "1111111001", "VOLUMF"));
    const bool equalToOtherFirstLetter = (key == getKey(20, 3, "1111111001", "WOLUME"));
    const bool equalToALongerYield = (key == getKey(20, 3, "1111111001", "VOLUMETOTAL"));

    // Assert
    EXPECT_FALSE(equalToOtherAge);
    EXPECT_FALSE(equalToOtherPeriod);
    EXPECT_FALSE(equalToOtherMask);
    EXPECT_FALSE(equalToALongerMask);
    EXPECT_FALSE(equalToOtherLastLetter);
    EXPECT_FALSE(equalToOtherFirstLetter);
    EXPECT_FALSE(equalToALongerYield);
}

// An age or a period is kept on 8 bits: 256 is 0, 300 is 44, as for FMTDevelopment (Tests/Unit/Core/FMTDevelopment.cpp).
TEST(FMTYieldDevelopment, GivenAnAgeAndAPeriodAbove255WhenConstructedThenKeepsThemModulo256)
{
    // Arrange
    const Core::FMTYieldDevelopment modulo = getKey(0, 44, "1111111001", "VOLUME");

    // Act
    const Core::FMTYieldDevelopment above255 = getKey(256, 300, "1111111001", "VOLUME");

    // Assert
    EXPECT_TRUE(above255 == modulo);
    EXPECT_EQ(above255.getPeriod(), 44);
}

TEST(FMTYieldDevelopment, GivenANegativeAgeAndPeriodWhenConstructedThenKeepsThemAs255)
{
    // Arrange
    const Core::FMTYieldDevelopment largest = getKey(255, 255, "1111111001", "VOLUME");

    // Act
    const Core::FMTYieldDevelopment negative = getKey(-1, -1, "1111111001", "VOLUME");

    // Assert
    EXPECT_TRUE(negative == largest);
    EXPECT_EQ(negative.getPeriod(), 255);
}

TEST(FMTYieldDevelopment, GivenKeysInAHashedSetWhenFindThenFindsTheEqualKeyOnly)
{
    // Arrange
    boost::unordered_set<Core::FMTYieldDevelopment> keys;
    keys.insert(getKey(20, 3, "1111111001", "VOLUME"));
    keys.insert(getKey(20, 3, "1111111001", "STEMS"));

    // Act
    const bool findsVolume = (keys.find(getKey(20, 3, "1111111001", "VOLUME")) != keys.end());
    const bool findsVolumeOfAnotherAge = (keys.find(getKey(30, 3, "1111111001", "VOLUME")) != keys.end());

    // Assert
    EXPECT_EQ(keys.size(), 2u);
    EXPECT_TRUE(findsVolume);
    EXPECT_FALSE(findsVolumeOfAnotherAge);
}
