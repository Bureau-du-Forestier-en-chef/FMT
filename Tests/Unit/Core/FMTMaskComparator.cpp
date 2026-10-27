/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTMask.h"
#include "FMTTheme.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <iterator>
#include <map>
#include <string>
#include <vector>

// FMTMaskComparator finds a mask in a container by its bits (FMTMask::operator==), whatever its name. The themes are
// those of Tests/Unit/Core/FMTTheme.cpp.

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

TEST(FMTMaskComparator, GivenAMaskWhenComparedThenMatchesTheMasksOfTheSameBitsOnly)
{
    // Arrange
    const Core::FMTMaskComparator comparator(getMask("UNITE1 PEUPLEMENT1 UTR1"));

    // Act
    const bool sameMask = comparator(getMask("UNITE1 PEUPLEMENT1 UTR1"));
    const bool otherMask = comparator(getMask("UNITE1 PEUPLEMENT2 UTR1"));

    // Assert
    EXPECT_TRUE(sameMask);
    EXPECT_FALSE(otherMask);
}

// UC is the aggregate of UNITE1 and UNITE2: UC ? ? and a mask of the same bits without name match.
TEST(FMTMaskComparator, GivenAMaskWhenComparedThenIgnoresTheNamesOfTheMasks)
{
    // Arrange
    const Core::FMTMask aggregate = getMask("UC ? ?");
    const Core::FMTMask bitsOnly(aggregate.getBitsetReference());
    const Core::FMTMaskComparator comparator(aggregate);

    // Act
    const bool matches = comparator(bitsOnly);

    // Assert
    EXPECT_TRUE(matches);
    EXPECT_NE(std::string(bitsOnly), std::string(aggregate));
}

TEST(FMTMaskComparator, GivenMasksWhenFindIfWithAComparatorThenFindsTheMaskOfTheSameBits)
{
    // Arrange
    const std::vector<Core::FMTMask> masks = { getMask("UNITE1 PEUPLEMENT1 UTR1"), getMask("? FEU ?"),
        getMask("UC PROD ?") };

    // Act
    const std::vector<Core::FMTMask>::const_iterator burnt =
        std::find_if(masks.begin(), masks.end(), Core::FMTMaskComparator(getMask("? FEU ?")));
    const std::vector<Core::FMTMask>::const_iterator missing =
        std::find_if(masks.begin(), masks.end(), Core::FMTMaskComparator(getMask("? ? ?")));

    // Assert
    EXPECT_EQ(std::distance(masks.begin(), burnt), 1);
    EXPECT_TRUE(missing == masks.end());
}
