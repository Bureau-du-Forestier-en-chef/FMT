/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "PickleArchive.h"
#include "ParserRoundTrip.h"
#include "FMTMask.h"
#include "FMTModel.h"
#include "FMTTheme.h"

#include <gtest/gtest.h>

#include <string>
#include <vector>

// FMTTheme through the archive of the pickle of Python. A theme does not save where its attributes are: it rebuilds
// that table when it is read (FMTTheme::serialize), which a mask built from the unpickled themes uses.

TEST(FMTTheme, GivenTheThemesOfTWDLandWhenPickledThenUnpicklesTheSameThemes)
{
    // Arrange
    const std::vector<Core::FMTTheme> themes = Testing::readScenario("ROOT").getThemes();

    // Act
    std::vector<Core::FMTTheme> unpickled;
    for (const Core::FMTTheme& theme : themes)
    {
        unpickled.push_back(Testing::pickleAndUnpickle(theme));
    }

    // Assert
    ASSERT_EQ(unpickled.size(), 3u);
    for (size_t theme = 0; theme < themes.size(); ++theme)
    {
        EXPECT_TRUE(unpickled.at(theme) == themes.at(theme)) << "theme " << theme;
        EXPECT_EQ(unpickled.at(theme).getId(), themes.at(theme).getId());
        EXPECT_EQ(unpickled.at(theme).getStart(), themes.at(theme).getStart());
        EXPECT_EQ(unpickled.at(theme).getName(), themes.at(theme).getName());
        EXPECT_EQ(unpickled.at(theme).getAggregates(), themes.at(theme).getAggregates());
        EXPECT_EQ(std::string(unpickled.at(theme)), std::string(themes.at(theme)));
    }
}

TEST(FMTTheme, GivenTheUnpickledThemesOfTWDLandWhenBuildingAMaskOfAnAggregateThenSelectsTheSameAttributes)
{
    // Arrange
    const std::vector<Core::FMTTheme> themes = Testing::readScenario("ROOT").getThemes();
    std::vector<Core::FMTTheme> unpickled;
    for (const Core::FMTTheme& theme : themes)
    {
        unpickled.push_back(Testing::pickleAndUnpickle(theme));
    }

    // Act
    const Core::FMTMask mask("UNITE1 PROD UTA1", unpickled);

    // Assert
    EXPECT_TRUE(mask == Core::FMTMask("UNITE1 PROD UTA1", themes));
}

TEST(FMTTheme, GivenAThemeConstructedByDefaultWhenPickledThenUnpicklesAnEmptyTheme)
{
    // Arrange
    const Core::FMTTheme theme;

    // Act
    const Core::FMTTheme unpickled = Testing::pickleAndUnpickle(theme);

    // Assert
    EXPECT_TRUE(unpickled == theme);
    EXPECT_TRUE(unpickled.empty());
    EXPECT_EQ(unpickled.size(), 0u);
}
