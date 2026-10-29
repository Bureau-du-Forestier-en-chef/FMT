/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "PickleArchive.h"
#include "ParserRoundTrip.h"
#include "FMTActualDevelopment.h"
#include "FMTFuturDevelopment.h"
#include "FMTModel.h"

#include <gtest/gtest.h>

// FMTFuturDevelopment through the archive of the pickle of Python: a development of the graph, without area.

TEST(FMTFuturDevelopment, GivenAFuturDevelopmentWhenPickledThenUnpicklesTheSameDevelopmentWithoutArea)
{
    // Arrange
    const Core::FMTActualDevelopment actual = Testing::readScenario("ROOT").getArea().at(0);
    Core::FMTFuturDevelopment development(actual);
    development.setAge(actual.getAge() + 5);
    development.setPeriod(2);

    // Act
    const Core::FMTFuturDevelopment unpickled = Testing::pickleAndUnpickle(development);

    // Assert
    EXPECT_TRUE(unpickled == development);
    EXPECT_EQ(unpickled.getAge(), actual.getAge() + 5);
    EXPECT_EQ(unpickled.getPeriod(), 2);
    EXPECT_DOUBLE_EQ(unpickled.getArea(), 0.0);
}
