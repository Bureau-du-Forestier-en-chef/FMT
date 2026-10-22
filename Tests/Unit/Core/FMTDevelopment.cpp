/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTDevelopment.h"
#include "FMTAction.h"
#include "FMTAgeBounds.h"
#include "FMTAgeYieldHandler.h"
#include "FMTDevelopmentPath.h"
#include "FMTFork.h"
#include "FMTFuturDevelopment.h"
#include "FMTLockBounds.h"
#include "FMTMask.h"
#include "FMTPerBounds.h"
#include "FMTSpec.h"
#include "FMTTheme.h"
#include "FMTTransition.h"
#include "FMTTransitionMask.h"
#include "FMTYieldRequest.h"
#include "FMTYields.h"
#include "FMTYldBounds.h"
#include "FMTutility.h"

#include "ExceptionChain.h"

#include <gtest/gtest.h>

#include <boost/functional/hash.hpp>
#include <boost/unordered_set.hpp>

#include <limits>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

// A development of TWD_land, with the actions and the transitions of TWD_land built in code as FMTActionParser,
// FMTTransitionParser and FMTModel build them: a spec or a fork per mask, then update, which shrinks the masks and
// sets the age and period bounds that operable tests first. The themes are those of Tests/Unit/Core/FMTTheme.cpp,
// and VOLUMETOTAL that of peuplement1 for every development. A yield request keeps the address of its development,
// which must outlive it (Tests/ETAT.md, section 7).

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

    Core::FMTSpec getAgeSpec(int p_lower, int p_upper)
    {
        Core::FMTSpec spec;
        spec.addBounds(Core::FMTAgeBounds(Core::FMTsection::Action, p_upper, p_lower));
        return spec;
    }

    // An action of one operability condition: *ACTION <name> <Y or N> [_LOCKEXEMPT], *OPERABLE <name>, <mask> <spec>.
    Core::FMTAction getAction(const std::string& p_name, bool p_respectLock, bool p_resetAge, const std::string& p_mask,
        const Core::FMTSpec& p_spec)
    {
        Core::FMTAction action(p_name, p_respectLock, p_resetAge);
        action.push_back(getMask(p_mask), p_spec);
        action.update();
        return action;
    }

    // *ACTION aFire N: ? prod ? _AGE >= 1
    Core::FMTAction getFire()
    {
        return getAction("AFIRE", true, false, "? PROD ?", getAgeSpec(1, NO_UPPER_BOUND));
    }

    // *ACTION COUPEtotale Y: ? prod ? volumetotal >= 100
    Core::FMTAction getClearcut()
    {
        Core::FMTSpec spec;
        spec.addBounds(Core::FMTYldBounds(Core::FMTsection::Action, "VOLUMETOTAL", (std::numeric_limits<double>::max)(),
            100.0));
        return getAction("COUPETOTALE", true, true, "? PROD ?", spec);
    }

    // *ACTION aCaribou N: UC prod ? _AGE >= 8
    Core::FMTAction getCaribou()
    {
        return getAction("ACARIBOU", true, false, "UC PROD ?", getAgeSpec(8, NO_UPPER_BOUND));
    }

    // *ACTION plantation Y _LOCKEXEMPT: ? prod ? _AGE >= 1 AND _AGE <= 3
    Core::FMTAction getPlantation()
    {
        return getAction("PLANTATION", false, true, "? PROD ?", getAgeSpec(1, 3));
    }

    // A target of a transition: *TARGET <mask> <proportion>.
    Core::FMTTransitionMask getTarget(const std::string& p_mask, double p_proportion)
    {
        return Core::FMTTransitionMask(p_mask, getThemes(), p_proportion);
    }

    // A transition of one source: *CASE <name>, *SOURCE <mask>, then the targets.
    Core::FMTTransition getTransition(const std::string& p_name, const std::string& p_source,
        const std::vector<Core::FMTTransitionMask>& p_targets)
    {
        Core::FMTFork fork;
        for (const Core::FMTTransitionMask& target : p_targets)
        {
            fork.add(target);
        }
        Core::FMTTransition transition(p_name);
        transition.push_back(getMask(p_source), fork);
        transition.update();
        return transition;
    }

    // Each development path as <mask>|<age>|<lock>|<period>.
    std::vector<std::string> describe(const std::vector<Core::FMTDevelopmentPath>& p_paths)
    {
        std::vector<std::string> descriptions;
        for (const Core::FMTDevelopmentPath& path : p_paths)
        {
            const Core::FMTDevelopment& development = path.getDevelopment();
            descriptions.push_back(std::string(development.getMask()) + "|" + std::to_string(development.getAge()) + "|"
                + std::to_string(development.getLock()) + "|" + std::to_string(development.getPeriod()));
        }
        return descriptions;
    }
}

TEST(FMTDevelopment, GivenAMaskAnAgeALockAndAPeriodWhenConstructedThenKeepsThem)
{
    // Arrange
    const Core::FMTMask mask = getMask("UNITE1 PEUPLEMENT1 UTR1");

    // Act
    const Core::FMTDevelopment development(mask, 10, 2, 3);

    // Assert
    EXPECT_TRUE(development.getMask() == mask);
    EXPECT_EQ(development.getAge(), 10);
    EXPECT_EQ(development.getShortAge(), 10);
    EXPECT_EQ(development.getLock(), 2);
    EXPECT_EQ(development.getPeriod(), 3);
    EXPECT_EQ(development.getArea(), 0.0);
}

TEST(FMTDevelopment, GivenAMaskAnAgeAndALockWhenConstructedThenStartsAtPeriodZero)
{
    // Arrange
    const Core::FMTMask mask = getMask("UNITE1 PEUPLEMENT1 UTR1");

    // Act
    const Core::FMTDevelopment development(mask, 10, 2);

    // Assert
    EXPECT_EQ(development.getPeriod(), 0);
    EXPECT_EQ(development.getAge(), 10);
}

TEST(FMTDevelopment, GivenADefaultDevelopmentWhenConvertedToStringThenHasNoMaskAndZeroValues)
{
    // Arrange
    const Core::FMTDevelopment development;

    // Act
    const std::string written = std::string(development);

    // Assert
    EXPECT_EQ(written, " 0 0 0 ");
    EXPECT_TRUE(development.getMask().empty());
    EXPECT_EQ(development.getAge(), 0);
    EXPECT_EQ(development.getLock(), 0);
    EXPECT_EQ(development.getPeriod(), 0);
}

TEST(FMTDevelopment, GivenADevelopmentWhenCopiedOrClonedThenTheCopyIsEqual)
{
    // Arrange
    const Core::FMTDevelopment original = getDevelopment("UNITE2 FEU UTR3", 12, 1, 2);

    // Act
    const Core::FMTDevelopment copy(original);
    Core::FMTDevelopment assigned;
    assigned = original;
    const std::unique_ptr<Core::FMTDevelopment> clone = original.clone();

    // Assert
    EXPECT_TRUE(copy == original);
    EXPECT_TRUE(assigned == original);
    ASSERT_NE(clone.get(), nullptr);
    EXPECT_TRUE(*clone == original);
}

TEST(FMTDevelopment, GivenADevelopmentWhenMovedThenTheTargetHoldsItsValues)
{
    // Arrange
    Core::FMTDevelopment original = getDevelopment("UNITE2 FEU UTR3", 12, 1, 2);
    const Core::FMTDevelopment expected(original);

    // Act
    const Core::FMTDevelopment moved(std::move(original));

    // Assert
    EXPECT_TRUE(moved == expected);
}

TEST(FMTDevelopment, GivenDevelopmentsThatDifferByOneValueWhenComparedThenTheyDiffer)
{
    // Arrange
    const Core::FMTDevelopment development = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 10, 1, 2);

    // Act
    const bool sameValues = (development == getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 10, 1, 2));
    const bool otherMask = (development == getDevelopment("UNITE1 PEUPLEMENT2 UTR1", 10, 1, 2));
    const bool otherAge = (development == getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 11, 1, 2));
    const bool otherLock = (development == getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 10, 0, 2));
    const bool otherPeriod = (development == getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 10, 1, 3));
    const bool different = (development != getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 10, 1, 3));

    // Assert
    EXPECT_TRUE(sameValues);
    EXPECT_FALSE(otherMask);
    EXPECT_FALSE(otherAge);
    EXPECT_FALSE(otherLock);
    EXPECT_FALSE(otherPeriod);
    EXPECT_TRUE(different);
}

// The order compares the masks first, then the ages, the locks and the periods.
TEST(FMTDevelopment, GivenDevelopmentsWhenComparedByOrderThenTheMaskComesBeforeTheAgeTheLockAndThePeriod)
{
    // Arrange
    const Core::FMTDevelopment young = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 10, 1, 2);
    const Core::FMTDevelopment old = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 11, 0, 1);
    const Core::FMTDevelopment unlocked = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 10, 0, 2);
    const Core::FMTDevelopment later = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 10, 1, 3);
    const bool youngMaskIsLess = (young.getMask() < getMask("UNITE2 PEUPLEMENT1 UTR1"));
    const Core::FMTDevelopment otherMask = getDevelopment("UNITE2 PEUPLEMENT1 UTR1", 0, 0, 0);

    // Act
    const bool youngBeforeOld = (young < old);
    const bool unlockedBeforeLocked = (unlocked < young);
    const bool earlierBeforeLater = (young < later);
    const bool youngBeforeOtherMask = (young < otherMask);
    const bool youngBeforeItself = (young < young);

    // Assert
    EXPECT_TRUE(youngBeforeOld);
    EXPECT_TRUE(unlockedBeforeLocked);
    EXPECT_TRUE(earlierBeforeLater);
    EXPECT_EQ(youngBeforeOtherMask, youngMaskIsLess);
    EXPECT_FALSE(youngBeforeItself);
}

TEST(FMTDevelopment, GivenEqualDevelopmentsWhenHashThenTheyHaveTheSameHash)
{
    // Arrange
    const Core::FMTDevelopment first = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 10, 1, 2);
    const Core::FMTDevelopment second = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 10, 1, 2);

    // Act
    const size_t firstHash = first.hash();
    const size_t secondHash = boost::hash<Core::FMTDevelopment>()(second);

    // Assert
    EXPECT_EQ(firstHash, secondHash);
}

TEST(FMTDevelopment, GivenDevelopmentsThatDifferByTheirAgeWhenHashThenTheirHashesDiffer)
{
    // Arrange
    const Core::FMTDevelopment young = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 10, 1, 2);
    const Core::FMTDevelopment old = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 11, 1, 2);

    // Act
    const size_t youngHash = young.hash();
    const size_t oldHash = old.hash();

    // Assert
    EXPECT_NE(youngHash, oldHash);
}

TEST(FMTDevelopment, GivenDevelopmentsInAHashedSetWhenFindThenFindsTheEqualDevelopmentOnly)
{
    // Arrange
    boost::unordered_set<Core::FMTDevelopment> developments;
    developments.insert(getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 10, 0, 1));
    developments.insert(getDevelopment("UNITE1 FEU UTR1", 10, 0, 1));

    // Act
    const bool findsEqual = developments.find(getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 10, 0, 1)) != developments.end();
    const bool findsOtherPeriod = developments.find(getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 10, 0, 2)) != developments.end();

    // Assert
    EXPECT_TRUE(findsEqual);
    EXPECT_FALSE(findsOtherPeriod);
}

TEST(FMTDevelopment, GivenADevelopmentWhenConvertedToStringThenWritesItsMaskAgeLockAndPeriod)
{
    // Arrange
    const Core::FMTDevelopment development = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 10, 2, 3);

    // Act
    const std::string written = std::string(development);

    // Assert
    EXPECT_EQ(written, "UNITE1 PEUPLEMENT1 UTR1 10 2 3 ");
}

TEST(FMTDevelopment, GivenADevelopmentWhenSetThenChangesTheValueSetOnly)
{
    // Arrange
    Core::FMTDevelopment development = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 10, 2, 3);

    // Act
    development.setAge(20);
    development.setLock(0);
    development.setPeriod(4);
    development.setMask(getMask("UNITE3 FEU UTR2"));

    // Assert
    EXPECT_TRUE(development == getDevelopment("UNITE3 FEU UTR2", 20, 0, 4));
}

TEST(FMTDevelopment, GivenALockedDevelopmentWhenClearLockThenReturnsTheSameDevelopmentUnlocked)
{
    // Arrange
    const Core::FMTDevelopment development = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 10, 2, 3);

    // Act
    const Core::FMTDevelopment unlocked = development.clearLock();

    // Assert
    EXPECT_TRUE(unlocked == getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 10, 0, 3));
    EXPECT_EQ(development.getLock(), 2);
}

TEST(FMTDevelopment, GivenADevelopmentWhenGrowThenAddsOneToItsAgeAndItsPeriod)
{
    // Arrange
    const Core::FMTDevelopment development = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 10, 0, 1);

    // Act
    const Core::FMTFuturDevelopment grown = development.grow();

    // Assert
    EXPECT_TRUE(grown.getMask() == development.getMask());
    EXPECT_EQ(grown.getAge(), 11);
    EXPECT_EQ(grown.getLock(), 0);
    EXPECT_EQ(grown.getPeriod(), 2);
}

TEST(FMTDevelopment, GivenALockedDevelopmentWhenGrowThenRemovesOneLevelOfLockPerPeriodDownToZero)
{
    // Arrange
    const Core::FMTDevelopment development = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 10, 2, 1);

    // Act
    const Core::FMTFuturDevelopment once = development.grow();
    const Core::FMTFuturDevelopment twice = once.grow();
    const Core::FMTFuturDevelopment thrice = twice.grow();

    // Assert
    EXPECT_EQ(once.getLock(), 1);
    EXPECT_EQ(twice.getLock(), 0);
    EXPECT_EQ(thrice.getLock(), 0);
    EXPECT_EQ(thrice.getAge(), 13);
}

TEST(FMTDevelopment, GivenADevelopmentWhenGetYieldRequestThenTheRequestReadsTheYieldsAtItsAge)
{
    // Arrange
    const Core::FMTYields yields = getYields();
    const Core::FMTDevelopment development = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 7);

    // Act
    const Core::FMTYieldRequest request = development.getYieldRequest();

    // Assert
    EXPECT_EQ(&request.getDevelopment(), &development);
    EXPECT_EQ(request.getVertexGraphInfo(), nullptr);
    EXPECT_DOUBLE_EQ(yields.get(request, "VOLUMETOTAL"), 120.0);
}

TEST(FMTDevelopment, GivenAGrownDevelopmentWhenGetYieldRequestThenReadsTheYieldsAtItsNewAge)
{
    // Arrange
    const Core::FMTYields yields = getYields();
    const Core::FMTFuturDevelopment grown = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 7).grow();

    // Act
    const Core::FMTYieldRequest request = grown.getYieldRequest();

    // Assert
    EXPECT_DOUBLE_EQ(yields.get(request, "VOLUMETOTAL"), 130.0);
    EXPECT_DOUBLE_EQ(grown.getInventoryCoef(yields, "VOLUMETOTAL"), 130.0);
}

TEST(FMTDevelopment, GivenAMinimumAgeWhenOperableThenAllowsTheAgesFromThatAge)
{
    // Arrange
    const Core::FMTYields yields = getYields();
    const Core::FMTAction fire = getFire();

    // Act
    const bool atAgeZero = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 0).operable(fire, yields);
    const bool atAgeOne = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 1).operable(fire, yields);
    const bool atAgeFifty = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 50).operable(fire, yields);

    // Assert
    EXPECT_FALSE(atAgeZero);
    EXPECT_TRUE(atAgeOne);
    EXPECT_TRUE(atAgeFifty);
}

TEST(FMTDevelopment, GivenAnAgeRangeWhenOperableThenAllowsTheAgesOfTheRangeOnly)
{
    // Arrange
    const Core::FMTYields yields = getYields();
    const Core::FMTAction plantation = getPlantation();

    // Act
    const bool atAgeZero = getDevelopment("UNITE1 PEUPLEMENT3 UTR1", 0).operable(plantation, yields);
    const bool atAgeThree = getDevelopment("UNITE1 PEUPLEMENT3 UTR1", 3).operable(plantation, yields);
    const bool atAgeFour = getDevelopment("UNITE1 PEUPLEMENT3 UTR1", 4).operable(plantation, yields);

    // Assert
    EXPECT_FALSE(atAgeZero);
    EXPECT_TRUE(atAgeThree);
    EXPECT_FALSE(atAgeFour);
}

TEST(FMTDevelopment, GivenAMinimumYieldWhenOperableThenAllowsTheAgesWhereTheYieldReachesIt)
{
    // Arrange: VOLUMETOTAL is 80 at age 4 and 100 at age 5
    const Core::FMTYields yields = getYields();
    const Core::FMTAction clearcut = getClearcut();

    // Act
    const bool atAgeFour = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 4).operable(clearcut, yields);
    const bool atAgeFive = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 5).operable(clearcut, yields);

    // Assert
    EXPECT_FALSE(atAgeFour);
    EXPECT_TRUE(atAgeFive);
}

TEST(FMTDevelopment, GivenTheMaskOfAnActionWhenOperableThenRefusesTheDevelopmentsOutsideIt)
{
    // Arrange: UC prod ?
    const Core::FMTYields yields = getYields();
    const Core::FMTAction caribou = getCaribou();

    // Act
    const bool inFirstUnit = getDevelopment("UNITE1 PEUPLEMENT2 UTR2", 8).operable(caribou, yields);
    const bool inThirdUnit = getDevelopment("UNITE3 PEUPLEMENT2 UTR2", 8).operable(caribou, yields);
    const bool burnt = getDevelopment("UNITE1 FEU UTR2", 8).operable(caribou, yields);

    // Assert
    EXPECT_TRUE(inFirstUnit);
    EXPECT_FALSE(inThirdUnit);
    EXPECT_FALSE(burnt);
}

TEST(FMTDevelopment, GivenALockedDevelopmentWhenOperableThenAnActionThatRespectsTheLockRefusesIt)
{
    // Arrange
    const Core::FMTYields yields = getYields();
    const Core::FMTAction fire = getFire();

    // Act
    const bool unlocked = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 5, 0).operable(fire, yields);
    const bool locked = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 5, 1).operable(fire, yields);

    // Assert
    EXPECT_TRUE(unlocked);
    EXPECT_FALSE(locked);
}

TEST(FMTDevelopment, GivenALockedDevelopmentWhenOperableThenALockExemptActionAllowsIt)
{
    // Arrange
    const Core::FMTYields yields = getYields();
    const Core::FMTAction plantation = getPlantation();

    // Act
    const bool locked = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 2, 2).operable(plantation, yields);

    // Assert
    EXPECT_FALSE(plantation.doRespectLock());
    EXPECT_TRUE(locked);
}

TEST(FMTDevelopment, GivenAPeriodRangeWhenOperableThenAllowsTheDevelopmentsOfItsPeriodsOnly)
{
    // Arrange: ? ? ? _CP >= 2 AND _CP <= 3
    const Core::FMTYields yields = getYields();
    Core::FMTSpec spec;
    spec.setBounds(Core::FMTPerBounds(Core::FMTsection::Action, 3, 2));
    const Core::FMTAction action = getAction("APERIOD", true, false, "? ? ?", spec);

    // Act
    const bool atPeriodOne = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 10, 0, 1).operable(action, yields);
    const bool atPeriodTwo = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 10, 0, 2).operable(action, yields);
    const bool atPeriodThree = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 10, 0, 3).operable(action, yields);
    const bool atPeriodFour = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 10, 0, 4).operable(action, yields);

    // Assert
    EXPECT_FALSE(atPeriodOne);
    EXPECT_TRUE(atPeriodTwo);
    EXPECT_TRUE(atPeriodThree);
    EXPECT_FALSE(atPeriodFour);
}

TEST(FMTDevelopment, GivenSeveralActionsWhenAnyOperableThenTellsWhetherOneOfThemIsOperable)
{
    // Arrange
    const Core::FMTYields yields = getYields();
    const Core::FMTAction fire = getFire();
    const Core::FMTAction clearcut = getClearcut();
    const std::vector<const Core::FMTAction*> actions = { &clearcut, &fire };

    // Act
    const bool atAgeZero = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 0).anyOperable(actions, yields);
    const bool atAgeOne = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 1).anyOperable(actions, yields);

    // Assert
    EXPECT_FALSE(atAgeZero);
    EXPECT_TRUE(atAgeOne);
}

// worthTestingOperability reads the lock and the age and period bounds that update sets on the action, not its
// yields: a clearcut, without age bound, is worth testing at age 0, where VOLUMETOTAL makes it not operable.
TEST(FMTDevelopment, GivenAnActionWhenWorthTestingOperabilityThenChecksItsLockAgeAndPeriodBoundsOnly)
{
    // Arrange
    const Core::FMTYields yields = getYields();
    const Core::FMTAction fire = getFire();
    const Core::FMTAction clearcut = getClearcut();
    const Core::FMTDevelopment seedling = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 0);
    const Core::FMTDevelopment locked = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 10, 1);

    // Act
    const bool fireOnSeedling = seedling.worthTestingOperability(fire);
    const bool clearcutOnSeedling = seedling.worthTestingOperability(clearcut);
    const bool fireOnLocked = locked.worthTestingOperability(fire);

    // Assert
    EXPECT_FALSE(fireOnSeedling);
    EXPECT_TRUE(clearcutOnSeedling);
    EXPECT_FALSE(seedling.operable(clearcut, yields));
    EXPECT_FALSE(fireOnLocked);
}

TEST(FMTDevelopment, GivenActionsWhenAnyWorthTestingOperabilityThenReturnsThePositionsOfThoseWorthTesting)
{
    // Arrange: aFire (_AGE >= 1), COUPEtotale (no age bound) and plantation (_AGE >= 1 AND _AGE <= 3)
    const std::vector<Core::FMTAction> allActions = { getFire(), getClearcut(), getPlantation() };
    const std::vector<const Core::FMTAction*> actions = { &allActions.at(0), &allActions.at(1), &allActions.at(2) };
    const Core::FMTDevelopment development = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 5);

    // Act
    const std::vector<int> positions = development.anyWorthTestingOperability(actions, allActions.at(0));
    const std::vector<bool> worth = development.isAnyWorthTestingOperability(actions, allActions);

    // Assert
    EXPECT_EQ(positions, std::vector<int>({ 0, 1 }));
    EXPECT_EQ(worth, std::vector<bool>({ true, true, false }));
}

TEST(FMTDevelopment, GivenADevelopmentBelowTheMinimumAgeWhenGetNonOperabilityInfoThenNamesTheActionAndTheAgeBound)
{
    // Arrange
    const Core::FMTYields yields = getYields();
    const Core::FMTAction fire = getFire();

    // Act
    const std::string tooYoung = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 0).getNonOperabilityInfo(fire, yields, nullptr);
    const std::string operable = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 1).getNonOperabilityInfo(fire, yields, nullptr);

    // Assert
    EXPECT_EQ(tooYoung, "For action AFIRE\nAge of 0 vs Lower bound of 1\n");
    EXPECT_EQ(operable, "");
}

TEST(FMTDevelopment, GivenADevelopmentBelowTheMinimumYieldWhenGetNonOperabilityInfoThenGivesTheValueOfTheYield)
{
    // Arrange
    const Core::FMTYields yields = getYields();
    const Core::FMTAction clearcut = getClearcut();

    // Act
    const std::string info = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 4).getNonOperabilityInfo(clearcut, yields, nullptr);

    // Assert
    EXPECT_EQ(info, "For action COUPETOTALE\nWith Yields values of \nVOLUMETOTAL:80.000000");
}

TEST(FMTDevelopment, GivenASpecWhenIsThenChecksTheDevelopmentAgainstItsAgeAndYieldBounds)
{
    // Arrange: _AGE <= 10 AND VOLUMETOTAL >= 100
    const Core::FMTYields yields = getYields();
    Core::FMTSpec spec = getAgeSpec(0, 10);
    spec.addBounds(Core::FMTYldBounds(Core::FMTsection::Action, "VOLUMETOTAL", (std::numeric_limits<double>::max)(), 100.0));

    // Act
    const bool atAgeFour = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 4).is(spec, yields);
    const bool atAgeFive = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 5).is(spec, yields);
    const bool atAgeEleven = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 11).is(spec, yields);

    // Assert
    EXPECT_FALSE(atAgeFour);
    EXPECT_TRUE(atAgeFive);
    EXPECT_FALSE(atAgeEleven);
}

TEST(FMTDevelopment, GivenAnAgeResetActionWhenOperateThenLeadsToTheSameMaskAtAgeZero)
{
    // Arrange: *CASE Coupetotale, *SOURCE ? ? ?, *TARGET ? ? ? 100.0
    const Core::FMTYields yields = getYields();
    const Core::FMTAction clearcut = getClearcut();
    const Core::FMTTransition transition = getTransition("COUPETOTALE", "? ? ?", { getTarget("? ? ?", 100.0) });
    const Core::FMTDevelopment development = getDevelopment("UNITE2 PEUPLEMENT2 UTR3", 20, 0, 2);

    // Act
    const std::vector<Core::FMTDevelopmentPath> paths = development.operate(clearcut, transition, yields, getThemes());

    // Assert
    EXPECT_EQ(describe(paths), std::vector<std::string>({ "UNITE2 PEUPLEMENT2 UTR3|0|0|2" }));
    ASSERT_EQ(paths.size(), 1u);
    EXPECT_DOUBLE_EQ(paths.at(0).getProportion(), 100.0);
}

TEST(FMTDevelopment, GivenATargetAttributeWhenOperateThenChangesThatAttributeOnly)
{
    // Arrange: *CASE plantation, *SOURCE ? ? ?, *TARGET ? peuplement1 ? 100.00
    const Core::FMTYields yields = getYields();
    const Core::FMTAction plantation = getPlantation();
    const Core::FMTTransition transition = getTransition("PLANTATION", "? ? ?", { getTarget("? PEUPLEMENT1 ?", 100.0) });
    const Core::FMTDevelopment development = getDevelopment("UNITE2 PEUPLEMENT3 UTR2", 2, 0, 1);

    // Act
    const std::vector<Core::FMTDevelopmentPath> paths = development.operate(plantation, transition, yields, getThemes());

    // Assert
    EXPECT_EQ(describe(paths), std::vector<std::string>({ "UNITE2 PEUPLEMENT1 UTR2|0|0|1" }));
}

TEST(FMTDevelopment, GivenAnActionWithoutAgeResetWhenOperateThenKeepsTheAgeOfTheDevelopment)
{
    // Arrange: *CASE aFire, *SOURCE ? ? ?, *TARGET ? feu ? 100.00
    const Core::FMTYields yields = getYields();
    const Core::FMTAction fire = getFire();
    const Core::FMTTransition transition = getTransition("AFIRE", "? ? ?", { getTarget("? FEU ?", 100.0) });
    const Core::FMTDevelopment development = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 12, 0, 1);

    // Act
    const std::vector<Core::FMTDevelopmentPath> paths = development.operate(fire, transition, yields, getThemes());

    // Assert
    EXPECT_EQ(describe(paths), std::vector<std::string>({ "UNITE1 FEU UTR1|12|0|1" }));
}

TEST(FMTDevelopment, GivenATargetLockWhenOperateThenLocksTheDevelopmentAndKeepsItsAge)
{
    // Arrange: *CASE aCaribou, *SOURCE ? ? ?, *TARGET ? ? ? 100.00 _LOCK 1
    const Core::FMTYields yields = getYields();
    const Core::FMTAction caribou = getCaribou();
    Core::FMTTransitionMask target = getTarget("? ? ?", 100.0);
    target.addBounds(Core::FMTLockBounds(Core::FMTsection::Transition, Core::FMTkwor::Target, 1, 1));
    const Core::FMTTransition transition = getTransition("ACARIBOU", "? ? ?", { target });
    const Core::FMTDevelopment development = getDevelopment("UNITE1 PEUPLEMENT2 UTR2", 8, 0, 1);

    // Act
    const std::vector<Core::FMTDevelopmentPath> paths = development.operate(caribou, transition, yields, getThemes());

    // Assert
    EXPECT_EQ(describe(paths), std::vector<std::string>({ "UNITE1 PEUPLEMENT2 UTR2|8|1|1" }));
}

TEST(FMTDevelopment, GivenATargetAgeWhenOperateThenSetsThatAge)
{
    // Arrange: *TARGET ? ? ? 100.00 _AGE 5, for an action without age reset
    const Core::FMTYields yields = getYields();
    const Core::FMTAction fire = getFire();
    Core::FMTTransitionMask target = getTarget("? ? ?", 100.0);
    target.addBounds(Core::FMTAgeBounds(Core::FMTsection::Transition, Core::FMTkwor::Target, 5, 5));
    const Core::FMTTransition transition = getTransition("AFIRE", "? ? ?", { target });
    const Core::FMTDevelopment development = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 20, 0, 1);

    // Act
    const std::vector<Core::FMTDevelopmentPath> paths = development.operate(fire, transition, yields, getThemes());

    // Assert
    EXPECT_EQ(describe(paths), std::vector<std::string>({ "UNITE1 PEUPLEMENT1 UTR1|5|0|1" }));
}

TEST(FMTDevelopment, GivenTwoTargetsWhenOperateThenLeadsToOnePathPerTargetWithItsProportion)
{
    // Arrange: *TARGET ? peuplement1 ? 60.0, *TARGET ? peuplement2 ? 40.0
    const Core::FMTYields yields = getYields();
    const Core::FMTAction clearcut = getClearcut();
    const Core::FMTTransition transition = getTransition("COUPETOTALE", "? ? ?",
        { getTarget("? PEUPLEMENT1 ?", 60.0), getTarget("? PEUPLEMENT2 ?", 40.0) });
    const Core::FMTDevelopment development = getDevelopment("UNITE1 PEUPLEMENT3 UTR1", 20, 0, 1);

    // Act
    const std::vector<Core::FMTDevelopmentPath> paths = development.operate(clearcut, transition, yields, getThemes());

    // Assert
    EXPECT_EQ(describe(paths), std::vector<std::string>({ "UNITE1 PEUPLEMENT1 UTR1|0|0|1", "UNITE1 PEUPLEMENT2 UTR1|0|0|1" }));
    ASSERT_EQ(paths.size(), 2u);
    EXPECT_DOUBLE_EQ(paths.at(0).getProportion(), 60.0);
    EXPECT_DOUBLE_EQ(paths.at(1).getProportion(), 40.0);
}

// Two sources of the same mask: the first whose condition holds gives the targets (FMTTransition::getFork).
TEST(FMTDevelopment, GivenTwoSourcesOfTheSameMaskWhenOperateThenTheFirstSourceWhoseConditionHoldsGivesTheTarget)
{
    // Arrange: *SOURCE ? ? ? @AGE(10.._MAXAGE), *TARGET ? peuplement1 ? 100, then *SOURCE ? ? ?, *TARGET ? peuplement2 ? 100
    const Core::FMTYields yields = getYields();
    const Core::FMTAction clearcut = getClearcut();
    Core::FMTFork oldStands;
    oldStands.addBounds(Core::FMTAgeBounds(Core::FMTsection::Transition, Core::FMTkwor::Source, NO_UPPER_BOUND, 10));
    oldStands.add(getTarget("? PEUPLEMENT1 ?", 100.0));
    Core::FMTFork otherStands;
    otherStands.add(getTarget("? PEUPLEMENT2 ?", 100.0));
    Core::FMTTransition transition("COUPETOTALE");
    transition.push_back(getMask("? ? ?"), oldStands);
    transition.push_back(getMask("? ? ?"), otherStands);
    transition.update();
    const Core::FMTDevelopment old = getDevelopment("UNITE1 PEUPLEMENT3 UTR1", 15, 0, 1);
    const Core::FMTDevelopment young = getDevelopment("UNITE1 PEUPLEMENT3 UTR1", 5, 0, 1);

    // Act
    const std::vector<Core::FMTDevelopmentPath> oldPaths = old.operate(clearcut, transition, yields, getThemes());
    const std::vector<Core::FMTDevelopmentPath> youngPaths = young.operate(clearcut, transition, yields, getThemes());

    // Assert
    EXPECT_EQ(describe(oldPaths), std::vector<std::string>({ "UNITE1 PEUPLEMENT1 UTR1|0|0|1" }));
    EXPECT_EQ(describe(youngPaths), std::vector<std::string>({ "UNITE1 PEUPLEMENT2 UTR1|0|0|1" }));
}

TEST(FMTDevelopment, GivenATransitionWithoutASourceForTheDevelopmentWhenOperateThenRaisesAnInvalidTransitionCase)
{
    // Arrange: *CASE arecup, *SOURCE ? feu ?, for a development of peuplement1
    const Core::FMTYields yields = getYields();
    const Core::FMTAction clearcut = getClearcut();
    const Core::FMTTransition transition = getTransition("ARECUP", "? FEU ?", { getTarget("? PEUPLEMENT1 ?", 100.0) });
    const Core::FMTDevelopment development = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 20, 0, 1);
    const std::vector<Core::FMTTheme> themes = getThemes();

    // Act
    const std::vector<Exception::FMTexc> raised = Testing::getRaisedTypes([&]()
        {
        development.operate(clearcut, transition, yields, themes);
        });

    // Assert
    EXPECT_TRUE(Testing::hasType(raised, Exception::FMTexc::FMTinvalid_transition_case));
}

// The guard against a loop in the graph (Tests/ETAT.md, section 7): a target that changes nothing, for an action that
// does not reset the age.
TEST(FMTDevelopment, GivenATargetThatChangesNothingWhenOperateWithoutAgeResetThenRaisesASourceToTargetTransition)
{
    // Arrange: *CASE aFire, *SOURCE ? ? ?, *TARGET ? ? ? 100
    const Core::FMTYields yields = getYields();
    const Core::FMTAction fire = getFire();
    const Core::FMTTransition transition = getTransition("AFIRE", "? ? ?", { getTarget("? ? ?", 100.0) });
    const Core::FMTDevelopment development = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 20, 0, 1);
    const std::vector<Core::FMTTheme> themes = getThemes();

    // Act
    const std::vector<Exception::FMTexc> raised = Testing::getRaisedTypes([&]()
        {
        development.operate(fire, transition, yields, themes);
        });

    // Assert
    EXPECT_TRUE(Testing::hasType(raised, Exception::FMTexc::FMTsourcetotarget_transition));
}

TEST(FMTDevelopment, GivenTwoTargetsLeadingToTheSameDevelopmentWhenOperateThenRaisesSameTransitionTargets)
{
    // Arrange: *TARGET ? peuplement1 ? 60.0, *TARGET ? peuplement1 ? 40.0
    const Core::FMTYields yields = getYields();
    const Core::FMTAction clearcut = getClearcut();
    const Core::FMTTransition transition = getTransition("COUPETOTALE", "? ? ?",
        { getTarget("? PEUPLEMENT1 ?", 60.0), getTarget("? PEUPLEMENT1 ?", 40.0) });
    const Core::FMTDevelopment development = getDevelopment("UNITE1 PEUPLEMENT3 UTR1", 20, 0, 1);
    const std::vector<Core::FMTTheme> themes = getThemes();

    // Act
    const std::vector<Exception::FMTexc> raised = Testing::getRaisedTypes([&]()
        {
        development.operate(clearcut, transition, yields, themes);
        });

    // Assert
    EXPECT_TRUE(Testing::hasType(raised, Exception::FMTexc::FMTsame_transitiontargets));
}

TEST(FMTDevelopment, GivenAnActionThatIsNotPartialWhenGetHarvestCoefThenReturnsTheYieldOfTheDevelopment)
{
    // Arrange
    const Core::FMTYields yields = getYields();
    const Core::FMTAction clearcut = getClearcut();
    const Core::FMTTransition transition = getTransition("COUPETOTALE", "? ? ?", { getTarget("? ? ?", 100.0) });
    const Core::FMTDevelopment development = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 20, 0, 1);
    const std::vector<Core::FMTDevelopmentPath> paths = development.operate(clearcut, transition, yields, getThemes());

    // Act
    const double harvest = development.getHarvestCoef(paths, clearcut, yields, "VOLUMETOTAL");

    // Assert
    EXPECT_DOUBLE_EQ(harvest, 250.0);
    EXPECT_DOUBLE_EQ(development.getInventoryCoef(yields, "VOLUMETOTAL"), 250.0);
}

// A partial action (*PARTIAL) harvests the difference between the yield before and after, weighted by the proportion
// of each path: from 250 at age 20 to 100 at age 5.
TEST(FMTDevelopment, GivenAPartialActionWhenGetHarvestCoefThenWeighsTheYieldRemovedOnEachPathByItsProportion)
{
    // Arrange: *ACTION aThinning N, *PARTIAL aThinning VOLUMETOTAL, *TARGET ? ? ? 100.00 _AGE 5
    const Core::FMTYields yields = getYields();
    Core::FMTAction thinning("ATHINNING", true, false);
    thinning.push_back(getMask("? PROD ?"), getAgeSpec(10, NO_UPPER_BOUND));
    thinning.pushPartials("VOLUMETOTAL");
    thinning.update();
    Core::FMTTransitionMask target = getTarget("? ? ?", 100.0);
    target.addBounds(Core::FMTAgeBounds(Core::FMTsection::Transition, Core::FMTkwor::Target, 5, 5));
    const Core::FMTTransition transition = getTransition("ATHINNING", "? ? ?", { target });
    const Core::FMTDevelopment development = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 20, 0, 1);
    const std::vector<Core::FMTDevelopmentPath> paths = development.operate(thinning, transition, yields, getThemes());

    // Act
    const double harvest = development.getHarvestCoef(paths, thinning, yields, "VOLUMETOTAL");

    // Assert
    EXPECT_TRUE(thinning.partial("VOLUMETOTAL"));
    EXPECT_DOUBLE_EQ(harvest, 150.0);
}
