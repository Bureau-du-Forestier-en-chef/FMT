/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTLineGraph.h"
#include "FMTActualDevelopment.h"
#include "FMTMask.h"
#include "FMTModel.h"
#include "FMTOutputNode.h"
#include "FMTTheme.h"

#include "TestTools.h"

#include <gtest/gtest.h>

#include <exception>
#include <string>
#include <vector>

// The diagnostic of the layout of a line graph, written in the errors of the spatial simulation (issue vector too
// long): a graph built by initialize is coherent, an empty graph has fewer than 2 periods, and a source asked to an
// empty graph raises vector too long with the layout of the graph in the error.

namespace
{
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

    // The messages of an exception and of the exceptions it nests, outermost first.
    std::string getMessages(const std::exception& p_exception)
    {
        std::string messages;
        Testing::visitNested(p_exception, [&messages](const std::exception& p_nested, std::size_t)
            {
            messages += std::string(p_nested.what()) + "\n";
            });
        return messages;
    }
}

TEST(FMTLineGraph, GivenAnInitializedGraphWhenItsIncoherencesAreAskedThenThereAreNone)
{
    const Graph::FMTLineGraph GRAPH = getInitializedGraph();

    const std::string INCOHERENCES = GRAPH.getIncoherences();

    EXPECT_EQ(INCOHERENCES, "");
}

TEST(FMTLineGraph, GivenACopyOfAnInitializedGraphWhenItsIncoherencesAreAskedThenThereAreNone)
{
    const Graph::FMTLineGraph GRAPH = getInitializedGraph();
    const Graph::FMTLineGraph COPY(GRAPH);

    const std::string INCOHERENCES = COPY.getIncoherences();

    EXPECT_EQ(INCOHERENCES, "");
}

TEST(FMTLineGraph, GivenAnEmptyGraphWhenItsIncoherencesAreAskedThenItHasFewerThanTwoPeriods)
{
    const Graph::FMTLineGraph GRAPH;

    const std::string INCOHERENCES = GRAPH.getIncoherences();

    EXPECT_NE(INCOHERENCES.find("fewer than 2 periods"), std::string::npos) << INCOHERENCES;
}

TEST(FMTLineGraph, GivenAnInitializedGraphWhenItsDiagnosticIsAskedThenItDescribesItsTwoPeriods)
{
    const Graph::FMTLineGraph GRAPH = getInitializedGraph();

    const std::string DIAGNOSTIC = GRAPH.getDiagnosticString();

    EXPECT_NE(DIAGNOSTIC.find("size() 2"), std::string::npos) << DIAGNOSTIC;
    EXPECT_NE(DIAGNOSTIC.find("periods 0x1 1x1"), std::string::npos) << DIAGNOSTIC;
    EXPECT_EQ(DIAGNOSTIC.find("incoherences"), std::string::npos) << DIAGNOSTIC;
}

TEST(FMTLineGraph, GivenAnEmptyGraphWhenItsDiagnosticIsAskedThenItDescribesItWithoutThrowing)
{
    const Graph::FMTLineGraph GRAPH;

    const std::string DIAGNOSTIC = GRAPH.getDiagnosticString();
    const std::string BASE = GRAPH.getBaseDiagnostic();

    EXPECT_NE(DIAGNOSTIC.find("size() 0"), std::string::npos) << DIAGNOSTIC;
    EXPECT_NE(DIAGNOSTIC.find("fewer than 2 periods"), std::string::npos) << DIAGNOSTIC;
    EXPECT_EQ(BASE, "no vertex");
}

TEST(FMTLineGraph, GivenAnEmptyGraphWhenASourceIsAskedThenTheErrorDescribesTheGraph)
{
    const Graph::FMTLineGraph GRAPH;
    const Models::FMTModel MODEL;
    const double AREA = 1.0;
    std::string messages;

    try
    {
        GRAPH.getSource(MODEL, Core::FMTOutputNode(), 1, Core::FMTTheme(), &AREA);
    }
    catch (const std::exception& error)
    {
        messages = getMessages(error);
    }

    EXPECT_NE(messages.find("vector too long"), std::string::npos) << messages;
    EXPECT_NE(messages.find("For node"), std::string::npos) << messages;
    EXPECT_NE(messages.find("at period 1"), std::string::npos) << messages;
    EXPECT_NE(messages.find("fewer than 2 periods"), std::string::npos) << messages;
}
