/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTOutputNode.h"
#include "FMTMask.h"
#include "FMTOutputSource.h"
#include "FMTSpec.h"
#include "FMTTheme.h"
#include "FMTutility.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <string>
#include <vector>

// FMTOutputNodeOriginComparator orders nodes by the output their source comes from (FMTOutputSource::getOutputOrigin),
// as FMTOutput::getNodes sorts the nodes of an output made of other outputs. The nodes are those of
// Tests/Unit/Core/FMTOutputNode.cpp, on one theme.

namespace
{
    std::vector<Core::FMTTheme> getThemes()
    {
        return { Core::FMTTheme({ "A1", "A2" }, 0, 0, "THEME1") };
    }

    // The volume harvested by COUPE in the output of origin p_origin.
    Core::FMTOutputNode getNodeOfOrigin(int p_origin)
    {
        Core::FMTOutputSource source(Core::FMTSpec(), Core::FMTMask("?", getThemes()), Core::FMTotar::actual, "VOLUME", "COUPE");
        source.setOutputOrigin(p_origin);
        return Core::FMTOutputNode(source, Core::FMTOutputSource(Core::FMTotar::val, 1.0), 1.0);
    }
}

TEST(FMTOutputNodeOriginComparator, GivenNodesOfSeveralOutputsWhenSortedThenComeInTheOrderOfTheirOutputs)
{
    // Arrange
    std::vector<Core::FMTOutputNode> nodes = { getNodeOfOrigin(2), getNodeOfOrigin(0), getNodeOfOrigin(1) };

    // Act
    std::sort(nodes.begin(), nodes.end(), Core::FMTOutputNodeOriginComparator());

    // Assert
    std::vector<int> origins;
    for (const Core::FMTOutputNode& node : nodes)
    {
        origins.push_back(node.getOutputId());
    }
    EXPECT_EQ(origins, std::vector<int>({ 0, 1, 2 }));
}

// A node without origin (-1, the default of a source) comes before the nodes of the outputs.
TEST(FMTOutputNodeOriginComparator, GivenNodesOfTheSameOutputWhenComparedThenNeitherComesFirst)
{
    // Arrange
    const Core::FMTOutputNodeOriginComparator comparator;
    const Core::FMTOutputNode first = getNodeOfOrigin(1);
    Core::FMTOutputNode second = getNodeOfOrigin(1);
    second.constant = 5.0;

    // Act
    const bool firstBefore = comparator(first, second);
    const bool secondBefore = comparator(second, first);

    // Assert
    EXPECT_FALSE(firstBefore);
    EXPECT_FALSE(secondBefore);
    EXPECT_TRUE(comparator(getNodeOfOrigin(-1), first));
}
