/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTTimeYieldHandler.h"
#include "FMTData.h"
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
    // The value of p_yield for a development at period p_period: the handler reads only the period of the request.
    double getAtPeriod(const Core::FMTYieldHandler& p_handler, const std::string& p_yield, int p_period)
    {
        const Core::FMTDevelopment DEVELOPMENT(Core::FMTMask(), 0, 0, p_period);
        const Core::FMTYieldRequest REQUEST(DEVELOPMENT);
        return p_handler.get(p_yield, REQUEST);
    }

    // _DISCOUNTFACTOR(4%,5,<p_type>) as FMTYieldParser builds it: the rate and the years per period in data, the
    // type in the sources.
    Core::FMTData getDiscountFactor(const std::string& p_type)
    {
        return Core::FMTData({ 0.04, 5.0 }, Core::FMTyieldparserop::FMTdiscountfactor, { p_type },
            { false, false, true });
    }

    // The factors below are 1 / (1 + rate)^(years per period x exponent), with the exponent p for FULL, p - 1 for
    // NONE and p - 0.5 for HALF at period p (fix of issue #357, 15427a6d), computed for a rate of 4% and 5 years per
    // period in Tests/ETAT.md (section 5.9).
    constexpr double TOLERANCE = 1e-12;
}

TEST(FMTTimeYieldHandler, GivenValuesFromPeriodOneWhenGetAtEachPeriodThenTakesTheValueOfThatPeriod)
{
    // Arrange
    Core::FMTTimeYieldHandler handler;
    handler.setYieldValues("PERIODVALUE", 1, { 10.0, 20.0, 30.0 });

    // Act
    const double atPeriod1 = getAtPeriod(handler, "PERIODVALUE", 1);
    const double atPeriod2 = getAtPeriod(handler, "PERIODVALUE", 2);
    const double atPeriod3 = getAtPeriod(handler, "PERIODVALUE", 3);

    // Assert
    EXPECT_DOUBLE_EQ(atPeriod1, 10.0);
    EXPECT_DOUBLE_EQ(atPeriod2, 20.0);
    EXPECT_DOUBLE_EQ(atPeriod3, 30.0);
}

TEST(FMTTimeYieldHandler, GivenValuesFromPeriodTwoWhenGetBeforePeriodTwoThenReturnsZero)
{
    // Arrange
    Core::FMTTimeYieldHandler handler;
    handler.setYieldValues("PERIODVALUE", 2, { 10.0, 20.0 });

    // Act
    const double atPeriod0 = getAtPeriod(handler, "PERIODVALUE", 0);
    const double atPeriod1 = getAtPeriod(handler, "PERIODVALUE", 1);

    // Assert
    EXPECT_EQ(atPeriod0, 0.0);
    EXPECT_EQ(atPeriod1, 0.0);
}

TEST(FMTTimeYieldHandler, GivenValuesFromPeriodOneWhenGetAfterTheLastPeriodThenKeepsTheLastValue)
{
    // Arrange
    Core::FMTTimeYieldHandler handler;
    handler.setYieldValues("PERIODVALUE", 1, { 10.0, 20.0, 30.0 });

    // Act
    const double atPeriod5 = getAtPeriod(handler, "PERIODVALUE", 5);

    // Assert
    EXPECT_DOUBLE_EQ(atPeriod5, 30.0);
}

TEST(FMTTimeYieldHandler, GivenAYieldThatTheHandlerDoesNotHoldWhenGetThenReturnsZero)
{
    // Arrange
    Core::FMTTimeYieldHandler handler;
    handler.setYieldValues("PERIODVALUE", 1, { 10.0, 20.0, 30.0 });

    // Act
    const double value = getAtPeriod(handler, "UNKNOWN", 1);

    // Assert
    EXPECT_EQ(value, 0.0);
}

TEST(FMTTimeYieldHandler, GivenTwoTimeYieldsStartingAtTheSamePeriodWhenGetThenEachTakesItsOwnValues)
{
    // Arrange: the lines "PERIODVALUE 1 10 20 30" and "LATE 1 5 6" of a *YT section, as FMTYieldParser reads them.
    Core::FMTTimeYieldHandler handler;
    handler.pushBase(1);
    handler.pushData("PERIODVALUE", 10.0);
    handler.pushData("PERIODVALUE", 20.0);
    handler.pushData("PERIODVALUE", 30.0);
    handler.pushBase(1);
    handler.pushData("LATE", 5.0);
    handler.pushData("LATE", 6.0);

    // Act
    const double periodValueAtPeriod3 = getAtPeriod(handler, "PERIODVALUE", 3);
    const double lateAtPeriod2 = getAtPeriod(handler, "LATE", 2);

    // Assert
    EXPECT_DOUBLE_EQ(periodValueAtPeriod3, 30.0);
    EXPECT_DOUBLE_EQ(lateAtPeriod2, 6.0);
}

// Reveals the defect of issue #388 (defect 12 of section 2.7 of Tests/ETAT.md): get reads the first starting period
// of the handler for every yield. FMTYieldParser pushes one starting period per yield of a *YT section, kept in the
// order of the yield names (cmpYieldString, the shortest name first), so that LATE, shorter, comes first: PERIODVALUE
// then starts at period 3, and gives 0 at period 1 and 10 at period 3.
TEST(FMTTimeYieldHandler, DISABLED_GivenTwoTimeYieldsStartingAtDifferentPeriodsWhenGetThenEachStartsAtItsOwnPeriod)
{
    // Arrange: the lines "PERIODVALUE 1 10 20 30" and "LATE 3 5 6" of a *YT section, as FMTYieldParser reads them.
    Core::FMTTimeYieldHandler handler;
    handler.pushBase(1);
    handler.pushData("PERIODVALUE", 10.0);
    handler.pushData("PERIODVALUE", 20.0);
    handler.pushData("PERIODVALUE", 30.0);
    handler.pushBase(3);
    handler.pushData("LATE", 5.0);
    handler.pushData("LATE", 6.0);

    // Act
    const double atPeriod1 = getAtPeriod(handler, "PERIODVALUE", 1);
    const double atPeriod3 = getAtPeriod(handler, "PERIODVALUE", 3);

    // Assert
    EXPECT_DOUBLE_EQ(atPeriod1, 10.0);
    EXPECT_DOUBLE_EQ(atPeriod3, 30.0);
}

TEST(FMTTimeYieldHandler, GivenAFullDiscountFactorWhenGetOverThreePeriodsThenDiscountsEachWholePeriod)
{
    // Arrange
    Core::FMTTimeYieldHandler handler;
    handler.pushData("DISCOUNTFULL", getDiscountFactor("FULL"));

    // Act
    const double atPeriod1 = getAtPeriod(handler, "DISCOUNTFULL", 1);
    const double atPeriod2 = getAtPeriod(handler, "DISCOUNTFULL", 2);
    const double atPeriod3 = getAtPeriod(handler, "DISCOUNTFULL", 3);

    // Assert
    EXPECT_NEAR(atPeriod1, 0.821927106759, TOLERANCE);
    EXPECT_NEAR(atPeriod2, 0.675564168826, TOLERANCE);
    EXPECT_NEAR(atPeriod3, 0.555264502713, TOLERANCE);
}

TEST(FMTTimeYieldHandler, GivenANoneDiscountFactorWhenGetOverThreePeriodsThenDoesNotDiscountTheFirstPeriod)
{
    // Arrange
    Core::FMTTimeYieldHandler handler;
    handler.pushData("DISCOUNTNONE", getDiscountFactor("NONE"));

    // Act
    const double atPeriod1 = getAtPeriod(handler, "DISCOUNTNONE", 1);
    const double atPeriod2 = getAtPeriod(handler, "DISCOUNTNONE", 2);
    const double atPeriod3 = getAtPeriod(handler, "DISCOUNTNONE", 3);

    // Assert
    EXPECT_NEAR(atPeriod1, 1.0, TOLERANCE);
    EXPECT_NEAR(atPeriod2, 0.821927106759, TOLERANCE);
    EXPECT_NEAR(atPeriod3, 0.675564168826, TOLERANCE);
}

TEST(FMTTimeYieldHandler, GivenAHalfDiscountFactorWhenGetOverThreePeriodsThenDiscountsHalfOfTheFirstPeriod)
{
    // Arrange
    Core::FMTTimeYieldHandler handler;
    handler.pushData("DISCOUNTHALF", getDiscountFactor("HALF"));

    // Act
    const double atPeriod1 = getAtPeriod(handler, "DISCOUNTHALF", 1);
    const double atPeriod2 = getAtPeriod(handler, "DISCOUNTHALF", 2);
    const double atPeriod3 = getAtPeriod(handler, "DISCOUNTHALF", 3);

    // Assert
    EXPECT_NEAR(atPeriod1, 0.906601956075, TOLERANCE);
    EXPECT_NEAR(atPeriod2, 0.745160722739, TOLERANCE);
    EXPECT_NEAR(atPeriod3, 0.612467796912, TOLERANCE);
}

TEST(FMTTimeYieldHandler, GivenAnotherStartingPeriodWhenSetYieldValuesThenRaisesARangeError)
{
    // Arrange
    Core::FMTTimeYieldHandler handler;
    handler.setYieldValues("PERIODVALUE", 1, { 10.0, 20.0, 30.0 });

    // Act
    const std::vector<Exception::FMTexc> raised = Testing::getRaisedTypes([&]()
        {
        handler.setYieldValues("LATE", 3, { 5.0, 6.0 });
        });

    // Assert
    EXPECT_TRUE(Testing::hasType(raised, Exception::FMTexc::FMTrangeerror));
}

TEST(FMTTimeYieldHandler, GivenAHandlerWhenCopiedThenTheCopyIsEqual)
{
    // Arrange
    Core::FMTTimeYieldHandler original;
    original.setYieldValues("PERIODVALUE", 1, { 10.0, 20.0, 30.0 });
    original.pushData("DISCOUNTFULL", getDiscountFactor("FULL"));

    // Act
    const Core::FMTTimeYieldHandler copy(original);
    Core::FMTTimeYieldHandler assigned;
    assigned = original;

    // Assert
    EXPECT_TRUE(copy == original);
    EXPECT_TRUE(assigned == original);
}

TEST(FMTTimeYieldHandler, GivenAHandlerWhenCloneThenTheCloneIsATimeYieldHandlerWithTheSameValues)
{
    // Arrange
    Core::FMTTimeYieldHandler original;
    original.setYieldValues("PERIODVALUE", 1, { 10.0, 20.0, 30.0 });

    // Act
    const std::unique_ptr<Core::FMTYieldHandler> clone = original.clone();

    // Assert
    EXPECT_TRUE(clone->getType() == Core::FMTyldtype::FMTtimeyld);
    EXPECT_DOUBLE_EQ(getAtPeriod(*clone, "PERIODVALUE", 2), 20.0);
}

TEST(FMTTimeYieldHandler, GivenAFactorWhenGetFromFactorThenMultipliesTheValuesOfThatYield)
{
    // Arrange
    Core::FMTTimeYieldHandler handler;
    handler.setYieldValues("PERIODVALUE", 1, { 10.0, 20.0, 30.0 });

    // Act
    const std::unique_ptr<Core::FMTYieldHandler> multiplied = handler.getFromFactor(0.5, { "PERIODVALUE" });

    // Assert
    EXPECT_DOUBLE_EQ(getAtPeriod(*multiplied, "PERIODVALUE", 2), 10.0);
}

// The section *YT of a yield file: the mask, then a line per yield with its starting period and its values, written
// without their trailing zeros.
TEST(FMTTimeYieldHandler, GivenATimeYieldWhenConvertedToStringThenWritesItsStartingPeriodAndItsValues)
{
    // Arrange
    const std::vector<Core::FMTTheme> themes = { Core::FMTTheme({ "A1", "A2" }, 0, 0, "THEME1") };
    Core::FMTTimeYieldHandler handler(Core::FMTMask("?", themes));
    handler.setYieldValues("PERIODVALUE", 1, { 10.0, 20.5, 30.0 });

    // Act
    const std::string written = std::string(handler);

    // Assert
    EXPECT_EQ(written, "*YT ?\nPERIODVALUE 1 10 20.5 30 \n");
}

// Reveals the defect of issue #380: operator std::string ignores the operator of the data, and writes the rate and
// the years of a _DISCOUNTFACTOR as the values of a time yield, after the starting period of another yield
// ("DISCOUNTFULL 1 0.04 5"). Read back, DISCOUNTFULL is 0.04 at period 1 and 5 at period 2.
TEST(FMTTimeYieldHandler, DISABLED_GivenADiscountFactorWhenConvertedToStringThenWritesItAsADiscountFactor)
{
    // Arrange: the lines "PERIODVALUE 1 10 20 30" and "DISCOUNTFULL _DISCOUNTFACTOR(4%,5,FULL)" of a *YT section.
    const std::vector<Core::FMTTheme> themes = { Core::FMTTheme({ "A1", "A2" }, 0, 0, "THEME1") };
    Core::FMTTimeYieldHandler handler(Core::FMTMask("?", themes));
    handler.setYieldValues("PERIODVALUE", 1, { 10.0, 20.0, 30.0 });
    handler.pushData("DISCOUNTFULL", getDiscountFactor("FULL"));

    // Act
    const std::string written = std::string(handler);

    // Assert
    EXPECT_NE(written.find("DISCOUNTFULL _DISCOUNTFACTOR("), std::string::npos);
}

// Reveals the defect of issue #380: without a starting period, as in a *YT section of discount factors only,
// operator std::string reads the starting period of the yield in an empty vector, and raises.
TEST(FMTTimeYieldHandler, DISABLED_GivenOnlyADiscountFactorWhenConvertedToStringThenWritesItWithoutRaising)
{
    // Arrange: the line "DISCOUNTFULL _DISCOUNTFACTOR(4%,5,FULL)", alone in a *YT section.
    const std::vector<Core::FMTTheme> themes = { Core::FMTTheme({ "A1", "A2" }, 0, 0, "THEME1") };
    Core::FMTTimeYieldHandler handler(Core::FMTMask("?", themes));
    handler.pushData("DISCOUNTFULL", getDiscountFactor("FULL"));
    std::string written;

    // Act
    const std::vector<Exception::FMTexc> raised = Testing::getRaisedTypes([&]()
        {
        written = std::string(handler);
        });

    // Assert
    EXPECT_TRUE(raised.empty());
    EXPECT_NE(written.find("DISCOUNTFULL _DISCOUNTFACTOR("), std::string::npos);
}
