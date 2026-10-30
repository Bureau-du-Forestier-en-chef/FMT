/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "PickleArchive.h"
#include "ParserRoundTrip.h"
#include "FMTConstraint.h"
#include "FMTModel.h"

#include <gtest/gtest.h>

#include <string>
#include <vector>

// FMTConstraint through the archive of the pickle of Python: the objective and the constraints of a model, an output
// with a specification and a type.

TEST(FMTConstraint, GivenTheObjectiveAndTheConstraintsOfTheLPScenarioWhenPickledThenUnpicklesTheSameConstraints)
{
    // Arrange: the scenario LP of TWD_land has an objective and constraints
    const std::vector<Core::FMTConstraint> constraints = Testing::readScenario("LP").getConstraints();

    // Act
    std::vector<Core::FMTConstraint> unpickled;
    for (const Core::FMTConstraint& constraint : constraints)
    {
        unpickled.push_back(Testing::pickleAndUnpickle(constraint));
    }

    // Assert
    ASSERT_GT(constraints.size(), 1u);
    ASSERT_EQ(unpickled.size(), constraints.size());
    for (size_t constraint = 0; constraint < constraints.size(); ++constraint)
    {
        EXPECT_TRUE(unpickled.at(constraint) == constraints.at(constraint)) << constraints.at(constraint).getName();
        EXPECT_EQ(unpickled.at(constraint).getConstraintType(), constraints.at(constraint).getConstraintType());
        EXPECT_EQ(std::string(unpickled.at(constraint)), std::string(constraints.at(constraint)));
    }
}

TEST(FMTConstraint, GivenAConstraintConstructedByDefaultWhenPickledThenUnpicklesTheSameConstraint)
{
    // Arrange
    const Core::FMTConstraint constraint;

    // Act
    const Core::FMTConstraint unpickled = Testing::pickleAndUnpickle(constraint);

    // Assert
    EXPECT_TRUE(unpickled == constraint);
    EXPECT_EQ(unpickled.getConstraintType(), constraint.getConstraintType());
}
