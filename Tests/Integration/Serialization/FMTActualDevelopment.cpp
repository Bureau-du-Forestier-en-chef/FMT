/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "PickleArchive.h"
#include "ParserRoundTrip.h"
#include "FMTActualDevelopment.h"
#include "FMTModel.h"

#include <gtest/gtest.h>

#include <vector>

// FMTActualDevelopment through the archive of the pickle of Python: the developments of the area, which a model
// carries with their area.

TEST(FMTActualDevelopment, GivenTheAreaOfTWDLandWhenPickledThenUnpicklesTheSameDevelopmentsAndAreas)
{
    // Arrange
    const std::vector<Core::FMTActualDevelopment> area = Testing::readScenario("ROOT").getArea();

    // Act
    std::vector<Core::FMTActualDevelopment> unpickled;
    for (const Core::FMTActualDevelopment& development : area)
    {
        unpickled.push_back(Testing::pickleAndUnpickle(development));
    }

    // Assert
    ASSERT_EQ(unpickled.size(), area.size());
    ASSERT_FALSE(unpickled.empty());
    for (size_t development = 0; development < area.size(); ++development)
    {
        EXPECT_TRUE(unpickled.at(development) == area.at(development)) << "development " << development;
        EXPECT_DOUBLE_EQ(unpickled.at(development).getArea(), area.at(development).getArea());
    }
}
