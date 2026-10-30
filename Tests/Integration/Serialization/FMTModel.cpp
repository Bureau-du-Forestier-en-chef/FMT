/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "PickleArchive.h"
#include "ParserRoundTrip.h"
#include "FMTModel.h"
#include "FMTModelParameters.h"

#include <gtest/gtest.h>

// Models::FMTModel through the archive of the pickle of Python, as Python pickles an FMTmodel: its sections are compared
// as those of a model written then read back (Testing::expectSameModel), its yields by their values.

// Defect 26 (#409) of Tests/ETAT.md: saving the yields throws "unregistered class". Behind it, defect 25: the
// actions, the transitions, the yields and the lifespans are FMTList, whose load does not read what its save writes.
TEST(FMTModel, DISABLED_GivenTheRootModelOfTWDLandWhenPickledThenUnpicklesTheSameModel)
{
    // Arrange
    Models::FMTModel model = Testing::readScenario("ROOT");
    model.setParameter(Models::FMTintmodelparameters::LENGTH, 7);

    // Act
    const Models::FMTModel unpickled = Testing::pickleAndUnpickle(model);

    // Assert
    Testing::expectSameModel(model, unpickled);
    EXPECT_EQ(unpickled.getName(), model.getName());
    EXPECT_EQ(unpickled.getParameter(Models::FMTintmodelparameters::LENGTH), 7);
}

// Defect 25 (#408) of Tests/ETAT.md: a model without any section saves, but its yields and its lifespans are
// FMTList, whose load does not read the base FMTObject that its save writes.
TEST(FMTModel, DISABLED_GivenAModelConstructedByDefaultWhenPickledThenUnpicklesAModelWithoutAnySection)
{
    // Arrange
    const Models::FMTModel model;

    // Act
    const Models::FMTModel unpickled = Testing::pickleAndUnpickle(model);

    // Assert
    EXPECT_TRUE(unpickled.getThemes().empty());
    EXPECT_TRUE(unpickled.getActions().empty());
    EXPECT_TRUE(unpickled.getYields().empty());
    EXPECT_TRUE(unpickled.getOutputs().empty());
}
