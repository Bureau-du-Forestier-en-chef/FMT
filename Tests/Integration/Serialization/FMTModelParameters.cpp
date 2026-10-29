/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "PickleArchive.h"
#include "FMTModelParameters.h"

#include <gtest/gtest.h>

#include <vector>

// Models::FMTModelParameters through the archive of the pickle of Python: the parameters that a model saves with it.

TEST(FMTModelParameters, GivenNumericAndBooleanParametersWhenPickledThenUnpicklesTheSameValues)
{
    // Arrange
    Models::FMTModelParameters parameters;
    parameters.setIntParameter(Models::FMTintmodelparameters::LENGTH, 7);
    parameters.setDblParameter(Models::FMTdblmodelparameters::TOLERANCE, 0.01);
    parameters.setBoolParameter(Models::FMTboolmodelparameters::POSTSOLVE, false);
    parameters.setPeriodCompressTime(2, 3);

    // Act
    const Models::FMTModelParameters unpickled = Testing::pickleAndUnpickle(parameters);

    // Assert
    EXPECT_EQ(unpickled.getIntParameter(Models::FMTintmodelparameters::LENGTH), 7);
    EXPECT_DOUBLE_EQ(unpickled.getDblParameter(Models::FMTdblmodelparameters::TOLERANCE), 0.01);
    EXPECT_FALSE(unpickled.getBoolParameter(Models::FMTboolmodelparameters::POSTSOLVE));
    EXPECT_EQ(unpickled.getCompressTime(), parameters.getCompressTime());
    EXPECT_EQ(unpickled.getPeriodCompressTime(2), 3);
}

// Defect 31 (#414) of Tests/ETAT.md: FMTModelParameters::serialize does not save the text parameters, the files
// of the cold and warm start parameters of the solver and the working directory of the simulated annealing.
TEST(FMTModelParameters, DISABLED_GivenATextParameterWhenPickledThenUnpicklesTheSameText)
{
    // Arrange
    Models::FMTModelParameters parameters;
    parameters.setStrParameter(Models::FMTstrmodelparameters::WORKING_DIRECTORY, "annealing");

    // Act
    const Models::FMTModelParameters unpickled = Testing::pickleAndUnpickle(parameters);

    // Assert
    EXPECT_EQ(unpickled.getStrParameter(Models::FMTstrmodelparameters::WORKING_DIRECTORY), "annealing");
}
