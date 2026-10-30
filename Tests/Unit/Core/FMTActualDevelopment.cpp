/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTActualDevelopment.h"
#include "FMTDevelopment.h"
#include "FMTLifespans.h"
#include "FMTMask.h"
#include "FMTMaskFilter.h"
#include "FMTObject.h"
#include "FMTTheme.h"

#include "ExceptionChain.h"

#include <gtest/gtest.h>

#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

// An actual development is a line of the area section: *A <mask> <age> <area> [_LOCK <lock>], at period 0. The
// themes are those of Tests/Unit/Core/FMTTheme.cpp, and the lifespans those of TWD_land (? feu ? 0, ? ? ? 80).

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

    // *A <mask> <age> <area> _LOCK <lock>
    Core::FMTActualDevelopment getActual(const std::string& p_mask, int p_age, double p_area, int p_lock = 0)
    {
        return Core::FMTActualDevelopment(getMask(p_mask), p_age, p_lock, p_area);
    }

    // The lifespan section of TWD_land, as FMTLifespansParser and FMTModel build it.
    Core::FMTLifespans getLifespans()
    {
        Core::FMTLifespans lifespans;
        lifespans.push_back(getMask("? FEU ?"), 0);
        lifespans.push_back(getMask("? ? ?"), 80);
        lifespans.update();
        return lifespans;
    }

    // The mask, age, lock and period of a development as <mask>|<age>|<lock>|<period>.
    std::string describe(const Core::FMTDevelopment& p_development)
    {
        return std::string(p_development.getMask()) + "|" + std::to_string(p_development.getAge()) + "|"
            + std::to_string(p_development.getLock()) + "|" + std::to_string(p_development.getPeriod());
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

TEST(FMTActualDevelopment, GivenAMaskAnAgeALockAndAnAreaWhenConstructedThenKeepsThemAtPeriodZero)
{
    // Arrange
    const Core::FMTMask mask = getMask("UNITE1 PEUPLEMENT1 UTR1");

    // Act
    const Core::FMTActualDevelopment development(mask, 12, 2, 403.28);

    // Assert
    EXPECT_TRUE(development.getMask() == mask);
    EXPECT_EQ(development.getAge(), 12);
    EXPECT_EQ(development.getLock(), 2);
    EXPECT_EQ(development.getPeriod(), 0);
    EXPECT_DOUBLE_EQ(development.getArea(), 403.28);
}

TEST(FMTActualDevelopment, GivenADevelopmentAndAnAreaWhenConstructedThenKeepsTheDevelopmentAndTheArea)
{
    // Arrange: a development of the graph at period 3, as FMTSrModel::getArea builds its actual developments
    const Core::FMTDevelopment development(getMask("UNITE2 PEUPLEMENT2 UTR3"), 19, 1, 3);

    // Act
    const Core::FMTActualDevelopment actual(development, 50.5);

    // Assert
    EXPECT_EQ(describe(actual), "UNITE2 PEUPLEMENT2 UTR3|19|1|3");
    EXPECT_DOUBLE_EQ(actual.getArea(), 50.5);
}

TEST(FMTActualDevelopment, GivenADefaultActualDevelopmentWhenConstructedThenHasNoArea)
{
    // Arrange and Act
    const Core::FMTActualDevelopment development;

    // Assert
    EXPECT_DOUBLE_EQ(development.getArea(), 0.0);
    EXPECT_TRUE(development.getMask().empty());
}

TEST(FMTActualDevelopment, GivenAnActualDevelopmentWhenCopiedAssignedOrClonedThenKeepsItsArea)
{
    // Arrange
    const Core::FMTActualDevelopment original = getActual("UNITE1 PEUPLEMENT1 UTR1", 12, 403.28, 1);

    // Act
    const Core::FMTActualDevelopment copy(original);
    Core::FMTActualDevelopment assigned;
    assigned = original;
    const std::unique_ptr<Core::FMTDevelopment> clone = original.clone();

    // Assert
    EXPECT_TRUE(copy == original);
    EXPECT_TRUE(assigned == original);
    ASSERT_NE(clone.get(), nullptr);
    EXPECT_NE(dynamic_cast<const Core::FMTActualDevelopment*>(clone.get()), nullptr);
    EXPECT_DOUBLE_EQ(clone->getArea(), 403.28);
}

TEST(FMTActualDevelopment, GivenAnActualDevelopmentWhenMovedThenTheTargetHoldsItsValues)
{
    // Arrange
    Core::FMTActualDevelopment constructed = getActual("UNITE1 PEUPLEMENT1 UTR1", 12, 403.28, 1);
    Core::FMTActualDevelopment assigned = getActual("UNITE1 PEUPLEMENT1 UTR1", 12, 403.28, 1);
    const Core::FMTActualDevelopment expected(constructed);

    // Act
    const Core::FMTActualDevelopment moved(std::move(constructed));
    Core::FMTActualDevelopment moveAssigned;
    moveAssigned = std::move(assigned);

    // Assert
    EXPECT_TRUE(moved == expected);
    EXPECT_TRUE(moveAssigned == expected);
}

TEST(FMTActualDevelopment, GivenActualDevelopmentsThatDifferByTheirAreaOnlyWhenComparedThenTheyDiffer)
{
    // Arrange
    const Core::FMTActualDevelopment development = getActual("UNITE1 PEUPLEMENT1 UTR1", 12, 403.28);
    const Core::FMTActualDevelopment otherArea = getActual("UNITE1 PEUPLEMENT1 UTR1", 12, 201.64);

    // Act
    const bool same = (development == otherArea);
    const bool different = (development != otherArea);
    const bool sameDevelopment = (static_cast<const Core::FMTDevelopment&>(development) == otherArea);

    // Assert
    EXPECT_FALSE(same);
    EXPECT_TRUE(different);
    EXPECT_TRUE(sameDevelopment);
}

TEST(FMTActualDevelopment, GivenAnEarlierDevelopmentOfASmallerAreaWhenComparedByOrderThenComesFirst)
{
    // Arrange
    const Core::FMTActualDevelopment young = getActual("UNITE1 PEUPLEMENT1 UTR1", 10, 5.0);
    const Core::FMTActualDevelopment old = getActual("UNITE1 PEUPLEMENT1 UTR1", 11, 6.0);

    // Act
    const bool youngFirst = (young < old);
    const bool oldFirst = (old < young);

    // Assert
    EXPECT_TRUE(youngFirst);
    EXPECT_FALSE(oldFirst);
}

// The order needs both the development and the area to be smaller: an earlier development of a larger area is neither
// before nor after the other, although they differ. It is not a strict weak ordering, which std::sort requires of the
// order of FMTSrModel::getArea (Tests/ETAT.md, section 5.15).
TEST(FMTActualDevelopment, GivenAnEarlierDevelopmentOfALargerAreaWhenComparedByOrderThenNeitherComesFirst)
{
    // Arrange
    const Core::FMTActualDevelopment young = getActual("UNITE1 PEUPLEMENT1 UTR1", 10, 6.0);
    const Core::FMTActualDevelopment old = getActual("UNITE1 PEUPLEMENT1 UTR1", 11, 5.0);

    // Act
    const bool youngFirst = (young < old);
    const bool oldFirst = (old < young);

    // Assert
    EXPECT_FALSE(youngFirst);
    EXPECT_FALSE(oldFirst);
    EXPECT_TRUE(young != old);
}

TEST(FMTActualDevelopment, GivenAnActualDevelopmentWhenConvertedToStringThenWritesALineOfTheAreaSection)
{
    // Arrange
    const Core::FMTActualDevelopment development = getActual("UNITE1 PEUPLEMENT1 UTR1", 12, 403.28);

    // Act
    const std::string written = std::string(development);

    // Assert
    EXPECT_EQ(written, "*A UNITE1 PEUPLEMENT1 UTR1 12 403.280000");
}

TEST(FMTActualDevelopment, GivenALockedActualDevelopmentWhenConvertedToStringThenWritesItsLock)
{
    // Arrange
    const Core::FMTActualDevelopment development = getActual("UNITE1 PEUPLEMENT1 UTR1", 12, 403.28, 2);

    // Act
    const std::string written = std::string(development);

    // Assert
    EXPECT_EQ(written, "*A UNITE1 PEUPLEMENT1 UTR1 12 403.280000 _lock 2");
}

TEST(FMTActualDevelopment, GivenAnActualDevelopmentWhenSetAreaThenChangesItsAreaOnly)
{
    // Arrange
    Core::FMTActualDevelopment development = getActual("UNITE1 PEUPLEMENT1 UTR1", 12, 403.28, 2);

    // Act
    development.setArea(100.0);

    // Assert
    EXPECT_DOUBLE_EQ(development.getArea(), 100.0);
    EXPECT_EQ(describe(development), "UNITE1 PEUPLEMENT1 UTR1|12|2|0");
}

TEST(FMTActualDevelopment, GivenAnActualDevelopmentThroughItsBaseClassWhenGetAreaThenReturnsItsArea)
{
    // Arrange
    const Core::FMTActualDevelopment actual = getActual("UNITE1 PEUPLEMENT1 UTR1", 12, 403.28);
    const Core::FMTDevelopment& development = actual;

    // Act
    const double area = development.getArea();

    // Assert
    EXPECT_DOUBLE_EQ(area, 403.28);
}

TEST(FMTActualDevelopment, GivenAnUnlockedDevelopmentWhenReduceLockToDeathThenReturnsItUnchanged)
{
    // Arrange
    const Core::FMTLifespans lifespans = getLifespans();
    const Core::FMTActualDevelopment development = getActual("UNITE1 PEUPLEMENT1 UTR1", 79, 10.0);

    // Act
    const Core::FMTActualDevelopment reduced = development.reduceLockToDeath(lifespans);

    // Assert
    EXPECT_TRUE(reduced == development);
}

TEST(FMTActualDevelopment, GivenALockThatEndsAtTheLifespanWhenReduceLockToDeathThenReturnsItUnchanged)
{
    // Arrange: 70 years and 10 periods of lock reach the lifespan of 80 years
    const Core::FMTLifespans lifespans = getLifespans();
    const Core::FMTActualDevelopment development = getActual("UNITE1 PEUPLEMENT1 UTR1", 70, 10.0, 10);

    // Act
    const Core::FMTActualDevelopment reduced = development.reduceLockToDeath(lifespans);

    // Assert
    EXPECT_TRUE(reduced == development);
}

TEST(FMTActualDevelopment, GivenALockThatEndsAfterTheLifespanWhenReduceLockToDeathThenRaisesADeathWithLock)
{
    // Arrange: 75 years and 10 periods of lock exceed the lifespan of 80 years
    const Core::FMTLifespans lifespans = getLifespans();
    const Core::FMTActualDevelopment development = getActual("UNITE1 PEUPLEMENT1 UTR1", 75, 10.0, 10);

    // Act
    const std::vector<Exception::FMTexc> raised = Testing::getRaisedTypes([&]()
        {
        development.reduceLockToDeath(lifespans);
        });

    // Assert
    EXPECT_TRUE(Testing::hasType(raised, Exception::FMTexc::FMTdeathwithlock));
}

// As a warning, the lock becomes the years left before the lifespan plus one, "to reproduce the behavior of WS" says
// the message of FMTActualDevelopment::reduceLockToDeath: 10 - (75 + 10 - 80) + 1 = 6.
TEST(FMTActualDevelopment, GivenADeathWithLockAsAWarningWhenReduceLockToDeathThenReducesTheLockToTheYearsLeftPlusOne)
{
    // Arrange
    const ErrorsAsWarnings warnings({ Exception::FMTexc::FMTdeathwithlock });
    const Core::FMTLifespans lifespans = getLifespans();
    const Core::FMTActualDevelopment development = getActual("UNITE1 PEUPLEMENT1 UTR1", 75, 10.0, 10);

    // Act
    const Core::FMTActualDevelopment reduced = development.reduceLockToDeath(lifespans);

    // Assert
    EXPECT_EQ(describe(reduced), "UNITE1 PEUPLEMENT1 UTR1|75|6|0");
    EXPECT_DOUBLE_EQ(reduced.getArea(), 10.0);
}

// The error is raised as soon as the lock ends after the lifespan, but the lock it gives is the years left plus one: a
// lock that ends one year after the lifespan raises the error, and keeps its value as a warning (Tests/ETAT.md, section
// 5.15).
TEST(FMTActualDevelopment, GivenALockThatEndsOneYearAfterTheLifespanAsAWarningWhenReduceLockToDeathThenKeepsTheLock)
{
    // Arrange: 71 years and 10 periods of lock, for a lifespan of 80 years
    const ErrorsAsWarnings warnings({ Exception::FMTexc::FMTdeathwithlock });
    const Core::FMTLifespans lifespans = getLifespans();
    const Core::FMTActualDevelopment development = getActual("UNITE1 PEUPLEMENT1 UTR1", 71, 10.0, 10);

    // Act
    const Core::FMTActualDevelopment reduced = development.reduceLockToDeath(lifespans);

    // Assert
    EXPECT_TRUE(reduced == development);
}

TEST(FMTActualDevelopment, GivenADevelopmentWithoutLifespanWhenReduceLockToDeathThenReturnsItUnchanged)
{
    // Arrange: a lifespan for the burnt developments only
    Core::FMTLifespans lifespans;
    lifespans.push_back(getMask("? FEU ?"), 0);
    lifespans.update();
    const Core::FMTActualDevelopment development = getActual("UNITE1 PEUPLEMENT1 UTR1", 75, 10.0, 10);

    // Act
    const Core::FMTActualDevelopment reduced = development.reduceLockToDeath(lifespans);

    // Assert
    EXPECT_TRUE(reduced == development);
}

TEST(FMTActualDevelopment, GivenAPreSolveFilterWhenPreSolveThenPresolvesTheMaskAndKeepsTheAgeTheLockAndTheArea)
{
    // Arrange
    const PreSolve preSolve = getPreSolve();
    const Core::FMTActualDevelopment development = getActual("UNITE1 PEUPLEMENT2 UTR1", 12, 201.64, 1);

    // Act
    const Core::FMTActualDevelopment presolved = development.preSolve(preSolve.filter, preSolve.themes);

    // Assert
    EXPECT_EQ(describe(presolved), "PEUPLEMENT2|12|1|0");
    EXPECT_DOUBLE_EQ(presolved.getArea(), 201.64);
}

TEST(FMTActualDevelopment, GivenAFilterWithoutSelectionWhenPreSolveThenKeepsTheMask)
{
    // Arrange
    const Core::FMTActualDevelopment development = getActual("UNITE1 PEUPLEMENT2 UTR1", 12, 201.64, 1);

    // Act
    const Core::FMTActualDevelopment presolved = development.preSolve(Core::FMTMaskFilter(), getThemes());

    // Assert
    EXPECT_TRUE(presolved == development);
}
