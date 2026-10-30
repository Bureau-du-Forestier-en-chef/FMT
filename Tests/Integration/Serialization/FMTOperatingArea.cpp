/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "PickleArchive.h"
#include "ParserRoundTrip.h"
#include "FMTMask.h"
#include "FMTModel.h"
#include "FMTOperatingArea.h"

#include <gtest/gtest.h>

#include <string>
#include <vector>

// Heuristics::FMTOperatingArea through the archive of the pickle of Python, as Python pickles an FMToperatingarea.

TEST(FMTOperatingArea, GivenAnOperatingAreaWithNeighborsWhenPickledThenUnpicklesTheSameArea)
{
    // Arrange
    const Models::FMTModel model = Testing::readScenario("ROOT");
    Heuristics::FMTOperatingArea area(Core::FMTMask("UNITE1 ? ?", model.getThemes()), 12.5);
    area.setNeighbors({ Core::FMTMask("UNITE2 ? ?", model.getThemes()), Core::FMTMask("UNITE3 ? ?", model.getThemes()) });
    area.setArea(403.28);

    // Act
    const Heuristics::FMTOperatingArea unpickled = Testing::pickleAndUnpickle(area);

    // Assert
    EXPECT_TRUE(unpickled == area);
    EXPECT_EQ(std::string(unpickled.getMask()), "UNITE1 ? ?");
    ASSERT_EQ(unpickled.getNeighbors().size(), 2u);
    EXPECT_EQ(std::string(unpickled.getNeighbors().at(1)), "UNITE3 ? ?");
    EXPECT_DOUBLE_EQ(unpickled.getNeighborsPerimeter(), 12.5);
    EXPECT_DOUBLE_EQ(unpickled.getArea(), 403.28);
}
