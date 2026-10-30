/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "PickleArchive.h"
#include "ParserRoundTrip.h"
#include "FMTModel.h"
#include "FMTOperator.h"
#include "FMTOutput.h"
#include "FMTOutputSource.h"

#include <gtest/gtest.h>

#include <string>
#include <vector>

// FMTOutput, its FMTOutputSource and its FMTOperator through the archive of the pickle of Python, which a model and its
// constraints carry. Python pickles an FMToutput with the suite of FMTTransition (defect 30, #413).

TEST(FMTOutput, GivenTheOutputsOfTWDLandWhenPickledThenUnpicklesTheSameOutputs)
{
    // Arrange
    const std::vector<Core::FMTOutput> outputs = Testing::readScenario("ROOT").getOutputs();

    // Act
    std::vector<Core::FMTOutput> unpickled;
    for (const Core::FMTOutput& output : outputs)
    {
        unpickled.push_back(Testing::pickleAndUnpickle(output));
    }

    // Assert
    ASSERT_FALSE(outputs.empty());
    ASSERT_EQ(unpickled.size(), outputs.size());
    for (size_t output = 0; output < outputs.size(); ++output)
    {
        EXPECT_TRUE(unpickled.at(output) == outputs.at(output)) << outputs.at(output).getName();
        EXPECT_EQ(unpickled.at(output).getDescription(), outputs.at(output).getDescription());
        EXPECT_EQ(std::string(unpickled.at(output)), std::string(outputs.at(output)));
    }
}

TEST(FMTOutput, GivenAnOutputConstructedByDefaultWhenPickledThenUnpicklesAnEmptyOutput)
{
    // Arrange
    const Core::FMTOutput output;

    // Act
    const Core::FMTOutput unpickled = Testing::pickleAndUnpickle(output);

    // Assert
    EXPECT_TRUE(unpickled == output);
    EXPECT_TRUE(unpickled.empty());
    EXPECT_EQ(unpickled.getName(), "");
}

// Defect 31 (#414) of Tests/ETAT.md: FMTOutput::serialize does not save m_group, the *GROUP of the output section
// that FMTOutputParser::write writes back; FMTOutput::operator== does not compare it.
TEST(FMTOutput, DISABLED_GivenAnOutputOfAGroupWhenPickledThenUnpicklesAnOutputOfTheSameGroup)
{
    // Arrange
    const Core::FMTOutput output("OVOLUME", "volume", "GROUPE1", std::vector<Core::FMTOutputSource>(),
        std::vector<Core::FMTOperator>());

    // Act
    const Core::FMTOutput unpickled = Testing::pickleAndUnpickle(output);

    // Assert
    EXPECT_EQ(unpickled.getGroup(), "GROUPE1");
}
