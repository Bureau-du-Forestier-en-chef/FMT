/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTOutput.h"
#include "FMTMask.h"
#include "FMTOutputSource.h"
#include "FMTSpec.h"
#include "FMTTheme.h"
#include "FMTutility.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <iterator>
#include <string>
#include <vector>

// FMTOutputComparator finds an output by its name, as FMTModel, FMTOptimizationParser and FMTOutputParser look for the
// output that a constraint or another output names. The outputs are those of Tests/Unit/Core/FMTOutput.cpp.

namespace
{
    std::vector<Core::FMTTheme> getThemes()
    {
        return { Core::FMTTheme({ "A1", "A2" }, 0, 0, "THEME1") };
    }

    // The output p_name of the yield p_yield harvested by COUPE.
    Core::FMTOutput getOutput(const std::string& p_name, const std::string& p_yield)
    {
        const Core::FMTOutputSource SOURCE(Core::FMTSpec(), Core::FMTMask("?", getThemes()), Core::FMTotar::actual, p_yield,
            "COUPE");
        return Core::FMTOutput(p_name, "", "", { SOURCE }, {});
    }

    // The position of the first output named p_name, or the number of outputs.
    long getFirst(const std::vector<Core::FMTOutput>& p_outputs, const std::string& p_name)
    {
        return static_cast<long>(std::distance(p_outputs.begin(),
            std::find_if(p_outputs.begin(), p_outputs.end(), Core::FMTOutputComparator(p_name))));
    }
}

TEST(FMTOutputComparator, GivenOutputsWhenFindIfANameThenFindsTheOutputOfThatName)
{
    // Arrange
    const std::vector<Core::FMTOutput> outputs = { getOutput("OVOL", "VOLUME"), getOutput("OSTEMS", "STEMS") };

    // Act
    const long stems = getFirst(outputs, "OSTEMS");

    // Assert
    EXPECT_EQ(stems, 1);
    EXPECT_TRUE(Core::FMTOutputComparator("OVOL")(outputs.at(0)));
}

// The names are compared as they are: the parsers write them in capitals before they look for them.
TEST(FMTOutputComparator, GivenANameOfAnotherCaseOrOfNoOutputWhenFindIfThenFindsNothing)
{
    // Arrange
    const std::vector<Core::FMTOutput> outputs = { getOutput("OVOL", "VOLUME"), getOutput("OSTEMS", "STEMS") };

    // Act
    const long lowerCase = getFirst(outputs, "ovol");
    const long absent = getFirst(outputs, "OAREA");
    const long empty = getFirst(outputs, "");

    // Assert
    EXPECT_EQ(lowerCase, 2);
    EXPECT_EQ(absent, 2);
    EXPECT_EQ(empty, 2);
}

TEST(FMTOutputComparator, GivenTwoOutputsOfTheSameNameWhenFindIfThenFindsTheFirst)
{
    // Arrange
    const std::vector<Core::FMTOutput> outputs = { getOutput("OVOL", "VOLUME"), getOutput("OVOL", "STEMS") };

    // Act
    const long first = getFirst(outputs, "OVOL");

    // Assert
    EXPECT_EQ(first, 0);
    EXPECT_EQ(outputs.at(0).getSources().at(0).getYield(), "VOLUME");
}
