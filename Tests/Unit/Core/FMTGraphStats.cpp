/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTGraphStats.h"

#include <gtest/gtest.h>

#include <string>
#include <vector>

// The statistics of a graph and of its matrix: FMTSrModel::buildPeriod gives those of each period it builds, and
// FMTGraph adds the differences of each build to its own (Graph namespace, outside Core).

namespace
{
    Graph::FMTGraphStats getStats(int p_first)
    {
        Graph::FMTGraphStats stats;
        stats.cols = p_first;
        stats.rows = p_first + 1;
        stats.vertices = p_first + 2;
        stats.edges = p_first + 3;
        stats.transfer_rows = p_first + 4;
        stats.output_rows = p_first + 5;
        stats.output_cols = p_first + 6;
        return stats;
    }

    std::vector<int> getValues(const Graph::FMTGraphStats& p_stats)
    {
        return { p_stats.cols, p_stats.rows, p_stats.vertices, p_stats.edges, p_stats.transfer_rows, p_stats.output_rows,
            p_stats.output_cols };
    }
}

TEST(FMTGraphStats, GivenNothingWhenDefaultConstructedThenEveryCountIsZero)
{
    // Arrange and Act
    const Graph::FMTGraphStats stats;

    // Assert
    EXPECT_EQ(getValues(stats), std::vector<int>(7, 0));
}

TEST(FMTGraphStats, GivenStatsWhenCopiedAndAssignedThenTheCopiesHaveTheSameCounts)
{
    // Arrange
    const Graph::FMTGraphStats stats = getStats(1);
    Graph::FMTGraphStats assigned;

    // Act
    const Graph::FMTGraphStats copy(stats);
    assigned = stats;

    // Assert
    EXPECT_EQ(getValues(copy), std::vector<int>({ 1, 2, 3, 4, 5, 6, 7 }));
    EXPECT_EQ(getValues(assigned), std::vector<int>({ 1, 2, 3, 4, 5, 6, 7 }));
}

TEST(FMTGraphStats, GivenTwoStatsWhenAddedAndSubtractedInPlaceThenEachCountIsAddedAndSubtracted)
{
    // Arrange
    Graph::FMTGraphStats added = getStats(1);
    Graph::FMTGraphStats subtracted = getStats(10);
    const Graph::FMTGraphStats other = getStats(1);

    // Act
    added += other;
    subtracted -= other;

    // Assert
    EXPECT_EQ(getValues(added), std::vector<int>({ 2, 4, 6, 8, 10, 12, 14 }));
    EXPECT_EQ(getValues(subtracted), std::vector<int>(7, 9));
}

// Observation, no defect announced: operator+ and operator- add to and subtract from their left operand, then return a
// copy of it. Their only use, the return of FMTGraph::naturalGrowth, subtracts from a copy made for it.
TEST(FMTGraphStats, GivenTwoStatsWhenAddedAndSubtractedThenTheResultIsAlsoTheirLeftOperand)
{
    // Arrange
    Graph::FMTGraphStats left = getStats(1);
    Graph::FMTGraphStats minuend = getStats(10);
    const Graph::FMTGraphStats right = getStats(1);

    // Act
    const Graph::FMTGraphStats sum = left + right;
    const Graph::FMTGraphStats difference = minuend - right;

    // Assert
    EXPECT_EQ(getValues(sum), std::vector<int>({ 2, 4, 6, 8, 10, 12, 14 }));
    EXPECT_EQ(getValues(left), getValues(sum));
    EXPECT_EQ(getValues(difference), std::vector<int>(7, 9));
    EXPECT_EQ(getValues(minuend), getValues(difference));
}

TEST(FMTGraphStats, GivenStatsThatDifferByOneCountWhenComparedThenTheyAreDifferent)
{
    // Arrange
    const Graph::FMTGraphStats stats = getStats(1);
    std::vector<Graph::FMTGraphStats> others(7, getStats(1));
    others.at(0).cols = 0;
    others.at(1).rows = 0;
    others.at(2).vertices = 0;
    others.at(3).edges = 0;
    others.at(4).transfer_rows = 0;
    others.at(5).output_rows = 0;
    others.at(6).output_cols = 0;

    // Act
    std::vector<bool> equal;
    std::vector<bool> different;
    for (const Graph::FMTGraphStats& other : others)
    {
        equal.push_back(stats == other);
        different.push_back(stats != other);
    }
    const bool equalToSame = (stats == getStats(1));
    const bool differentFromSame = (stats != getStats(1));

    // Assert
    EXPECT_EQ(equal, std::vector<bool>(7, false));
    EXPECT_EQ(different, std::vector<bool>(7, true));
    EXPECT_TRUE(equalToSame);
    EXPECT_FALSE(differentFromSame);
}

TEST(FMTGraphStats, GivenStatsWhenConvertedToStringThenNamesEachCount)
{
    // Arrange
    const Graph::FMTGraphStats stats = getStats(1);

    // Act
    const std::string text(stats);

    // Assert
    EXPECT_EQ(text, "Columns: 1 Rows: 2 Vertices: 3 Edges: 4 Transfer Rows: 5 Output Rows: 6 Output Columns: 7");
}
