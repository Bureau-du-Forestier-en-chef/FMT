/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "PickleArchive.h"
#include "FMTGraphStats.h"

#include <gtest/gtest.h>

// Graph::FMTGraphStats through the archive of the pickle of Python: the counts of a graph and of its matrix, which the
// graph of a model saves with it.

TEST(FMTGraphStats, GivenTheStatsOfAGraphWhenPickledThenUnpicklesTheSameCounts)
{
    // Arrange: the counts after two periods of the graph of TWD_land without yields
    Graph::FMTGraphStats stats;
    stats.cols = 100;
    stats.rows = 52;
    stats.vertices = 96;
    stats.edges = 116;
    stats.transfer_rows = 52;
    stats.output_rows = 3;
    stats.output_cols = 4;

    // Act
    const Graph::FMTGraphStats unpickled = Testing::pickleAndUnpickle(stats);

    // Assert
    EXPECT_TRUE(unpickled == stats);
    EXPECT_EQ(unpickled.cols, 100);
    EXPECT_EQ(unpickled.rows, 52);
    EXPECT_EQ(unpickled.vertices, 96);
    EXPECT_EQ(unpickled.edges, 116);
    EXPECT_EQ(unpickled.transfer_rows, 52);
    EXPECT_EQ(unpickled.output_rows, 3);
    EXPECT_EQ(unpickled.output_cols, 4);
}
