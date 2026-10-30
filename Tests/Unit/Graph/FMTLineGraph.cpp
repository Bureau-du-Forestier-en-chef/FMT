/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTLineGraph.h"
#include "FMTAction.h"
#include "FMTActualDevelopment.h"
#include "FMTAgeBounds.h"
#include "FMTFork.h"
#include "FMTLifespans.h"
#include "FMTMask.h"
#include "FMTModel.h"
#include "FMTOutput.h"
#include "FMTOutputNode.h"
#include "FMTSerie.h"
#include "FMTSpec.h"
#include "FMTTheme.h"
#include "FMTTransition.h"
#include "FMTTransitionMask.h"
#include "FMTYields.h"

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

// The *ACTIONSERIES of a model in a line graph, as the spatial simulation asks them before operating a cell: the
// rule of FMTGraph::build, where a stand on a serie waits for the next action of its serie (issue of model 8451,
// ACTPL -> APL broken by ACTPL;APLCC in the spatial simulation). ACTPL is operable at any age, APL only on A1 at
// age 0 and APLCC, its competitor, everywhere at age 0.

namespace
{
    Core::FMTAction getSerieAction(const std::string& p_name, bool p_respectLock, const std::string& p_mask,
        int p_lowerAge, int p_upperAge)
    {
        Core::FMTSpec spec;
        spec.addBounds(Core::FMTAgeBounds(Core::FMTsection::Action, p_upperAge, p_lowerAge));
        Core::FMTAction action(p_name, p_respectLock, true);
        action.push_back(Core::FMTMask(p_mask, getThemes()), spec);
        action.update();
        return action;
    }

    Core::FMTTransition getSameStandTransition(const std::string& p_name)
    {
        Core::FMTFork fork;
        fork.add(Core::FMTTransitionMask("?", getThemes(), 100.0));
        Core::FMTTransition transition(p_name);
        transition.push_back(Core::FMTMask("?", getThemes()), fork);
        transition.update();
        return transition;
    }

    // *ACTIONSERIES ACTPL -> APL on a model of 5 periods.
    Models::FMTModel getSerieModel()
    {
        std::vector<Core::FMTAction> actions = {
            getSerieAction("ACTPL", true, "?", 0, 1000),
            getSerieAction("APL", false, "A1", 0, 0),
            getSerieAction("APLCC", false, "?", 0, 0) };
        const std::vector<Core::FMTSerie> SERIES(1, Core::FMTSerie(std::vector<std::string>({ "ACTPL", "APL" }), false, false));
        std::vector<Core::FMTTransition> transitions;
        for (Core::FMTAction& action : actions)
        {
            action.setSeries(SERIES);
            transitions.push_back(getSameStandTransition(action.getName()));
        }
        Core::FMTYields yields;
        yields.update();
        Core::FMTLifespans lifespans;
        lifespans.push_back(Core::FMTMask("?", getThemes()), 1000);
        lifespans.update();
        const std::vector<Core::FMTActualDevelopment> AREA(1,
            Core::FMTActualDevelopment(Core::FMTMask("A1", getThemes()), 5, 0, 1.0));
        Models::FMTModel model(AREA, getThemes(), actions, transitions, yields, lifespans, "series",
            std::vector<Core::FMTOutput>());
        model.setParameter(Models::FMTintmodelparameters::LENGTH, 5);
        return model;
    }

    int getActionId(const Models::FMTModel& p_model, const std::string& p_name)
    {
        const std::vector<Core::FMTAction> ACTIONS = p_model.getActions();
        for (size_t id = 0; id < ACTIONS.size(); ++id)
        {
            if (ACTIONS.at(id).getName() == p_name)
            {
                return static_cast<int>(id);
            }
        }
        return -1;
    }

    // A stand of age 5 in the period 1, then the actions of p_history, a "GROW" moving it to the next period.
    Graph::FMTLineGraph getSerieGraph(const Models::FMTModel& p_model, const std::string& p_stand,
        const std::vector<std::string>& p_history)
    {
        const Core::FMTActualDevelopment DEVELOPMENT(Core::FMTMask(p_stand, getThemes()), 5, 0, 1.0);
        Graph::FMTLineGraph graph(10, DEVELOPMENT);
        const std::vector<Core::FMTAction> ACTIONS = p_model.getActions();
        const std::vector<Core::FMTTransition> TRANSITIONS = p_model.getTransitions();
        for (const std::string& step : p_history)
        {
            if (step == "GROW")
            {
                graph.grow();
            }
            else
            {
                const int ID = getActionId(p_model, step);
                graph.operate(ACTIONS.at(ID), ID, TRANSITIONS.at(ID), p_model.getYields(), getThemes());
            }
        }
        return graph;
    }

    bool isAllowed(const Models::FMTModel& p_model, const Graph::FMTLineGraph& p_graph, const std::string& p_action)
    {
        return p_graph.isAllowedBySeries(getActionId(p_model, p_action), p_model);
    }
}

TEST(FMTLineGraph, GivenAStandOnNoSerieWhenTheCompetitorIsAskedThenItIsAllowed)
{
    const Models::FMTModel MODEL = getSerieModel();
    const Graph::FMTLineGraph GRAPH = getSerieGraph(MODEL, "A1", {});

    EXPECT_TRUE(isAllowed(MODEL, GRAPH, "APLCC"));
    EXPECT_TRUE(isAllowed(MODEL, GRAPH, "ACTPL"));
}

TEST(FMTLineGraph, GivenAStandJustOperatedByTheFirstActionOfASerieWhenActionsAreAskedThenOnlyTheNextOneIsAllowed)
{
    const Models::FMTModel MODEL = getSerieModel();
    const Graph::FMTLineGraph GRAPH = getSerieGraph(MODEL, "A1", { "ACTPL" });

    EXPECT_TRUE(isAllowed(MODEL, GRAPH, "APL"));
    EXPECT_FALSE(isAllowed(MODEL, GRAPH, "APLCC"));
}

TEST(FMTLineGraph, GivenAStandThatEndedItsSerieWhenTheCompetitorIsAskedThenItIsAllowed)
{
    const Models::FMTModel MODEL = getSerieModel();
    const Graph::FMTLineGraph GRAPH = getSerieGraph(MODEL, "A1", { "ACTPL", "APL" });

    EXPECT_TRUE(isAllowed(MODEL, GRAPH, "APLCC"));
}

TEST(FMTLineGraph, GivenAStandOnASerieWhoseNextActionIsNeverOperableWhenTheCompetitorIsAskedThenItIsAllowed)
{
    const Models::FMTModel MODEL = getSerieModel();
    const Graph::FMTLineGraph GRAPH = getSerieGraph(MODEL, "A2", { "ACTPL" });

    EXPECT_TRUE(isAllowed(MODEL, GRAPH, "APLCC"));
}

TEST(FMTLineGraph, GivenAStandBackOnASerieRightAfterTheLastActionOfItsPreviousSerieWhenTheCompetitorIsAskedThenItIsRefused)
{
    const Models::FMTModel MODEL = getSerieModel();
    const Graph::FMTLineGraph GRAPH = getSerieGraph(MODEL, "A1", { "ACTPL", "APL", "GROW", "ACTPL" });

    EXPECT_TRUE(isAllowed(MODEL, GRAPH, "APL"));
    EXPECT_FALSE(isAllowed(MODEL, GRAPH, "APLCC"));
}
