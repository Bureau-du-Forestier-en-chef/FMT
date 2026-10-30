/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTFork.h"
#include "FMTAgeBounds.h"
#include "FMTAgeYieldHandler.h"
#include "FMTDevelopment.h"
#include "FMTDevelopmentPath.h"
#include "FMTMask.h"
#include "FMTMaskFilter.h"
#include "FMTObject.h"
#include "FMTTheme.h"
#include "FMTTransitionMask.h"
#include "FMTYields.h"
#include "FMTutility.h"

#include "ExceptionChain.h"

#include <gtest/gtest.h>

#include <limits>
#include <map>
#include <string>
#include <vector>

// A fork is a *SOURCE of the transition section: its condition (the FMTSpec it derives from) and its *TARGET, which
// split the area of a development between the paths they lead to. The themes are those of
// Tests/Unit/Core/FMTTheme.cpp, and VOLUMETOTAL that of peuplement1 for every development (as in
// Tests/Unit/Core/FMTDevelopment.cpp).

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

    Core::FMTFork getFork(const std::vector<Core::FMTTransitionMask>& p_targets)
    {
        Core::FMTFork fork;
        for (const Core::FMTTransitionMask& target : p_targets)
        {
            fork.add(target);
        }
        return fork;
    }

    // The source condition @AGE(10.._MAXAGE).
    void addOldStandsCondition(Core::FMTFork& p_fork)
    {
        p_fork.addBounds(Core::FMTAgeBounds(Core::FMTsection::Transition, Core::FMTkwor::Source, NO_UPPER_BOUND, 10));
    }

    // Each development path as <mask>|<age>|<lock>|<period>|<proportion>.
    std::vector<std::string> describe(const std::vector<Core::FMTDevelopmentPath>& p_paths)
    {
        std::vector<std::string> descriptions;
        for (const Core::FMTDevelopmentPath& path : p_paths)
        {
            const Core::FMTDevelopment& development = path.getDevelopment();
            descriptions.push_back(std::string(development.getMask()) + "|" + std::to_string(development.getAge()) + "|"
                + std::to_string(development.getLock()) + "|" + std::to_string(development.getPeriod()) + "|"
                + std::to_string(path.getProportion()));
        }
        return descriptions;
    }

    // Turns errors into warnings for the lifetime of the guard. The exception handler of FMTObject is shared by every
    // object (Include/FMTObject.h): the guard gives it back its errors when the case ends.
    class ErrorsAsWarnings
    {
    public:
        explicit ErrorsAsWarnings(const std::vector<Exception::FMTexc>& p_errors)
        {
            m_object.setErrorsToWarnings(p_errors);
        }
        ~ErrorsAsWarnings()
        {
            m_object.setErrorsToWarnings({});
        }
        ErrorsAsWarnings(const ErrorsAsWarnings&) = delete;
        ErrorsAsWarnings& operator=(const ErrorsAsWarnings&) = delete;
    private:
        Core::FMTObject m_object;
    };

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

TEST(FMTFork, GivenTargetsWhenAddThenKeepsThemInTheirOrder)
{
    // Arrange
    Core::FMTFork fork;

    // Act
    fork.add(getTarget("? PEUPLEMENT1 ?", 60.0));
    fork.add(getTarget("? PEUPLEMENT2 ?", 40.0));

    // Assert
    ASSERT_EQ(fork.size(), 2u);
    EXPECT_EQ(std::string(fork.getMaskTrans().at(0).getMask()), "? PEUPLEMENT1 ?");
    EXPECT_EQ(std::string(fork.getMaskTrans().at(1).getMask()), "? PEUPLEMENT2 ?");
    EXPECT_DOUBLE_EQ(fork.sumProp(), 100.0);
}

TEST(FMTFork, GivenAForkWithAConditionWhenClearThenKeepsItsConditionWithoutTarget)
{
    // Arrange
    Core::FMTFork fork = getFork({ getTarget("? PEUPLEMENT1 ?", 60.0), getTarget("? PEUPLEMENT2 ?", 40.0) });
    addOldStandsCondition(fork);

    // Act
    fork.clear();

    // Assert
    EXPECT_EQ(fork.size(), 0u);
    EXPECT_DOUBLE_EQ(fork.sumProp(), 0.0);
    EXPECT_FALSE(fork.emptyAge());
}

TEST(FMTFork, GivenOneTargetWhenGetPathsThenLeadsToOnePath)
{
    // Arrange: *TARGET ? feu ? 100, for an action without age reset
    const Core::FMTYields yields = getYields();
    const Core::FMTFork fork = getFork({ getTarget("? FEU ?", 100.0) });
    const Core::FMTDevelopment development = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 12);

    // Act
    const std::vector<Core::FMTDevelopmentPath> paths = fork.getPaths(development, yields, getThemes(), false);

    // Assert
    EXPECT_EQ(describe(paths), std::vector<std::string>({ "UNITE1 FEU UTR1|12|0|1|100.000000" }));
}

TEST(FMTFork, GivenTwoTargetsWhenGetPathsThenLeadsToOnePathPerTargetInTheirOrder)
{
    // Arrange: *TARGET ? peuplement1 ? 60, *TARGET ? peuplement2 ? 40, for an action of age reset
    const Core::FMTYields yields = getYields();
    const Core::FMTFork fork = getFork({ getTarget("? PEUPLEMENT1 ?", 60.0), getTarget("? PEUPLEMENT2 ?", 40.0) });
    const Core::FMTDevelopment development = getDevelopment("UNITE1 PEUPLEMENT3 UTR1", 20);

    // Act
    const std::vector<Core::FMTDevelopmentPath> paths = fork.getPaths(development, yields, getThemes(), true);

    // Assert
    EXPECT_EQ(describe(paths), std::vector<std::string>({ "UNITE1 PEUPLEMENT1 UTR1|0|0|1|60.000000",
        "UNITE1 PEUPLEMENT2 UTR1|0|0|1|40.000000" }));
}

TEST(FMTFork, GivenTwoTargetsLeadingToTheSameDevelopmentWhenGetPathsThenRaisesSameTransitionTargets)
{
    // Arrange: *TARGET ? peuplement1 ? 60, *TARGET ? peuplement1 ? 40
    const Core::FMTYields yields = getYields();
    const Core::FMTFork fork = getFork({ getTarget("? PEUPLEMENT1 ?", 60.0), getTarget("? PEUPLEMENT1 ?", 40.0) });
    const Core::FMTDevelopment development = getDevelopment("UNITE1 PEUPLEMENT3 UTR1", 20);
    const std::vector<Core::FMTTheme> themes = getThemes();

    // Act
    const std::vector<Exception::FMTexc> raised = Testing::getRaisedTypes([&]()
        {
        fork.getPaths(development, yields, themes, true);
        });

    // Assert
    EXPECT_TRUE(Testing::hasType(raised, Exception::FMTexc::FMTsame_transitiontargets));
}

// FMTsame_transitiontargets is one of the errors that FMTExceptionHandler::getErrorsToIgnore lists: as a warning, the
// fork adds the proportion of the second target to the path of the first.
TEST(FMTFork, GivenTwoTargetsLeadingToTheSameDevelopmentAsAWarningWhenGetPathsThenAddsTheirProportions)
{
    // Arrange
    const ErrorsAsWarnings warnings({ Exception::FMTexc::FMTsame_transitiontargets });
    const Core::FMTYields yields = getYields();
    const Core::FMTFork fork = getFork({ getTarget("? PEUPLEMENT1 ?", 60.0), getTarget("? PEUPLEMENT1 ?", 40.0) });
    const Core::FMTDevelopment development = getDevelopment("UNITE1 PEUPLEMENT3 UTR1", 20);

    // Act
    const std::vector<Core::FMTDevelopmentPath> paths = fork.getPaths(development, yields, getThemes(), true);

    // Assert
    EXPECT_EQ(describe(paths), std::vector<std::string>({ "UNITE1 PEUPLEMENT1 UTR1|0|0|1|100.000000" }));
}

// The guard against a loop in the graph (Tests/ETAT.md, section 7).
TEST(FMTFork, GivenATargetThatChangesNothingWithoutAgeResetWhenGetPathsThenRaisesASourceToTargetTransition)
{
    // Arrange: *TARGET ? ? ? 100, for an action without age reset
    const Core::FMTYields yields = getYields();
    const Core::FMTFork fork = getFork({ getTarget("? ? ?", 100.0) });
    const Core::FMTDevelopment development = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 20);
    const std::vector<Core::FMTTheme> themes = getThemes();

    // Act
    const std::vector<Exception::FMTexc> raised = Testing::getRaisedTypes([&]()
        {
        fork.getPaths(development, yields, themes, false);
        });

    // Assert
    EXPECT_TRUE(Testing::hasType(raised, Exception::FMTexc::FMTsourcetotarget_transition));
}

// As a warning, the development the target leads to is locked one level more than its source, so that it differs from
// it.
TEST(FMTFork, GivenATargetThatChangesNothingAsAWarningWhenGetPathsThenLocksTheDevelopmentOneLevelMore)
{
    // Arrange
    const ErrorsAsWarnings warnings({ Exception::FMTexc::FMTsourcetotarget_transition });
    const Core::FMTYields yields = getYields();
    const Core::FMTFork fork = getFork({ getTarget("? ? ?", 100.0) });
    const Core::FMTDevelopment development = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 20);

    // Act
    const std::vector<Core::FMTDevelopmentPath> paths = fork.getPaths(development, yields, getThemes(), false);

    // Assert
    EXPECT_EQ(describe(paths), std::vector<std::string>({ "UNITE1 PEUPLEMENT1 UTR1|20|1|1|100.000000" }));
}

// The guard looks at the actions without age reset only: with an age reset, a development of age 0 leads to itself.
TEST(FMTFork, GivenAnAgeResetWhenGetPathsOfADevelopmentOfAgeZeroThenLeadsToTheSameDevelopment)
{
    // Arrange: *TARGET ? ? ? 100, for an action of age reset
    const Core::FMTYields yields = getYields();
    const Core::FMTFork fork = getFork({ getTarget("? ? ?", 100.0) });
    const Core::FMTDevelopment development = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 0);

    // Act
    const std::vector<Core::FMTDevelopmentPath> paths = fork.getPaths(development, yields, getThemes(), true);

    // Assert
    EXPECT_EQ(describe(paths), std::vector<std::string>({ "UNITE1 PEUPLEMENT1 UTR1|0|0|1|100.000000" }));
}

TEST(FMTFork, GivenTargetsWhenSingleThenKeepsTheTargetOfTheLargestProportionAt100AndTheCondition)
{
    // Arrange
    Core::FMTFork fork = getFork({ getTarget("? PEUPLEMENT1 ?", 30.0), getTarget("? PEUPLEMENT2 ?", 70.0) });
    addOldStandsCondition(fork);

    // Act
    const Core::FMTFork single = fork.single();

    // Assert
    ASSERT_EQ(single.size(), 1u);
    EXPECT_EQ(std::string(single.getMaskTrans().at(0).getMask()), "? PEUPLEMENT2 ?");
    EXPECT_DOUBLE_EQ(single.getMaskTrans().at(0).getProportion(), 100.0);
    EXPECT_FALSE(single.emptyAge());
}

TEST(FMTFork, GivenTargetsOfTheSameProportionWhenSingleThenKeepsTheFirst)
{
    // Arrange
    const Core::FMTFork fork = getFork({ getTarget("? PEUPLEMENT1 ?", 50.0), getTarget("? PEUPLEMENT2 ?", 50.0) });

    // Act
    const Core::FMTFork single = fork.single();

    // Assert
    ASSERT_EQ(single.size(), 1u);
    EXPECT_EQ(std::string(single.getMaskTrans().at(0).getMask()), "? PEUPLEMENT1 ?");
}

// Include/FMTFork.h documents the development of the largest proportion; getMax never updates the largest proportion it
// has seen, and returns the development of the last target of a positive proportion. Nothing in FMT calls it
// (Tests/ETAT.md, section 5.15).
TEST(FMTFork, GivenTargetsWhenGetMaxThenReturnsTheDevelopmentOfTheLastTargetOfAPositiveProportion)
{
    // Arrange: *TARGET ? peuplement1 ? 70, *TARGET ? peuplement2 ? 30
    const Core::FMTYields yields = getYields();
    const Core::FMTFork fork = getFork({ getTarget("? PEUPLEMENT1 ?", 70.0), getTarget("? PEUPLEMENT2 ?", 30.0) });
    const Core::FMTDevelopment development = getDevelopment("UNITE1 PEUPLEMENT3 UTR1", 20);

    // Act
    const Core::FMTDevelopment largest = fork.getMax(development, yields, getThemes(), true);

    // Assert
    EXPECT_EQ(std::string(largest.getMask()), "UNITE1 PEUPLEMENT2 UTR1");
}

TEST(FMTFork, GivenForksWhenComparedThenTheyAreEqualOnlyWithTheSameConditionAndTargets)
{
    // Arrange
    Core::FMTFork fork = getFork({ getTarget("? PEUPLEMENT1 ?", 100.0) });
    addOldStandsCondition(fork);
    Core::FMTFork otherTarget = getFork({ getTarget("? PEUPLEMENT2 ?", 100.0) });
    addOldStandsCondition(otherTarget);
    const Core::FMTFork withoutCondition = getFork({ getTarget("? PEUPLEMENT1 ?", 100.0) });

    // Act
    const Core::FMTFork copy(fork);
    Core::FMTFork assigned;
    assigned = fork;
    const bool sameOtherTarget = (fork == otherTarget);
    const bool sameWithoutCondition = (fork == withoutCondition);

    // Assert
    EXPECT_TRUE(copy == fork);
    EXPECT_TRUE(assigned == fork);
    EXPECT_FALSE(sameOtherTarget);
    EXPECT_FALSE(sameWithoutCondition);
}

TEST(FMTFork, GivenAForkWhenConvertedToStringThenWritesItsConditionThenItsTargets)
{
    // Arrange: @AGE(10.._MAXAGE), *TARGET ? peuplement1 ? 60, *TARGET ? peuplement2 ? 40
    Core::FMTFork fork = getFork({ getTarget("? PEUPLEMENT1 ?", 60.0), getTarget("? PEUPLEMENT2 ?", 40.0) });
    addOldStandsCondition(fork);

    // Act
    const std::string written = std::string(fork);

    // Assert
    EXPECT_EQ(written, " @AGE(10.._MAXAGE) \n*TARGET ? PEUPLEMENT1 ? 60.000000 \n*TARGET ? PEUPLEMENT2 ? 40.000000 \n");
}

TEST(FMTFork, GivenAPreSolveFilterWhenPreSolveThenPresolvesTheMaskOfEachTarget)
{
    // Arrange
    const PreSolve preSolve = getPreSolve();
    const Core::FMTFork fork = getFork({ getTarget("? PEUPLEMENT1 ?", 60.0), getTarget("? ? ?", 40.0) });

    // Act
    const Core::FMTFork presolved = fork.preSolve(preSolve.filter, preSolve.themes);

    // Assert
    ASSERT_EQ(presolved.size(), 2u);
    EXPECT_EQ(std::string(presolved.getMaskTrans().at(0).getMask()), "PEUPLEMENT1");
    EXPECT_EQ(std::string(presolved.getMaskTrans().at(1).getMask()), "?");
    EXPECT_DOUBLE_EQ(presolved.sumProp(), 100.0);
    EXPECT_EQ(std::string(fork.getMaskTrans().at(0).getMask()), "? PEUPLEMENT1 ?");
}
