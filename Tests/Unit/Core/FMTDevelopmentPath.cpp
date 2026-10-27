/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTDevelopmentPath.h"
#include "FMTDevelopment.h"
#include "FMTMask.h"
#include "FMTTheme.h"

#include <gtest/gtest.h>

#include <map>
#include <string>
#include <vector>

// A development path is what a *TARGET gives to a development that an action operates: the development it leads to,
// and the proportion of the area that goes to it (FMTTransitionMask::disturb). The themes are those of
// Tests/Unit/Core/FMTTheme.cpp.

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
}

TEST(FMTDevelopmentPath, GivenADefaultPathWhenConstructedThenHasADefaultDevelopmentAndNoProportion)
{
    // Arrange and Act
    const Core::FMTDevelopmentPath path;

    // Assert
    EXPECT_TRUE(path.getDevelopment() == Core::FMTDevelopment());
    EXPECT_DOUBLE_EQ(path.getProportion(), 0.0);
}

TEST(FMTDevelopmentPath, GivenAMaskAnAgeALockAPeriodAndAProportionWhenConstructedThenTheDevelopmentKeepsThem)
{
    // Arrange
    const Core::FMTMask mask = getMask("UNITE1 FEU UTR1");

    // Act
    const Core::FMTDevelopmentPath path(mask, 12, 1, 3, 60.0);

    // Assert
    EXPECT_TRUE(path.getDevelopment() == Core::FMTDevelopment(mask, 12, 1, 3));
    EXPECT_DOUBLE_EQ(path.getProportion(), 60.0);
}

TEST(FMTDevelopmentPath, GivenAPathWhenSetProportionThenChangesTheProportionOnly)
{
    // Arrange
    Core::FMTDevelopmentPath path(getMask("UNITE1 FEU UTR1"), 12, 1, 3, 60.0);

    // Act
    path.setProportion(100.0);

    // Assert
    EXPECT_DOUBLE_EQ(path.getProportion(), 100.0);
    EXPECT_TRUE(path.getDevelopment() == Core::FMTDevelopment(getMask("UNITE1 FEU UTR1"), 12, 1, 3));
}

// FMTTransitionMask::disturb and FMTFork set the age and the lock of the development of a path through this reference.
TEST(FMTDevelopmentPath, GivenAPathWhenItsDevelopmentIsChangedThroughTheReferenceThenThePathHoldsTheChange)
{
    // Arrange
    Core::FMTDevelopmentPath path(getMask("UNITE1 FEU UTR1"), 12, 1, 3, 60.0);

    // Act
    Core::FMTDevelopment& development = path.getDevelopmentReference();
    development.setAge(0);
    development.setLock(2);

    // Assert
    EXPECT_TRUE(path.getDevelopment() == Core::FMTDevelopment(getMask("UNITE1 FEU UTR1"), 0, 2, 3));
}

TEST(FMTDevelopmentPath, GivenAPathWhenCopiedOrAssignedThenTheCopyHoldsTheSameDevelopmentAndProportion)
{
    // Arrange
    const Core::FMTDevelopmentPath original(getMask("UNITE1 FEU UTR1"), 12, 1, 3, 60.0);

    // Act
    const Core::FMTDevelopmentPath copy(original);
    Core::FMTDevelopmentPath assigned;
    assigned = original;

    // Assert
    EXPECT_TRUE(copy.getDevelopment() == original.getDevelopment());
    EXPECT_DOUBLE_EQ(copy.getProportion(), 60.0);
    EXPECT_TRUE(assigned.getDevelopment() == original.getDevelopment());
    EXPECT_DOUBLE_EQ(assigned.getProportion(), 60.0);
}
