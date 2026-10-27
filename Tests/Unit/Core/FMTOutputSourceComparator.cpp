/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTOutputSource.h"
#include "FMTMask.h"
#include "FMTSpec.h"
#include "FMTTheme.h"
#include "FMTutility.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <iterator>
#include <string>
#include <vector>

// FMTOutputSourceComparator finds a variable source (a source of a mask) or a constant among the sources of an output, as
// FMTOutputParser does to tell an output of constants. The sources are those of Tests/Unit/Core/FMTOutputSource.cpp, on
// one theme.

namespace
{
    std::vector<Core::FMTTheme> getThemes()
    {
        return { Core::FMTTheme({ "A1", "A2" }, 0, 0, "THEME1") };
    }

    // A constant, a level of constants, a time yield, then the inventory and the harvest of the developments of a mask.
    std::vector<Core::FMTOutputSource> getSources()
    {
        const Core::FMTMask EVERY = Core::FMTMask("?", getThemes());
        return { Core::FMTOutputSource(Core::FMTotar::val, 2.0),
            Core::FMTOutputSource(Core::FMTotar::level, std::vector<double>({ 1.0 }), -1, -1, "MYLEVEL"),
            Core::FMTOutputSource(Core::FMTotar::timeyld, 0.0, "PERIODVALUE"),
            Core::FMTOutputSource(Core::FMTSpec(), EVERY, Core::FMTotar::inventory, "VOLUME"),
            Core::FMTOutputSource(Core::FMTSpec(), EVERY, Core::FMTotar::actual, "VOLUME", "COUPE") };
    }

    // The position of the first source that p_comparator matches, or the number of sources.
    long getFirst(const std::vector<Core::FMTOutputSource>& p_sources, const Core::FMTOutputSourceComparator& p_comparator)
    {
        return static_cast<long>(std::distance(p_sources.begin(), std::find_if(p_sources.begin(), p_sources.end(), p_comparator)));
    }
}

TEST(FMTOutputSourceComparator, GivenSourcesWhenFindIfAVariableSourceThenFindsTheFirstSourceOfAMask)
{
    // Arrange
    const std::vector<Core::FMTOutputSource> sources = getSources();

    // Act
    const long first = getFirst(sources, Core::FMTOutputSourceComparator(true));

    // Assert
    EXPECT_EQ(first, 3);
}

// A level of constants and a time yield are neither variable nor constant: with the constant put last, it is the first
// constant found.
TEST(FMTOutputSourceComparator, GivenSourcesWhenFindIfAConstantThenFindsTheFirstConstantOnly)
{
    // Arrange
    std::vector<Core::FMTOutputSource> sources = getSources();
    std::rotate(sources.begin(), sources.begin() + 1, sources.end());

    // Act
    const long first = getFirst(sources, Core::FMTOutputSourceComparator(false));

    // Assert
    EXPECT_EQ(first, 4);
}

TEST(FMTOutputSourceComparator, GivenSourcesWithoutVariableWhenFindIfAVariableSourceThenFindsNothing)
{
    // Arrange
    const std::vector<Core::FMTOutputSource> sources = { Core::FMTOutputSource(Core::FMTotar::val, 1.0),
        Core::FMTOutputSource(Core::FMTotar::timeyld, 0.0, "PERIODVALUE") };

    // Act
    const long first = getFirst(sources, Core::FMTOutputSourceComparator(true));

    // Assert
    EXPECT_EQ(first, 2);
}
