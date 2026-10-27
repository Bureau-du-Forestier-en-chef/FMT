/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTMask.h"
#include "FMTMaskFilter.h"
#include "FMTTheme.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <iterator>
#include <map>
#include <string>
#include <vector>

// FMTThemeComparator finds a theme: an equal theme, or, after the presolve, the presolved theme of an original theme
// (FMTConstraint::preSolveRef finds so the theme of a _GROUP). The themes are those of Tests/Unit/Core/FMTTheme.cpp,
// and the presolve that of Tests/Unit/Core/FMTMask.cpp, which keeps PEUPLEMENT1 and PEUPLEMENT2 of the second theme.

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

    // The themes left by the presolve of the developments UNITE1 PEUPLEMENT1 UTR1 and UNITE1 PEUPLEMENT2 UTR1.
    std::vector<Core::FMTTheme> getPresolvedThemes()
    {
        const std::vector<Core::FMTTheme> themes = getThemes();
        const Core::FMTMask selection = Core::FMTMask("UNITE1 PEUPLEMENT1 UTR1", themes).getUnion(
            Core::FMTMask("UNITE1 PEUPLEMENT2 UTR1", themes));
        Core::FMTMaskFilter filter(selection);
        std::vector<Core::FMTTheme> presolvedThemes;
        size_t newId = 0;
        size_t newStart = 0;
        for (const Core::FMTTheme& theme : themes)
        {
            const Core::FMTTheme presolved = theme.preSolve(filter, newId, newStart);
            if (!presolved.empty())
            {
                presolvedThemes.push_back(presolved);
            }
        }
        return presolvedThemes;
    }
}

TEST(FMTThemeComparator, GivenAThemeWhenComparedThenMatchesAnEqualThemeOnly)
{
    // Arrange
    const std::vector<Core::FMTTheme> themes = getThemes();
    const Core::FMTThemeComparator comparator(themes.at(1));

    // Act
    const bool sameTheme = comparator(getThemes().at(1));
    const bool otherTheme = comparator(themes.at(0));

    // Assert
    EXPECT_TRUE(sameTheme);
    EXPECT_FALSE(otherTheme);
}

// With the presolve, the comparator matches a theme whose attributes are all attributes of the original theme.
TEST(FMTThemeComparator, GivenAPresolvedThemeWhenComparedWithThePresolveThenMatchesTheOriginalThemeOfItsAttributes)
{
    // Arrange
    const std::vector<Core::FMTTheme> themes = getThemes();
    const std::vector<Core::FMTTheme> presolvedThemes = getPresolvedThemes();
    ASSERT_EQ(presolvedThemes.size(), 1u);
    const Core::FMTTheme& presolved = presolvedThemes.at(0);

    // Act
    const bool ofSecondTheme = Core::FMTThemeComparator(themes.at(1), true)(presolved);
    const bool ofFirstTheme = Core::FMTThemeComparator(themes.at(0), true)(presolved);
    const bool ofSecondThemeWithoutPresolve = Core::FMTThemeComparator(themes.at(1))(presolved);

    // Assert
    EXPECT_TRUE(ofSecondTheme);
    EXPECT_FALSE(ofFirstTheme);
    EXPECT_FALSE(ofSecondThemeWithoutPresolve);
}

TEST(FMTThemeComparator, GivenPresolvedThemesWhenFindIfWithAComparatorWithThePresolveThenFindsThePresolvedTheme)
{
    // Arrange
    const std::vector<Core::FMTTheme> themes = getThemes();
    const std::vector<Core::FMTTheme> presolvedThemes = getPresolvedThemes();

    // Act
    const std::vector<Core::FMTTheme>::const_iterator ofSecondTheme = std::find_if(presolvedThemes.begin(),
        presolvedThemes.end(), Core::FMTThemeComparator(themes.at(1), true));
    const std::vector<Core::FMTTheme>::const_iterator ofThirdTheme = std::find_if(presolvedThemes.begin(),
        presolvedThemes.end(), Core::FMTThemeComparator(themes.at(2), true));

    // Assert
    EXPECT_EQ(std::distance(presolvedThemes.begin(), ofSecondTheme), 0);
    EXPECT_TRUE(ofThirdTheme == presolvedThemes.end());
}
