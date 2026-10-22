/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTYields.h"
#include "FMTAgeYieldHandler.h"
#include "FMTComplexYieldHandler.h"
#include "FMTData.h"
#include "FMTDevelopment.h"
#include "FMTMask.h"
#include "FMTSpec.h"
#include "FMTTheme.h"
#include "FMTTimeYieldHandler.h"
#include "FMTYieldRequest.h"
#include "FMTYldBounds.h"
#include "FMTutility.h"

#include "ExceptionChain.h"

#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>

// The yield section of a model, built in code as FMTYieldParser builds it: the handlers in the order of their
// sections, each with its mask (see Tests/Unit/Core/FMTComplexYieldHandler.cpp). A request serves one call only
// (Documentation/Architecture.md, Known Architectural Debt).

namespace
{
    // One theme of two attributes.
    std::vector<Core::FMTTheme> getThemes()
    {
        return { Core::FMTTheme({ "A1", "A2" }, 0, 0, "THEME1") };
    }

    Core::FMTMask getMask(const std::string& p_mask)
    {
        return Core::FMTMask(p_mask, getThemes());
    }

    // An age handler of the mask p_mask with VOLUME (p_volume at the ages 10, 20 and 30) and STEMS (50, 60 and 70).
    Core::FMTAgeYieldHandler getAgeHandler(const std::string& p_mask, const std::vector<double>& p_volume)
    {
        Core::FMTAgeYieldHandler handler(getMask(p_mask));
        handler.setYieldValues("VOLUME", { 10, 20, 30 }, p_volume);
        handler.setYieldValues("STEMS", { 10, 20, 30 }, { 50.0, 60.0, 70.0 });
        return handler;
    }

    // A time handler of the mask "?" with PERIODVALUE (10, 20 and 30 from period 1).
    Core::FMTTimeYieldHandler getTimeHandler()
    {
        Core::FMTTimeYieldHandler handler(getMask("?"));
        handler.setYieldValues("PERIODVALUE", 1, { 10.0, 20.0, 30.0 });
        return handler;
    }

    // A complex handler of the mask "?" with DOUBLEVOLUME _MULTIPLY(VOLUME,2).
    Core::FMTComplexYieldHandler getComplexHandler()
    {
        Core::FMTComplexYieldHandler handler(getMask("?"));
        handler.pushData("DOUBLEVOLUME",
            Core::FMTData({ 2.0 }, Core::FMTyieldparserop::FMTmultiply, { "VOLUME" }, { true, false }));
        return handler;
    }

    // The age, time and complex handlers above, in that order, all of the mask "?".
    Core::FMTYields getYields()
    {
        Core::FMTYields yields;
        yields.pushBackAgeHandler(getMask("?"), getAgeHandler("?", { 100.0, 200.0, 240.0 }));
        yields.pushBackTimeHandler(getMask("?"), getTimeHandler());
        yields.push_back(getMask("?"), getComplexHandler().clone());
        yields.update();
        return yields;
    }

    Core::FMTDevelopment getDevelopment(const std::string& p_attribute, int p_age, int p_period = 1)
    {
        return Core::FMTDevelopment(getMask(p_attribute), p_age, 0, p_period);
    }

    // The value of p_yield for a development of attribute p_attribute, of age p_age at period p_period. A request
    // keeps the address of its development, which must outlive it.
    double getYield(const Core::FMTYields& p_yields, const std::string& p_yield, int p_age, int p_period = 1,
        const std::string& p_attribute = "A1")
    {
        const Core::FMTDevelopment DEVELOPMENT = getDevelopment(p_attribute, p_age, p_period);
        const Core::FMTYieldRequest REQUEST(DEVELOPMENT);
        return p_yields.get(REQUEST, p_yield);
    }

    // The target of a transition that sets the age of a development where p_yield reaches p_value.
    Core::FMTSpec getYieldTarget(const std::string& p_yield, double p_value)
    {
        Core::FMTSpec spec;
        spec.addBounds(Core::FMTYldBounds(Core::FMTsection::Transition, Core::FMTkwor::Target, p_yield, p_value, p_value));
        return spec;
    }

    // _DISCOUNTFACTOR(4%,5,FULL) as FMTYieldParser builds it (see Tests/Unit/Core/FMTTimeYieldHandler.cpp): 1 / 1.04^5,
    // 0.821927106759, at period 1.
    Core::FMTYields getDiscountYields()
    {
        Core::FMTTimeYieldHandler handler(getMask("?"));
        handler.pushData("DISCOUNTFULL",
            Core::FMTData({ 0.04, 5.0 }, Core::FMTyieldparserop::FMTdiscountfactor, { "FULL" }, { false, false, true }));
        Core::FMTYields yields;
        yields.pushBackTimeHandler(getMask("?"), handler);
        yields.update();
        return yields;
    }

    constexpr double TOLERANCE = 1e-12;
}

TEST(FMTYields, GivenAnAgeYieldWhenGetThenReturnsItsValueAtTheAgeOfTheDevelopment)
{
    // Arrange
    const Core::FMTYields yields = getYields();

    // Act
    const double atBaseAge = getYield(yields, "VOLUME", 20);
    const double betweenBaseAges = getYield(yields, "VOLUME", 25);

    // Assert
    EXPECT_DOUBLE_EQ(atBaseAge, 200.0);
    EXPECT_DOUBLE_EQ(betweenBaseAges, 220.0);
}

TEST(FMTYields, GivenATimeYieldWhenGetThenReturnsItsValueAtThePeriodOfTheDevelopment)
{
    // Arrange
    const Core::FMTYields yields = getYields();

    // Act
    const double atPeriodTwo = getYield(yields, "PERIODVALUE", 20, 2);

    // Assert
    EXPECT_DOUBLE_EQ(atPeriodTwo, 20.0);
}

TEST(FMTYields, GivenAComplexYieldWhenGetThenComputesItFromTheYieldsOfTheOtherHandlers)
{
    // Arrange
    const Core::FMTYields yields = getYields();

    // Act
    const double value = getYield(yields, "DOUBLEVOLUME", 20);

    // Assert
    EXPECT_DOUBLE_EQ(value, 400.0);
}

// Known behavior (Tests/ETAT.md, section 7): a name that no handler holds is not an error.
TEST(FMTYields, GivenAYieldThatNoHandlerHoldsWhenGetThenReturnsZero)
{
    // Arrange
    const Core::FMTYields yields = getYields();

    // Act
    const double value = getYield(yields, "UNKNOWN", 20);

    // Assert
    EXPECT_EQ(value, 0.0);
}

TEST(FMTYields, GivenAYieldOnlyInAHandlerOfAnotherAttributeWhenGetThenRaisesAMissingYield)
{
    // Arrange: VOLUME in a handler of the attribute A2 only
    Core::FMTYields yields;
    yields.pushBackAgeHandler(getMask("A2"), getAgeHandler("A2", { 100.0, 200.0, 240.0 }));
    yields.update();

    // Act
    const std::vector<Exception::FMTexc> raised = Testing::getRaisedTypes([&]()
        {
        getYield(yields, "VOLUME", 20, 1, "A1");
        });

    // Assert
    EXPECT_TRUE(Testing::hasType(raised, Exception::FMTexc::FMTmissingyield));
}

// The first handler whose mask selects the development gives the value: a handler of the attribute A1 before a
// handler of every attribute.
TEST(FMTYields, GivenTwoHandlersOfTheSameYieldWhenGetThenTheFirstHandlerThatSelectsTheDevelopmentGivesTheValue)
{
    // Arrange
    Core::FMTYields yields;
    yields.pushBackAgeHandler(getMask("A1"), getAgeHandler("A1", { 1000.0, 2000.0, 2400.0 }));
    yields.pushBackAgeHandler(getMask("?"), getAgeHandler("?", { 100.0, 200.0, 240.0 }));
    yields.update();

    // Act
    const double ofA1 = getYield(yields, "VOLUME", 20, 1, "A1");
    const double ofA2 = getYield(yields, "VOLUME", 20, 1, "A2");

    // Assert
    EXPECT_DOUBLE_EQ(ofA1, 2000.0);
    EXPECT_DOUBLE_EQ(ofA2, 200.0);
}

TEST(FMTYields, GivenYieldsWhenIsYldThenReturnsTrueForTheirYieldsOnly)
{
    // Arrange
    const Core::FMTYields yields = getYields();

    // Act
    const bool hasVolume = yields.isYld("VOLUME");
    const bool hasComplex = yields.isYld("DOUBLEVOLUME");
    const bool hasUnknown = yields.isYld("UNKNOWN");
    const bool unknownIsNull = yields.isNullYld("UNKNOWN");
    const bool volumeIsNull = yields.isNullYld("VOLUME");

    // Assert
    EXPECT_TRUE(hasVolume);
    EXPECT_TRUE(hasComplex);
    EXPECT_FALSE(hasUnknown);
    EXPECT_TRUE(unknownIsNull);
    EXPECT_FALSE(volumeIsNull);
}

TEST(FMTYields, GivenYieldsWhenGetAllYieldNamesThenReturnsTheNullYieldThenEachYieldOnceInTheOrderOfTheHandlers)
{
    // Arrange
    Core::FMTYields yields = getYields();
    yields.pushBackAgeHandler(getMask("A2"), getAgeHandler("A2", { 1.0, 2.0, 3.0 }));
    yields.update();

    // Act
    const std::vector<std::string> names = yields.getAllYieldNames();

    // Assert
    EXPECT_EQ(names, std::vector<std::string>({ Core::FMTYields::getNullYield(), "STEMS", "VOLUME", "PERIODVALUE",
        "DOUBLEVOLUME" }));
    EXPECT_EQ(Core::FMTYields::getNullYield(), "~FMTNULLYIELD");
}

TEST(FMTYields, GivenAgeTimeAndComplexHandlersWhenGotYieldTypeThenReturnsTrueForTheirTypesOnly)
{
    // Arrange
    const Core::FMTYields yields = getYields();

    // Act
    const bool gotAge = yields.gotYieldType(Core::FMTyldtype::FMTageyld);
    const bool gotTime = yields.gotYieldType(Core::FMTyldtype::FMTtimeyld);
    const bool gotComplex = yields.gotYieldType(Core::FMTyldtype::FMTcomplexyld);
    const bool gotModel = yields.gotYieldType(Core::FMTyldtype::FMTmodelyld);

    // Assert
    EXPECT_TRUE(gotAge);
    EXPECT_TRUE(gotTime);
    EXPECT_TRUE(gotComplex);
    EXPECT_FALSE(gotModel);
}

TEST(FMTYields, GivenYieldsWhenCopiedThenTheCopyGivesTheSameValues)
{
    // Arrange
    const Core::FMTYields original = getYields();

    // Act
    const Core::FMTYields copy(original);
    Core::FMTYields assigned;
    assigned = original;

    // Assert
    EXPECT_DOUBLE_EQ(getYield(copy, "VOLUME", 25), 220.0);
    EXPECT_DOUBLE_EQ(getYield(copy, "DOUBLEVOLUME", 20), 400.0);
    EXPECT_DOUBLE_EQ(getYield(assigned, "PERIODVALUE", 20, 3), 30.0);
    EXPECT_EQ(assigned.size(), original.size());
}

TEST(FMTYields, GivenYieldsWhenComparedWithThemselvesThenTheyAreEqual)
{
    // Arrange
    const Core::FMTYields yields = getYields();

    // Act
    const bool equal = (yields == yields);
    const bool different = (yields != yields);

    // Assert
    EXPECT_TRUE(equal);
    EXPECT_FALSE(different);
}

// Disabled: FMTList::operator== compares the std::unique_ptr of the handlers, that is their addresses, so that a copy
// is never equal to its original (#383).
TEST(FMTYields, DISABLED_GivenYieldsWhenCopiedThenTheCopyIsEqual)
{
    // Arrange
    const Core::FMTYields original = getYields();

    // Act
    const Core::FMTYields copy(original);

    // Assert
    EXPECT_TRUE(copy == original);
}

TEST(FMTYields, GivenAFactorOfAYieldWhenGetFromFactorThenMultipliesThatYieldOnly)
{
    // Arrange
    const Core::FMTYields yields = getYields();

    // Act
    const Core::FMTYields multiplied = yields.getFromFactor(1.5, { "VOLUME" });

    // Assert
    EXPECT_DOUBLE_EQ(getYield(multiplied, "VOLUME", 20), 300.0);
    EXPECT_DOUBLE_EQ(getYield(multiplied, "STEMS", 20), 60.0);
    EXPECT_DOUBLE_EQ(getYield(multiplied, "DOUBLEVOLUME", 20), 600.0);
    EXPECT_DOUBLE_EQ(getYield(yields, "VOLUME", 20), 200.0);
}

TEST(FMTYields, GivenAFactorOfATimeYieldWhenGetFromFactorThenMultipliesItsValues)
{
    // Arrange
    const Core::FMTYields yields = getYields();

    // Act
    const Core::FMTYields multiplied = yields.getFromFactor(0.5, { "PERIODVALUE" });

    // Assert
    EXPECT_DOUBLE_EQ(getYield(multiplied, "PERIODVALUE", 20, 3), 15.0);
}

TEST(FMTYields, GivenYieldsWhenGetStackedThenReturnsTheSectionOfEachHandlerInTheirOrder)
{
    // Arrange
    Core::FMTYields yields;
    yields.pushBackTimeHandler(getMask("?"), getTimeHandler());
    yields.push_back(getMask("?"), getComplexHandler().clone());
    yields.update();

    // Act
    const std::vector<std::string> stacked = yields.getStacked();

    // Assert
    ASSERT_EQ(stacked.size(), 2u);
    EXPECT_EQ(stacked.at(0).rfind("*YT ?", 0), 0u);
    EXPECT_EQ(stacked.at(1), "*YC ?\nDOUBLEVOLUME _MULTIPLY(VOLUME,2)\n\n");
}

TEST(FMTYields, GivenYieldsWhenClearThenHoldsNoYield)
{
    // Arrange
    Core::FMTYields yields = getYields();

    // Act
    yields.clear();

    // Assert
    EXPECT_TRUE(yields.empty());
    EXPECT_FALSE(yields.isYld("VOLUME"));
    EXPECT_EQ(getYield(yields, "VOLUME", 20), 0.0);
}

TEST(FMTYields, GivenTwoYieldsWhenSwapThenEachHoldsTheYieldsOfTheOther)
{
    // Arrange
    Core::FMTYields yields = getYields();
    Core::FMTYields other;
    other.pushBackTimeHandler(getMask("?"), getTimeHandler());
    other.update();

    // Act
    yields.swap(other);

    // Assert
    EXPECT_FALSE(yields.isYld("VOLUME"));
    EXPECT_TRUE(yields.isYld("PERIODVALUE"));
    EXPECT_DOUBLE_EQ(getYield(other, "VOLUME", 20), 200.0);
}

// The age of a development disturbed by a transition whose target is a yield value: the base age at or below the
// age of the development where the yield is the closest to the value, here 200 at age 20 for 190.
TEST(FMTYields, GivenATargetOnAnAgeYieldWhenGetAgeThenReturnsTheBaseAgeWhereTheYieldIsTheClosest)
{
    // Arrange
    const Core::FMTYields yields = getYields();
    const Core::FMTDevelopment development = getDevelopment("A1", 30);
    const Core::FMTYieldRequest request(development);
    const Core::FMTSpec target = getYieldTarget("VOLUME", 190.0);

    // Act
    const int age = yields.getAge(request, target);

    // Assert
    EXPECT_EQ(age, 20);
}

TEST(FMTYields, GivenATargetOnAYieldThatNoHandlerHoldsWhenGetAgeThenKeepsTheAgeOfTheDevelopment)
{
    // Arrange
    const Core::FMTYields yields = getYields();
    const Core::FMTDevelopment development = getDevelopment("A1", 30);
    const Core::FMTYieldRequest request(development);
    const Core::FMTSpec target = getYieldTarget("UNKNOWN", 190.0);

    // Act
    const int age = yields.getAge(request, target);

    // Assert
    EXPECT_EQ(age, 30);
}

TEST(FMTYields, GivenADiscountFactorWhenGetThenReturnsTheFactorOfThePeriod)
{
    // Arrange
    const Core::FMTYields yields = getDiscountYields();

    // Act
    const double atPeriodOne = getYield(yields, "DISCOUNTFULL", 20, 1);

    // Assert
    EXPECT_NEAR(atPeriodOne, 0.821927106759, TOLERANCE);
}

// Disabled: FMTTimeYieldHandler::getFromFactor multiplies every value of the data of a yield, and those of a
// _DISCOUNTFACTOR are its rate and its years per period (FMTData::operator*): the factor gives 1 / 1.08^10 instead of
// 2 / 1.04^5 (Tests/ETAT.md, section 2.7, defect 15).
TEST(FMTYields, DISABLED_GivenAFactorOfADiscountFactorWhenGetFromFactorThenMultipliesItsValue)
{
    // Arrange
    const Core::FMTYields yields = getDiscountYields();

    // Act
    const Core::FMTYields multiplied = yields.getFromFactor(2.0, { "DISCOUNTFULL" });

    // Assert
    EXPECT_NEAR(getYield(multiplied, "DISCOUNTFULL", 20, 1), 2.0 * 0.821927106759, TOLERANCE);
}

// Disabled: getFromFactor multiplies the yields named only, and none when no yield is named, while its documentation
// says all when the list is empty, as the handlers do (Tests/ETAT.md, section 2.7, defect 16).
TEST(FMTYields, DISABLED_GivenAFactorWithoutYieldNamesWhenGetFromFactorThenMultipliesEveryYield)
{
    // Arrange
    const Core::FMTYields yields = getYields();

    // Act
    const Core::FMTYields multiplied = yields.getFromFactor(1.5);

    // Assert
    EXPECT_DOUBLE_EQ(getYield(multiplied, "VOLUME", 20), 300.0);
    EXPECT_DOUBLE_EQ(getYield(multiplied, "PERIODVALUE", 20, 3), 45.0);
}
