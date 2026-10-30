/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTSpatialGraphs.h"
#include "FMTActualDevelopment.h"
#include "FMTGraphInfo.h"
#include "FMTLineGraph.h"
#include "FMTMask.h"
#include "FMTTheme.h"

#include <gtest/gtest.h>

#include <cstddef>
#include <map>
#include <string>
#include <vector>

// The report of the graphs in error, written in the errors of the spatial simulation (issue vector too long): the
// graphs of each constraint family that break the invariants of their layout, the first 20 detailed. Each family
// here holds one graph: inserted in an empty map, the graph is never compared, so an empty graph can be one.

namespace
{
    using Families = std::vector<std::map<Graph::FMTLineGraph, Spatial::FMTGraphInfo>>;

    std::vector<Core::FMTTheme> getThemes()
    {
        return { Core::FMTTheme({ "A1", "A2" }, 0, 0, "THEME1") };
    }

    // A base graph: one development of age 1, grown to the period 1 by initialize.
    Graph::FMTLineGraph getInitializedGraph()
    {
        const Core::FMTActualDevelopment DEVELOPMENT(Core::FMTMask("A1", getThemes()), 1, 0, 1.0);
        return Graph::FMTLineGraph(2, DEVELOPMENT);
    }

    // A family of one graph of id p_id.
    std::map<Graph::FMTLineGraph, Spatial::FMTGraphInfo> getFamily(const Graph::FMTLineGraph& p_graph, size_t p_id)
    {
        std::map<Graph::FMTLineGraph, Spatial::FMTGraphInfo> family;
        family.emplace(p_graph, Spatial::FMTGraphInfo(p_id));
        return family;
    }

    size_t countOccurrences(const std::string& p_text, const std::string& p_searched)
    {
        size_t count = 0;
        for (size_t position = p_text.find(p_searched); position != std::string::npos;
            position = p_text.find(p_searched, position + p_searched.size()))
        {
            ++count;
        }
        return count;
    }
}

TEST(FMTSpatialGraphs, GivenCoherentGraphsWhenTheGraphsInErrorAreReportedThenNoneIsListed)
{
    Families families;
    families.push_back(getFamily(getInitializedGraph(), 0));
    families.push_back(getFamily(getInitializedGraph(), 1));

    const std::string REPORT = Spatial::FMTSpatialGraphs::getGraphsInErrorReport(families, 2, nullptr);

    EXPECT_NE(REPORT.find("Graphs in error: 0 of 2 graphs in 2 families"), std::string::npos) << REPORT;
    EXPECT_EQ(REPORT.find("| graph id"), std::string::npos) << REPORT;
}

TEST(FMTSpatialGraphs, GivenAnEmptyGraphWhenTheGraphsInErrorAreReportedThenItIsListedWithItsIncoherences)
{
    Families families;
    families.push_back(getFamily(Graph::FMTLineGraph(), 7));
    families.push_back(getFamily(getInitializedGraph(), 8));

    const std::string REPORT = Spatial::FMTSpatialGraphs::getGraphsInErrorReport(families, 9, nullptr);

    EXPECT_NE(REPORT.find("Graphs in error: 1 of 2"), std::string::npos) << REPORT;
    EXPECT_NE(REPORT.find("graph id 7 of family 0"), std::string::npos) << REPORT;
    EXPECT_NE(REPORT.find("base (no vertex)"), std::string::npos) << REPORT;
    EXPECT_NE(REPORT.find("fewer than 2 periods"), std::string::npos) << REPORT;
    EXPECT_EQ(REPORT.find("| graph id 8"), std::string::npos) << REPORT;
}

TEST(FMTSpatialGraphs, GivenMoreThanTwentyGraphsInErrorWhenTheyAreReportedThenOnlyTwentyAreDetailed)
{
    Families families;
    for (size_t id = 0; id < 25; ++id)
    {
        families.push_back(getFamily(Graph::FMTLineGraph(), id));
    }

    const std::string REPORT = Spatial::FMTSpatialGraphs::getGraphsInErrorReport(families, 25, nullptr);

    EXPECT_NE(REPORT.find("Graphs in error: 25 of 25"), std::string::npos) << REPORT;
    EXPECT_EQ(countOccurrences(REPORT, "| graph id "), 20u) << REPORT;
}
