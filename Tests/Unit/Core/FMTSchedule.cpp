/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTSchedule.h"
#include "FMTAction.h"
#include "FMTAgeBounds.h"
#include "FMTConstraint.h"
#include "FMTDevelopment.h"
#include "FMTMask.h"
#include "FMTMaskFilter.h"
#include "FMTObject.h"
#include "FMTOutput.h"
#include "FMTOutputSource.h"
#include "FMTSpec.h"
#include "FMTTheme.h"
#include "FMTYldBounds.h"
#include "FMTutility.h"

#include <gtest/gtest.h>

#include <boost/dynamic_bitset.hpp>
#include <boost/unordered_set.hpp>

#include <limits>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

// A schedule is the area that each action operates on each development during one period, as a *.seq file gives it
// (FMTScheduleParser) and as FMTSrModel::getSolution returns it. For each development, the vector holds an area by
// lock level: addEvent puts the area of a locked development at its lock, on the unlocked development, unless the
// schedule uses the lock (doUseLock). The themes are those of Tests/Unit/Core/FMTTheme.cpp, and the actions those of
// TWD_land.

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

    Core::FMTAction getAction(const std::string& p_name, bool p_respectLock, bool p_resetAge, int p_lowerAge,
        int p_upperAge)
    {
        Core::FMTAction action(p_name, p_respectLock, p_resetAge);
        Core::FMTSpec spec;
        spec.addBounds(Core::FMTAgeBounds(Core::FMTsection::Action, p_upperAge, p_lowerAge));
        action.push_back(getMask("? PROD ?"), spec);
        action.update();
        return action;
    }

    // *ACTION COUPEtotale Y, ? prod ? (an age condition in place of volumetotal >= 100)
    Core::FMTAction getClearcut()
    {
        return getAction("COUPETOTALE", true, true, 5, NO_UPPER_BOUND);
    }

    // *ACTION plantation Y _LOCKEXEMPT, ? prod ? _AGE >= 1 AND _AGE <= 3
    Core::FMTAction getPlantation()
    {
        return getAction("PLANTATION", false, true, 1, 3);
    }

    // *ACTION aFire N, ? prod ? _AGE >= 1
    Core::FMTAction getFire()
    {
        return getAction("AFIRE", true, false, 1, NO_UPPER_BOUND);
    }

    // A schedule of period 1, without lock, where plantation operates 3 ha of an unlocked development of 2 years and 5
    // ha of the same development locked at level 2.
    Core::FMTSchedule getPlantationSchedule()
    {
        Core::FMTSchedule schedule(1, Core::FMTObject(), false);
        schedule.addEvent(getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 2, 0), 3.0, getPlantation());
        schedule.addEvent(getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 2, 2), 5.0, getPlantation());
        return schedule;
    }

    // The output OVOL, as in Tests/Unit/Core/FMTConstraint.cpp, for a constraint of type p_type from period p_first.
    Core::FMTConstraint getConstraint(Core::FMTconstrainttype p_type, int p_first)
    {
        const Core::FMTOutputSource harvested(Core::FMTSpec(), getMask("? ? ?"), Core::FMTotar::actual, "VOLUMETOTAL",
            "COUPETOTALE");
        Core::FMTConstraint constraint(p_type, Core::FMTOutput("OVOL", "Volume harvested", "", { harvested }, {}));
        constraint.setLength(p_first, NO_UPPER_BOUND);
        return constraint;
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

    // The postsolve filter of FMTModel::getPostSolveFilter for the development p_mask: the bits of the presolved
    // attributes, PEUPLEMENT1 and PEUPLEMENT2 (bits 3 and 4), and the bits of the development in the removed themes.
    Core::FMTMaskFilter getPostSolveFilter(const std::string& p_mask)
    {
        boost::dynamic_bitset<uint8_t> bits(10, false);
        bits[3] = true;
        bits[4] = true;
        const Core::FMTMask selection(bits);
        return Core::FMTMaskFilter(selection, getMask(p_mask).getPostSolveMask(selection, getThemes()));
    }
}

TEST(FMTSchedule, GivenAPeriodWhenConstructedThenIsAnEmptyScheduleOfThatPeriod)
{
    // Arrange
    const Core::FMTObject object;

    // Act
    const Core::FMTSchedule schedule(2, object, false);
    const Core::FMTSchedule unset;

    // Assert
    EXPECT_EQ(schedule.getPeriod(), 2);
    EXPECT_FALSE(schedule.doUseLock());
    EXPECT_TRUE(schedule.empty());
    EXPECT_EQ(schedule.size(), 0u);
    EXPECT_DOUBLE_EQ(schedule.area(), 0.0);
    EXPECT_EQ(unset.getPeriod(), 0);
    EXPECT_TRUE(unset.empty());
}

TEST(FMTSchedule, GivenEventsOfAnActionWhenAddEventThenAddsTheirAreasByDevelopment)
{
    // Arrange
    Core::FMTSchedule schedule(1, Core::FMTObject(), false);
    const Core::FMTAction clearcut = getClearcut();
    const Core::FMTDevelopment first = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 20);
    const Core::FMTDevelopment second = getDevelopment("UNITE2 PEUPLEMENT2 UTR2", 22);

    // Act
    schedule.addEvent(first, 10.0, clearcut);
    schedule.addEvent(first, 10.0, clearcut);
    schedule.addEvent(second, 5.0, clearcut);

    // Assert
    EXPECT_EQ(schedule.size(), 1u);
    EXPECT_EQ(schedule.at(clearcut).at(first), std::vector<double>({ 20.0 }));
    EXPECT_EQ(schedule.at(clearcut).at(second), std::vector<double>({ 5.0 }));
    EXPECT_DOUBLE_EQ(schedule.actionArea(clearcut), 25.0);
    EXPECT_DOUBLE_EQ(schedule.area(), 25.0);
}

TEST(FMTSchedule, GivenLockedDevelopmentsWhenAddEventWithoutLockThenKeepsTheAreaAtTheLockLevelOfTheUnlockedDevelopment)
{
    // Arrange
    const Core::FMTAction plantation = getPlantation();
    const Core::FMTDevelopment unlocked = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 2, 0);

    // Act
    const Core::FMTSchedule schedule = getPlantationSchedule();

    // Assert
    ASSERT_EQ(schedule.at(plantation).size(), 1u);
    EXPECT_EQ(schedule.at(plantation).at(unlocked), std::vector<double>({ 3.0, 0.0, 5.0 }));
}

TEST(FMTSchedule, GivenAScheduleThatUsesTheLockWhenAddEventThenKeepsTheLockedDevelopment)
{
    // Arrange
    Core::FMTSchedule schedule(1, Core::FMTObject(), true);
    const Core::FMTAction plantation = getPlantation();
    const Core::FMTDevelopment locked = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 2, 2);

    // Act
    schedule.addEvent(locked, 5.0, plantation);

    // Assert
    EXPECT_TRUE(schedule.doUseLock());
    EXPECT_EQ(schedule.at(plantation).count(locked), 1u);
    EXPECT_EQ(schedule.at(plantation).count(locked.clearLock()), 0u);
    EXPECT_EQ(schedule.at(plantation).at(locked), std::vector<double>({ 5.0 }));
}

// A lock-exempt action, in a schedule without lock, finds the locked developments through their unlocked development.
TEST(FMTSchedule, GivenEventsWhenOperatedThenFindsTheDevelopmentsOperatedByEachAction)
{
    // Arrange
    Core::FMTSchedule schedule = getPlantationSchedule();
    const Core::FMTAction clearcut = getClearcut();
    schedule.addEvent(getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 20), 10.0, clearcut);

    // Act
    const bool clearcutOperated = schedule.operated(clearcut, getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 20));
    const bool clearcutOtherAge = schedule.operated(clearcut, getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 21));
    const bool fireOperated = schedule.operated(getFire(), getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 20));
    const bool plantationLocked = schedule.operated(getPlantation(), getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 2, 1));

    // Assert
    EXPECT_TRUE(clearcutOperated);
    EXPECT_FALSE(clearcutOtherAge);
    EXPECT_FALSE(fireOperated);
    EXPECT_TRUE(plantationLocked);
}

TEST(FMTSchedule, GivenTwoSchedulesWhenAddedThenSumsTheAreasOfEachDevelopmentByLockLevel)
{
    // Arrange: plantation of 3, 0 and 5 ha plus 1 ha at lock 0, and a clearcut of 4 ha in the second schedule only
    const Core::FMTSchedule first = getPlantationSchedule();
    Core::FMTSchedule second(1, Core::FMTObject(), false);
    second.addEvent(getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 2, 0), 1.0, getPlantation());
    second.addEvent(getDevelopment("UNITE2 PEUPLEMENT2 UTR2", 22), 4.0, getClearcut());
    Core::FMTSchedule added(first);

    // Act
    const Core::FMTSchedule sum = first + second;
    added += second;

    // Assert
    EXPECT_EQ(sum.at(getPlantation()).at(getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 2, 0)),
        std::vector<double>({ 4.0, 0.0, 5.0 }));
    EXPECT_EQ(sum.at(getClearcut()).at(getDevelopment("UNITE2 PEUPLEMENT2 UTR2", 22)), std::vector<double>({ 4.0 }));
    EXPECT_EQ(sum.size(), 2u);
    EXPECT_DOUBLE_EQ(sum.area(), 13.0);
    EXPECT_DOUBLE_EQ(first.area(), 8.0);
    EXPECT_TRUE(added == sum);
}

TEST(FMTSchedule, GivenSchedulesWhenConstructedFromThemThenAddsThemAtTheGivenPeriod)
{
    // Arrange
    Core::FMTSchedule second(1, Core::FMTObject(), false);
    second.addEvent(getDevelopment("UNITE2 PEUPLEMENT2 UTR2", 22), 4.0, getClearcut());
    std::vector<Core::FMTSchedule> schedules = { getPlantationSchedule(), second };
    std::vector<Core::FMTSchedule> none;

    // Act
    const Core::FMTSchedule combined(3, schedules);
    const Core::FMTSchedule empty(3, none);

    // Assert
    EXPECT_EQ(combined.getPeriod(), 3);
    EXPECT_EQ(combined.size(), 2u);
    EXPECT_DOUBLE_EQ(combined.area(), 12.0);
    EXPECT_FALSE(combined.doUseLock());
    EXPECT_EQ(empty.getPeriod(), 3);
    EXPECT_TRUE(empty.empty());
}

// The comparison adds the areas of each development over its lock levels: 3 ha at lock 0 and 2 ha at lock 1 equal 5 ha
// at lock 0.
TEST(FMTSchedule, GivenSchedulesOfTheSameTotalByDevelopmentWhenComparedThenTheyAreEqual)
{
    // Arrange
    Core::FMTSchedule byLock(1, Core::FMTObject(), false);
    byLock.addEvent(getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 2, 0), 3.0, getPlantation());
    byLock.addEvent(getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 2, 1), 2.0, getPlantation());
    Core::FMTSchedule total(1, Core::FMTObject(), false);
    total.addEvent(getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 2, 0), 5.0, getPlantation());
    Core::FMTSchedule otherPeriod(total);
    otherPeriod.setPeriod(2);
    Core::FMTSchedule usingLock(total);
    usingLock.setUseLock(true);
    Core::FMTSchedule otherArea(1, Core::FMTObject(), false);
    otherArea.addEvent(getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 2, 0), 4.0, getPlantation());
    Core::FMTSchedule otherDevelopment(1, Core::FMTObject(), false);
    otherDevelopment.addEvent(getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 3, 0), 5.0, getPlantation());

    // Act
    const bool sameTotal = (byLock == total);
    const bool sameOtherPeriod = (total == otherPeriod);
    const bool sameUsingLock = (total == usingLock);
    const bool sameOtherArea = (total == otherArea);
    const bool sameOtherDevelopment = (total == otherDevelopment);
    const bool differentOtherArea = (total != otherArea);

    // Assert
    EXPECT_TRUE(sameTotal);
    EXPECT_FALSE(sameOtherPeriod);
    EXPECT_FALSE(sameUsingLock);
    EXPECT_FALSE(sameOtherArea);
    EXPECT_FALSE(sameOtherDevelopment);
    EXPECT_TRUE(differentOtherArea);
}

// Without the lock, a line is written for each lock level, without the level.
TEST(FMTSchedule, GivenAScheduleWithoutLockWhenConvertedToStringThenWritesALinePerLockLevelWithoutTheLevel)
{
    // Arrange
    const Core::FMTSchedule schedule = getPlantationSchedule();

    // Act
    const std::string written = std::string(schedule);

    // Assert
    EXPECT_EQ(written, "UNITE1 PEUPLEMENT1 UTR1 2 3.000000 PLANTATION 1\nUNITE1 PEUPLEMENT1 UTR1 2 0.000000 PLANTATION 1\n"
        "UNITE1 PEUPLEMENT1 UTR1 2 5.000000 PLANTATION 1\n");
}

TEST(FMTSchedule, GivenAScheduleThatUsesTheLockWhenConvertedToStringThenWritesTheLockOfEachDevelopment)
{
    // Arrange
    Core::FMTSchedule schedule(1, Core::FMTObject(), true);
    schedule.addEvent(getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 2, 2), 5.0, getPlantation());

    // Act
    const std::string written = std::string(schedule);

    // Assert
    EXPECT_EQ(written, "UNITE1 PEUPLEMENT1 UTR1 2 5.000000 2 PLANTATION 1\n");
}

// After clean, the position of an area is no longer its lock level: FMTSrModel::setSolution spreads the areas of a
// lock-exempt action over the locked developments of the graph, whatever their position.
TEST(FMTSchedule, GivenEmptyAreasWhenCleanThenRemovesThemFromAScheduleWithoutLockOnly)
{
    // Arrange
    Core::FMTSchedule withoutLock = getPlantationSchedule();
    Core::FMTSchedule withLock(1, Core::FMTObject(), true);
    withLock.addEvent(getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 2, 2), 0.0, getPlantation());

    // Act
    withoutLock.clean();
    withLock.clean();

    // Assert
    EXPECT_EQ(withoutLock.at(getPlantation()).at(getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 2, 0)),
        std::vector<double>({ 3.0, 5.0 }));
    EXPECT_EQ(withLock.at(getPlantation()).at(getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 2, 2)),
        std::vector<double>({ 0.0 }));
}

TEST(FMTSchedule, GivenAreasWhenSortThenSortsTheAreasOfEachDevelopment)
{
    // Arrange: 5 ha at lock 0, 3 ha at lock 2
    Core::FMTSchedule schedule(1, Core::FMTObject(), false);
    schedule.addEvent(getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 2, 0), 5.0, getPlantation());
    schedule.addEvent(getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 2, 2), 3.0, getPlantation());

    // Act
    schedule.sort();

    // Assert
    EXPECT_EQ(schedule.at(getPlantation()).at(getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 2, 0)),
        std::vector<double>({ 0.0, 3.0, 5.0 }));
}

TEST(FMTSchedule, GivenAScheduleWhenGetNewScheduleThenMultipliesEachAreaByTheFactor)
{
    // Arrange
    const Core::FMTSchedule schedule = getPlantationSchedule();

    // Act
    const Core::FMTSchedule half = schedule.getNewSchedule(0.5);

    // Assert
    EXPECT_EQ(half.at(getPlantation()).at(getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 2, 0)),
        std::vector<double>({ 1.5, 0.0, 2.5 }));
    EXPECT_DOUBLE_EQ(half.area(), 4.0);
    EXPECT_DOUBLE_EQ(schedule.area(), 8.0);
}

TEST(FMTSchedule, GivenAScheduleWhenSetPeriodThenMovesItAndItsDevelopmentsToThatPeriod)
{
    // Arrange
    Core::FMTSchedule schedule = getPlantationSchedule();

    // Act
    schedule.setPeriod(4);

    // Assert
    EXPECT_EQ(schedule.getPeriod(), 4);
    EXPECT_EQ(schedule.at(getPlantation()).count(getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 2, 0, 4)), 1u);
    EXPECT_EQ(schedule.at(getPlantation()).count(getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 2, 0, 1)), 0u);
}

TEST(FMTSchedule, GivenActionsWhenGetOperabilitiesThenGivesTheDevelopmentsOfEachActionInTheOrderOfTheActions)
{
    // Arrange
    Core::FMTSchedule schedule = getPlantationSchedule();
    schedule.addEvent(getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 20), 10.0, getClearcut());
    schedule.addEvent(getDevelopment("UNITE2 PEUPLEMENT2 UTR2", 22), 4.0, getClearcut());
    const std::vector<Core::FMTAction> actions = { getFire(), getClearcut(), getPlantation() };

    // Act
    const std::vector<boost::unordered_set<Core::FMTDevelopment>> operabilities = schedule.getOperabilities(actions);

    // Assert
    ASSERT_EQ(operabilities.size(), 3u);
    EXPECT_TRUE(operabilities.at(0).empty());
    EXPECT_EQ(operabilities.at(1).size(), 2u);
    EXPECT_EQ(operabilities.at(1).count(getDevelopment("UNITE2 PEUPLEMENT2 UTR2", 22)), 1u);
    EXPECT_EQ(operabilities.at(2).size(), 1u);
}

TEST(FMTSchedule, GivenAnActionOutsideTheScheduleWhenActionAreaAndFindThenGivesNoAreaAndTheEnd)
{
    // Arrange
    const Core::FMTSchedule schedule = getPlantationSchedule();
    const Core::FMTAction fire = getFire();

    // Act
    const double area = schedule.actionArea(fire);
    const bool found = (schedule.find(fire) != schedule.end());

    // Assert
    EXPECT_DOUBLE_EQ(area, 0.0);
    EXPECT_FALSE(found);
}

TEST(FMTSchedule, GivenAnActionOutsideTheScheduleWhenAtThenRaisesAnOutOfRange)
{
    // Arrange
    const Core::FMTSchedule schedule = getPlantationSchedule();
    const Core::FMTAction fire = getFire();

    // Act and Assert
    EXPECT_THROW(schedule.at(fire), std::out_of_range);
}

// The actions are the keys of the schedule, compared by name (FMTAction::operator<): another action of the same name
// finds the areas.
TEST(FMTSchedule, GivenAnActionOfTheSameNameWhenFindThenFindsTheAreasOfTheAction)
{
    // Arrange
    const Core::FMTSchedule schedule = getPlantationSchedule();
    const Core::FMTAction sameName("PLANTATION", true, false);

    // Act
    const Core::FMTSchedule::const_iterator found = schedule.find(sameName);

    // Assert
    ASSERT_TRUE(found != schedule.end());
    EXPECT_FALSE(found->first.doRespectLock());
}

TEST(FMTSchedule, GivenAnActionWhenOperatorBracketsThenAddsItWithoutDevelopment)
{
    // Arrange
    Core::FMTSchedule schedule(1, Core::FMTObject(), false);

    // Act
    const std::map<Core::FMTDevelopment, std::vector<double>>& developments = schedule[getFire()];

    // Assert
    EXPECT_TRUE(developments.empty());
    EXPECT_EQ(schedule.size(), 1u);
    EXPECT_FALSE(schedule.empty());
    EXPECT_DOUBLE_EQ(schedule.area(), 0.0);
}

// The areas follow the order of the locks, without the locks missing: the area of lock 2 comes second.
TEST(FMTSchedule, GivenAreasByLockWhenConstructedFromThemThenKeepsTheAreasInTheOrderOfTheLocks)
{
    // Arrange: 3 ha at lock 0 and 5 ha at lock 2, as FMTScheduleParser reads them
    const Core::FMTDevelopment development = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 2, 0);
    std::map<Core::FMTAction, std::map<Core::FMTDevelopment, std::map<int, double>>> mapping;
    mapping[getPlantation()][development][2] = 5.0;
    mapping[getPlantation()][development][0] = 3.0;

    // Act
    const Core::FMTSchedule schedule(1, mapping);

    // Assert
    EXPECT_EQ(schedule.getPeriod(), 1);
    EXPECT_FALSE(schedule.doUseLock());
    EXPECT_EQ(schedule.at(getPlantation()).at(development), std::vector<double>({ 3.0, 5.0 }));
}

TEST(FMTSchedule, GivenAConstraintAcrossPeriodsFromThePeriodOfTheScheduleWhenIsFuturConstraintsThenReturnsTrue)
{
    // Arrange: _EVEN(OVOL) from period 2, against _EVEN(OVOL) from period 1 and OVOL >= ... from period 2
    const Core::FMTSchedule schedule(2, Core::FMTObject(), false);
    const std::vector<Core::FMTConstraint> fromItsPeriod = {
        getConstraint(Core::FMTconstrainttype::FMTevenflow, 2) };
    const std::vector<Core::FMTConstraint> others = { getConstraint(Core::FMTconstrainttype::FMTevenflow, 1),
        getConstraint(Core::FMTconstrainttype::FMTstandard, 2) };

    // Act
    const bool withItsPeriod = schedule.isFuturConstraints(fromItsPeriod);
    const bool withOthers = schedule.isFuturConstraints(others);
    const bool withoutConstraint = schedule.isFuturConstraints({});

    // Assert
    EXPECT_TRUE(withItsPeriod);
    EXPECT_FALSE(withOthers);
    EXPECT_FALSE(withoutConstraint);
}

// UNITE1 PEUPLEMENT3 UTR1 cannot be presolved, the presolve keeping peuplement1 and peuplement2 only: its areas are
// dropped with a warning (FMTignore).
TEST(FMTSchedule, GivenAPreSolveFilterWhenPreSolveThenPresolvesTheDevelopmentsAndDropsThoseThatCannotBe)
{
    // Arrange
    const PreSolve preSolve = getPreSolve();
    Core::FMTSchedule schedule(1, Core::FMTObject(), false);
    schedule.addEvent(getDevelopment("UNITE1 PEUPLEMENT2 UTR1", 20), 10.0, getClearcut());
    schedule.addEvent(getDevelopment("UNITE1 PEUPLEMENT3 UTR1", 20), 5.0, getClearcut());
    const std::vector<Core::FMTAction> presolvedActions = { getClearcut() };

    // Act
    const Core::FMTSchedule presolved = schedule.preSolve(preSolve.filter, preSolve.themes, presolvedActions);

    // Assert
    ASSERT_EQ(presolved.size(), 1u);
    const std::map<Core::FMTDevelopment, std::vector<double>>& developments = presolved.at(getClearcut());
    ASSERT_EQ(developments.size(), 1u);
    EXPECT_EQ(std::string(developments.begin()->first.getMask()), "PEUPLEMENT2");
    EXPECT_EQ(developments.begin()->second, std::vector<double>({ 10.0 }));
}

TEST(FMTSchedule, GivenAnActionThatThePreSolveRemovedWhenPreSolveThenDropsItsAreas)
{
    // Arrange
    const PreSolve preSolve = getPreSolve();
    Core::FMTSchedule schedule(1, Core::FMTObject(), false);
    schedule.addEvent(getDevelopment("UNITE1 PEUPLEMENT2 UTR1", 20), 10.0, getClearcut());

    // Act
    const Core::FMTSchedule presolved = schedule.preSolve(preSolve.filter, preSolve.themes, { getFire() });

    // Assert
    EXPECT_TRUE(presolved.empty());
    EXPECT_EQ(presolved.getPeriod(), 1);
}

TEST(FMTSchedule, GivenAPresolvedScheduleWhenPostSolveThenGivesBackTheScheduleBeforeThePresolve)
{
    // Arrange
    const PreSolve preSolve = getPreSolve();
    Core::FMTSchedule schedule(1, Core::FMTObject(), false);
    schedule.addEvent(getDevelopment("UNITE1 PEUPLEMENT2 UTR1", 20), 10.0, getClearcut());
    schedule.addEvent(getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 20), 5.0, getClearcut());
    const std::vector<Core::FMTAction> actions = { getClearcut() };
    const Core::FMTSchedule presolved = schedule.preSolve(preSolve.filter, preSolve.themes, actions);

    // Act
    const Core::FMTSchedule postsolved = presolved.postSolve(getPostSolveFilter("UNITE1 PEUPLEMENT2 UTR1"), getThemes(),
        actions);

    // Assert
    EXPECT_TRUE(postsolved == schedule);
    EXPECT_EQ(postsolved.at(getClearcut()).count(getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 20)), 1u);
}
