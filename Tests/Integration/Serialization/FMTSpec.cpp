/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "PickleArchive.h"
#include "FMTAgeBounds.h"
#include "FMTLockBounds.h"
#include "FMTPerBounds.h"
#include "FMTSpec.h"
#include "FMTYldBounds.h"
#include "FMTutility.h"

#include <gtest/gtest.h>

#include <limits>
#include <string>

// FMTSpec and its bounds through the archive of the pickle of Python: the conditions of an action, of a transition
// source, of an output source and of a constraint.

TEST(FMTSpec, GivenASpecificationWithBoundsOfEveryKindWhenPickledThenUnpicklesTheSameBounds)
{
    // Arrange: _AGE >= 10 AND _AGE <= 80, periods 1 to 5, _LOCK 0, VOLUMETOTAL >= 100
    Core::FMTSpec specification;
    specification.setBounds(Core::FMTPerBounds(Core::FMTsection::Action, 5, 1));
    specification.addBounds(Core::FMTAgeBounds(Core::FMTsection::Action, 80, 10));
    specification.addBounds(Core::FMTLockBounds(Core::FMTsection::Action, 0, 0));
    specification.addBounds(Core::FMTYldBounds(Core::FMTsection::Action, "VOLUMETOTAL",
        (std::numeric_limits<double>::max)(), 100.0));

    // Act
    const Core::FMTSpec unpickled = Testing::pickleAndUnpickle(specification);

    // Assert
    EXPECT_TRUE(unpickled == specification);
    EXPECT_EQ(std::string(unpickled), std::string(specification));
    EXPECT_EQ(unpickled.getYieldBound("VOLUMETOTAL").getLower(), 100.0);
    EXPECT_TRUE(unpickled.allowWithoutYield(3, 40, 0));
    EXPECT_FALSE(unpickled.allowWithoutYield(6, 40, 0));
}

TEST(FMTSpec, GivenASpecificationConstructedByDefaultWhenPickledThenUnpicklesAnEmptySpecification)
{
    // Arrange
    const Core::FMTSpec specification;

    // Act
    const Core::FMTSpec unpickled = Testing::pickleAndUnpickle(specification);

    // Assert
    EXPECT_TRUE(unpickled == specification);
    EXPECT_TRUE(unpickled.empty());
}
