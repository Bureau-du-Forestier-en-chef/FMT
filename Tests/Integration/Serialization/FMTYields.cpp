/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "PickleArchive.h"
#include "ParserRoundTrip.h"
#include "FMTModel.h"
#include "FMTYields.h"

#include <gtest/gtest.h>

// FMTYields through the archive of the pickle of Python, as Python pickles an FMTyields. FMTYields::operator== compares
// the addresses of the handlers (issue #383): the unpickled yields are compared by the values of their yields, in a
// copy of the model that takes them (Testing::expectSameYieldValues).

// Defect 26 of Tests/ETAT.md (section 2.7): the yields are saved through a pointer to FMTYieldHandler, and no derived
// handler is exported to Boost.Serialization (BOOST_CLASS_EXPORT): saving throws "unregistered class". Behind it,
// defect 25: FMTYields is an FMTList.
TEST(FMTYields, DISABLED_GivenTheYieldsOfTWDLandWhenPickledThenGivesTheSameValues)
{
    // Arrange: TWD_land has age, time and complex yields
    const Models::FMTModel model = Testing::readScenario("ROOT");

    // Act
    const Core::FMTYields unpickled = Testing::pickleAndUnpickle(model.getYields());

    // Assert
    Models::FMTModel withUnpickledYields(model);
    withUnpickledYields.setYields(unpickled);
    EXPECT_EQ(unpickled.size(), model.getYields().size());
    Testing::expectSameYieldValues(model, withUnpickledYields);
}

// Without any handler, the yields read back empty. FMTList::load does not read the 5 bytes of the base FMTObject
// that FMTList::save writes (defect 25 of Tests/ETAT.md, section 2.7), but they are zeros, which read as empty
// containers: this case does not show the defect, and a model constructed by default does
// (Tests/Integration/Serialization/FMTModel.cpp).
TEST(FMTYields, GivenYieldsWithoutAnyHandlerWhenPickledThenUnpicklesYieldsWithoutAnyHandler)
{
    // Arrange
    const Core::FMTYields yields;

    // Act
    const Core::FMTYields unpickled = Testing::pickleAndUnpickle(yields);

    // Assert
    EXPECT_TRUE(unpickled.empty());
    EXPECT_EQ(unpickled.size(), 0u);
}
