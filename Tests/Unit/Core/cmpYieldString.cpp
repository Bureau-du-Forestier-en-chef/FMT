/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTYieldHandler.h"
#include "FMTComplexYieldHandler.h"
#include "FMTData.h"
#include "FMTDevelopment.h"
#include "FMTutility.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <map>
#include <string>
#include <vector>

// cmpYieldString orders the names of the yields of a handler (the keys of its map of FMTData): the shorter name first,
// then, between names of the same length, by their characters from the last one. It is not the alphabetical order, and
// it is the order in which a handler gives its yields, which defect 12 (#388) involves.

TEST(cmpYieldString, GivenNamesOfDifferentLengthsWhenComparedThenTheShorterComesFirst)
{
    // Arrange
    const Core::cmpYieldString comparator;

    // Act
    const bool shorterFirst = comparator("ZZ", "AAA");
    const bool longerFirst = comparator("AAA", "ZZ");
    const bool emptyFirst = comparator("", "A");

    // Assert
    EXPECT_TRUE(shorterFirst);
    EXPECT_FALSE(longerFirst);
    EXPECT_TRUE(emptyFirst);
}

// BA comes before AB: their last characters, A and B, decide.
TEST(cmpYieldString, GivenNamesOfTheSameLengthWhenComparedThenTheirCharactersFromTheLastOneDecide)
{
    // Arrange
    const Core::cmpYieldString comparator;

    // Act
    const bool lastLetterFirst = comparator("BA", "AB");
    const bool lastLetterSecond = comparator("AB", "BA");
    const bool firstLetterDecides = comparator("AX", "BX");

    // Assert
    EXPECT_TRUE(lastLetterFirst);
    EXPECT_FALSE(lastLetterSecond);
    EXPECT_TRUE(firstLetterDecides);
}

TEST(cmpYieldString, GivenTheSameNameWhenComparedThenNeitherComesFirst)
{
    // Arrange
    const Core::cmpYieldString comparator;

    // Act
    const bool volumeFirst = comparator("VOLUME", "VOLUME");
    const bool emptyFirst = comparator("", "");

    // Assert
    EXPECT_FALSE(volumeFirst);
    EXPECT_FALSE(emptyFirst);
}

TEST(cmpYieldString, GivenNamesWhenSortedThenComeByLengthThenByTheirLastCharacters)
{
    // Arrange
    std::vector<std::string> names = { "VOLUMETOTAL", "VOLUME", "AB", "STEMS", "BA", "AGE" };

    // Act
    std::sort(names.begin(), names.end(), Core::cmpYieldString());

    // Assert
    EXPECT_EQ(names, std::vector<std::string>({ "BA", "AB", "AGE", "STEMS", "VOLUME", "VOLUMETOTAL" }));
}

// A handler keeps its yields in a map ordered by cmpYieldString (FMTComplexYieldHandler::getDataElements): BA before AB,
// where the alphabetical order puts AB, AGE, BA, then STEMS.
TEST(cmpYieldString, GivenTheYieldsOfAHandlerWhenGetDataElementsThenComeInTheOrderOfTheComparator)
{
    // Arrange
    Core::FMTComplexYieldHandler handler;
    const std::vector<std::string> NAMES = { "STEMS", "AB", "AGE", "BA" };
    for (const std::string& name : NAMES)
    {
        handler.pushData(name, Core::FMTData({ 2.0 }, Core::FMTyieldparserop::FMTmultiply, { "VOLUME" }, { true, false }));
    }

    // Act
    std::vector<std::string> names;
    for (const auto& element : handler.getDataElements())
    {
        names.push_back(element.first);
    }

    // Assert
    EXPECT_EQ(names, std::vector<std::string>({ "BA", "AB", "AGE", "STEMS" }));
}
