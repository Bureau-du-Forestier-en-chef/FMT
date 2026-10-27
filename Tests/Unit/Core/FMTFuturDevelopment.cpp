/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTFuturDevelopment.h"
#include "FMTActualDevelopment.h"
#include "FMTDevelopment.h"
#include "FMTMask.h"
#include "FMTTheme.h"

#include <gtest/gtest.h>

#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

// A future development is a development that the graph creates by growth (FMTDevelopment::grow) or by an action: it
// has no area of its own. The themes are those of Tests/Unit/Core/FMTTheme.cpp.

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

    Core::FMTDevelopment getDevelopment(const std::string& p_mask, int p_age, int p_lock, int p_period)
    {
        return Core::FMTDevelopment(Core::FMTMask(p_mask, getThemes()), p_age, p_lock, p_period);
    }
}

TEST(FMTFuturDevelopment, GivenADevelopmentWhenConstructedFromItThenKeepsItsMaskAgeLockAndPeriod)
{
    // Arrange
    const Core::FMTDevelopment development = getDevelopment("UNITE2 FEU UTR3", 12, 1, 2);

    // Act
    const Core::FMTFuturDevelopment futur(development);

    // Assert
    EXPECT_TRUE(static_cast<const Core::FMTDevelopment&>(futur) == development);
    EXPECT_EQ(futur.getAge(), 12);
    EXPECT_EQ(futur.getLock(), 1);
    EXPECT_EQ(futur.getPeriod(), 2);
}

TEST(FMTFuturDevelopment, GivenADefaultFuturDevelopmentWhenConstructedThenHasNoMaskAndZeroValues)
{
    // Arrange and Act
    const Core::FMTFuturDevelopment futur;

    // Assert
    EXPECT_TRUE(futur.getMask().empty());
    EXPECT_EQ(futur.getAge(), 0);
    EXPECT_EQ(futur.getLock(), 0);
    EXPECT_EQ(futur.getPeriod(), 0);
}

TEST(FMTFuturDevelopment, GivenAFuturDevelopmentWhenGetAreaThenReturnsZeroEvenThroughItsBaseClass)
{
    // Arrange
    const Core::FMTFuturDevelopment futur(getDevelopment("UNITE2 FEU UTR3", 12, 1, 2));
    const Core::FMTDevelopment& development = futur;

    // Act
    const double area = development.getArea();

    // Assert
    EXPECT_DOUBLE_EQ(area, 0.0);
    EXPECT_DOUBLE_EQ(futur.getArea(), 0.0);
}

TEST(FMTFuturDevelopment, GivenAnActualDevelopmentWhenAFuturDevelopmentIsConstructedFromItThenItHasNoArea)
{
    // Arrange
    const Core::FMTActualDevelopment actual(Core::FMTMask("UNITE1 PEUPLEMENT1 UTR1", getThemes()), 12, 0, 403.28);

    // Act
    const Core::FMTFuturDevelopment futur(actual);

    // Assert
    EXPECT_DOUBLE_EQ(futur.getArea(), 0.0);
    EXPECT_TRUE(static_cast<const Core::FMTDevelopment&>(futur) == actual);
}

TEST(FMTFuturDevelopment, GivenAFuturDevelopmentWhenCopiedAssignedOrClonedThenTheCopyIsEqual)
{
    // Arrange
    const Core::FMTFuturDevelopment original(getDevelopment("UNITE2 FEU UTR3", 12, 1, 2));

    // Act
    const Core::FMTFuturDevelopment copy(original);
    Core::FMTFuturDevelopment assigned;
    assigned = original;
    const std::unique_ptr<Core::FMTDevelopment> clone = original.clone();

    // Assert
    EXPECT_TRUE(copy == original);
    EXPECT_TRUE(assigned == original);
    ASSERT_NE(clone.get(), nullptr);
    EXPECT_NE(dynamic_cast<const Core::FMTFuturDevelopment*>(clone.get()), nullptr);
    EXPECT_TRUE(*clone == original);
    EXPECT_DOUBLE_EQ(clone->getArea(), 0.0);
}

TEST(FMTFuturDevelopment, GivenAFuturDevelopmentWhenMovedThenTheTargetHoldsItsValues)
{
    // Arrange
    Core::FMTFuturDevelopment constructed(getDevelopment("UNITE2 FEU UTR3", 12, 1, 2));
    Core::FMTFuturDevelopment assigned(getDevelopment("UNITE2 FEU UTR3", 12, 1, 2));
    const Core::FMTFuturDevelopment expected(constructed);

    // Act
    const Core::FMTFuturDevelopment moved(std::move(constructed));
    Core::FMTFuturDevelopment moveAssigned;
    moveAssigned = std::move(assigned);

    // Assert
    EXPECT_TRUE(moved == expected);
    EXPECT_TRUE(moveAssigned == expected);
}

TEST(FMTFuturDevelopment, GivenFuturDevelopmentsThatDifferByTheirAgeWhenComparedThenTheyDiffer)
{
    // Arrange
    const Core::FMTFuturDevelopment young(getDevelopment("UNITE2 FEU UTR3", 12, 1, 2));
    const Core::FMTFuturDevelopment old(getDevelopment("UNITE2 FEU UTR3", 13, 1, 2));

    // Act
    const bool sameValues = (young == Core::FMTFuturDevelopment(getDevelopment("UNITE2 FEU UTR3", 12, 1, 2)));
    const bool otherAge = (young == old);

    // Assert
    EXPECT_TRUE(sameValues);
    EXPECT_FALSE(otherAge);
}
