/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTTransitionMask.h"
#include "FMTAgeBounds.h"
#include "FMTAgeYieldHandler.h"
#include "FMTDevelopment.h"
#include "FMTDevelopmentPath.h"
#include "FMTLockBounds.h"
#include "FMTMask.h"
#include "FMTMaskFilter.h"
#include "FMTTheme.h"
#include "FMTYields.h"
#include "FMTYldBounds.h"
#include "FMTutility.h"

#include "ExceptionChain.h"

#include <gtest/gtest.h>

#include <map>
#include <string>
#include <vector>

// A *TARGET of the transition section, built as FMTTransitionParser builds it: the target mask, the proportion, and
// the _AGE, _LOCK or yield given to the development it leads to. The themes are those of
// Tests/Unit/Core/FMTTheme.cpp, and VOLUMETOTAL that of peuplement1 for every development (as in
// Tests/Unit/Core/FMTDevelopment.cpp).

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

    // The themes of TWD_land and a fourth theme of two attributes, added as FMTModel::pushTheme adds the theme of an
    // action that FMTModel::buildAction builds: its attributes are the bits 10 and 11.
    std::vector<Core::FMTTheme> getThemesWithANewTheme()
    {
        std::vector<Core::FMTTheme> themes = getThemes();
        themes.emplace_back(std::vector<std::string>({ "VPRE", "VPOST" }), themes.size(), 10, "~FMT_THEME_ABUILT");
        return themes;
    }

    Core::FMTDevelopment getDevelopment(const std::string& p_mask, int p_age, int p_lock = 0, int p_period = 1)
    {
        return Core::FMTDevelopment(getMask(p_mask), p_age, p_lock, p_period);
    }

    // VOLUMETOTAL of peuplement1 in TWD_land: 0, 100, 150, 200, 250, 300 and 350 at the ages 0 to 30, by 5 years.
    Core::FMTYields getYields()
    {
        Core::FMTAgeYieldHandler handler(getMask("? ? ?"));
        handler.setYieldValues("VOLUMETOTAL", { 0, 5, 10, 15, 20, 25, 30 },
            { 0.0, 100.0, 150.0, 200.0, 250.0, 300.0, 350.0 });
        Core::FMTYields yields;
        yields.pushBackAgeHandler(getMask("? ? ?"), handler);
        yields.update();
        return yields;
    }

    // *TARGET <mask> <proportion>
    Core::FMTTransitionMask getTarget(const std::string& p_mask, double p_proportion)
    {
        return Core::FMTTransitionMask(p_mask, getThemes(), p_proportion);
    }

    // *TARGET <mask> <proportion> _LOCK <lock>
    Core::FMTTransitionMask getLockTarget(const std::string& p_mask, double p_proportion, int p_lock)
    {
        Core::FMTTransitionMask target = getTarget(p_mask, p_proportion);
        target.addBounds(Core::FMTLockBounds(Core::FMTsection::Transition, Core::FMTkwor::Target, p_lock, p_lock));
        return target;
    }

    // *TARGET <mask> <proportion> _AGE <age>
    Core::FMTTransitionMask getAgeTarget(const std::string& p_mask, double p_proportion, int p_age)
    {
        Core::FMTTransitionMask target = getTarget(p_mask, p_proportion);
        target.addBounds(Core::FMTAgeBounds(Core::FMTsection::Transition, Core::FMTkwor::Target, p_age, p_age));
        return target;
    }

    // The mask, age, lock and period of a development as <mask>|<age>|<lock>|<period>.
    std::string describe(const Core::FMTDevelopment& p_development)
    {
        return std::string(p_development.getMask()) + "|" + std::to_string(p_development.getAge()) + "|"
            + std::to_string(p_development.getLock()) + "|" + std::to_string(p_development.getPeriod());
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
}

TEST(FMTTransitionMask, GivenAMaskThemesAndAProportionWhenConstructedThenKeepsThemWithoutCondition)
{
    // Arrange
    const std::vector<Core::FMTTheme> themes = getThemes();

    // Act
    const Core::FMTTransitionMask target("? FEU ?", themes, 60.0);

    // Assert
    EXPECT_EQ(std::string(target.getMask()), "? FEU ?");
    EXPECT_TRUE(target.getMaskReference() == getMask("? FEU ?"));
    EXPECT_DOUBLE_EQ(target.getProportion(), 60.0);
    EXPECT_TRUE(target.emptyAge());
    EXPECT_TRUE(target.emptyYlds());
}

TEST(FMTTransitionMask, GivenADefaultTargetWhenConstructedThenHasNoMaskAndNoProportion)
{
    // Arrange and Act
    const Core::FMTTransitionMask target;

    // Assert
    EXPECT_TRUE(target.getMask().empty());
    EXPECT_DOUBLE_EQ(target.getProportion(), 0.0);
}

// The constructor keeps any proportion; setProportion limits it to 100.
TEST(FMTTransitionMask, GivenAProportionAbove100WhenSetProportionThenLimitsItTo100)
{
    // Arrange
    Core::FMTTransitionMask above = getTarget("? FEU ?", 40.0);
    Core::FMTTransitionMask below = getTarget("? FEU ?", 40.0);
    const Core::FMTTransitionMask constructedAbove = getTarget("? FEU ?", 150.0);

    // Act
    above.setProportion(150.0);
    below.setProportion(25.5);

    // Assert
    EXPECT_DOUBLE_EQ(above.getProportion(), 100.0);
    EXPECT_DOUBLE_EQ(below.getProportion(), 25.5);
    EXPECT_DOUBLE_EQ(constructedAbove.getProportion(), 150.0);
}

TEST(FMTTransitionMask, GivenATargetAttributeWhenTransThenReplacesTheAttributeOfThatThemeOnly)
{
    // Arrange
    const Core::FMTTransitionMask target = getTarget("? FEU ?", 100.0);

    // Act
    const Core::FMTMask transformed = target.trans(getMask("UNITE2 PEUPLEMENT3 UTR1"), getThemes());

    // Assert
    EXPECT_EQ(std::string(transformed), "UNITE2 FEU UTR1");
    EXPECT_TRUE(transformed == getMask("UNITE2 FEU UTR1"));
}

TEST(FMTTransitionMask, GivenAWildcardTargetWhenTransThenKeepsTheMaskOfTheDevelopment)
{
    // Arrange
    const Core::FMTTransitionMask target = getTarget("? ? ?", 100.0);

    // Act
    const Core::FMTMask transformed = target.trans(getMask("UNITE2 PEUPLEMENT3 UTR1"), getThemes());

    // Assert
    EXPECT_TRUE(transformed == getMask("UNITE2 PEUPLEMENT3 UTR1"));
}

TEST(FMTTransitionMask, GivenAnAttributeInEveryThemeWhenTransThenGivesTheTargetMask)
{
    // Arrange
    const Core::FMTTransitionMask target = getTarget("UNITE3 PEUPLEMENT1 UTR2", 100.0);

    // Act
    const Core::FMTMask transformed = target.trans(getMask("UNITE1 FEU UTR1"), getThemes());

    // Assert
    EXPECT_EQ(std::string(transformed), "UNITE3 PEUPLEMENT1 UTR2");
}

// FMTTheme::isAttribute accepts an aggregate (Tests/ETAT.md, section 5.11): the target keeps the bits of the aggregate,
// so that the development it leads to has both attributes of POURPL.
TEST(FMTTransitionMask, GivenATargetAggregateWhenTransThenTheMaskTakesAllTheAttributesOfTheAggregate)
{
    // Arrange
    const Core::FMTTransitionMask target = getTarget("? POURPL ?", 100.0);

    // Act
    const Core::FMTMask transformed = target.trans(getMask("UNITE1 FEU UTR1"), getThemes());

    // Assert
    EXPECT_TRUE(transformed == getMask("UNITE1 POURPL UTR1"));
    EXPECT_EQ(transformed.getSubsetCount(getThemes().at(1)), 2u);
}

TEST(FMTTransitionMask, GivenAnAgeResetWhenDisturbThenLeadsToTheTargetMaskAtAgeZeroWithTheProportionOfTheTarget)
{
    // Arrange: *TARGET ? peuplement1 ? 100.00, for an action of age reset
    const Core::FMTYields yields = getYields();
    const Core::FMTTransitionMask target = getTarget("? PEUPLEMENT1 ?", 100.0);
    const Core::FMTDevelopment development = getDevelopment("UNITE2 PEUPLEMENT3 UTR2", 20, 0, 2);

    // Act
    const Core::FMTDevelopmentPath path = target.disturb(development, yields, getThemes(), true);

    // Assert
    EXPECT_EQ(describe(path.getDevelopment()), "UNITE2 PEUPLEMENT1 UTR2|0|0|2");
    EXPECT_DOUBLE_EQ(path.getProportion(), 100.0);
}

TEST(FMTTransitionMask, GivenNoAgeResetWhenDisturbThenKeepsTheAgeTheLockAndThePeriodOfTheDevelopment)
{
    // Arrange: *TARGET ? feu ? 60
    const Core::FMTYields yields = getYields();
    const Core::FMTTransitionMask target = getTarget("? FEU ?", 60.0);
    const Core::FMTDevelopment development = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 12, 2, 3);

    // Act
    const Core::FMTDevelopmentPath path = target.disturb(development, yields, getThemes(), false);

    // Assert
    EXPECT_EQ(describe(path.getDevelopment()), "UNITE1 FEU UTR1|12|2|3");
    EXPECT_DOUBLE_EQ(path.getProportion(), 60.0);
}

TEST(FMTTransitionMask, GivenATargetAgeWhenDisturbWithAgeResetThenSetsTheTargetAge)
{
    // Arrange: *TARGET ? ? ? 100 _AGE 5
    const Core::FMTYields yields = getYields();
    const Core::FMTTransitionMask target = getAgeTarget("? ? ?", 100.0, 5);
    const Core::FMTDevelopment development = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 20);

    // Act
    const Core::FMTDevelopmentPath path = target.disturb(development, yields, getThemes(), true);

    // Assert
    EXPECT_EQ(path.getDevelopment().getAge(), 5);
}

TEST(FMTTransitionMask, GivenATargetLockWhenDisturbThenKeepsTheHigherOfTheTwoLocks)
{
    // Arrange: *TARGET ? ? ? 100 _LOCK 1
    const Core::FMTYields yields = getYields();
    const Core::FMTTransitionMask target = getLockTarget("? ? ?", 100.0, 1);
    const Core::FMTDevelopment unlocked = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 8, 0);
    const Core::FMTDevelopment locked = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 8, 3);

    // Act
    const Core::FMTDevelopmentPath fromUnlocked = target.disturb(unlocked, yields, getThemes(), false);
    const Core::FMTDevelopmentPath fromLocked = target.disturb(locked, yields, getThemes(), false);

    // Assert
    EXPECT_EQ(fromUnlocked.getDevelopment().getLock(), 1);
    EXPECT_EQ(fromLocked.getDevelopment().getLock(), 3);
    EXPECT_EQ(fromUnlocked.getDevelopment().getAge(), 8);
}

// The age of a target yield is the base age, up to that of the development, whose value is the closest to the target
// (FMTAgeYieldHandler::getAge): VOLUMETOTAL is 150 at age 10.
TEST(FMTTransitionMask, GivenATargetYieldWhenDisturbThenSetsTheAgeWhereTheYieldReachesItsValue)
{
    // Arrange: *TARGET ? ? ? 100 VOLUMETOTAL 150, for an action of age reset
    const Core::FMTYields yields = getYields();
    Core::FMTTransitionMask target = getTarget("? ? ?", 100.0);
    target.addBounds(Core::FMTYldBounds(Core::FMTsection::Transition, Core::FMTkwor::Target, "VOLUMETOTAL", 150.0, 150.0));
    const Core::FMTDevelopment development = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 20);

    // Act
    const Core::FMTDevelopmentPath path = target.disturb(development, yields, getThemes(), true);

    // Assert
    EXPECT_EQ(path.getDevelopment().getAge(), 10);
}

TEST(FMTTransitionMask, GivenTargetsWhenComparedThenTheyAreEqualOnlyWithTheSameMaskProportionAndCondition)
{
    // Arrange
    const Core::FMTTransitionMask target = getLockTarget("? ? ?", 100.0, 1);

    // Act
    const bool sameValues = (target == getLockTarget("? ? ?", 100.0, 1));
    const bool otherProportion = (target == getLockTarget("? ? ?", 50.0, 1));
    const bool otherMask = (target == getLockTarget("? FEU ?", 100.0, 1));
    const bool otherCondition = (target == getTarget("? ? ?", 100.0));

    // Assert
    EXPECT_TRUE(sameValues);
    EXPECT_FALSE(otherProportion);
    EXPECT_FALSE(otherMask);
    EXPECT_FALSE(otherCondition);
}

TEST(FMTTransitionMask, GivenTargetsWhenConvertedToStringThenWritesTheirMaskProportionAndCondition)
{
    // Arrange
    const Core::FMTTransitionMask fire = getTarget("? FEU ?", 100.0);
    const Core::FMTTransitionMask caribou = getLockTarget("? ? ?", 100.0, 1);
    const Core::FMTTransitionMask aged = getAgeTarget("? ? ?", 100.0, 5);

    // Act
    const std::vector<std::string> written = { std::string(fire), std::string(caribou), std::string(aged) };

    // Assert
    EXPECT_EQ(written, std::vector<std::string>({ "*TARGET ? FEU ? 100.000000 ", "*TARGET ? ? ? 100.000000 _LOCK 1",
        "*TARGET ? ? ? 100.000000 _AGE 5 " }));
}

// The themes of TWD_land have no name: each is named THEME_ and its position from 1.
TEST(FMTTransitionMask, GivenThemesWithoutNameWhenGetThenMapsEachThemeByItsPositionToTheValueOfTheTarget)
{
    // Arrange
    const Core::FMTTransitionMask target = getTarget("? FEU ?", 100.0);

    // Act
    const std::map<std::string, std::string> values = target.get(getThemes());

    // Assert
    EXPECT_EQ(values, (std::map<std::string, std::string>({ { "THEME_1", "?" }, { "THEME_2", "FEU" },
        { "THEME_3", "?" } })));
}

TEST(FMTTransitionMask, GivenAnotherMaskWhenConstructedFromATargetThenKeepsItsProportionAndConditionWithTheNewMask)
{
    // Arrange
    const Core::FMTYields yields = getYields();
    const Core::FMTTransitionMask caribou = getLockTarget("? ? ?", 60.0, 1);
    const Core::FMTDevelopment development = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 8);

    // Act
    const Core::FMTTransitionMask burnt(caribou, getMask("? FEU ?"), getThemes());

    // Assert
    EXPECT_EQ(std::string(burnt.getMask()), "? FEU ?");
    EXPECT_DOUBLE_EQ(burnt.getProportion(), 60.0);
    EXPECT_EQ(describe(burnt.disturb(development, yields, getThemes(), false).getDevelopment()), "UNITE1 FEU UTR1|8|1|1");
}

TEST(FMTTransitionMask, GivenATargetBuiltWithANewThemeWhenTransThenKeepsTheAttributeOfTheNewTheme)
{
    // Arrange: *TARGET ? feu ? ? 100, on the themes of TWD_land and a fourth theme
    const std::vector<Core::FMTTheme> themes = getThemesWithANewTheme();
    const Core::FMTTransitionMask target("? FEU ? ?", themes, 100.0);

    // Act
    const Core::FMTMask transformed = target.trans(Core::FMTMask("UNITE1 PEUPLEMENT1 UTR1 VPOST", themes), themes);

    // Assert
    EXPECT_TRUE(transformed == Core::FMTMask("UNITE1 FEU UTR1 VPOST", themes));
}

// Defect 21 (Tests/ETAT.md, section 2.7): setMask changes the mask without rebuilding the selection of the target, so
// that trans still works on the mask the target was built with, of 10 bits, and loses the fourth theme. FMTModel::pushTheme,
// which FMTModel::buildAction calls, moves the targets of the transitions to a new theme so.
TEST(FMTTransitionMask, DISABLED_GivenAMaskOfANewThemeWhenSetMaskThenTransAppliesTheNewMask)
{
    // Arrange: the target ? feu ? of the three themes, then the mask ? feu ? ? of the four themes, as pushTheme sets it
    const std::vector<Core::FMTTheme> themes = getThemesWithANewTheme();
    Core::FMTTransitionMask target = getTarget("? FEU ?", 100.0);
    const Core::FMTMask base("UNITE1 PEUPLEMENT1 UTR1 VPOST", themes);
    Core::FMTMask transformed;

    // Act
    target.setMask(Core::FMTMask("? FEU ? ?", themes));
    const std::vector<Exception::FMTexc> raised = Testing::getRaisedTypes([&]()
        {
        transformed = target.trans(base, themes);
        });

    // Assert
    EXPECT_TRUE(raised.empty());
    EXPECT_TRUE(transformed == Core::FMTMask("UNITE1 FEU UTR1 VPOST", themes));
}

// After the presolve, the target works on the presolved masks: peuplement1 for a presolved development of peuplement2.
TEST(FMTTransitionMask, GivenAPreSolveFilterWhenPreSolveThenTheTargetTransformsThePresolvedMasks)
{
    // Arrange
    const PreSolve preSolve = getPreSolve();
    const Core::FMTTransitionMask target = getTarget("? PEUPLEMENT1 ?", 100.0);
    const Core::FMTMask presolvedDevelopment = getMask("UNITE1 PEUPLEMENT2 UTR1").preSolve(preSolve.filter, preSolve.themes);

    // Act
    const Core::FMTTransitionMask presolved = target.preSolve(preSolve.filter, preSolve.themes);

    // Assert
    EXPECT_EQ(std::string(presolved.getMask()), "PEUPLEMENT1");
    EXPECT_TRUE(presolved.trans(presolvedDevelopment, preSolve.themes)
        == getMask("UNITE1 PEUPLEMENT1 UTR1").preSolve(preSolve.filter, preSolve.themes));
    EXPECT_DOUBLE_EQ(presolved.getProportion(), 100.0);
}
