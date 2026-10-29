/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "PickleArchive.h"
#include "ParserRoundTrip.h"
#include "FMTAction.h"
#include "FMTActualDevelopment.h"
#include "FMTDevelopment.h"
#include "FMTModel.h"
#include "FMTObject.h"
#include "FMTSchedule.h"

#include <gtest/gtest.h>

#include <map>
#include <vector>

// FMTSchedule through the archive of the pickle of Python, as Python pickles an FMTschedule.

TEST(FMTSchedule, GivenAScheduleWithoutActionWhenPickledThenUnpicklesAnEmptyScheduleOfTheSamePeriod)
{
    // Arrange
    const Core::FMTSchedule schedule(3, Core::FMTObject(), false);

    // Act
    const Core::FMTSchedule unpickled = Testing::pickleAndUnpickle(schedule);

    // Assert
    EXPECT_TRUE(unpickled == schedule);
    EXPECT_EQ(unpickled.getPeriod(), 3);
    EXPECT_TRUE(unpickled.empty());
}

// Defect 25 of Tests/ETAT.md (section 2.7): the actions of a schedule are FMTList, whose load does not read the base
// FMTObject that its save writes.
TEST(FMTSchedule, DISABLED_GivenAScheduleOfAnActionOfTWDLandWhenPickledThenUnpicklesTheSameAreas)
{
    // Arrange: the first action of TWD_land operates 10 ha of the first development of the area at period 1
    const Models::FMTModel model = Testing::readScenario("ROOT");
    const Core::FMTAction action = model.getActions().at(0);
    const Core::FMTDevelopment development(model.getArea().at(0));
    std::map<Core::FMTAction, std::map<Core::FMTDevelopment, std::vector<double>>> areas;
    areas[action][development] = std::vector<double>(1, 10.0);
    const Core::FMTSchedule schedule(1, areas);

    // Act
    const Core::FMTSchedule unpickled = Testing::pickleAndUnpickle(schedule);

    // Assert
    EXPECT_TRUE(unpickled == schedule);
    EXPECT_EQ(unpickled.getPeriod(), 1);
    EXPECT_DOUBLE_EQ(unpickled.actionArea(action), 10.0);
}

// Defect 31 of Tests/ETAT.md (section 2.7): FMTSchedule::serialize does not save m_uselock, which says whether the
// areas of a schedule are by lock level: an unpickled schedule never uses them.
TEST(FMTSchedule, DISABLED_GivenAScheduleThatUsesLockLevelsWhenPickledThenUnpicklesAScheduleThatUsesThem)
{
    // Arrange
    const Core::FMTSchedule schedule(2, Core::FMTObject(), true);

    // Act
    const Core::FMTSchedule unpickled = Testing::pickleAndUnpickle(schedule);

    // Assert
    EXPECT_TRUE(unpickled.doUseLock());
    EXPECT_TRUE(unpickled == schedule);
}
