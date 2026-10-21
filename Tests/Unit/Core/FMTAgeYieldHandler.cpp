/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTAgeYieldHandler.h"
#include "FMTDevelopment.h"
#include "FMTMask.h"
#include "FMTTheme.h"
#include "FMTYieldRequest.h"
#include "FMTutility.h"

#include "ExceptionChain.h"

#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>

namespace
{
    // An age yield VOLUME of 100, 200 and 240 at the base ages 10, 20 and 30.
    Core::FMTAgeYieldHandler getVolumeHandler()
    {
        Core::FMTAgeYieldHandler handler;
        handler.setYieldValues("VOLUME", { 10, 20, 30 }, { 100.0, 200.0, 240.0 });
        return handler;
    }

    // The value of p_yield for a development of age p_age: the handler reads only the age of the request.
    double getAtAge(const Core::FMTYieldHandler& p_handler, const std::string& p_yield, int p_age)
    {
        const Core::FMTDevelopment DEVELOPMENT(Core::FMTMask(), p_age, 0);
        const Core::FMTYieldRequest REQUEST(DEVELOPMENT);
        return p_handler.get(p_yield, REQUEST);
    }
}

TEST(FMTAgeYieldHandler, GivenBaseAgesWhenGetAtABaseAgeThenReturnsItsValue)
{
    // Arrange
    const Core::FMTAgeYieldHandler handler = getVolumeHandler();

    // Act
    const double value = getAtAge(handler, "VOLUME", 20);

    // Assert
    EXPECT_DOUBLE_EQ(value, 200.0);
}

TEST(FMTAgeYieldHandler, GivenBaseAgesWhenGetBetweenTwoBaseAgesThenInterpolatesLinearly)
{
    // Arrange
    const Core::FMTAgeYieldHandler handler = getVolumeHandler();

    // Act
    const double atAge15 = getAtAge(handler, "VOLUME", 15);
    const double atAge25 = getAtAge(handler, "VOLUME", 25);

    // Assert
    EXPECT_DOUBLE_EQ(atAge15, 150.0);
    EXPECT_DOUBLE_EQ(atAge25, 220.0);
}

TEST(FMTAgeYieldHandler, GivenBaseAgesWhenGetBeforeTheFirstBaseAgeThenInterpolatesFromZeroAtAgeZero)
{
    // Arrange
    const Core::FMTAgeYieldHandler handler = getVolumeHandler();

    // Act
    const double atAge0 = getAtAge(handler, "VOLUME", 0);
    const double atAge5 = getAtAge(handler, "VOLUME", 5);

    // Assert
    EXPECT_DOUBLE_EQ(atAge0, 0.0);
    EXPECT_DOUBLE_EQ(atAge5, 50.0);
}

TEST(FMTAgeYieldHandler, GivenBaseAgesWhenGetAfterTheLastBaseAgeThenKeepsTheLastValue)
{
    // Arrange
    const Core::FMTAgeYieldHandler handler = getVolumeHandler();

    // Act
    const double value = getAtAge(handler, "VOLUME", 40);

    // Assert
    EXPECT_DOUBLE_EQ(value, 240.0);
}

TEST(FMTAgeYieldHandler, GivenAYieldThatTheHandlerDoesNotHoldWhenGetThenReturnsZero)
{
    // Arrange
    const Core::FMTAgeYieldHandler handler = getVolumeHandler();

    // Act
    const double value = getAtAge(handler, "UNKNOWN", 20);

    // Assert
    EXPECT_EQ(value, 0.0);
}

TEST(FMTAgeYieldHandler, GivenTheLastBaseAgeWhenGetYieldLinearValueWithoutOutOfRangeThenReturnsTheLastValue)
{
    // Arrange
    const Core::FMTAgeYieldHandler handler = getVolumeHandler();
    const Core::FMTDevelopment development(Core::FMTMask(), 30, 0);
    const Core::FMTYieldRequest request(development);
    const bool allowOutOfRange = false;

    // Act
    const double value = handler.getYieldLinearValue("VOLUME", request, allowOutOfRange);

    // Assert
    EXPECT_DOUBLE_EQ(value, 240.0);
}

TEST(FMTAgeYieldHandler, GivenAnAgeAfterTheLastBaseAgeWhenGetYieldLinearValueWithoutOutOfRangeThenRaisesAnOutOfRangeYield)
{
    // Arrange
    const Core::FMTAgeYieldHandler handler = getVolumeHandler();
    const Core::FMTDevelopment development(Core::FMTMask(), 31, 0);
    const Core::FMTYieldRequest request(development);
    const bool allowOutOfRange = false;

    // Act
    const std::vector<Exception::FMTexc> raised = Testing::getRaisedTypes([&]()
        {
        handler.getYieldLinearValue("VOLUME", request, allowOutOfRange);
        });

    // Assert
    EXPECT_TRUE(Testing::hasType(raised, Exception::FMTexc::FMToutofrangeyield));
}

TEST(FMTAgeYieldHandler, GivenValuesAndBaseAgesOfDifferentSizesWhenSetYieldValuesThenRaisesARangeError)
{
    // Arrange
    Core::FMTAgeYieldHandler handler;

    // Act
    const std::vector<Exception::FMTexc> raised = Testing::getRaisedTypes([&]()
        {
        handler.setYieldValues("VOLUME", { 10, 20, 30 }, { 100.0, 200.0 });
        });

    // Assert
    EXPECT_TRUE(Testing::hasType(raised, Exception::FMTexc::FMTrangeerror));
}

TEST(FMTAgeYieldHandler, GivenOtherBaseAgesWhenSetYieldValuesThenRaisesARangeError)
{
    // Arrange
    Core::FMTAgeYieldHandler handler = getVolumeHandler();

    // Act
    const std::vector<Exception::FMTexc> raised = Testing::getRaisedTypes([&]()
        {
        handler.setYieldValues("STEMS", { 10, 20 }, { 50.0, 60.0 });
        });

    // Assert
    EXPECT_TRUE(Testing::hasType(raised, Exception::FMTexc::FMTrangeerror));
}

TEST(FMTAgeYieldHandler, GivenTheSameBaseAgesWhenSetYieldValuesThenAddsTheYield)
{
    // Arrange
    Core::FMTAgeYieldHandler handler = getVolumeHandler();

    // Act
    handler.setYieldValues("STEMS", { 10, 20, 30 }, { 50.0, 60.0, 70.0 });

    // Assert
    EXPECT_EQ(handler.size(), 2u);
    EXPECT_DOUBLE_EQ(getAtAge(handler, "STEMS", 25), 65.0);
}

TEST(FMTAgeYieldHandler, GivenYieldsWhenGetYieldNamesThenReturnsThemInAlphabeticalOrder)
{
    // Arrange
    Core::FMTAgeYieldHandler handler;
    handler.setYieldValues("VOLUME", { 10 }, { 100.0 });
    handler.setYieldValues("AREA", { 10 }, { 1.0 });
    handler.setYieldValues("B", { 10 }, { 2.0 });

    // Act
    const std::vector<std::string> names = handler.getYieldNames();

    // Assert
    EXPECT_EQ(names, std::vector<std::string>({ "AREA", "B", "VOLUME" }));
}

TEST(FMTAgeYieldHandler, GivenAHandlerWithOneYieldWhenContainsYieldThenFindsThatYieldOnly)
{
    // Arrange
    const Core::FMTAgeYieldHandler handler = getVolumeHandler();

    // Act
    const bool containsVolume = handler.containsYield("VOLUME");
    const bool containsStems = handler.containsYield("STEMS");

    // Assert
    EXPECT_TRUE(containsVolume);
    EXPECT_FALSE(containsStems);
}

TEST(FMTAgeYieldHandler, GivenAHandlerWhenCopiedThenTheCopyIsEqual)
{
    // Arrange
    const Core::FMTAgeYieldHandler original = getVolumeHandler();

    // Act
    const Core::FMTAgeYieldHandler copy(original);
    Core::FMTAgeYieldHandler assigned;
    assigned = original;

    // Assert
    EXPECT_TRUE(copy == original);
    EXPECT_TRUE(assigned == original);
}

TEST(FMTAgeYieldHandler, GivenAHandlerWhenCloneThenTheCloneIsAnAgeYieldHandlerWithTheSameValues)
{
    // Arrange
    const Core::FMTAgeYieldHandler original = getVolumeHandler();

    // Act
    const std::unique_ptr<Core::FMTYieldHandler> clone = original.clone();

    // Assert
    EXPECT_TRUE(clone->getType() == Core::FMTyldtype::FMTageyld);
    EXPECT_DOUBLE_EQ(getAtAge(*clone, "VOLUME", 25), 220.0);
}

TEST(FMTAgeYieldHandler, GivenAFactorAndAYieldNameWhenGetFromFactorThenMultipliesThatYieldOnly)
{
    // Arrange
    Core::FMTAgeYieldHandler handler = getVolumeHandler();
    handler.setYieldValues("STEMS", { 10, 20, 30 }, { 50.0, 60.0, 70.0 });

    // Act
    const std::unique_ptr<Core::FMTYieldHandler> multiplied = handler.getFromFactor(2.0, { "VOLUME" });

    // Assert
    EXPECT_DOUBLE_EQ(getAtAge(*multiplied, "VOLUME", 20), 400.0);
    EXPECT_DOUBLE_EQ(getAtAge(*multiplied, "STEMS", 20), 60.0);
}

TEST(FMTAgeYieldHandler, GivenAFactorWithoutYieldNameWhenGetFromFactorThenMultipliesEveryYield)
{
    // Arrange
    Core::FMTAgeYieldHandler handler = getVolumeHandler();
    handler.setYieldValues("STEMS", { 10, 20, 30 }, { 50.0, 60.0, 70.0 });

    // Act
    const std::unique_ptr<Core::FMTYieldHandler> multiplied = handler.getFromFactor(2.0);

    // Assert
    EXPECT_DOUBLE_EQ(getAtAge(*multiplied, "VOLUME", 20), 400.0);
    EXPECT_DOUBLE_EQ(getAtAge(*multiplied, "STEMS", 20), 120.0);
}

TEST(FMTAgeYieldHandler, GivenAPeakAtTheThirdBaseAgeWhenGetPeakfromThenReturnsTheIndexOfThatBaseAge)
{
    // Arrange
    Core::FMTAgeYieldHandler handler;
    handler.setYieldValues("VOLUME", { 10, 20, 30, 40 }, { 100.0, 200.0, 240.0, 230.0 });

    // Act
    const double peak = handler.getPeakfrom("VOLUME");

    // Assert
    EXPECT_EQ(peak, 2.0);
}

// getPeak gives the years to the peak, the yield _YTP: the age of the peak minus the target age.
TEST(FMTAgeYieldHandler, GivenAPeakAtAge30WhenGetPeakAtAge25ThenReturnsFiveYearsToThePeak)
{
    // Arrange
    Core::FMTAgeYieldHandler handler;
    handler.setYieldValues("VOLUME", { 10, 20, 30, 40 }, { 100.0, 200.0, 240.0, 230.0 });
    const Core::FMTDevelopment development(Core::FMTMask(), 25, 0);
    const Core::FMTYieldRequest request(development);

    // Act
    const double yearsToPeak = handler.getPeak(request, "VOLUME", 25);

    // Assert
    EXPECT_DOUBLE_EQ(yearsToPeak, 5.0);
}

TEST(FMTAgeYieldHandler, GivenAnAgeYieldHandlerWhenGetTypeThenReturnsAgeYield)
{
    // Arrange
    const Core::FMTAgeYieldHandler handler = getVolumeHandler();

    // Act
    const Core::FMTyldtype type = handler.getType();

    // Assert
    EXPECT_TRUE(type == Core::FMTyldtype::FMTageyld);
}

// The section *Y of a yield file: the mask, the line _AGE of the yield names, then a line per base age.
TEST(FMTAgeYieldHandler, GivenAHandlerWhenConvertedToStringThenWritesItsMaskThenALinePerBaseAge)
{
    // Arrange
    const std::vector<Core::FMTTheme> themes = { Core::FMTTheme({ "A1", "A2" }, 0, 0, "THEME1") };
    Core::FMTAgeYieldHandler handler(Core::FMTMask("?", themes));
    handler.setYieldValues("VOLUME", { 10, 20 }, { 100.0, 200.5 });

    // Act
    const std::string written = std::string(handler);

    // Assert
    EXPECT_EQ(written, "*Y ?\n_AGE \tVOLUME\t\n10\t100.000000\t\n20\t200.500000\t\n");
}
