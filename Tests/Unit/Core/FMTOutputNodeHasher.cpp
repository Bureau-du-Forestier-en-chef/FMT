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

#include <map>
#include <string>
#include <unordered_set>
#include <vector>

// FMTOutputNodeHasher hashes a node by its values (FMTOutputNode::hashForValue): its source with its yield, its factor
// and its constant, which FMTOutputNodeValueComparator compares. With it, an unordered container of nodes keeps one node
// per values. Nothing in FMT uses it today. The nodes are those of Tests/Unit/Core/FMTOutputNode.cpp.

namespace
{
    std::vector<Core::FMTTheme> getThemes()
    {
        const std::vector<std::map<std::string, double>> NO_INDEXES;
        return {
            Core::FMTTheme({ "UNITE1", "UNITE2", "UNITE3" }, { "", "", "" }, { "UC" }, { { "UNITE1", "UNITE2" } },
                NO_INDEXES, 0, 0, ""),
            Core::FMTTheme({ "PEUPLEMENT1", "PEUPLEMENT2", "PEUPLEMENT3", "FEU" }, { "", "", "", "" },
                { "PROD", "POURPL" }, { { "PEUPLEMENT1", "PEUPLEMENT2", "PEUPLEMENT3" }, { "PEUPLEMENT2", "PEUPLEMENT3" } },
                NO_INDEXES, 1, 3, ""),
            Core::FMTTheme({ "UTR1", "UTR2", "UTR3" }, { "", "", "" }, { "UTA1", "UTA2" }, { { "UTR1", "UTR2" }, { "UTR3" } },
                NO_INDEXES, 2, 7, "") };
    }

    // The yield p_yield harvested by COUPETOTALE, times p_factor and p_constant.
    Core::FMTOutputNode getHarvestNode(double p_factor, double p_constant, const std::string& p_yield = "VOLUMETOTAL")
    {
        const Core::FMTOutputSource SOURCE(Core::FMTSpec(), Core::FMTMask("? PEUPLEMENT1 ?", getThemes()), Core::FMTotar::actual,
            p_yield, "COUPETOTALE");
        return Core::FMTOutputNode(SOURCE, Core::FMTOutputSource(Core::FMTotar::val, p_factor), p_constant);
    }
}

TEST(FMTOutputNodeHasher, GivenNodesOfTheSameValuesWhenHashedThenTheyHaveTheSameHash)
{
    // Arrange
    const Core::FMTOutputNodeHasher hasher;
    const Core::FMTOutputNode first = getHarvestNode(2.0, 3.0);
    const Core::FMTOutputNode second = getHarvestNode(2.0, 3.0);

    // Act
    const size_t firstHash = hasher(first);
    const size_t secondHash = hasher(second);

    // Assert
    EXPECT_EQ(firstHash, secondHash);
    EXPECT_EQ(firstHash, first.hashForValue());
}

TEST(FMTOutputNodeHasher, GivenNodesThatDifferByTheirConstantTheirFactorOrTheYieldOfTheirSourceWhenHashedThenTheirHashesDiffer)
{
    // Arrange
    const Core::FMTOutputNodeHasher hasher;
    const size_t hash = hasher(getHarvestNode(2.0, 3.0));

    // Act
    const size_t ofOtherConstant = hasher(getHarvestNode(2.0, 4.0));
    const size_t ofOtherFactor = hasher(getHarvestNode(5.0, 3.0));
    const size_t ofOtherYield = hasher(getHarvestNode(2.0, 3.0, "STEMS"));

    // Assert
    EXPECT_NE(hash, ofOtherConstant);
    EXPECT_NE(hash, ofOtherFactor);
    EXPECT_NE(hash, ofOtherYield);
}

TEST(FMTOutputNodeHasher, GivenNodesInAnUnorderedSetOfTheHasherAndTheValueComparatorWhenInsertedThenKeepsOneNodePerValues)
{
    // Arrange
    std::unordered_set<Core::FMTOutputNode, Core::FMTOutputNodeHasher, Core::FMTOutputNodeValueComparator> nodes;

    // Act
    nodes.insert(getHarvestNode(2.0, 3.0));
    nodes.insert(getHarvestNode(2.0, 3.0));
    nodes.insert(getHarvestNode(2.0, 4.0));
    nodes.insert(getHarvestNode(2.0, 3.0, "STEMS"));

    // Assert
    EXPECT_EQ(nodes.size(), 3u);
    EXPECT_TRUE(nodes.find(getHarvestNode(2.0, 3.0)) != nodes.end());
    EXPECT_TRUE(nodes.find(getHarvestNode(2.0, 5.0)) == nodes.end());
}
