/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTYieldHandler.h"
#include "FMTAgeYieldHandler.h"
#include "FMTDevelopment.h"
#include "FMTMask.h"
#include "FMTMaskFilter.h"
#include "FMTTheme.h"
#include "FMTYieldRequest.h"
#include "FMTutility.h"

#include "ExceptionChain.h"

#include <gtest/gtest.h>

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

// FMTYieldHandler is the base of the age, time, complex and model handlers: it holds their mask and their base ages, and
// interpolates a value between the base ages. The methods its derived classes define raise FMTfunctionfailed
// ("Calling pure virtual function") on the base. What the derived classes do is in the suites of
// FMTAgeYieldHandler, FMTTimeYieldHandler and FMTComplexYieldHandler; these cases check the base, directly or through
// an age handler. The themes are those of Tests/Unit/Core/FMTTheme.cpp.

namespace
{
    std::vector<Core::FMTTheme> getThemes()
    {
        const std::vector<std::map<std::string, double>> NO_INDEXES;
        return {
            Core::FMTTheme({ "UNITE1", "UNITE2", "UNITE3" }, { "", "", "" }, { "UC" }, { { "UNITE1", "UNITE2" } },
                NO_INDEXES, 0, 0, ""),
            Core::FMTTheme({ "PEUPLEMENT1", "PEUPLEMENT2", "PEUPLEMENT3", "FEU" }, { "", "", "", "" },
                { "PROD", "POURPL" }, { { "PEUPLEMENT1", "PEUPLEMENT2", "PEUPLEMENT3" }, { "PEUPLEMENT2", "PEUPLEMENT3" } },
                NO_INDEXES, 1, 3, ""),
            Core::FMTTheme({ "UTR1", "UTR2", "UTR3" }, { "", "", "" }, { "UTA1", "UTA2" }, { { "UTR1", "UTR2" }, { "UTR3" } },
                NO_INDEXES, 2, 7, "") };
    }

    Core::FMTMask getMask(const std::string& p_mask)
    {
        return Core::FMTMask(p_mask, getThemes());
    }

    // An age handler of the mask p_mask with VOLUME, of p_values at the ages 10, 20 and 30.
    Core::FMTAgeYieldHandler getVolumeHandler(const std::string& p_mask, const std::vector<double>& p_values)
    {
        Core::FMTAgeYieldHandler handler(getMask(p_mask));
        handler.setYieldValues("VOLUME", { 10, 20, 30 }, p_values);
        return handler;
    }

    // The value of p_yield for a development of age p_age: an age handler reads only the age of the request.
    double getAtAge(const Core::FMTYieldHandler& p_handler, const std::string& p_yield, int p_age)
    {
        const Core::FMTDevelopment DEVELOPMENT(Core::FMTMask(), p_age, 0);
        const Core::FMTYieldRequest REQUEST(DEVELOPMENT);
        return p_handler.get(p_yield, REQUEST);
    }

    // The presolve of Tests/Unit/Core/FMTMask.cpp: the filter of the union of UNITE1 PEUPLEMENT1 UTR1 and
    // UNITE1 PEUPLEMENT2 UTR1, and the themes left with more than one attribute, here PEUPLEMENT1 and PEUPLEMENT2.
    struct PreSolve
    {
        Core::FMTMaskFilter filter;
        std::vector<Core::FMTTheme> themes;
    };

    PreSolve getPreSolve()
    {
        PreSolve preSolve;
        const Core::FMTMask selection = getMask("UNITE1 PEUPLEMENT1 UTR1").getUnion(getMask("UNITE1 PEUPLEMENT2 UTR1"));
        preSolve.filter = Core::FMTMaskFilter(selection);
        size_t newId = 0;
        size_t newStart = 0;
        for (const Core::FMTTheme& theme : getThemes())
        {
            const Core::FMTTheme presolved = theme.preSolve(preSolve.filter, newId, newStart);
            if (!presolved.empty())
            {
                preSolve.themes.push_back(presolved);
            }
        }
        return preSolve;
    }

    // True when p_call raises FMTfunctionfailed.
    bool raisesFunctionFailed(const std::function<void()>& p_call)
    {
        return Testing::hasType(Testing::getRaisedTypes(p_call), Exception::FMTexc::FMTfunctionfailed);
    }
}

TEST(FMTYieldHandler, GivenABaseHandlerWhenCalledThroughTheMethodsOfItsDerivedClassesThenRaisesAFunctionFailed)
{
    // Arrange
    const Core::FMTYieldHandler handler(getMask("? ? ?"));
    const Core::FMTDevelopment development(getMask("UNITE1 PEUPLEMENT1 UTR1"), 20, 0);
    const Core::FMTYieldRequest request(development);

    // Act
    const std::vector<bool> raised = {
        raisesFunctionFailed([&]() { handler.get("VOLUME", request); }),
        raisesFunctionFailed([&]() { handler.getType(); }),
        raisesFunctionFailed([&]() { handler.clone(); }),
        raisesFunctionFailed([&]() { handler.empty(); }),
        raisesFunctionFailed([&]() { handler.size(); }),
        raisesFunctionFailed([&]() { handler.containsYield("VOLUME"); }),
        raisesFunctionFailed([&]() { handler.getYieldNames(); }),
        raisesFunctionFailed([&]() { handler.at("VOLUME"); }),
        raisesFunctionFailed([&]() { const std::string text(handler); }) };

    // Assert
    EXPECT_EQ(raised, std::vector<bool>(raised.size(), true));
}

TEST(FMTYieldHandler, GivenAMaskWhenConstructedThenGetMaskReturnsItAndSetMaskReplacesIt)
{
    // Arrange
    Core::FMTYieldHandler handler(getMask("UNITE1 ? ?"));
    const Core::FMTMask constructed = handler.getMask();

    // Act
    handler.setMask(getMask("UNITE2 ? ?"));

    // Assert
    EXPECT_EQ(std::string(constructed), "UNITE1 ? ?");
    EXPECT_EQ(std::string(handler.getMask()), "UNITE2 ? ?");
    EXPECT_TRUE(handler.getMask() == getMask("UNITE2 ? ?"));
}

// pushBase is documented to return true when it pushes the base age; it always returns false.
TEST(FMTYieldHandler, GivenBaseAgesWhenPushedAndSetThenGetBasesAndGetLastBaseReadThem)
{
    // Arrange
    Core::FMTYieldHandler handler(getMask("? ? ?"));
    const int lastOfNone = handler.getLastBase();

    // Act
    const bool pushed = handler.pushBase(10);
    handler.pushBase(20);
    const std::vector<int> pushedBases = handler.getBases();
    const int lastPushed = handler.getLastBase();
    handler.setBase({ 5, 15 });

    // Assert
    EXPECT_EQ(lastOfNone, 0);
    EXPECT_FALSE(pushed);
    EXPECT_EQ(pushedBases, std::vector<int>({ 10, 20 }));
    EXPECT_EQ(lastPushed, 20);
    EXPECT_EQ(handler.getBases(), std::vector<int>({ 5, 15 }));
    EXPECT_EQ(handler.getLastBase(), 15);
}

// getLinearValue reads the values it is given, one per base age: 10 and 30 at 10 and 20 years give 20 at 15 years, 5 at
// 5 years (from 0 at age 0), and 30 at 20 years and after.
TEST(FMTYieldHandler, GivenValuesOfTheBaseAgesWhenGetLinearValueThenInterpolatesThemFromZeroAtAgeZero)
{
    // Arrange
    Core::FMTYieldHandler handler(getMask("? ? ?"));
    handler.setBase({ 10, 20 });
    const std::vector<double> values = { 10.0, 30.0 };

    // Act
    const double atFive = handler.getLinearValue(values, 5, true);
    const double atTen = handler.getLinearValue(values, 10, true);
    const double atFifteen = handler.getLinearValue(values, 15, true);
    const double atTwenty = handler.getLinearValue(values, 20, false);
    const double atForty = handler.getLinearValue(values, 40, true);

    // Assert
    EXPECT_DOUBLE_EQ(atFive, 5.0);
    EXPECT_DOUBLE_EQ(atTen, 10.0);
    EXPECT_DOUBLE_EQ(atFifteen, 20.0);
    EXPECT_DOUBLE_EQ(atTwenty, 30.0);
    EXPECT_DOUBLE_EQ(atForty, 30.0);
}

// Known behavior, #383: FMTAgeYieldHandler declares its own operator== instead of overriding that of the base, which
// compares the mask and the base ages only. Through the base, two handlers of other values are equal: a correction of
// #383 that compared the handlers of FMTYields through FMTYieldHandler would take them for equal.
TEST(FMTYieldHandler, GivenTwoAgeHandlersThatDifferByTheirValuesOnlyWhenComparedThroughTheBaseClassThenTheyAreEqual)
{
    // Arrange
    const Core::FMTAgeYieldHandler first = getVolumeHandler("? ? ?", { 100.0, 200.0, 240.0 });
    const Core::FMTAgeYieldHandler second = getVolumeHandler("? ? ?", { 1000.0, 2000.0, 2400.0 });
    Core::FMTAgeYieldHandler otherAges(getMask("? ? ?"));
    otherAges.setYieldValues("VOLUME", { 5, 20, 30 }, { 100.0, 200.0, 240.0 });
    const Core::FMTYieldHandler& firstBase = first;
    const Core::FMTYieldHandler& secondBase = second;
    const Core::FMTYieldHandler& otherAgesBase = otherAges;

    // Act
    const bool equalThroughTheBase = (firstBase == secondBase);
    const bool equalAsAgeHandlers = (first == second);
    const bool equalToOtherAges = (firstBase == otherAgesBase);

    // Assert
    EXPECT_TRUE(equalThroughTheBase);
    EXPECT_FALSE(equalAsAgeHandlers);
    EXPECT_FALSE(equalToOtherAges);
}

TEST(FMTYieldHandler, GivenAHandlerWhenCompareThenReturnsTheGivenNamesThatItHoldsInTheirOrder)
{
    // Arrange
    Core::FMTAgeYieldHandler handler = getVolumeHandler("? ? ?", { 100.0, 200.0, 240.0 });
    handler.setYieldValues("STEMS", { 10, 20, 30 }, { 50.0, 60.0, 70.0 });
    const Core::FMTYieldHandler& base = handler;

    // Act
    const std::vector<std::string> same = base.compare({ "VOLUME", "UNKNOWN", "STEMS" });
    const std::vector<std::string> none = base.compare({});

    // Assert
    EXPECT_EQ(same, std::vector<std::string>({ "VOLUME", "STEMS" }));
    EXPECT_TRUE(none.empty());
}

// Only the complex handler overrides a section (_OVERRIDE): the base gives no override index, no tabou and no index of
// theme, and its age handler keeps them.
TEST(FMTYieldHandler, GivenABaseAndAnAgeHandlerWhenGetOverrideIndexTabousAndIndexesThenReturnZeroAndNone)
{
    // Arrange
    Core::FMTYieldHandler base(getMask("? ? ?"));
    Core::FMTAgeYieldHandler ageHandler = getVolumeHandler("? ? ?", { 100.0, 200.0, 240.0 });

    // Act
    base.setOverrideIndex(3);
    ageHandler.setOverrideIndex(3);

    // Assert
    EXPECT_EQ(base.getOverrideIndex(), 0u);
    EXPECT_TRUE(base.getTabous().empty());
    EXPECT_TRUE(base.indexes({ "VOLUME" }).empty());
    EXPECT_FALSE(base.inLookAt("VOLUME"));
    EXPECT_EQ(ageHandler.getOverrideIndex(), 0u);
    EXPECT_TRUE(ageHandler.getTabous().empty());
}

// The presolve of the base clones the handler and presolves its mask: the yields stay.
TEST(FMTYieldHandler, GivenAnAgeHandlerWhenPreSolveThenTheCopyHasThePresolvedMaskAndTheSameValues)
{
    // Arrange
    const PreSolve preSolve = getPreSolve();
    const Core::FMTAgeYieldHandler handler = getVolumeHandler("UNITE1 PEUPLEMENT2 UTR1", { 100.0, 200.0, 240.0 });

    // Act
    const std::unique_ptr<Core::FMTYieldHandler> presolved = handler.preSolve(preSolve.filter, preSolve.themes);

    // Assert
    ASSERT_NE(presolved, nullptr);
    EXPECT_EQ(std::string(presolved->getMask()), "PEUPLEMENT2");
    EXPECT_EQ(presolved->getBases(), std::vector<int>({ 10, 20, 30 }));
    EXPECT_DOUBLE_EQ(getAtAge(*presolved, "VOLUME", 20), 200.0);
    EXPECT_EQ(std::string(handler.getMask()), "UNITE1 PEUPLEMENT2 UTR1");
}
