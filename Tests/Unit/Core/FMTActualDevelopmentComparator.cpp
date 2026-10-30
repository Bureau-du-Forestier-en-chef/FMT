/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTActualDevelopment.h"
#include "FMTDevelopment.h"
#include "FMTMask.h"
#include "FMTTheme.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <iterator>
#include <map>
#include <string>
#include <vector>

// FMTActualDevelopmentComparator finds, in the area section, the actual development of a development: same mask, age,
// lock and period, whatever its area. The themes are those of Tests/Unit/Core/FMTTheme.cpp, and the area the first
// lines of the area section of TWD_land.

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

    // *A UNITE1 PEUPLEMENT1 UTR1 12 403.28, *A UNITE2 PEUPLEMENT2 UTR1 16 201.64, *A UNITE1 PEUPLEMENT3 UTR2 13 201.64
    std::vector<Core::FMTActualDevelopment> getArea()
    {
        return { Core::FMTActualDevelopment(getMask("UNITE1 PEUPLEMENT1 UTR1"), 12, 0, 403.28),
            Core::FMTActualDevelopment(getMask("UNITE2 PEUPLEMENT2 UTR1"), 16, 0, 201.64),
            Core::FMTActualDevelopment(getMask("UNITE1 PEUPLEMENT3 UTR2"), 13, 0, 201.64) };
    }
}

TEST(FMTActualDevelopmentComparator, GivenADevelopmentWhenComparedThenMatchesTheActualDevelopmentOfTheSameValuesOnly)
{
    // Arrange
    const Core::FMTDevelopment development(getMask("UNITE1 PEUPLEMENT1 UTR1"), 12, 0, 0);
    const Core::FMTActualDevelopmentComparator comparator(&development);

    // Act
    const bool sameValues = comparator(Core::FMTActualDevelopment(getMask("UNITE1 PEUPLEMENT1 UTR1"), 12, 0, 403.28));
    const bool otherAge = comparator(Core::FMTActualDevelopment(getMask("UNITE1 PEUPLEMENT1 UTR1"), 13, 0, 403.28));
    const bool otherLock = comparator(Core::FMTActualDevelopment(getMask("UNITE1 PEUPLEMENT1 UTR1"), 12, 1, 403.28));
    const bool otherMask = comparator(Core::FMTActualDevelopment(getMask("UNITE1 PEUPLEMENT2 UTR1"), 12, 0, 403.28));

    // Assert
    EXPECT_TRUE(sameValues);
    EXPECT_FALSE(otherAge);
    EXPECT_FALSE(otherLock);
    EXPECT_FALSE(otherMask);
}

// The comparator compares through FMTDevelopment::operator==: an actual development of another area matches.
TEST(FMTActualDevelopmentComparator, GivenAnActualDevelopmentAsTheBaseWhenComparedThenIgnoresTheArea)
{
    // Arrange
    const Core::FMTActualDevelopment base(getMask("UNITE1 PEUPLEMENT1 UTR1"), 12, 0, 1.0);
    const Core::FMTActualDevelopmentComparator comparator(&base);

    // Act
    const bool otherArea = comparator(Core::FMTActualDevelopment(getMask("UNITE1 PEUPLEMENT1 UTR1"), 12, 0, 403.28));

    // Assert
    EXPECT_TRUE(otherArea);
}

TEST(FMTActualDevelopmentComparator, GivenTheAreaSectionWhenFindIfWithAComparatorThenFindsTheActualDevelopment)
{
    // Arrange
    const std::vector<Core::FMTActualDevelopment> area = getArea();
    const Core::FMTDevelopment present(getMask("UNITE2 PEUPLEMENT2 UTR1"), 16, 0, 0);
    const Core::FMTDevelopment absent(getMask("UNITE2 PEUPLEMENT2 UTR1"), 17, 0, 0);

    // Act
    const std::vector<Core::FMTActualDevelopment>::const_iterator found =
        std::find_if(area.begin(), area.end(), Core::FMTActualDevelopmentComparator(&present));
    const std::vector<Core::FMTActualDevelopment>::const_iterator missing =
        std::find_if(area.begin(), area.end(), Core::FMTActualDevelopmentComparator(&absent));

    // Assert
    EXPECT_EQ(std::distance(area.begin(), found), 1);
    EXPECT_TRUE(missing == area.end());
}
