/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTYieldsCache.h"
#include "FMTAgeYieldHandler.h"
#include "FMTComplexYieldHandler.h"
#include "FMTData.h"
#include "FMTDevelopment.h"
#include "FMTMask.h"
#include "FMTTheme.h"
#include "FMTYieldRequest.h"
#include "FMTYields.h"
#include "FMTutility.h"

#include <gtest/gtest.h>

#include <map>
#include <string>
#include <vector>

// FMTYieldsCache keeps the values of complex yields, keyed by the age and the period of a development, its resume mask
// (Tests/Unit/Core/FMTYieldRequest.cpp) and the name of the yield (Tests/Unit/Core/FMTYieldDevelopment.cpp).
// FMTComplexYieldHandler::get serves a yield from it when it is there, and puts there a yield whose calculation took
// more than 0.05 ms. Every cache of the process shares one static map that nothing empties but a lack of memory
// (Documentation/Architecture.md, Known Architectural Debt): each case uses yield names that no other case of UnitCore
// uses, and none expects a key to be absent after a case has set it, so that the cases keep their results in any order
// and when repeated.

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

    Core::FMTDevelopment getDevelopment(const std::string& p_mask, int p_age, int p_lock = 0, int p_period = 1)
    {
        return Core::FMTDevelopment(getMask(p_mask), p_age, p_lock, p_period);
    }

    // VOLUME of UNITE1 (1000, 2000 and 2400 at the ages 10, 20 and 30), VOLUME of every development (100, 200 and 240),
    // and the complex yield p_yield, _MULTIPLY(VOLUME,<p_factor>), of every development.
    Core::FMTYields getYields(const std::string& p_yield, double p_factor)
    {
        Core::FMTAgeYieldHandler unit1(getMask("UNITE1 ? ?"));
        unit1.setYieldValues("VOLUME", { 10, 20, 30 }, { 1000.0, 2000.0, 2400.0 });
        Core::FMTAgeYieldHandler every(getMask("? ? ?"));
        every.setYieldValues("VOLUME", { 10, 20, 30 }, { 100.0, 200.0, 240.0 });
        Core::FMTComplexYieldHandler complex(getMask("? ? ?"));
        complex.pushData(p_yield, Core::FMTData({ p_factor }, Core::FMTyieldparserop::FMTmultiply, { "VOLUME" }, { true, false }));
        Core::FMTYields yields;
        yields.pushBackAgeHandler(getMask("UNITE1 ? ?"), unit1);
        yields.pushBackAgeHandler(getMask("? ? ?"), every);
        yields.push_back(getMask("? ? ?"), complex.clone());
        yields.update();
        return yields;
    }
}

TEST(FMTYieldsCache, GivenAValueSetForARequestAndAYieldWhenTryToGetThenFindsThatValue)
{
    // Arrange
    Core::FMTYieldsCache cache;
    const Core::FMTDevelopment development = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 20);
    const Core::FMTYieldRequest request(development);
    double found = -1.0;

    // Act
    cache.set(12.5, request, "CACHESET");
    const bool isFound = cache.tryToGet(found, request, "CACHESET");

    // Assert
    EXPECT_TRUE(isFound);
    EXPECT_DOUBLE_EQ(found, 12.5);
    EXPECT_DOUBLE_EQ(cache.get(request, "CACHESET"), 12.5);
    EXPECT_TRUE(cache.inCache(request, "CACHESET"));
}

TEST(FMTYieldsCache, GivenAYieldNeverSetWhenTryToGetThenFindsNothingAndGetReturnsZero)
{
    // Arrange
    const Core::FMTYieldsCache cache;
    const Core::FMTDevelopment development = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 20);
    const Core::FMTYieldRequest request(development);
    double found = -1.0;

    // Act
    const bool isFound = cache.tryToGet(found, request, "CACHENEVERSET");

    // Assert
    EXPECT_FALSE(isFound);
    EXPECT_DOUBLE_EQ(found, -1.0);
    EXPECT_DOUBLE_EQ(cache.get(request, "CACHENEVERSET"), 0.0);
    EXPECT_FALSE(cache.inCache(request, "CACHENEVERSET"));
}

// The key holds the age and the period of the development, not its lock.
TEST(FMTYieldsCache, GivenAValueSetAtAnAgeAndAPeriodWhenTryToGetThenFindsItAtThatAgeAndPeriodOnlyWhateverTheLock)
{
    // Arrange
    Core::FMTYieldsCache cache;
    const Core::FMTDevelopment development = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 20, 0, 1);
    const Core::FMTDevelopment older = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 21, 0, 1);
    const Core::FMTDevelopment later = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 20, 0, 2);
    const Core::FMTDevelopment locked = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 20, 2, 1);
    const Core::FMTYieldRequest request(development);
    cache.set(7.0, request, "CACHEAGEPERIOD");

    // Act
    const bool findsOlder = cache.inCache(Core::FMTYieldRequest(older), "CACHEAGEPERIOD");
    const bool findsLater = cache.inCache(Core::FMTYieldRequest(later), "CACHEAGEPERIOD");
    const bool findsLocked = cache.inCache(Core::FMTYieldRequest(locked), "CACHEAGEPERIOD");

    // Assert
    EXPECT_FALSE(findsOlder);
    EXPECT_FALSE(findsLater);
    EXPECT_TRUE(findsLocked);
}

// A request that FMTYields has not prepared has an empty resume mask: its key holds no mask at all.
TEST(FMTYieldsCache, GivenRequestsNotPreparedByYieldsWhenTheirDevelopmentsDifferByTheirMaskOnlyThenTheyShareTheirValues)
{
    // Arrange
    Core::FMTYieldsCache cache;
    const Core::FMTDevelopment ofUnit1 = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 20);
    const Core::FMTDevelopment ofUnit3 = getDevelopment("UNITE3 PEUPLEMENT3 UTR3", 20);
    cache.set(8.0, Core::FMTYieldRequest(ofUnit1), "CACHENOMASK");
    double found = -1.0;

    // Act
    const bool isFound = cache.tryToGet(found, Core::FMTYieldRequest(ofUnit3), "CACHENOMASK");

    // Assert
    EXPECT_TRUE(isFound);
    EXPECT_DOUBLE_EQ(found, 8.0);
}

TEST(FMTYieldsCache, GivenRequestsPreparedByYieldsWhenTheirDevelopmentsShareTheirResumeMaskThenTheyShareTheirValues)
{
    // Arrange: the resume mask keeps the attribute of the first theme only
    Core::FMTYieldsCache cache;
    const Core::FMTYields yields = getYields("CACHERESUME", 2.0);
    const Core::FMTDevelopment first = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 20);
    const Core::FMTDevelopment second = getDevelopment("UNITE1 PEUPLEMENT3 UTR2", 20);
    const Core::FMTDevelopment otherUnit = getDevelopment("UNITE2 PEUPLEMENT1 UTR1", 20);
    const Core::FMTYieldRequest firstRequest(first);
    const Core::FMTYieldRequest secondRequest(second);
    const Core::FMTYieldRequest otherUnitRequest(otherUnit);
    yields.get(firstRequest, "VOLUME");
    yields.get(secondRequest, "VOLUME");
    yields.get(otherUnitRequest, "VOLUME");
    cache.set(9.0, firstRequest, "CACHERESUMEVALUE");

    // Act
    const bool findsSecond = cache.inCache(secondRequest, "CACHERESUMEVALUE");
    const bool findsOtherUnit = cache.inCache(otherUnitRequest, "CACHERESUMEVALUE");

    // Assert
    EXPECT_TRUE(findsSecond);
    EXPECT_FALSE(findsOtherUnit);
}

// set inserts a value only when its key is not there: a value cached stays, whatever is set after it.
TEST(FMTYieldsCache, GivenAValueAlreadySetWhenSetAgainThenKeepsTheFirstValue)
{
    // Arrange
    Core::FMTYieldsCache cache;
    const Core::FMTDevelopment development = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 20);
    const Core::FMTYieldRequest request(development);
    cache.set(1.0, request, "CACHETWICE");

    // Act
    cache.set(2.0, request, "CACHETWICE");

    // Assert
    EXPECT_DOUBLE_EQ(cache.get(request, "CACHETWICE"), 1.0);
}

// Every cache of the process shares the same values: a cache is not the cache of its handler.
TEST(FMTYieldsCache, GivenTwoCachesWhenOneSetsAValueThenTheOtherFindsIt)
{
    // Arrange
    Core::FMTYieldsCache first;
    const Core::FMTYieldsCache second;
    const Core::FMTDevelopment development = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 20);
    const Core::FMTYieldRequest request(development);

    // Act
    first.set(3.0, request, "CACHESHARED");

    // Assert
    EXPECT_DOUBLE_EQ(second.get(request, "CACHESHARED"), 3.0);
}

// The value of _MULTIPLY(VOLUME,2) at 20 years is 2 x 200; once cached, it is served by the cache.
TEST(FMTYieldsCache, GivenAComplexYieldCachedWithItsValueWhenYieldsGetItAgainThenServesTheSameValue)
{
    // Arrange
    Core::FMTYieldsCache cache;
    const Core::FMTYields yields = getYields("CACHESERVED", 2.0);
    const Core::FMTDevelopment development = getDevelopment("UNITE2 PEUPLEMENT1 UTR1", 20);
    const Core::FMTYieldRequest request(development);
    const double computed = yields.get(request, "CACHESERVED");
    cache.set(computed, request, "CACHESERVED");

    // Act
    const double served = yields.get(request, "CACHESERVED");

    // Assert
    EXPECT_DOUBLE_EQ(computed, 400.0);
    EXPECT_DOUBLE_EQ(served, computed);
    EXPECT_DOUBLE_EQ(cache.get(request, "CACHESERVED"), computed);
}

// FMTComplexYieldHandler::get looks in the cache before its definition: the value cached for the key of a complex yield
// is the yield, whatever its definition gives (400).
TEST(FMTYieldsCache, GivenAValueInTheCacheForAComplexYieldWhenYieldsGetThenServesThatValueInsteadOfItsDefinition)
{
    // Arrange
    Core::FMTYieldsCache cache;
    const Core::FMTYields yields = getYields("CACHEPRESET", 2.0);
    const Core::FMTDevelopment development = getDevelopment("UNITE2 PEUPLEMENT1 UTR1", 20);
    const Core::FMTYieldRequest request(development);
    yields.get(request, "VOLUME");
    cache.set(999.0, request, "CACHEPRESET");

    // Act
    const double value = yields.get(request, "CACHEPRESET");

    // Assert
    EXPECT_DOUBLE_EQ(value, 999.0);
}

// Defect 22 (#403, Tests/ETAT.md, section 2.7): the cache serves a complex yield computed for another section of yields
// of the same masks, such as the yields of another scenario or those that getFromFactor returns. The first section is
// cached with the value it computes (400), as FMTComplexYieldHandler caches a calculation of more than 0.05 ms; the
// second one defines the yield as _MULTIPLY(VOLUME,3), 3 x 200.
TEST(FMTYieldsCache, DISABLED_GivenAComplexYieldCachedForOneSectionOfYieldsWhenAnotherSectionDefinesItOtherwiseThenComputesItFromItsOwnDefinition)
{
    // Arrange
    Core::FMTYieldsCache cache;
    const Core::FMTYields first = getYields("CACHEOTHERSECTION", 2.0);
    const Core::FMTYields second = getYields("CACHEOTHERSECTION", 3.0);
    const Core::FMTDevelopment development = getDevelopment("UNITE2 PEUPLEMENT1 UTR1", 20);
    const Core::FMTYieldRequest firstRequest(development);
    const double ofFirst = first.get(firstRequest, "CACHEOTHERSECTION");
    cache.set(ofFirst, firstRequest, "CACHEOTHERSECTION");
    const Core::FMTYieldRequest secondRequest(development);

    // Act
    const double ofSecond = second.get(secondRequest, "CACHEOTHERSECTION");

    // Assert
    EXPECT_DOUBLE_EQ(ofFirst, 400.0);
    EXPECT_DOUBLE_EQ(ofSecond, 600.0);
}
