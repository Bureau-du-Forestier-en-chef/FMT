/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "PickleArchive.h"
#include "ParserRoundTrip.h"
#include "FMTDevelopment.h"
#include "FMTMask.h"
#include "FMTModel.h"

#include <gtest/gtest.h>

#include <string>

// FMTDevelopment through the archive of the pickle of Python, as Python pickles an FMTdevelopment: by value.

TEST(FMTDevelopment, GivenADevelopmentWithAnAgeALockAndAPeriodWhenPickledThenUnpicklesTheSameDevelopment)
{
    // Arrange
    const Models::FMTModel model = Testing::readScenario("ROOT");
    const Core::FMTDevelopment development(Core::FMTMask("UNITE2 PEUPLEMENT3 UTR2", model.getThemes()), 12, 2, 3);

    // Act
    const Core::FMTDevelopment unpickled = Testing::pickleAndUnpickle(development);

    // Assert
    EXPECT_TRUE(unpickled == development);
    EXPECT_EQ(unpickled.getAge(), 12);
    EXPECT_EQ(unpickled.getLock(), 2);
    EXPECT_EQ(unpickled.getPeriod(), 3);
    EXPECT_EQ(std::string(unpickled.getMask()), "UNITE2 PEUPLEMENT3 UTR2");
}

TEST(FMTDevelopment, GivenADevelopmentConstructedByDefaultWhenPickledThenUnpicklesTheSameDevelopment)
{
    // Arrange
    const Core::FMTDevelopment development;

    // Act
    const Core::FMTDevelopment unpickled = Testing::pickleAndUnpickle(development);

    // Assert
    EXPECT_TRUE(unpickled == development);
    EXPECT_EQ(unpickled.getAge(), development.getAge());
    EXPECT_EQ(unpickled.getLock(), development.getLock());
    EXPECT_EQ(unpickled.getPeriod(), development.getPeriod());
    EXPECT_TRUE(unpickled.getMask().empty());
}
