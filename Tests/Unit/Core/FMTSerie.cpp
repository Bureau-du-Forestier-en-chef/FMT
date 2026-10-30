/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTSerie.h"

#include <gtest/gtest.h>

#include <set>
#include <stdexcept>
#include <string>
#include <vector>

// A serie is a sequence of actions and a length in periods. FMTActionParser builds one for each line of an
// *ACTIONSERIES section, with _ASAP or _ALAP, from two actions or more; FMTAction::setSeries keeps its first parts;
// FMTSrModel::getRotations gives the series of the graph, their length the periods between their first and their last
// action. The actions here are those of TWD_land.

namespace
{
    const std::vector<std::string> ACTIONS = { "COUPETOTALE", "PLANTATION", "EPC" };
}

TEST(FMTSerie, GivenNothingWhenDefaultConstructedThenIsEmptyOfLengthZero)
{
    // Arrange and Act
    const Core::FMTSerie serie;

    // Assert
    EXPECT_TRUE(serie.isEmpty());
    EXPECT_TRUE(serie.getActions().empty());
    EXPECT_EQ(serie.getLength(), 0);
    EXPECT_FALSE(serie.isASAP());
}

// Observation, no defect announced: getSerie removes the last separator, " -> ", which an empty serie does not have:
// std::string::erase raises. Every serie of FMT has two actions or more; isAllowedInSerie calls it on the serie
// it is given (below). An empty serie is ALAP, since its length, 0, is its size.
TEST(FMTSerie, GivenAnEmptySerieWhenGetSerieThenRaisesAnOutOfRangeAndIsAsLateAsPossible)
{
    // Arrange
    const Core::FMTSerie serie;

    // Act and Assert
    EXPECT_THROW(serie.getSerie(), std::out_of_range);
    EXPECT_TRUE(serie.isALAP());
}

TEST(FMTSerie, GivenActionsAndALengthWhenConstructedThenKeepsThemAndJoinsTheActionsWithArrows)
{
    // Arrange
    const int length = 10;

    // Act
    const Core::FMTSerie serie(ACTIONS, length);

    // Assert
    EXPECT_FALSE(serie.isEmpty());
    EXPECT_EQ(serie.getActions(), ACTIONS);
    EXPECT_EQ(serie.getLength(), 10);
    EXPECT_EQ(serie.getSerie(), "COUPETOTALE -> PLANTATION -> EPC");
    EXPECT_EQ(std::string(serie), "COUPETOTALE -> PLANTATION -> EPC");
    EXPECT_FALSE(serie.isASAP());
    EXPECT_FALSE(serie.isALAP());
}

// As FMTActionParser builds a serie: _ALAP gives the length of the number of actions, _ASAP the length -1, neither the
// length 0; with both, _ALAP is kept.
TEST(FMTSerie, GivenTheFlagsOfAnActionSerieLineWhenConstructedThenGivesTheLengthThatEachFlagMeans)
{
    // Arrange and Act
    const Core::FMTSerie alap(ACTIONS, false, true);
    const Core::FMTSerie asap(ACTIONS, true, false);
    const Core::FMTSerie both(ACTIONS, true, true);
    const Core::FMTSerie neither(ACTIONS, false, false);

    // Assert
    EXPECT_EQ(alap.getLength(), 3);
    EXPECT_TRUE(alap.isALAP());
    EXPECT_FALSE(alap.isASAP());
    EXPECT_EQ(asap.getLength(), -1);
    EXPECT_TRUE(asap.isASAP());
    EXPECT_FALSE(asap.isALAP());
    EXPECT_EQ(both.getLength(), 3);
    EXPECT_TRUE(both.isALAP());
    EXPECT_EQ(neither.getLength(), 0);
    EXPECT_FALSE(neither.isALAP());
    EXPECT_FALSE(neither.isASAP());
}

TEST(FMTSerie, GivenSeriesWithFlagsWhenConvertedToStringThenAddsTheirFlag)
{
    // Arrange
    const Core::FMTSerie alap(ACTIONS, false, true);
    const Core::FMTSerie asap(ACTIONS, true, false);

    // Act
    const std::string alapText(alap);
    const std::string asapText(asap);

    // Assert
    EXPECT_EQ(alapText, "COUPETOTALE -> PLANTATION -> EPC _ALAP");
    EXPECT_EQ(asapText, "COUPETOTALE -> PLANTATION -> EPC _ASAP");
}

// Observation, no defect announced: the constructor from a string splits it on each character of the separator
// " -> " (boost::is_any_of), so that each separator gives three empty actions. Nothing in FMT calls it: the parser
// splits the line itself (boost::algorithm::split_regex on -> and <-).
TEST(FMTSerie, GivenAStringOfActionsJoinedByArrowsWhenConstructedThenKeepsAnEmptyActionForEachCharacterBetweenThem)
{
    // Arrange
    const std::string text = "COUPETOTALE -> PLANTATION";

    // Act
    const Core::FMTSerie serie(text, 5);

    // Assert
    EXPECT_EQ(serie.getActions(), std::vector<std::string>({ "COUPETOTALE", "", "", "", "PLANTATION" }));
    EXPECT_EQ(serie.getLength(), 5);
}

TEST(FMTSerie, GivenSeriesThatDifferByTheirLengthOrAnActionWhenComparedThenOnlyTheSameActionsAndLengthAreEqual)
{
    // Arrange
    const Core::FMTSerie serie(ACTIONS, 10);
    const Core::FMTSerie same(ACTIONS, 10);
    const Core::FMTSerie otherLength(ACTIONS, 11);
    const Core::FMTSerie otherAction(std::vector<std::string>({ "COUPETOTALE", "PLANTATION", "EPC2" }), 10);
    const Core::FMTSerie copy(serie);

    // Act
    const std::vector<bool> equal = { serie == same, serie == otherLength, serie == otherAction, serie == copy };
    const std::vector<bool> different = { serie != same, serie != otherLength, serie != otherAction, serie != copy };

    // Assert
    EXPECT_EQ(equal, std::vector<bool>({ true, false, false, true }));
    EXPECT_EQ(different, std::vector<bool>({ false, true, true, false }));
}

// The order is the number of actions, then the length, then the actions in the order of their names; operator> is its
// reverse, and two equal series are neither before nor after each other.
TEST(FMTSerie, GivenSeriesWhenOrderedThenTheShortestComeFirstThenTheShortestLengthThenTheActions)
{
    // Arrange
    const Core::FMTSerie twoActions(std::vector<std::string>({ "PLANTATION", "EPC" }), 20);
    const Core::FMTSerie shortLength(ACTIONS, 5);
    const Core::FMTSerie longLength(ACTIONS, 10);
    const Core::FMTSerie otherActions(std::vector<std::string>({ "COUPETOTALE", "PLANTATION", "PLANTATION" }), 10);

    // Act
    const std::set<Core::FMTSerie> series = { otherActions, longLength, twoActions, shortLength };

    // Assert
    EXPECT_EQ(std::vector<Core::FMTSerie>(series.begin(), series.end()),
        std::vector<Core::FMTSerie>({ twoActions, shortLength, longLength, otherActions }));
    EXPECT_TRUE(twoActions < shortLength);
    EXPECT_TRUE(shortLength > twoActions);
    EXPECT_TRUE(longLength < otherActions);
    EXPECT_TRUE(otherActions > longLength);
    EXPECT_FALSE(longLength < Core::FMTSerie(ACTIONS, 10));
    EXPECT_FALSE(longLength > Core::FMTSerie(ACTIONS, 10));
}

// isAllowedInSerie is what FMTAction::isAllowedInSerie asks of each serie of an action, with the actions done before
// on the development: true when they end on a beginning of the serie that is not the whole serie.
TEST(FMTSerie, GivenActionsThatEndOnABeginningOfTheSerieWhenIsAllowedInSerieThenTheyAreAllowed)
{
    // Arrange
    const Core::FMTSerie serie(ACTIONS, 10);

    // Act
    const std::vector<bool> allowed = { serie.isAllowedInSerie({ "COUPETOTALE" }),
        serie.isAllowedInSerie({ "COUPETOTALE", "PLANTATION" }),
        serie.isAllowedInSerie({ "AFEU", "COUPETOTALE", "PLANTATION" }) };

    // Assert
    EXPECT_EQ(allowed, std::vector<bool>(3, true));
}

TEST(FMTSerie, GivenTheWholeSerieOrActionsThatDoNotBeginItWhenIsAllowedInSerieThenTheyAreNotAllowed)
{
    // Arrange
    const Core::FMTSerie serie(ACTIONS, 10);

    // Act
    const std::vector<bool> allowed = { serie.isAllowedInSerie(ACTIONS), serie.isAllowedInSerie({ "PLANTATION" }),
        serie.isAllowedInSerie({ "COUPETOTALE", "EPC" }), serie.isAllowedInSerie({ "AFEU" }) };

    // Assert
    EXPECT_EQ(allowed, std::vector<bool>(4, false));
}
