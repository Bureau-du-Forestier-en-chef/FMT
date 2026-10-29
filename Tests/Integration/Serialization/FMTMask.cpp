/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "PickleArchive.h"
#include "ParserRoundTrip.h"
#include "FMTMask.h"
#include "FMTModel.h"

#include <gtest/gtest.h>

#include <string>

// FMTMask through the archive of the pickle of Python: its bits and its name, which a mask keeps as it was written.

TEST(FMTMask, GivenAMaskOfTheThemesOfTWDLandWhenPickledThenUnpicklesTheSameBitsAndName)
{
    // Arrange
    const Models::FMTModel model = Testing::readScenario("ROOT");
    const Core::FMTMask mask("UNITE1 PEUPLEMENT1 UTR1", model.getThemes());

    // Act
    const Core::FMTMask unpickled = Testing::pickleAndUnpickle(mask);

    // Assert
    EXPECT_TRUE(unpickled == mask);
    EXPECT_EQ(unpickled.size(), mask.size());
    EXPECT_EQ(unpickled.count(), 3u);
    EXPECT_EQ(std::string(unpickled), "UNITE1 PEUPLEMENT1 UTR1");
}

TEST(FMTMask, GivenAMaskOfAnAggregateWhenPickledThenKeepsTheAggregateInItsName)
{
    // Arrange: PROD is the aggregate of PEUPLEMENT1, PEUPLEMENT2 and PEUPLEMENT3 in TWD_land
    const Models::FMTModel model = Testing::readScenario("ROOT");
    const Core::FMTMask mask("UNITE1 PROD ?", model.getThemes());

    // Act
    const Core::FMTMask unpickled = Testing::pickleAndUnpickle(mask);

    // Assert
    EXPECT_TRUE(unpickled == mask);
    EXPECT_EQ(std::string(unpickled), "UNITE1 PROD ?");
    EXPECT_EQ(unpickled.count(), mask.count());
}

TEST(FMTMask, GivenAMaskOfZeroWidthWhenPickledThenUnpicklesAMaskOfZeroWidth)
{
    // Arrange
    const Core::FMTMask mask;

    // Act
    const Core::FMTMask unpickled = Testing::pickleAndUnpickle(mask);

    // Assert
    EXPECT_TRUE(unpickled == mask);
    EXPECT_TRUE(unpickled.empty());
    EXPECT_EQ(unpickled.size(), 0u);
    EXPECT_EQ(std::string(unpickled), "");
}
