/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "PickleArchive.h"
#include "ParserRoundTrip.h"
#include "FMTAction.h"
#include "FMTModel.h"

#include <gtest/gtest.h>

#include <string>
#include <vector>

// FMTAction through the archive of the pickle of Python, as Python pickles an FMTaction.

// Defect 25 (#408) of Tests/ETAT.md: FMTList::save writes its base FMTObject, which FMTList::load does not read;
// every class derived from FMTList, FMTAction among them, reads the rest of its archive from the wrong place.
TEST(FMTAction, DISABLED_GivenTheActionsOfTWDLandWhenPickledThenUnpicklesTheSameActions)
{
    // Arrange
    const std::vector<Core::FMTAction> actions = Testing::readScenario("ROOT").getActions();

    // Act
    std::vector<Core::FMTAction> unpickled;
    for (const Core::FMTAction& action : actions)
    {
        unpickled.push_back(Testing::pickleAndUnpickle(action));
    }

    // Assert
    ASSERT_FALSE(actions.empty());
    ASSERT_EQ(unpickled.size(), actions.size());
    for (size_t action = 0; action < actions.size(); ++action)
    {
        EXPECT_TRUE(unpickled.at(action) == actions.at(action)) << actions.at(action).getName();
        EXPECT_EQ(std::string(unpickled.at(action)), std::string(actions.at(action)));
    }
}
