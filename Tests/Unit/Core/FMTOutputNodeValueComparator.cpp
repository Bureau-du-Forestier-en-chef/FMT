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
#include <vector>

// FMTOutputNodeValueComparator tells whether two nodes have the same values (FMTOutputNode::isSameValues): the same
// source with its yield and its values, the same factor and the same constant. Its documentation says it returns true
// when the first node is less than the second: it is an equality, which a node satisfies with itself, so that it serves
// an unordered container with FMTOutputNodeHasher (Tests/Unit/Core/FMTOutputNodeHasher.cpp), not an ordered one.
// Nothing in FMT uses it today. The nodes are those of Tests/Unit/Core/FMTOutputNode.cpp.

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

TEST(FMTOutputNodeValueComparator, GivenNodesOfTheSameSourceFactorAndConstantWhenComparedThenReturnsTrue)
{
    // Arrange
    const Core::FMTOutputNodeValueComparator comparator;
    const Core::FMTOutputNode node = getHarvestNode(2.0, 3.0);

    // Act
    const bool withAnEqualNode = comparator(node, getHarvestNode(2.0, 3.0));
    const bool withItself = comparator(node, node);

    // Assert
    EXPECT_TRUE(withAnEqualNode);
    EXPECT_TRUE(withItself);
}

TEST(FMTOutputNodeValueComparator, GivenNodesThatDifferByOneValueWhenComparedThenReturnsFalse)
{
    // Arrange
    const Core::FMTOutputNodeValueComparator comparator;
    const Core::FMTOutputNode node = getHarvestNode(2.0, 3.0);
    Core::FMTOutputNode average = getHarvestNode(2.0, 3.0);
    average.source.setAverage();

    // Act
    const bool withOtherConstant = comparator(node, getHarvestNode(2.0, 4.0));
    const bool withOtherFactor = comparator(node, getHarvestNode(5.0, 3.0));
    const bool withOtherYield = comparator(node, getHarvestNode(2.0, 3.0, "STEMS"));
    const bool withAnAverage = comparator(node, average);

    // Assert
    EXPECT_FALSE(withOtherConstant);
    EXPECT_FALSE(withOtherFactor);
    EXPECT_FALSE(withOtherYield);
    EXPECT_FALSE(withAnAverage);
}
