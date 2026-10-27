/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTTransition.h"
#include "FMTAction.h"
#include "FMTAgeBounds.h"
#include "FMTAgeYieldHandler.h"
#include "FMTDevelopment.h"
#include "FMTFork.h"
#include "FMTMask.h"
#include "FMTMaskFilter.h"
#include "FMTSpec.h"
#include "FMTTheme.h"
#include "FMTTransitionMask.h"
#include "FMTYields.h"
#include "FMTYldBounds.h"
#include "FMTutility.h"

#include "ExceptionChain.h"

#include <gtest/gtest.h>

#include <limits>
#include <map>
#include <string>
#include <utility>
#include <vector>

// The transitions of TWD_land, built in code as FMTTransitionParser and FMTModel build them: a fork per *SOURCE, with
// the condition of the source and its *TARGET, then update, which shrinks the source masks. The themes are those of
// Tests/Unit/Core/FMTTheme.cpp, and VOLUMETOTAL that of peuplement1 for every development (as in
// Tests/Unit/Core/FMTDevelopment.cpp). A yield request keeps the address of its development, which must outlive it
// (Tests/ETAT.md, section 7).

namespace
{
    const int NO_UPPER_BOUND = (std::numeric_limits<int>::max)();

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

    // The targets of a *SOURCE without condition.
    Core::FMTFork getFork(const std::vector<Core::FMTTransitionMask>& p_targets)
    {
        Core::FMTFork fork;
        for (const Core::FMTTransitionMask& target : p_targets)
        {
            fork.add(target);
        }
        return fork;
    }

    // *CASE <name>, then *SOURCE <mask> and its fork, for each source, as FMTTransitionParser leaves it.
    Core::FMTTransition buildTransition(const std::string& p_name,
        const std::vector<std::pair<std::string, Core::FMTFork>>& p_sources)
    {
        Core::FMTTransition transition(p_name);
        for (const std::pair<std::string, Core::FMTFork>& source : p_sources)
        {
            transition.push_back(getMask(source.first), source.second);
        }
        return transition;
    }

    Core::FMTTransition getTransition(const std::string& p_name,
        const std::vector<std::pair<std::string, Core::FMTFork>>& p_sources)
    {
        Core::FMTTransition transition = buildTransition(p_name, p_sources);
        transition.update();
        return transition;
    }

    // *CASE aFire, *SOURCE ? ? ?, *TARGET ? feu ? 100.00
    Core::FMTTransition getFireTransition()
    {
        return getTransition("AFIRE", { { "? ? ?", getFork({ getTarget("? FEU ?", 100.0) }) } });
    }

    // *CASE _DEATH, *SOURCE ? feu ?, *TARGET ? peuplement1 ? 100.00, *SOURCE ? ? ?, *TARGET ? ? ? 100.00
    Core::FMTTransition getDeathTransition()
    {
        return getTransition("_DEATH", { { "? FEU ?", getFork({ getTarget("? PEUPLEMENT1 ?", 100.0) }) },
            { "? ? ?", getFork({ getTarget("? ? ?", 100.0) }) } });
    }

    // *CASE arecup, *SOURCE ? feu ?, *TARGET ? peuplement1 ? 100.00
    Core::FMTTransition getSalvageTransition()
    {
        return getTransition("ARECUP", { { "? FEU ?", getFork({ getTarget("? PEUPLEMENT1 ?", 100.0) }) } });
    }

    // *SOURCE ? ? ? @AGE(10.._MAXAGE), *TARGET ? peuplement1 ? 100
    Core::FMTFork getOldStandsFork()
    {
        Core::FMTFork fork = getFork({ getTarget("? PEUPLEMENT1 ?", 100.0) });
        fork.addBounds(Core::FMTAgeBounds(Core::FMTsection::Transition, Core::FMTkwor::Source, NO_UPPER_BOUND, 10));
        return fork;
    }

    // The name of the mask of the first target of p_fork, or "none" without fork.
    std::string getFirstTarget(const Core::FMTFork* p_fork)
    {
        if (p_fork == nullptr)
        {
            return "none";
        }
        return std::string(p_fork->getMaskTrans().at(0).getMask());
    }

    // The presolve of FMTModel::preSolve for the developments of UNITE1 PEUPLEMENT1 UTR1 and UNITE1 PEUPLEMENT2 UTR1,
    // as in Tests/Unit/Core/FMTMask.cpp: the filter of the union of their masks, and the themes left with more than
    // one attribute, here PEUPLEMENT1 and PEUPLEMENT2 of the second theme.
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

TEST(FMTTransition, GivenANameWhenConstructedThenHasNoFork)
{
    // Arrange
    const std::string name = "AFIRE";

    // Act
    const Core::FMTTransition named(name);
    const Core::FMTTransition unnamed;

    // Assert
    EXPECT_EQ(named.getName(), name);
    EXPECT_TRUE(named.empty());
    EXPECT_EQ(named.size(), 0u);
    EXPECT_EQ(unnamed.getName(), "");
    EXPECT_TRUE(unnamed.empty());
}

TEST(FMTTransition, GivenATransitionWhenSetNameThenChangesItsNameOnly)
{
    // Arrange
    Core::FMTTransition transition = getFireTransition();

    // Act
    transition.setName("AFEU");

    // Assert
    EXPECT_EQ(transition.getName(), "AFEU");
    EXPECT_EQ(transition.size(), 1u);
}

TEST(FMTTransition, GivenATransitionWhenCopiedOrAssignedThenTheCopyIsEqual)
{
    // Arrange
    const Core::FMTTransition original = getDeathTransition();

    // Act
    const Core::FMTTransition copy(original);
    Core::FMTTransition assigned;
    assigned = original;

    // Assert
    EXPECT_TRUE(copy == original);
    EXPECT_TRUE(assigned == original);
    EXPECT_EQ(assigned.getName(), "_DEATH");
    EXPECT_EQ(assigned.size(), 2u);
}

// Include/FMTTransition.h documents a comparison of the names only: the code also compares the forks.
TEST(FMTTransition, GivenTransitionsThatDifferByTheirNameOrTheirForksWhenComparedThenTheyDiffer)
{
    // Arrange
    const Core::FMTTransition transition = getFireTransition();
    Core::FMTTransition renamed = getFireTransition();
    renamed.setName("AFEU");
    const Core::FMTTransition otherTarget = getTransition("AFIRE",
        { { "? ? ?", getFork({ getTarget("? PEUPLEMENT1 ?", 100.0) }) } });

    // Act
    const bool sameValues = (transition == getFireTransition());
    const bool sameRenamed = (transition == renamed);
    const bool sameOtherTarget = (transition == otherTarget);
    const bool differentOtherTarget = (transition != otherTarget);

    // Assert
    EXPECT_TRUE(sameValues);
    EXPECT_FALSE(sameRenamed);
    EXPECT_FALSE(sameOtherTarget);
    EXPECT_TRUE(differentOtherTarget);
}

TEST(FMTTransition, GivenTransitionsWhenComparedByOrderThenComparesTheirNames)
{
    // Arrange
    const Core::FMTTransition fire = getFireTransition();
    const Core::FMTTransition salvage = getSalvageTransition();

    // Act
    const bool fireFirst = (fire < salvage);
    const bool salvageFirst = (salvage < fire);
    const bool beforeItself = (fire < fire);

    // Assert
    EXPECT_TRUE(fireFirst);
    EXPECT_FALSE(salvageFirst);
    EXPECT_FALSE(beforeItself);
}

TEST(FMTTransition, GivenATransitionWhenConvertedToStringThenWritesItsCaseThenEachSourceWithItsTargets)
{
    // Arrange
    const Core::FMTTransition death = getDeathTransition();

    // Act
    const std::string written = std::string(death);

    // Assert
    EXPECT_EQ(written, "*CASE _DEATH\n*SOURCE ? FEU ? \n*TARGET ? PEUPLEMENT1 ? 100.000000 \n"
        "*SOURCE ? ? ? \n*TARGET ? ? ? 100.000000 \n");
}

TEST(FMTTransition, GivenASourceConditionWhenConvertedToStringThenWritesItAfterTheSourceMask)
{
    // Arrange
    const Core::FMTTransition transition = getTransition("COUPETOTALE", { { "? ? ?", getOldStandsFork() } });

    // Act
    const std::string written = std::string(transition);

    // Assert
    EXPECT_EQ(written, "*CASE COUPETOTALE\n*SOURCE ? ? ? @AGE(10.._MAXAGE) \n*TARGET ? PEUPLEMENT1 ? 100.000000 \n");
}

TEST(FMTTransition, GivenTwoSourcesWhenGetForkThenTheFirstSourceWhoseMaskHoldsTheDevelopmentGivesTheFork)
{
    // Arrange
    const Core::FMTYields yields = getYields();
    const Core::FMTTransition death = getDeathTransition();
    const Core::FMTDevelopment burnt = getDevelopment("UNITE1 FEU UTR1", 80);
    const Core::FMTDevelopment productive = getDevelopment("UNITE1 PEUPLEMENT2 UTR1", 80);

    // Act
    const Core::FMTFork* burntFork = death.getFork(burnt, yields);
    const Core::FMTFork* productiveFork = death.getFork(productive, yields);

    // Assert
    EXPECT_EQ(getFirstTarget(burntFork), "? PEUPLEMENT1 ?");
    EXPECT_EQ(getFirstTarget(productiveFork), "? ? ?");
}

TEST(FMTTransition, GivenASourceAgeConditionWhenGetForkThenGivesNoForkOutsideItsAges)
{
    // Arrange: *SOURCE ? ? ? @AGE(10.._MAXAGE)
    const Core::FMTYields yields = getYields();
    const Core::FMTTransition transition = getTransition("COUPETOTALE", { { "? ? ?", getOldStandsFork() } });
    const Core::FMTDevelopment old = getDevelopment("UNITE1 PEUPLEMENT3 UTR1", 15);
    const Core::FMTDevelopment young = getDevelopment("UNITE1 PEUPLEMENT3 UTR1", 5);

    // Act
    const Core::FMTFork* oldFork = transition.getFork(old, yields);
    const Core::FMTFork* youngFork = transition.getFork(young, yields);

    // Assert
    EXPECT_EQ(getFirstTarget(oldFork), "? PEUPLEMENT1 ?");
    EXPECT_EQ(youngFork, nullptr);
}

// #358: FMTParser::_setSpec drops a source condition written as in the action section (_AGE >= 10), and leaves the
// fork without condition, as here: getFork then gives the fork at every age. The integration case
// FMTTransitionParser.GivenASourceConditionInTheActionFormWhenReadThenAppliesTheSourceToEveryAge characterizes the
// reading.
TEST(FMTTransition, GivenASourceWithoutConditionWhenGetForkThenGivesTheForkAtEveryAge)
{
    // Arrange: *SOURCE ? ? ?, *TARGET ? peuplement1 ? 100
    const Core::FMTYields yields = getYields();
    const Core::FMTTransition transition = getTransition("COUPETOTALE",
        { { "? ? ?", getFork({ getTarget("? PEUPLEMENT1 ?", 100.0) }) } });
    const Core::FMTDevelopment old = getDevelopment("UNITE1 PEUPLEMENT3 UTR1", 15);
    const Core::FMTDevelopment young = getDevelopment("UNITE1 PEUPLEMENT3 UTR1", 5);

    // Act
    const Core::FMTFork* oldFork = transition.getFork(old, yields);
    const Core::FMTFork* youngFork = transition.getFork(young, yields);

    // Assert
    ASSERT_NE(oldFork, nullptr);
    EXPECT_EQ(youngFork, oldFork);
}

TEST(FMTTransition, GivenASourceYieldConditionWhenGetForkThenReadsTheYieldAtTheAgeOfTheDevelopment)
{
    // Arrange: *SOURCE ? ? ? @YLD(VOLUMETOTAL,100.._MAXAGE); VOLUMETOTAL is 80 at age 4 and 100 at age 5
    const Core::FMTYields yields = getYields();
    Core::FMTFork fork = getFork({ getTarget("? PEUPLEMENT1 ?", 100.0) });
    fork.addBounds(Core::FMTYldBounds(Core::FMTsection::Transition, Core::FMTkwor::Source, "VOLUMETOTAL",
        (std::numeric_limits<double>::max)(), 100.0));
    const Core::FMTTransition transition = getTransition("COUPETOTALE", { { "? ? ?", fork } });
    const Core::FMTDevelopment atAgeFour = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 4);
    const Core::FMTDevelopment atAgeFive = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 5);

    // Act
    const Core::FMTFork* forkAtAgeFour = transition.getFork(atAgeFour, yields);
    const Core::FMTFork* forkAtAgeFive = transition.getFork(atAgeFive, yields);

    // Assert
    EXPECT_EQ(forkAtAgeFour, nullptr);
    EXPECT_EQ(getFirstTarget(forkAtAgeFive), "? PEUPLEMENT1 ?");
}

TEST(FMTTransition, GivenADevelopmentOutsideEverySourceWhenGetForkThenGivesNoFork)
{
    // Arrange: *SOURCE ? feu ?, for a development of peuplement1
    const Core::FMTYields yields = getYields();
    const Core::FMTTransition salvage = getSalvageTransition();
    const Core::FMTDevelopment development = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 20);

    // Act
    const Core::FMTFork* fork = salvage.getFork(development, yields);

    // Assert
    EXPECT_EQ(fork, nullptr);
}

// Include/FMTTransition.h documents a total proportion of 1: the proportions are percentages, and the code compares
// their sum to 100.
TEST(FMTTransition, GivenTargetsThatDoNotSumTo100WhenIsLeakingThenReturnsTrue)
{
    // Arrange
    const Core::FMTTransition complete = getTransition("COUPETOTALE",
        { { "? ? ?", getFork({ getTarget("? PEUPLEMENT1 ?", 60.0), getTarget("? PEUPLEMENT2 ?", 40.0) }) } });
    const Core::FMTTransition leaking = getTransition("COUPETOTALE",
        { { "? ? ?", getFork({ getTarget("? PEUPLEMENT1 ?", 60.0), getTarget("? PEUPLEMENT2 ?", 30.0) }) } });

    // Act
    const bool completeLeaks = complete.isLeaking();
    const bool leakingLeaks = leaking.isLeaking();

    // Assert
    EXPECT_FALSE(completeLeaks);
    EXPECT_TRUE(leakingLeaks);
}

TEST(FMTTransition, GivenTwoTargetsWhenSingleThenKeepsTheTargetOfTheLargestProportionAt100)
{
    // Arrange
    const Core::FMTTransition transition = getTransition("COUPETOTALE",
        { { "? ? ?", getFork({ getTarget("? PEUPLEMENT1 ?", 30.0), getTarget("? PEUPLEMENT2 ?", 70.0) }) } });

    // Act
    const Core::FMTTransition single = transition.single();

    // Assert
    EXPECT_EQ(single.getName(), "COUPETOTALE");
    ASSERT_EQ(single.size(), 1u);
    const std::vector<Core::FMTTransitionMask> targets = single.begin()->second.getMaskTrans();
    ASSERT_EQ(targets.size(), 1u);
    EXPECT_EQ(std::string(targets.at(0).getMask()), "? PEUPLEMENT2 ?");
    EXPECT_DOUBLE_EQ(targets.at(0).getProportion(), 100.0);
}

// The average is an integer: (10 + 21) / 2 gives 15. The development of age 0 is not operable by aFire (_AGE >= 1).
TEST(FMTTransition, GivenOperableDevelopmentsWhenAgeAfterThenAveragesTheirAgesAfterTheAction)
{
    // Arrange: *ACTION aFire N, ? prod ? _AGE >= 1, without age reset
    const Core::FMTYields yields = getYields();
    Core::FMTAction fire("AFIRE", true, false);
    Core::FMTSpec spec;
    spec.addBounds(Core::FMTAgeBounds(Core::FMTsection::Action, NO_UPPER_BOUND, 1));
    fire.push_back(getMask("? PROD ?"), spec);
    fire.update();
    const Core::FMTTransition transition = getFireTransition();
    const std::vector<Core::FMTDevelopment> developments = { getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 10),
        getDevelopment("UNITE2 PEUPLEMENT2 UTR2", 21), getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 0) };
    const std::vector<Core::FMTDevelopment> notOperable = { getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 0) };

    // Act
    const unsigned int average = transition.ageAfter(developments, fire, yields, getThemes());
    const unsigned int withoutOperable = transition.ageAfter(notOperable, fire, yields, getThemes());

    // Assert
    EXPECT_EQ(average, 15u);
    EXPECT_EQ(withoutOperable, 0u);
}

TEST(FMTTransition, GivenDevelopmentsWhenMainTargetThenReturnsTheTargetOfMostDevelopments)
{
    // Arrange: two burnt developments go to ? peuplement1 ?, one other to ? ? ?
    const Core::FMTYields yields = getYields();
    const Core::FMTTransition death = getDeathTransition();
    const std::vector<Core::FMTDevelopment> developments = { getDevelopment("UNITE1 FEU UTR1", 5),
        getDevelopment("UNITE3 PEUPLEMENT3 UTR3", 7), getDevelopment("UNITE1 FEU UTR1", 9) };

    // Act
    const Core::FMTMask target = death.mainTarget(developments, yields);
    const Core::FMTMask withoutDevelopment = death.mainTarget({}, yields);

    // Assert
    EXPECT_EQ(std::string(target), "? PEUPLEMENT1 ?");
    EXPECT_TRUE(target == getMask("? PEUPLEMENT1 ?"));
    EXPECT_TRUE(withoutDevelopment.empty());
}

TEST(FMTTransition, GivenDevelopmentsWhenAttributeTargetsThenGroupsThemByTheTransitionAndTheAttributesOfTheirMask)
{
    // Arrange
    const Core::FMTYields yields = getYields();
    const Core::FMTTransition death = getDeathTransition();
    const std::vector<Core::FMTDevelopment> developments = { getDevelopment("UNITE1 FEU UTR1", 5),
        getDevelopment("UNITE3 PEUPLEMENT3 UTR3", 7), getDevelopment("UNITE1 FEU UTR1", 9) };

    // Act
    const std::map<std::string, std::vector<Core::FMTDevelopment>> groups =
        death.attributeTargets(developments, yields, getThemes());

    // Assert
    ASSERT_EQ(groups.size(), 2u);
    ASSERT_EQ(groups.count("_DEATH-UNITE1-FEU-UTR1"), 1u);
    ASSERT_EQ(groups.count("_DEATH-UNITE3-PEUPLEMENT3-UTR3"), 1u);
    EXPECT_EQ(groups.at("_DEATH-UNITE1-FEU-UTR1").size(), 2u);
    EXPECT_EQ(groups.at("_DEATH-UNITE3-PEUPLEMENT3-UTR3").size(), 1u);
}

TEST(FMTTransition, GivenTargetsThatChangeOneThemeWhenGetStaticThemesThenReturnsTheOtherThemes)
{
    // Arrange: *TARGET ? feu ?
    const std::vector<Core::FMTTheme> themes = getThemes();
    const std::vector<const Core::FMTTheme*> allThemes = { &themes.at(0), &themes.at(1), &themes.at(2) };
    const Core::FMTTransition fire = getFireTransition();

    // Act
    const std::vector<const Core::FMTTheme*> staticThemes = fire.getStaticThemes(allThemes);

    // Assert
    EXPECT_EQ(staticThemes, std::vector<const Core::FMTTheme*>({ &themes.at(0), &themes.at(2) }));
}

// arecup leads from ? feu ? to ? peuplement1 ?: a development of peuplement1 can come from a burnt development of the
// same unit and UTR, and a development of peuplement2 from none.
TEST(FMTTransition, GivenAMaskWhenCanProduceThenReturnsTheSourcesThatLeadToIt)
{
    // Arrange
    const Core::FMTTransition salvage = getSalvageTransition();

    // Act
    const std::vector<Core::FMTMask> toFirstStand = salvage.canProduce(getMask("UNITE1 PEUPLEMENT1 UTR1"), getThemes());
    const std::vector<Core::FMTMask> toSecondStand = salvage.canProduce(getMask("UNITE1 PEUPLEMENT2 UTR1"), getThemes());

    // Assert
    ASSERT_EQ(toFirstStand.size(), 1u);
    EXPECT_EQ(std::string(toFirstStand.at(0)), "UNITE1 FEU UTR1");
    EXPECT_TRUE(toFirstStand.at(0) == getMask("UNITE1 FEU UTR1"));
    EXPECT_TRUE(toSecondStand.empty());
}

TEST(FMTTransition, GivenTwoTransitionsNotUpdatedWhenAddedThenAppendsTheSourcesOfTheOther)
{
    // Arrange
    Core::FMTTransition fire = buildTransition("AFIRE", { { "? ? ?", getFork({ getTarget("? FEU ?", 100.0) }) } });
    const Core::FMTTransition salvage = buildTransition("ARECUP",
        { { "? FEU ?", getFork({ getTarget("? PEUPLEMENT1 ?", 100.0) }) } });

    // Act
    fire += salvage;

    // Assert
    EXPECT_EQ(fire.getName(), "AFIRE");
    ASSERT_EQ(fire.size(), 2u);
    EXPECT_EQ(std::string((fire.begin() + 1)->first), "? FEU ?");
}

TEST(FMTTransition, GivenAnUpdatedTransitionWhenAddedThenRaisesAnInvalidAction)
{
    // Arrange
    Core::FMTTransition fire = buildTransition("AFIRE", { { "? ? ?", getFork({ getTarget("? FEU ?", 100.0) }) } });
    const Core::FMTTransition salvage = getSalvageTransition();

    // Act
    const std::vector<Exception::FMTexc> raised = Testing::getRaisedTypes([&]()
        {
        fire += salvage;
        });

    // Assert
    EXPECT_TRUE(Testing::hasType(raised, Exception::FMTexc::FMTinvalid_action));
}

// The source ? feu ? cannot be presolved, the presolve keeping peuplement1 and peuplement2 only: the source ? ? ? is
// left, and its source and target masks become ? in the presolved themes.
TEST(FMTTransition, GivenAPreSolveFilterWhenPreSolveThenKeepsTheSourcesOfThePresolvedDevelopmentsOnly)
{
    // Arrange
    const PreSolve preSolve = getPreSolve();
    std::vector<Core::FMTTheme> newThemes = preSolve.themes;
    const Core::FMTTransition death = getDeathTransition();

    // Act
    const Core::FMTTransition presolved = death.preSolve(preSolve.filter, getThemes(), newThemes);

    // Assert
    EXPECT_EQ(presolved.getName(), "_DEATH");
    ASSERT_EQ(presolved.size(), 1u);
    EXPECT_EQ(std::string(presolved.begin()->first), "?");
    EXPECT_EQ(getFirstTarget(&presolved.begin()->second), "?");
    EXPECT_EQ(death.size(), 2u);
}
