/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTLifespans.h"
#include "FMTMask.h"
#include "FMTMaskFilter.h"
#include "FMTTheme.h"

#include <gtest/gtest.h>

#include <map>
#include <string>
#include <vector>

// The lifespan section gives the age at which the developments of a mask die (the _DEATH action of FMTModel). The
// themes are those of Tests/Unit/Core/FMTTheme.cpp, and the lifespans those of TWD_land: ? feu ? 0, then ? ? ? 80.

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

    // The lifespans of TWD_land, as FMTLifespansParser and FMTModel build them.
    Core::FMTLifespans getLifespans(int p_lifespan = 80)
    {
        Core::FMTLifespans lifespans;
        lifespans.push_back(getMask("? FEU ?"), 0);
        lifespans.push_back(getMask("? ? ?"), p_lifespan);
        lifespans.update();
        return lifespans;
    }

    // The lifespans that the list gives to the developments of the mask p_mask, in their order.
    std::vector<int> getLifespansOf(const Core::FMTLifespans& p_lifespans, const std::string& p_mask)
    {
        std::vector<int> found;
        for (const Core::FMTLifespans::const_iterator lifespan : p_lifespans.findSets(getMask(p_mask)))
        {
            found.push_back(lifespan->second);
        }
        return found;
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

TEST(FMTLifespans, GivenMasksAndAgesWhenPushBackThenKeepsThemInTheirOrder)
{
    // Arrange
    Core::FMTLifespans lifespans;

    // Act
    lifespans.push_back(getMask("? FEU ?"), 0);
    lifespans.push_back(getMask("? ? ?"), 80);

    // Assert
    ASSERT_EQ(lifespans.size(), 2u);
    EXPECT_TRUE(lifespans.begin()->first == getMask("? FEU ?"));
    EXPECT_EQ(lifespans.begin()->second, 0);
    EXPECT_EQ((lifespans.begin() + 1)->second, 80);
}

TEST(FMTLifespans, GivenLifespansWhenConvertedToStringThenWritesALinePerMask)
{
    // Arrange
    const Core::FMTLifespans lifespans = getLifespans();

    // Act
    const std::string written = std::string(lifespans);

    // Assert
    EXPECT_EQ(written, "? FEU ? 0\n? ? ? 80\n");
}

// FMTActualDevelopment::reduceLockToDeath and FMTModel take the first lifespan that findSets gives.
TEST(FMTLifespans, GivenLifespansWhenFindSetsThenGivesTheLifespansOfADevelopmentInTheirOrder)
{
    // Arrange
    const Core::FMTLifespans lifespans = getLifespans();

    // Act
    const std::vector<int> ofBurnt = getLifespansOf(lifespans, "UNITE1 FEU UTR1");
    const std::vector<int> ofProductive = getLifespansOf(lifespans, "UNITE1 PEUPLEMENT1 UTR1");

    // Assert
    EXPECT_EQ(ofBurnt, std::vector<int>({ 0, 80 }));
    EXPECT_EQ(ofProductive, std::vector<int>({ 80 }));
}

TEST(FMTLifespans, GivenLifespansWhenCopiedOrAssignedThenTheCopyIsEqual)
{
    // Arrange
    const Core::FMTLifespans original = getLifespans();

    // Act
    const Core::FMTLifespans copy(original);
    Core::FMTLifespans assigned;
    assigned = original;

    // Assert
    EXPECT_TRUE(copy == original);
    EXPECT_TRUE(assigned == original);
    EXPECT_FALSE(assigned != original);
    EXPECT_EQ(getLifespansOf(assigned, "UNITE1 FEU UTR1"), std::vector<int>({ 0, 80 }));
}

TEST(FMTLifespans, GivenLifespansThatDifferByAnAgeWhenComparedThenTheyDiffer)
{
    // Arrange
    const Core::FMTLifespans lifespans = getLifespans(80);
    const Core::FMTLifespans longer = getLifespans(90);

    // Act
    const bool same = (lifespans == longer);
    const bool different = (lifespans != longer);

    // Assert
    EXPECT_FALSE(same);
    EXPECT_TRUE(different);
}

TEST(FMTLifespans, GivenTwoLifespansWhenSwapThenExchangesTheirElements)
{
    // Arrange
    Core::FMTLifespans lifespans = getLifespans(80);
    Core::FMTLifespans longer = getLifespans(90);

    // Act
    lifespans.swap(longer);

    // Assert
    EXPECT_TRUE(lifespans == getLifespans(90));
    EXPECT_TRUE(longer == getLifespans(80));
    EXPECT_EQ(getLifespansOf(lifespans, "UNITE1 PEUPLEMENT1 UTR1"), std::vector<int>({ 90 }));
}

// The presolve keeps peuplement1 and peuplement2 only: ? feu ? cannot be presolved, and ? ? ? becomes ? in the presolved
// themes.
TEST(FMTLifespans, GivenAPreSolveFilterWhenPreSolveThenKeepsTheLifespansOfThePresolvedDevelopmentsOnly)
{
    // Arrange
    const PreSolve preSolve = getPreSolve();
    std::vector<Core::FMTTheme> newThemes = preSolve.themes;
    const Core::FMTLifespans lifespans = getLifespans();

    // Act
    const Core::FMTLifespans presolved = lifespans.preSolve(preSolve.filter, getThemes(), newThemes);

    // Assert
    EXPECT_EQ(std::string(presolved), "? 80\n");
    EXPECT_EQ(lifespans.size(), 2u);
}

TEST(FMTLifespans, GivenNoLifespanWhenPreSolveThenGivesNoLifespan)
{
    // Arrange
    const PreSolve preSolve = getPreSolve();
    std::vector<Core::FMTTheme> newThemes = preSolve.themes;
    const Core::FMTLifespans lifespans;

    // Act
    const Core::FMTLifespans presolved = lifespans.preSolve(preSolve.filter, getThemes(), newThemes);

    // Assert
    EXPECT_TRUE(presolved.empty());
}
