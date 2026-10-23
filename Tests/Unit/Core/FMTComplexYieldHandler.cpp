/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTComplexYieldHandler.h"
#include "FMTAgeYieldHandler.h"
#include "FMTData.h"
#include "FMTDevelopment.h"
#include "FMTMask.h"
#include "FMTSpec.h"
#include "FMTTheme.h"
#include "FMTTimeYieldHandler.h"
#include "FMTYieldRequest.h"
#include "FMTYields.h"
#include "FMTYldBounds.h"
#include "FMTutility.h"

#include "ExceptionChain.h"

#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>

// A complex yield finds the yields it is computed from only through a request that FMTYields::get prepared: the
// cases evaluate it through FMTYields::get (Documentation/Architecture.md, Known Architectural Debt). The data are
// built as FMTYieldParser builds them (see Tests/Unit/Core/FMTData.cpp).

namespace
{
    // One theme of two attributes: the handlers select every development with the mask "?".
    std::vector<Core::FMTTheme> getThemes()
    {
        return { Core::FMTTheme({ "A1", "A2" }, 0, 0, "THEME1") };
    }

    // The age yields VOLUME (100, 200 and 240 at the ages 10, 20 and 30), STEMS (50, 60 and 70) and ZERO, the time
    // yield PERIODVALUE (10, 20 and 30 from period 1), then the complex yields of p_complex, in that order.
    Core::FMTYields getYields(const Core::FMTComplexYieldHandler& p_complex)
    {
        const Core::FMTMask ALL("?", getThemes());
        Core::FMTAgeYieldHandler ageYields(ALL);
        ageYields.setYieldValues("VOLUME", { 10, 20, 30 }, { 100.0, 200.0, 240.0 });
        ageYields.setYieldValues("STEMS", { 10, 20, 30 }, { 50.0, 60.0, 70.0 });
        ageYields.setYieldValues("ZERO", { 10, 20, 30 }, { 0.0, 0.0, 0.0 });
        Core::FMTTimeYieldHandler timeYields(ALL);
        timeYields.setYieldValues("PERIODVALUE", 1, { 10.0, 20.0, 30.0 });
        Core::FMTYields yields;
        yields.pushBackAgeHandler(ALL, ageYields);
        yields.pushBackTimeHandler(ALL, timeYields);
        yields.push_back(ALL, p_complex.clone());
        yields.update();
        return yields;
    }

    // A complex yield handler of the mask "?" that holds p_data under the name p_yield.
    Core::FMTComplexYieldHandler getHandler(const std::string& p_yield, const Core::FMTData& p_data)
    {
        Core::FMTComplexYieldHandler handler(Core::FMTMask("?", getThemes()));
        handler.pushData(p_yield, p_data);
        return handler;
    }

    // The value of p_yield for a development of age p_age at period p_period. A request serves one call only
    // (Documentation/Architecture.md, Known Architectural Debt).
    double getYield(const Core::FMTYields& p_yields, const std::string& p_yield, int p_age, int p_period = 1)
    {
        const Core::FMTDevelopment DEVELOPMENT(Core::FMTMask("A1", getThemes()), p_age, 0, p_period);
        const Core::FMTYieldRequest REQUEST(DEVELOPMENT);
        return p_yields.get(REQUEST, p_yield);
    }
}

TEST(FMTComplexYieldHandler, GivenASumOfAYieldAndANumberWhenGetThenAddsThem)
{
    // Arrange: SUMWITHNUMBER _SUM(VOLUME,4)
    const Core::FMTYields yields = getYields(getHandler("SUMWITHNUMBER",
        Core::FMTData({ 4.0 }, Core::FMTyieldparserop::FMTsum, { "VOLUME" }, { true, false })));

    // Act
    const double value = getYield(yields, "SUMWITHNUMBER", 20);

    // Assert
    EXPECT_DOUBLE_EQ(value, 204.0);
}

TEST(FMTComplexYieldHandler, GivenASumOfTheSameYieldTwiceWhenGetThenAddsItTwice)
{
    // Arrange: SUMTWICE _SUM(VOLUME,VOLUME)
    const Core::FMTYields yields = getYields(getHandler("SUMTWICE",
        Core::FMTData({}, Core::FMTyieldparserop::FMTsum, { "VOLUME", "VOLUME" }, { true, true })));

    // Act
    const double value = getYield(yields, "SUMTWICE", 20);

    // Assert
    EXPECT_DOUBLE_EQ(value, 400.0);
}

TEST(FMTComplexYieldHandler, GivenASumWithAYieldThatNoHandlerHoldsWhenGetThenCountsThatYieldAsZero)
{
    // Arrange: WITHUNKNOWN _SUM(VOLUME,UNKNOWN)
    const Core::FMTYields yields = getYields(getHandler("WITHUNKNOWN",
        Core::FMTData({}, Core::FMTyieldparserop::FMTsum, { "VOLUME", "UNKNOWN" }, { true, true })));

    // Act
    const double value = getYield(yields, "WITHUNKNOWN", 20);

    // Assert
    EXPECT_DOUBLE_EQ(value, 200.0);
}

TEST(FMTComplexYieldHandler, GivenAProductOfAYieldAndANumberWhenGetThenMultipliesThem)
{
    // Arrange: HALFVOLUME _MULTIPLY(VOLUME,0.5)
    const Core::FMTYields yields = getYields(getHandler("HALFVOLUME",
        Core::FMTData({ 0.5 }, Core::FMTyieldparserop::FMTmultiply, { "VOLUME" }, { true, false })));

    // Act
    const double value = getYield(yields, "HALFVOLUME", 20);

    // Assert
    EXPECT_DOUBLE_EQ(value, 100.0);
}

TEST(FMTComplexYieldHandler, GivenASubtractionWhenGetThenSubtractsTheFollowingArgumentsFromTheFirst)
{
    // Arrange: LESS _SUBTRACT(300,VOLUME)
    const Core::FMTYields yields = getYields(getHandler("LESS",
        Core::FMTData({ 300.0 }, Core::FMTyieldparserop::FMTsubstract, { "VOLUME" }, { false, true })));

    // Act
    const double value = getYield(yields, "LESS", 20);

    // Assert
    EXPECT_DOUBLE_EQ(value, 100.0);
}

TEST(FMTComplexYieldHandler, GivenAQuotientWhenGetThenDividesTheFirstArgumentByTheFollowing)
{
    // Arrange: QUARTER _DIVIDE(VOLUME,4)
    const Core::FMTYields yields = getYields(getHandler("QUARTER",
        Core::FMTData({ 4.0 }, Core::FMTyieldparserop::FMTdivide, { "VOLUME" }, { true, false })));

    // Act
    const double value = getYield(yields, "QUARTER", 20);

    // Assert
    EXPECT_DOUBLE_EQ(value, 50.0);
}

TEST(FMTComplexYieldHandler, GivenAQuotientByAZeroYieldWhenGetThenReturnsZero)
{
    // Arrange: BYZERO _DIVIDE(VOLUME,ZERO)
    const Core::FMTYields yields = getYields(getHandler("BYZERO",
        Core::FMTData({}, Core::FMTyieldparserop::FMTdivide, { "VOLUME", "ZERO" }, { true, true })));

    // Act
    const double value = getYield(yields, "BYZERO", 20);

    // Assert
    EXPECT_EQ(value, 0.0);
}

TEST(FMTComplexYieldHandler, GivenAQuotientWithManyDecimalsWhenGetThenRoundsItToEightDecimals)
{
    // Arrange: THIRD _DIVIDE(VOLUME,3)
    const Core::FMTYields yields = getYields(getHandler("THIRD",
        Core::FMTData({ 3.0 }, Core::FMTyieldparserop::FMTdivide, { "VOLUME" }, { true, false })));

    // Act
    const double value = getYield(yields, "THIRD", 20);

    // Assert
    EXPECT_DOUBLE_EQ(value, 66.66666667);
}

TEST(FMTComplexYieldHandler, GivenARangeWhenGetInsideItsBoundsThenReturnsOne)
{
    // Arrange: INRANGE _RANGE(VOLUME,150,220)
    const Core::FMTYields yields = getYields(getHandler("INRANGE",
        Core::FMTData({ 150.0, 220.0 }, Core::FMTyieldparserop::FMTrange, { "VOLUME" }, { true, false, false })));

    // Act
    const double atLowerBound = getYield(yields, "INRANGE", 15);
    const double inside = getYield(yields, "INRANGE", 20);

    // Assert
    EXPECT_EQ(atLowerBound, 1.0);
    EXPECT_EQ(inside, 1.0);
}

TEST(FMTComplexYieldHandler, GivenARangeWhenGetOutsideItsBoundsThenReturnsZero)
{
    // Arrange: INRANGE _RANGE(VOLUME,150,220)
    const Core::FMTYields yields = getYields(getHandler("INRANGE",
        Core::FMTData({ 150.0, 220.0 }, Core::FMTyieldparserop::FMTrange, { "VOLUME" }, { true, false, false })));

    // Act
    const double below = getYield(yields, "INRANGE", 10);
    const double above = getYield(yields, "INRANGE", 30);

    // Assert
    EXPECT_EQ(below, 0.0);
    EXPECT_EQ(above, 0.0);
}

TEST(FMTComplexYieldHandler, GivenAMeanAnnualIncrementWhenGetThenDividesTheYieldByTheAge)
{
    // Arrange: GROWTH _MAI(VOLUME)
    const Core::FMTYields yields = getYields(getHandler("GROWTH",
        Core::FMTData({}, Core::FMTyieldparserop::FMTmai, { "VOLUME" }, { true })));

    // Act
    const double value = getYield(yields, "GROWTH", 20);

    // Assert
    EXPECT_DOUBLE_EQ(value, 10.0);
}

TEST(FMTComplexYieldHandler, GivenAMeanAnnualIncrementWithYearsPerPeriodWhenGetThenDividesTheYieldByTheAgeInYears)
{
    // Arrange: GROWTH _MAI(VOLUME,5)
    const Core::FMTYields yields = getYields(getHandler("GROWTH",
        Core::FMTData({ 5.0 }, Core::FMTyieldparserop::FMTmai, { "VOLUME" }, { true, false })));

    // Act
    const double value = getYield(yields, "GROWTH", 20);

    // Assert
    EXPECT_DOUBLE_EQ(value, 2.0);
}

// _MAI reads its yield without a value after the last base age, which FMTYieldHandler::getLinearValue refuses.
TEST(FMTComplexYieldHandler, GivenAMeanAnnualIncrementAfterTheLastBaseAgeWhenGetThenRaisesAnOutOfRangeYield)
{
    // Arrange: GROWTH _MAI(VOLUME)
    const Core::FMTYields yields = getYields(getHandler("GROWTH",
        Core::FMTData({}, Core::FMTyieldparserop::FMTmai, { "VOLUME" }, { true })));

    // Act
    const std::vector<Exception::FMTexc> raised = Testing::getRaisedTypes([&]()
        {
        getYield(yields, "GROWTH", 31);
        });

    // Assert
    EXPECT_TRUE(Testing::hasType(raised, Exception::FMTexc::FMToutofrangeyield));
}

TEST(FMTComplexYieldHandler, GivenACurrentAnnualIncrementWhenGetThenSubtractsTheYieldOfThePreviousAge)
{
    // Arrange: CURRENT _CAI(VOLUME)
    const Core::FMTYields yields = getYields(getHandler("CURRENT",
        Core::FMTData({}, Core::FMTyieldparserop::FMTcai, { "VOLUME" }, { true })));

    // Act
    const double value = getYield(yields, "CURRENT", 25);

    // Assert
    EXPECT_DOUBLE_EQ(value, 4.0);
}

TEST(FMTComplexYieldHandler, GivenYearsToPeakWhenGetThenReturnsTheYearsLeftBeforeTheAgeOfThePeak)
{
    // Arrange: TOPEAK _YTP(VOLUME), whose peak is 240 at age 30
    const Core::FMTYields yields = getYields(getHandler("TOPEAK",
        Core::FMTData({}, Core::FMTyieldparserop::FMTytp, { "VOLUME" }, { true })));

    // Act
    const double value = getYield(yields, "TOPEAK", 25);

    // Assert
    EXPECT_DOUBLE_EQ(value, 5.0);
}

TEST(FMTComplexYieldHandler, GivenAnEquationOfYieldsWhenGetThenEvaluatesIt)
{
    // Arrange: TWICE _EQUATION(VOLUME+VOLUME)
    const Core::FMTYields yields = getYields(getHandler("TWICE",
        Core::FMTData({ 0.0, 0.0, 0.0 }, Core::FMTyieldparserop::FMTequation, { "VOLUME", "+", "VOLUME" })));

    // Act
    const double value = getYield(yields, "TWICE", 20);

    // Assert
    EXPECT_DOUBLE_EQ(value, 400.0);
}

TEST(FMTComplexYieldHandler, GivenAnEquationWithANumberAmongItsValuesWhenGetThenEvaluatesTheNumber)
{
    // Arrange: DOUBLE _EQUATION(VOLUME*2), its number in data as FMTYieldParser::_cleanUp rebuilds it
    const Core::FMTYields yields = getYields(getHandler("DOUBLE",
        Core::FMTData({ 0.0, 0.0, 2.0 }, Core::FMTyieldparserop::FMTequation, { "VOLUME", "*", "" })));

    // Act
    const double value = getYield(yields, "DOUBLE", 20);

    // Assert
    EXPECT_DOUBLE_EQ(value, 400.0);
}

TEST(FMTComplexYieldHandler, GivenAnEquationWithAFunctionWhenGetThenAppliesTheFunctionToTheYield)
{
    // Arrange: LOGARITHM _EQUATION(LN(VOLUME))
    const Core::FMTYields yields = getYields(getHandler("LOGARITHM",
        Core::FMTData({ 0.0, 0.0, 0.0, 0.0 }, Core::FMTyieldparserop::FMTequation, { "LN", "(", "VOLUME", ")" })));

    // Act
    const double value = getYield(yields, "LOGARITHM", 20);

    // Assert
    EXPECT_DOUBLE_EQ(value, 5.29831737);
}

TEST(FMTComplexYieldHandler, GivenAMaximumOfYieldsWhenGetThenReturnsTheLargest)
{
    // Arrange: LARGEST _MAX(VOLUME,STEMS)
    const Core::FMTYields yields = getYields(getHandler("LARGEST",
        Core::FMTData({}, Core::FMTyieldparserop::FMTmax, { "VOLUME", "STEMS" }, { true, true })));

    // Act
    const double value = getYield(yields, "LARGEST", 20);

    // Assert
    EXPECT_DOUBLE_EQ(value, 200.0);
}

TEST(FMTComplexYieldHandler, GivenAMinimumOfYieldsWhenGetThenReturnsTheSmallest)
{
    // Arrange: SMALLEST _MIN(VOLUME,STEMS)
    const Core::FMTYields yields = getYields(getHandler("SMALLEST",
        Core::FMTData({}, Core::FMTyieldparserop::FMTmin, { "VOLUME", "STEMS" }, { true, true })));

    // Act
    const double value = getYield(yields, "SMALLEST", 20);

    // Assert
    EXPECT_DOUBLE_EQ(value, 60.0);
}

// _SHIFT moves the age and the period of the development by its lag.
TEST(FMTComplexYieldHandler, GivenAShiftWhenGetThenReturnsTheYieldOfTheShiftedAge)
{
    // Arrange: SHIFTED _SHIFT(VOLUME,5)
    const Core::FMTYields yields = getYields(getHandler("SHIFTED",
        Core::FMTData({ 5.0 }, Core::FMTyieldparserop::FMTShift, { "VOLUME" }, { true, false })));

    // Act
    const double value = getYield(yields, "SHIFTED", 20);

    // Assert
    EXPECT_DOUBLE_EQ(value, 220.0);
}

TEST(FMTComplexYieldHandler, GivenADeltaWithALagOfMinusOneWhenGetThenReturnsTheChangeSinceThePreviousPeriod)
{
    // Arrange: CHANGE _DELTA(PERIODVALUE,-1)
    const Core::FMTYields yields = getYields(getHandler("CHANGE",
        Core::FMTData({ -1.0 }, Core::FMTyieldparserop::FMTdelta, { "PERIODVALUE" }, { true, false })));

    // Act
    const double value = getYield(yields, "CHANGE", 20, 3);

    // Assert
    EXPECT_DOUBLE_EQ(value, 10.0);
}

// The bounds are reached at the ages 10 and 20. _DISTANCE skips age 0 (Tests/ETAT.md, section 7): a lower bound of 0
// would give the distance from age 1.
TEST(FMTComplexYieldHandler, GivenADistanceWhenGetThenReturnsTheYearsFromTheLowerToTheUpperBound)
{
    // Arrange: TOVOLUME _DISTANCE(VOLUME,100,200)
    const Core::FMTYields yields = getYields(getHandler("TOVOLUME",
        Core::FMTData({ 100.0, 200.0 }, Core::FMTyieldparserop::FMTdistance, { "VOLUME" }, { true, false, false })));

    // Act
    const double value = getYield(yields, "TOVOLUME", 20);

    // Assert
    EXPECT_DOUBLE_EQ(value, 10.0);
}

TEST(FMTComplexYieldHandler, GivenAComplexYieldOfAnotherComplexYieldWhenGetThenEvaluatesBoth)
{
    // Arrange: DOUBLED _MULTIPLY(VOLUME,2) and PLUSONE _SUM(DOUBLED,1), in the same handler
    Core::FMTComplexYieldHandler handler = getHandler("DOUBLED",
        Core::FMTData({ 2.0 }, Core::FMTyieldparserop::FMTmultiply, { "VOLUME" }, { true, false }));
    handler.pushData("PLUSONE", Core::FMTData({ 1.0 }, Core::FMTyieldparserop::FMTsum, { "DOUBLED" }, { true, false }));
    const Core::FMTYields yields = getYields(handler);

    // Act
    const double value = getYield(yields, "PLUSONE", 20);

    // Assert
    EXPECT_DOUBLE_EQ(value, 401.0);
}

// FIRST, in a handler, is computed from SECOND, in another handler, which is computed from FIRST.
TEST(FMTComplexYieldHandler, GivenTwoComplexYieldsOfEachOtherInTwoHandlersWhenGetThenRaisesAnInvalidYield)
{
    // Arrange: FIRST _SUM(SECOND,1), then SECOND _SUM(FIRST,1) in a second *YC section
    const Core::FMTMask all("?", getThemes());
    const Core::FMTComplexYieldHandler first = getHandler("FIRST",
        Core::FMTData({ 1.0 }, Core::FMTyieldparserop::FMTsum, { "SECOND" }, { true, false }));
    const Core::FMTComplexYieldHandler second = getHandler("SECOND",
        Core::FMTData({ 1.0 }, Core::FMTyieldparserop::FMTsum, { "FIRST" }, { true, false }));
    Core::FMTYields yields;
    yields.push_back(all, first.clone());
    yields.push_back(all, second.clone());
    yields.update();

    // Act
    const std::vector<Exception::FMTexc> raised = Testing::getRaisedTypes([&]()
        {
        getYield(yields, "FIRST", 20);
        });

    // Assert
    EXPECT_TRUE(Testing::hasType(raised, Exception::FMTexc::FMTinvalid_yield));
}

// indexes gives the names of an equation that are not yields, the theme indexes that FMTYieldParser::_cleanUp
// replaces by their value for each attribute.
TEST(FMTComplexYieldHandler, GivenAnEquationWithAThemeIndexWhenIndexesThenReturnsTheIndex)
{
    // Arrange: WITHINDEX _EQUATION(VOLUME*FACTOR), FACTOR being an index of a theme
    const Core::FMTComplexYieldHandler handler = getHandler("WITHINDEX",
        Core::FMTData({ 0.0, 0.0, 0.0 }, Core::FMTyieldparserop::FMTequation, { "VOLUME", "*", "FACTOR" }));
    const std::vector<std::string> yieldNames = { "VOLUME", "WITHINDEX" };

    // Act
    const std::vector<std::string> indexes = handler.indexes(yieldNames);

    // Assert
    EXPECT_EQ(indexes, std::vector<std::string>({ "FACTOR" }));
}

TEST(FMTComplexYieldHandler, GivenAnEquationOfYieldsOnlyWhenIndexesThenReturnsNone)
{
    // Arrange: TWICE _EQUATION(VOLUME+VOLUME)
    const Core::FMTComplexYieldHandler handler = getHandler("TWICE",
        Core::FMTData({ 0.0, 0.0, 0.0 }, Core::FMTyieldparserop::FMTequation, { "VOLUME", "+", "VOLUME" }));
    const std::vector<std::string> yieldNames = { "VOLUME", "TWICE" };

    // Act
    const std::vector<std::string> indexes = handler.indexes(yieldNames);

    // Assert
    EXPECT_TRUE(indexes.empty());
}

TEST(FMTComplexYieldHandler, GivenASumOfAnUnknownNameWhenIndexesThenReturnsNone)
{
    // Arrange: WITHUNKNOWN _SUM(VOLUME,UNKNOWN): indexes reads the equations only
    const Core::FMTComplexYieldHandler handler = getHandler("WITHUNKNOWN",
        Core::FMTData({}, Core::FMTyieldparserop::FMTsum, { "VOLUME", "UNKNOWN" }, { true, true }));
    const std::vector<std::string> yieldNames = { "VOLUME", "WITHUNKNOWN" };

    // Act
    const std::vector<std::string> indexes = handler.indexes(yieldNames);

    // Assert
    EXPECT_TRUE(indexes.empty());
}

// Characterizes a known behavior (issue #347): FMTYieldParser::_getEq keeps the text of the numbers of an equation
// among its sources, and indexes takes such a number for a theme index. The *YC section of that equation then goes
// through the branch of the theme indexes of FMTYieldParser::_cleanUp, which rebuilds each of its yields as an
// _EQUATION (see FMTData.GivenASumWhenToExpressionThenJoinsItsSourcesWithoutAnOperator).
TEST(FMTComplexYieldHandler, GivenAnEquationWhoseNumberIsKeptAsTextWhenIndexesThenTakesTheNumberForAThemeIndex)
{
    // Arrange: DOUBLE _EQUATION(VOLUME*2), as FMTYieldParser::_getEq reads it
    const Core::FMTComplexYieldHandler handler = getHandler("DOUBLE",
        Core::FMTData({ 0.0, 0.0, 2.0 }, Core::FMTyieldparserop::FMTequation, { "VOLUME", "*", "2" }));
    const std::vector<std::string> yieldNames = { "VOLUME", "DOUBLE" };

    // Act
    const std::vector<std::string> indexes = handler.indexes(yieldNames);

    // Assert
    EXPECT_EQ(indexes, std::vector<std::string>({ "2" }));
}

TEST(FMTComplexYieldHandler, GivenAComplexYieldHandlerWhenGetTypeThenReturnsComplexYield)
{
    // Arrange
    const Core::FMTComplexYieldHandler handler = getHandler("SUMWITHNUMBER",
        Core::FMTData({ 4.0 }, Core::FMTyieldparserop::FMTsum, { "VOLUME" }, { true, false }));

    // Act
    const Core::FMTyldtype type = handler.getType();

    // Assert
    EXPECT_TRUE(type == Core::FMTyldtype::FMTcomplexyld);
}

TEST(FMTComplexYieldHandler, GivenAHandlerWhenCopiedThenTheCopyIsEqual)
{
    // Arrange
    const Core::FMTComplexYieldHandler original = getHandler("SUMWITHNUMBER",
        Core::FMTData({ 4.0 }, Core::FMTyieldparserop::FMTsum, { "VOLUME" }, { true, false }));

    // Act
    const Core::FMTComplexYieldHandler copy(original);
    Core::FMTComplexYieldHandler assigned;
    assigned = original;

    // Assert
    EXPECT_TRUE(copy == original);
    EXPECT_TRUE(assigned == original);
}

TEST(FMTComplexYieldHandler, GivenAHandlerWhenCloneThenTheCloneComputesTheSameYield)
{
    // Arrange
    const Core::FMTComplexYieldHandler original = getHandler("SUMWITHNUMBER",
        Core::FMTData({ 4.0 }, Core::FMTyieldparserop::FMTsum, { "VOLUME" }, { true, false }));

    // Act
    const std::unique_ptr<Core::FMTYieldHandler> clone = original.clone();

    // Assert
    ASSERT_TRUE(clone->getType() == Core::FMTyldtype::FMTcomplexyld);
    EXPECT_DOUBLE_EQ(getYield(getYields(dynamic_cast<const Core::FMTComplexYieldHandler&>(*clone)),
        "SUMWITHNUMBER", 20), 204.0);
}

// The section *YC of a yield file: the mask, then a line per yield in alphabetical order.
TEST(FMTComplexYieldHandler, GivenComplexYieldsWhenConvertedToStringThenWritesALinePerYieldInAlphabeticalOrder)
{
    // Arrange
    Core::FMTComplexYieldHandler handler = getHandler("SUMWITHNUMBER",
        Core::FMTData({ 4.0 }, Core::FMTyieldparserop::FMTsum, { "VOLUME" }, { true, false }));
    handler.pushData("DOUBLED", Core::FMTData({ 2.0 }, Core::FMTyieldparserop::FMTmultiply, { "VOLUME" }, { true, false }));

    // Act
    const std::string written = std::string(handler);

    // Assert
    EXPECT_EQ(written, "*YC ?\nDOUBLED _MULTIPLY(VOLUME,2)\nSUMWITHNUMBER _SUM(VOLUME,4)\n");
}

TEST(FMTComplexYieldHandler, GivenAMinimumOfNegativeYieldsWhenGetThenReturnsTheSmallest)
{
    // Arrange: NEGATIVE _MULTIPLY(VOLUME,-1), MORENEGATIVE _MULTIPLY(VOLUME,-2) and SMALLEST _MIN(NEGATIVE,MORENEGATIVE)
    Core::FMTComplexYieldHandler handler = getHandler("NEGATIVE",
        Core::FMTData({ -1.0 }, Core::FMTyieldparserop::FMTmultiply, { "VOLUME" }, { true, false }));
    handler.pushData("MORENEGATIVE", Core::FMTData({ -2.0 }, Core::FMTyieldparserop::FMTmultiply, { "VOLUME" }, { true, false }));
    handler.pushData("SMALLEST", Core::FMTData({}, Core::FMTyieldparserop::FMTmin, { "NEGATIVE", "MORENEGATIVE" }, { true, true }));
    const Core::FMTYields yields = getYields(handler);

    // Act
    const double value = getYield(yields, "SMALLEST", 20);

    // Assert
    EXPECT_DOUBLE_EQ(value, -400.0);
}

// Disabled: _MAX starts from 0 (FMTComplexYieldHandler::_getMax), while _MIN starts from the highest double, so that
// the maximum of yields that are all negative is 0 (#389, defect 14 of section 2.7 of Tests/ETAT.md).
TEST(FMTComplexYieldHandler, DISABLED_GivenAMaximumOfNegativeYieldsWhenGetThenReturnsTheLargest)
{
    // Arrange: NEGATIVE _MULTIPLY(VOLUME,-1), MORENEGATIVE _MULTIPLY(VOLUME,-2) and LARGEST _MAX(NEGATIVE,MORENEGATIVE)
    Core::FMTComplexYieldHandler handler = getHandler("NEGATIVE",
        Core::FMTData({ -1.0 }, Core::FMTyieldparserop::FMTmultiply, { "VOLUME" }, { true, false }));
    handler.pushData("MORENEGATIVE", Core::FMTData({ -2.0 }, Core::FMTyieldparserop::FMTmultiply, { "VOLUME" }, { true, false }));
    handler.pushData("LARGEST", Core::FMTData({}, Core::FMTyieldparserop::FMTmax, { "NEGATIVE", "MORENEGATIVE" }, { true, true }));
    const Core::FMTYields yields = getYields(handler);

    // Act
    const double value = getYield(yields, "LARGEST", 20);

    // Assert
    EXPECT_DOUBLE_EQ(value, -200.0);
}

// _ENDPOINT(VOLUME,VOLUME,150,250): when VOLUME is within its bounds, neither of its branches runs.
TEST(FMTComplexYieldHandler, GivenAnEndPointWhoseSourceIsWithinItsBoundsWhenGetThenReturnsZero)
{
    // Arrange: WITHIN _ENDPOINT(VOLUME,VOLUME,150,250), VOLUME being 200 at age 20
    const Core::FMTYields yields = getYields(getHandler("WITHIN",
        Core::FMTData({ 150.0, 250.0 }, Core::FMTyieldparserop::FMTendpoint, { "VOLUME", "VOLUME" },
            { true, true, false, false })));

    // Act
    const double value = getYield(yields, "WITHIN", 20);

    // Assert
    EXPECT_EQ(value, 0.0);
}

// Disabled: when its source leaves its bounds, _ENDPOINT reads the base ages of the complex handler, which has none
// (FMTYieldHandler::_getChangesFrom): an access violation, raised as FMTexc(85) on Windows (#392, defect 17 of
// section 2.7 of Tests/ETAT.md). Its value outside its bounds is not settled (Tests/ETAT.md, section 5.5): the case
// asks only that it raises nothing.
TEST(FMTComplexYieldHandler, DISABLED_GivenAnEndPointWhoseSourceIsBelowItsLowerBoundWhenGetThenRaisesNothing)
{
    // Arrange: BELOW _ENDPOINT(VOLUME,VOLUME,150,250), VOLUME being 100 at age 10
    const Core::FMTYields yields = getYields(getHandler("BELOW",
        Core::FMTData({ 150.0, 250.0 }, Core::FMTyieldparserop::FMTendpoint, { "VOLUME", "VOLUME" },
            { true, true, false, false })));

    // Act
    const std::vector<Exception::FMTexc> raised = Testing::getRaisedTypes([&]()
        {
        getYield(yields, "BELOW", 10);
        });

    // Assert
    EXPECT_TRUE(raised.empty());
}

// A complex yield is turned into an age yield over the ages 0 to the age of the development (complexYldToAgeYld), whose
// getAge gives the age (see FMTAgeYieldHandler): DOUBLED is 380 at age 19, where VOLUME is 190.
TEST(FMTComplexYieldHandler, GivenATargetOnAComplexYieldWhenGetAgeThenReturnsTheAgeWhereTheComplexYieldIsTheClosest)
{
    // Arrange: DOUBLED _MULTIPLY(VOLUME,2), and the target of a transition at DOUBLED 380
    const Core::FMTYields yields = getYields(getHandler("DOUBLED",
        Core::FMTData({ 2.0 }, Core::FMTyieldparserop::FMTmultiply, { "VOLUME" }, { true, false })));
    const Core::FMTDevelopment development(Core::FMTMask("A1", getThemes()), 30, 0, 1);
    const Core::FMTYieldRequest request(development);
    Core::FMTSpec target;
    target.addBounds(Core::FMTYldBounds(Core::FMTsection::Transition, Core::FMTkwor::Target, "DOUBLED", 380.0, 380.0));

    // Act
    const int age = yields.getAge(request, target);

    // Assert
    EXPECT_EQ(age, 19);
}

// complexYldToAgeYld reads the yields of the request, which FMTYields::get prepares (Documentation/Architecture.md,
// Known Architectural Debt): the handler is the one held by the yields.
TEST(FMTComplexYieldHandler, GivenAPreparedRequestWhenComplexYldToAgeYldThenReturnsAnAgeYieldOfEachAgeUpToTheDevelopment)
{
    // Arrange: DOUBLED _MULTIPLY(VOLUME,2), third handler of the yields
    const Core::FMTYields yields = getYields(getHandler("DOUBLED",
        Core::FMTData({ 2.0 }, Core::FMTyieldparserop::FMTmultiply, { "VOLUME" }, { true, false })));
    const Core::FMTDevelopment development(Core::FMTMask("A1", getThemes()), 30, 0, 1);
    const Core::FMTYieldRequest request(development);
    yields.get(request, "DOUBLED");
    const Core::FMTComplexYieldHandler& handler =
        dynamic_cast<const Core::FMTComplexYieldHandler&>(*(yields.begin() + 2)->second);
    Core::FMTSpec target;
    target.addBounds(Core::FMTYldBounds(Core::FMTsection::Transition, Core::FMTkwor::Target, "DOUBLED", 380.0, 380.0));

    // Act
    const std::unique_ptr<Core::FMTYieldHandler> ageYield = handler.complexYldToAgeYld(request, target);

    // Assert
    ASSERT_TRUE(ageYield->getType() == Core::FMTyldtype::FMTageyld);
    ASSERT_EQ(ageYield->getBases().size(), 31u);
    EXPECT_EQ(ageYield->getBases().front(), 0);
    EXPECT_EQ(ageYield->getBases().back(), 30);
    const Core::FMTDevelopment atAge19(Core::FMTMask("A1", getThemes()), 19, 0, 1);
    EXPECT_DOUBLE_EQ(ageYield->get("DOUBLED", Core::FMTYieldRequest(atAge19)), 380.0);
}

// compareSources tells whether none of the yields of the handler is computed from a yield: FMTYieldParser raises
// FMToveridedyield for a complex section, after an _OVERRIDE section, that uses the yield overridden.
TEST(FMTComplexYieldHandler, GivenAComplexYieldOfVolumeWhenCompareSourcesThenReturnsFalseForVolumeOnly)
{
    // Arrange: DOUBLED _MULTIPLY(VOLUME,2)
    const Core::FMTComplexYieldHandler handler = getHandler("DOUBLED",
        Core::FMTData({ 2.0 }, Core::FMTyieldparserop::FMTmultiply, { "VOLUME" }, { true, false }));
    const Core::FMTComplexYieldHandler overriding;

    // Act
    const bool volumeUnused = handler.compareSources("VOLUME", overriding);
    const bool stemsUnused = handler.compareSources("STEMS", overriding);

    // Assert
    EXPECT_FALSE(volumeUnused);
    EXPECT_TRUE(stemsUnused);
}

TEST(FMTComplexYieldHandler, GivenATabouWhenGetTabousThenReturnsIt)
{
    // Arrange
    Core::FMTComplexYieldHandler handler = getHandler("DOUBLED",
        Core::FMTData({ 2.0 }, Core::FMTyieldparserop::FMTmultiply, { "VOLUME" }, { true, false }));
    const std::vector<size_t> before = handler.getTabous();

    // Act
    handler.setTabou(2);

    // Assert
    EXPECT_TRUE(before.empty());
    EXPECT_EQ(handler.getTabous(), std::vector<size_t>({ 2 }));
}

// FMTYieldParser gives the tabous of a handler to the handler that replaces it (setTabou of a handler).
TEST(FMTComplexYieldHandler, GivenAHandlerWhenSetTabouFromItThenTakesItsOverrideIndexAndItsTabous)
{
    // Arrange
    Core::FMTComplexYieldHandler original = getHandler("DOUBLED",
        Core::FMTData({ 2.0 }, Core::FMTyieldparserop::FMTmultiply, { "VOLUME" }, { true, false }));
    original.setOverrideIndex(1);
    original.setTabou(3);
    Core::FMTComplexYieldHandler replacing(Core::FMTMask("?", getThemes()));

    // Act
    replacing.setTabou(original);

    // Assert
    EXPECT_EQ(replacing.getOverrideIndex(), 1u);
    EXPECT_EQ(replacing.getTabous(), std::vector<size_t>({ 3 }));
}

TEST(FMTComplexYieldHandler, GivenAnOverridingHandlerWhenConvertedToStringThenMarksItsSectionOverride)
{
    // Arrange: *YC ? _OVERRIDE with VOLUME _MULTIPLY(STEMS,10)
    Core::FMTComplexYieldHandler handler = getHandler("VOLUME",
        Core::FMTData({ 10.0 }, Core::FMTyieldparserop::FMTmultiply, { "STEMS" }, { true, false }));
    handler.setOverrideIndex(1);

    // Act
    const std::string written = std::string(handler);

    // Assert
    EXPECT_EQ(written, "*YC ? _OVERRIDE\nVOLUME _MULTIPLY(STEMS,10)\n");
}

// A *YC section marked _OVERRIDE comes first and replaces a yield of the sections that follow. FMTYieldParser gives it
// an override index, and to each complex section after it that overlaps its mask that index as a tabou, so that the
// yields of that section keep reading the yield replaced (FMTYieldParser::_setOveridedYlds).
TEST(FMTComplexYieldHandler, GivenAnOverridingSectionWhenGetThenTheComplexYieldsOfItsTabousReadTheYieldReplaced)
{
    // Arrange: *YC ? _OVERRIDE with VOLUME _MULTIPLY(STEMS,10), the age yields, then DOUBLED _MULTIPLY(VOLUME,2) with
    // the tabou 1
    const Core::FMTMask all("?", getThemes());
    Core::FMTComplexYieldHandler overriding = getHandler("VOLUME",
        Core::FMTData({ 10.0 }, Core::FMTyieldparserop::FMTmultiply, { "STEMS" }, { true, false }));
    overriding.setOverrideIndex(1);
    Core::FMTAgeYieldHandler ageYields(all);
    ageYields.setYieldValues("VOLUME", { 10, 20, 30 }, { 100.0, 200.0, 240.0 });
    ageYields.setYieldValues("STEMS", { 10, 20, 30 }, { 50.0, 60.0, 70.0 });
    Core::FMTComplexYieldHandler withTabou = getHandler("DOUBLED",
        Core::FMTData({ 2.0 }, Core::FMTyieldparserop::FMTmultiply, { "VOLUME" }, { true, false }));
    withTabou.setTabou(1);
    Core::FMTYields yields;
    yields.push_back(all, overriding.clone());
    yields.pushBackAgeHandler(all, ageYields);
    yields.push_back(all, withTabou.clone());
    yields.update();

    // Act
    const double volume = getYield(yields, "VOLUME", 20);
    const double doubled = getYield(yields, "DOUBLED", 20);

    // Assert
    EXPECT_DOUBLE_EQ(volume, 600.0);
    EXPECT_DOUBLE_EQ(doubled, 400.0);
}

TEST(FMTComplexYieldHandler, GivenAnOverridingSectionWhenGetAComplexYieldWithoutTabouThenItReadsTheOverridingValue)
{
    // Arrange: the same sections, DOUBLED without the tabou 1
    const Core::FMTMask all("?", getThemes());
    Core::FMTComplexYieldHandler overriding = getHandler("VOLUME",
        Core::FMTData({ 10.0 }, Core::FMTyieldparserop::FMTmultiply, { "STEMS" }, { true, false }));
    overriding.setOverrideIndex(1);
    Core::FMTAgeYieldHandler ageYields(all);
    ageYields.setYieldValues("VOLUME", { 10, 20, 30 }, { 100.0, 200.0, 240.0 });
    ageYields.setYieldValues("STEMS", { 10, 20, 30 }, { 50.0, 60.0, 70.0 });
    const Core::FMTComplexYieldHandler withoutTabou = getHandler("DOUBLED",
        Core::FMTData({ 2.0 }, Core::FMTyieldparserop::FMTmultiply, { "VOLUME" }, { true, false }));
    Core::FMTYields yields;
    yields.push_back(all, overriding.clone());
    yields.pushBackAgeHandler(all, ageYields);
    yields.push_back(all, withoutTabou.clone());
    yields.update();

    // Act
    const double doubled = getYield(yields, "DOUBLED", 20);

    // Assert
    EXPECT_DOUBLE_EQ(doubled, 1200.0);
}
