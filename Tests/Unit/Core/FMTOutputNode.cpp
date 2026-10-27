/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTOutputNode.h"
#include "FMTAction.h"
#include "FMTAgeBounds.h"
#include "FMTMask.h"
#include "FMTOutputSource.h"
#include "FMTPerBounds.h"
#include "FMTSpec.h"
#include "FMTTheme.h"
#include "FMTutility.h"

#include <gtest/gtest.h>

#include <limits>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

// A node of an output, as FMTOutput::getNodes gives them to the graph: a source (Tests/Unit/Core/FMTOutputSource.cpp),
// the factor that multiplies it (a constant or a time yield) and a constant. setToGraph chooses the periods of the graph
// where FMTGraph::getNode looks for the developments of the source, and returns the period of the node, or -1 when the
// node needs no development. The themes are those of Tests/Unit/Core/FMTTheme.cpp.

namespace
{
    const int NO_UPPER_BOUND = (std::numeric_limits<int>::max)();

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

    Core::FMTMask getMask(const std::string& p_mask)
    {
        return Core::FMTMask(p_mask, getThemes());
    }

    // <mask> <action> <yield> of an output.
    Core::FMTOutputSource getActionSource(const std::string& p_mask, const std::string& p_action, const std::string& p_yield)
    {
        return Core::FMTOutputSource(Core::FMTSpec(), getMask(p_mask), Core::FMTotar::actual, p_yield, p_action);
    }

    // <mask> _INVENT <yield> of an output: the inventory at the beginning of the next period.
    Core::FMTOutputSource getInventorySource(const std::string& p_mask, const std::string& p_yield)
    {
        return Core::FMTOutputSource(Core::FMTSpec(), getMask(p_mask), Core::FMTotar::inventory, p_yield);
    }

    // p_source restricted to the periods p_lower to p_upper (#<lower>..<upper> in the output section).
    Core::FMTOutputSource getOfPeriods(Core::FMTOutputSource p_source, int p_lower, int p_upper)
    {
        p_source.setBounds(Core::FMTPerBounds(Core::FMTsection::Outputs, p_upper, p_lower));
        return p_source;
    }

    Core::FMTOutputSource getConstant(double p_value)
    {
        return Core::FMTOutputSource(Core::FMTotar::val, p_value);
    }

    // The volume harvested by COUPETOTALE, times p_factor and p_constant.
    Core::FMTOutputNode getHarvestNode(double p_factor = 1.0, double p_constant = 1.0)
    {
        return Core::FMTOutputNode(getActionSource("? PEUPLEMENT1 ?", "COUPETOTALE", "VOLUMETOTAL"), getConstant(p_factor),
            p_constant);
    }

    // The periods that setToGraph targets for p_node at the period p_period of a graph of p_maxPeriod periods, then the
    // period it returns.
    struct Targets
    {
        std::vector<int> periods;
        int nodePeriod;
    };

    Targets setToGraph(Core::FMTOutputNode& p_node, int p_period, int p_maxPeriod)
    {
        Targets targets;
        targets.nodePeriod = p_node.setToGraph(targets.periods, p_period, p_maxPeriod);
        return targets;
    }

    // An action of the mask ? PROD ?, operable from age 1, of the aggregates p_aggregates.
    Core::FMTAction getAction(const std::string& p_name, const std::vector<std::string>& p_aggregates = {})
    {
        Core::FMTAction action(p_name, true, false);
        Core::FMTSpec spec;
        spec.addBounds(Core::FMTAgeBounds(Core::FMTsection::Action, NO_UPPER_BOUND, 1));
        action.push_back(getMask("? PROD ?"), spec);
        for (const std::string& aggregate : p_aggregates)
        {
            action.pushAggregate(aggregate);
        }
        action.update();
        return action;
    }
}

TEST(FMTOutputNode, GivenASourceAFactorAndAConstantWhenConstructedThenKeepsThem)
{
    // Arrange
    const Core::FMTOutputSource source = getActionSource("? PEUPLEMENT1 ?", "COUPETOTALE", "VOLUMETOTAL");

    // Act
    const Core::FMTOutputNode node(source, getConstant(2.0), 3.0);

    // Assert
    EXPECT_TRUE(node.source.isEqualByValue(source));
    EXPECT_DOUBLE_EQ(node.factor.getValue(), 2.0);
    EXPECT_DOUBLE_EQ(node.constant, 3.0);
    EXPECT_TRUE(node.isActionBased());
}

// The general node of the scheduler heuristic: the area of the developments of the mask operated by the action.
TEST(FMTOutputNode, GivenAMaskAndAnActionWhenConstructedAsAGeneralNodeThenTargetsTheAreaOfTheActionTimesOne)
{
    // Arrange
    const Core::FMTMask mask = getMask("UNITE1 ? ?");

    // Act
    const Core::FMTOutputNode node(mask, "COUPES");

    // Assert
    EXPECT_TRUE(node.source.useOutEdges());
    EXPECT_EQ(node.source.getAction(), "COUPES");
    EXPECT_TRUE(node.source.getYield().empty());
    EXPECT_EQ(std::string(node.source.getMask()), "UNITE1 ? ?");
    EXPECT_DOUBLE_EQ(node.factor.getValue(), 1.0);
    EXPECT_DOUBLE_EQ(node.constant, 1.0);
}

TEST(FMTOutputNode, GivenNodesWhenIsNullThenTrueForAFactorOrAConstantOfZeroOnly)
{
    // Arrange
    const Core::FMTOutputNode zeroFactor = getHarvestNode(0.0, 1.0);
    const Core::FMTOutputNode zeroConstant = getHarvestNode(2.0, 0.0);
    const Core::FMTOutputNode nonZero = getHarvestNode(2.0, 1.0);
    const Core::FMTOutputNode timeYieldFactor(getActionSource("? ? ?", "COUPETOTALE", "VOLUMETOTAL"),
        Core::FMTOutputSource(Core::FMTotar::timeyld, 0.0, "PERIODVALUE"), 1.0);

    // Act
    const std::vector<bool> isNull = { zeroFactor.isNull(), zeroConstant.isNull(), nonZero.isNull(), timeYieldFactor.isNull() };

    // Assert
    EXPECT_EQ(isNull, std::vector<bool>({ true, true, false, false }));
}

// The factor of a default node is a constant without value (Tests/Unit/Core/FMTOutputSource.cpp): isNull reads it.
TEST(FMTOutputNode, GivenADefaultNodeWhenIsNullThenRaisesAnOutOfRange)
{
    // Arrange
    const Core::FMTOutputNode node;

    // Act and Assert
    EXPECT_DOUBLE_EQ(node.constant, 0.0);
    EXPECT_THROW(node.isNull(), std::out_of_range);
}

TEST(FMTOutputNode, GivenPeriodBoundsWhenSinglePeriodMultiPeriodAndIsPastPeriodThenTellWhichPeriodsTheNodeCovers)
{
    // Arrange
    const Core::FMTOutputSource source = getActionSource("? ? ?", "COUPETOTALE", "VOLUMETOTAL");
    const Core::FMTOutputNode withoutPeriod(source, getConstant(1.0), 1.0);
    const Core::FMTOutputNode ofOnePeriod(getOfPeriods(source, 2, 2), getConstant(1.0), 1.0);
    const Core::FMTOutputNode ofThreePeriods(getOfPeriods(source, 1, 3), getConstant(1.0), 1.0);
    const Core::FMTOutputNode ofThePreviousPeriod(getOfPeriods(source, -1, -1), getConstant(1.0), 1.0);

    // Act
    const std::vector<bool> single = { withoutPeriod.singlePeriod(), ofOnePeriod.singlePeriod(), ofThreePeriods.singlePeriod(),
        ofThePreviousPeriod.singlePeriod() };
    const std::vector<bool> multi = { withoutPeriod.multiPeriod(), ofOnePeriod.multiPeriod(), ofThreePeriods.multiPeriod(),
        ofThePreviousPeriod.multiPeriod() };
    const std::vector<bool> past = { withoutPeriod.isPastPeriod(), ofOnePeriod.isPastPeriod(), ofThreePeriods.isPastPeriod(),
        ofThePreviousPeriod.isPastPeriod() };

    // Assert
    EXPECT_EQ(single, std::vector<bool>({ false, true, false, true }));
    EXPECT_EQ(multi, std::vector<bool>({ false, false, true, false }));
    EXPECT_EQ(past, std::vector<bool>({ false, false, false, true }));
}

TEST(FMTOutputNode, GivenANodeWhenSetPeriodThenTheCopyCoversThatPeriodOnlyAndTheNodeIsUnchanged)
{
    // Arrange
    const Core::FMTOutputNode node = getHarvestNode();

    // Act
    const Core::FMTOutputNode ofPeriod4 = node.setPeriod(4);

    // Assert
    EXPECT_TRUE(ofPeriod4.singlePeriod());
    EXPECT_EQ(ofPeriod4.source.getPeriodLowerBound(), 4);
    EXPECT_EQ(ofPeriod4.factor.getPeriodUpperBound(), 4);
    EXPECT_TRUE(node.source.emptyPeriod());
}

TEST(FMTOutputNode, GivenANodeWithoutPeriodWhenSetToGraphThenTargetsThePeriodAsked)
{
    // Arrange
    Core::FMTOutputNode node = getHarvestNode();

    // Act
    const Targets targets = setToGraph(node, 3, 10);

    // Assert
    EXPECT_EQ(targets.nodePeriod, 3);
    EXPECT_EQ(targets.periods, std::vector<int>({ 3 }));
}

// _INVENT without an action is the inventory at the beginning of the next period.
TEST(FMTOutputNode, GivenAnInventoryNodeWithoutActionWhenSetToGraphThenTargetsTheNextPeriod)
{
    // Arrange
    Core::FMTOutputNode node(getInventorySource("? ? ?", "VOLUMETOTAL"), getConstant(1.0), 1.0);

    // Act
    const Targets targets = setToGraph(node, 3, 10);

    // Assert
    EXPECT_EQ(targets.nodePeriod, 4);
    EXPECT_EQ(targets.periods, std::vector<int>({ 4 }));
}

TEST(FMTOutputNode, GivenANodeOfOnePeriodWhenSetToGraphThenTargetsItsPeriodWhateverThePeriodAsked)
{
    // Arrange
    Core::FMTOutputNode ofAnAction(getOfPeriods(getActionSource("? ? ?", "COUPETOTALE", "VOLUMETOTAL"), 2, 2),
        getConstant(1.0), 1.0);
    Core::FMTOutputNode ofTheInventory(getOfPeriods(getInventorySource("? ? ?", "VOLUMETOTAL"), 2, 2), getConstant(1.0), 1.0);

    // Act
    const Targets actionTargets = setToGraph(ofAnAction, 5, 10);
    const Targets inventoryTargets = setToGraph(ofTheInventory, 5, 10);

    // Assert
    EXPECT_EQ(actionTargets.nodePeriod, 2);
    EXPECT_EQ(actionTargets.periods, std::vector<int>({ 2 }));
    EXPECT_EQ(inventoryTargets.nodePeriod, 3);
    EXPECT_EQ(inventoryTargets.periods, std::vector<int>({ 3 }));
    EXPECT_EQ(ofTheInventory.source.getPeriodLowerBound(), 3);
    EXPECT_EQ(ofTheInventory.factor.getPeriodUpperBound(), 3);
}

// A period of -1 is the period before the one asked: at period 0, there is none, and the node needs no development.
TEST(FMTOutputNode, GivenANodeOfThePreviousPeriodWhenSetToGraphThenTargetsThePeriodBeforeThePeriodAsked)
{
    // Arrange
    const Core::FMTOutputSource previous = getOfPeriods(getActionSource("? ? ?", "COUPETOTALE", "VOLUMETOTAL"), -1, -1);
    Core::FMTOutputNode atPeriod3(previous, getConstant(1.0), 1.0);
    Core::FMTOutputNode atPeriod0(previous, getConstant(1.0), 1.0);

    // Act
    const Targets targets3 = setToGraph(atPeriod3, 3, 10);
    const Targets targets0 = setToGraph(atPeriod0, 0, 10);

    // Assert
    EXPECT_EQ(targets3.nodePeriod, 2);
    EXPECT_EQ(targets3.periods, std::vector<int>({ 2 }));
    EXPECT_EQ(atPeriod3.source.getPeriodLowerBound(), 2);
    EXPECT_EQ(targets0.nodePeriod, -1);
    EXPECT_TRUE(targets0.periods.empty());
}

// The periods of the node from period 1 up to the last period of the graph, whatever the period asked.
TEST(FMTOutputNode, GivenANodeOfSeveralPeriodsWhenSetToGraphThenTargetsItsPeriodsFromOneToTheLastPeriodOfTheGraph)
{
    // Arrange
    Core::FMTOutputNode node(getOfPeriods(getActionSource("? ? ?", "COUPETOTALE", "VOLUMETOTAL"), 0, 10), getConstant(1.0), 1.0);

    // Act
    const Targets targets = setToGraph(node, 2, 4);

    // Assert
    EXPECT_EQ(targets.nodePeriod, 2);
    EXPECT_EQ(targets.periods, std::vector<int>({ 1, 2, 3, 4 }));
}

TEST(FMTOutputNode, GivenASumWithoutPeriodWhenSetToGraphThenTargetsEachPeriodUpToThePeriodAsked)
{
    // Arrange
    Core::FMTOutputNode node = getHarvestNode(1.0, 2.0);
    node.source.setSum();

    // Act
    const Targets targets = setToGraph(node, 3, 10);

    // Assert
    EXPECT_EQ(targets.periods, std::vector<int>({ 1, 2, 3 }));
    EXPECT_DOUBLE_EQ(node.constant, 2.0);
}

// The constant of an average is divided by the number of periods it targets: 2 / 4.
TEST(FMTOutputNode, GivenAnAverageWithoutPeriodWhenSetToGraphThenTargetsEachPeriodUpToThePeriodAskedAndDividesTheConstant)
{
    // Arrange
    Core::FMTOutputNode node = getHarvestNode(1.0, 2.0);
    node.source.setAverage();

    // Act
    const Targets targets = setToGraph(node, 4, 10);

    // Assert
    EXPECT_EQ(targets.periods, std::vector<int>({ 1, 2, 3, 4 }));
    EXPECT_DOUBLE_EQ(node.constant, 0.5);
}

TEST(FMTOutputNode, GivenAnAverageOfThreePeriodsWhenSetToGraphThenDividesTheConstantByThree)
{
    // Arrange
    Core::FMTOutputNode node(getOfPeriods(getActionSource("? ? ?", "COUPETOTALE", "VOLUMETOTAL"), 1, 3), getConstant(1.0), 3.0);
    node.source.setAverage();

    // Act
    const Targets targets = setToGraph(node, 5, 10);

    // Assert
    EXPECT_EQ(targets.periods, std::vector<int>({ 1, 2, 3 }));
    EXPECT_DOUBLE_EQ(node.constant, 1.0);
}

// At period 0, an average without period targets no period, and its constant becomes 1 / 0: the node then brings no
// development, so that the value stays 0.
TEST(FMTOutputNode, GivenAnAverageWithoutPeriodWhenSetToGraphAtPeriodZeroThenTargetsNoPeriodAndTheConstantBecomesInfinite)
{
    // Arrange
    Core::FMTOutputNode node = getHarvestNode(1.0, 1.0);
    node.source.setAverage();

    // Act
    const Targets targets = setToGraph(node, 0, 10);

    // Assert
    EXPECT_EQ(targets.nodePeriod, 0);
    EXPECT_TRUE(targets.periods.empty());
    EXPECT_EQ(node.constant, std::numeric_limits<double>::infinity());
}

// Equality, order and hash read the source only (without its yield nor its values): hashForValue and isSameValues read
// the factor and the constant too (Tests/Unit/Core/FMTOutputNodeHasher.cpp).
TEST(FMTOutputNode, GivenNodesOfTheSameSourceWhenComparedThenTheyAreEqualWhateverTheirFactorAndConstant)
{
    // Arrange
    const Core::FMTOutputNode first = getHarvestNode(1.0, 1.0);
    const Core::FMTOutputNode second = getHarvestNode(2.0, 3.0);

    // Act
    const bool equal = (first == second);
    const bool ordered = (first < second) || (second < first);

    // Assert
    EXPECT_TRUE(equal);
    EXPECT_FALSE(ordered);
    EXPECT_EQ(first.hash(), second.hash());
    EXPECT_NE(first.hashForValue(), second.hashForValue());
    EXPECT_FALSE(first.isSameValues(second));
    EXPECT_TRUE(first.isSameValues(getHarvestNode(1.0, 1.0)));
}

TEST(FMTOutputNode, GivenNodesOfOtherSourcesWhenComparedThenTheyAreOrderedAsTheirSources)
{
    // Arrange
    const Core::FMTOutputSource firstSource = getActionSource("? PEUPLEMENT1 ?", "COUPETOTALE", "VOLUMETOTAL");
    const Core::FMTOutputSource secondSource = getActionSource("? PEUPLEMENT2 ?", "COUPETOTALE", "VOLUMETOTAL");
    const Core::FMTOutputNode first(firstSource, getConstant(1.0), 1.0);
    const Core::FMTOutputNode second(secondSource, getConstant(1.0), 1.0);

    // Act
    const bool firstBefore = (first < second);
    const bool secondBefore = (second < first);

    // Assert
    EXPECT_FALSE(first == second);
    EXPECT_EQ(firstBefore, firstSource < secondSource);
    EXPECT_EQ(secondBefore, secondSource < firstSource);
    EXPECT_NE(firstBefore, secondBefore);
}

// Known behavior (Tests/Unit/Core/FMTOutputSource.cpp): the equality of the sources ignores their yield.
TEST(FMTOutputNode, GivenNodesThatDifferByTheYieldOfTheirSourceWhenComparedThenTheyAreEqualButNotOfTheSameValues)
{
    // Arrange
    const Core::FMTOutputNode volume(getActionSource("? ? ?", "COUPETOTALE", "VOLUMETOTAL"), getConstant(1.0), 1.0);
    const Core::FMTOutputNode stems(getActionSource("? ? ?", "COUPETOTALE", "STEMS"), getConstant(1.0), 1.0);

    // Act
    const bool equal = (volume == stems);
    const bool sameValues = volume.isSameValues(stems);

    // Assert
    EXPECT_TRUE(equal);
    EXPECT_FALSE(sameValues);
}

TEST(FMTOutputNode, GivenANodeWhenConvertedToStringThenWritesItsSourceItsFactorAndItsConstant)
{
    // Arrange
    const Core::FMTOutputNode node = getHarvestNode(1.0, 2.0);

    // Act
    const std::string text(node);

    // Assert
    EXPECT_EQ(text, "? PEUPLEMENT1 ? COUPETOTALE VOLUMETOTAL  1 *2.000000");
    EXPECT_EQ(node.getHashString(), text);
}

TEST(FMTOutputNode, GivenNodesOfTheSameValuesWhenGetHashMaskThenTheirMasksAreEqual)
{
    // Arrange
    const Core::FMTOutputNode first = getHarvestNode(1.0, 2.0);
    const Core::FMTOutputNode second = getHarvestNode(1.0, 2.0);
    const Core::FMTOutputNode otherConstant = getHarvestNode(1.0, 3.0);

    // Act
    const Core::FMTMask firstMask = first.getHashMask();
    const Core::FMTMask secondMask = second.getHashMask();
    const Core::FMTMask otherMask = otherConstant.getHashMask();

    // Assert
    EXPECT_TRUE(firstMask == secondMask);
    EXPECT_FALSE(firstMask == otherMask);
}

TEST(FMTOutputNode, GivenNodesWhenIsSubsetOfThenComparesTheirSourcesWithAndWithoutTheActions)
{
    // Arrange
    const std::vector<Core::FMTAction> actions = { getAction("AFIRE"), getAction("COUPETOTALE", { "COUPES" }) };
    const Core::FMTOutputNode node(getActionSource("UNITE1 PEUPLEMENT1 ?", "COUPETOTALE", "VOLUMETOTAL"), getConstant(1.0), 1.0);
    const Core::FMTOutputNode ofALargerMask(getActionSource("? PEUPLEMENT1 ?", "COUPETOTALE", "VOLUMETOTAL"), getConstant(5.0), 2.0);
    const Core::FMTOutputNode ofTheAggregate(getActionSource("? ? ?", "COUPES", "VOLUMETOTAL"), getConstant(1.0), 1.0);
    const Core::FMTOutputNode ofFire(getActionSource("UNITE1 PEUPLEMENT1 ?", "AFIRE", "VOLUMETOTAL"), getConstant(1.0), 1.0);

    // Act
    const bool inTheLargerMask = node.isSubsetOf(ofALargerMask);
    const bool inTheAggregate = node.isSubsetOf(ofTheAggregate, actions);
    const bool inTheAggregateWithoutActions = node.isSubsetOf(ofTheAggregate);
    const bool sameButFire = node.isSameButDifferentAction(ofFire);

    // Assert
    EXPECT_TRUE(inTheLargerMask);
    EXPECT_TRUE(inTheAggregate);
    EXPECT_FALSE(inTheAggregateWithoutActions);
    EXPECT_TRUE(sameButFire);
}

// The cache of the graph keeps the developments of a node only when its factor is not a time yield.
TEST(FMTOutputNode, GivenNodesWhenAllowCashDeductionThenFalseForAFactorOfATimeYieldOnly)
{
    // Arrange
    const Core::FMTOutputNode ofAConstant = getHarvestNode();
    const Core::FMTOutputNode ofATimeYield(getActionSource("? ? ?", "COUPETOTALE", "VOLUMETOTAL"),
        Core::FMTOutputSource(Core::FMTotar::timeyld, 0.0, "PERIODVALUE"), 1.0);

    // Act
    const bool constantAllows = ofAConstant.allowCashDeduction();
    const bool timeYieldAllows = ofATimeYield.allowCashDeduction();

    // Assert
    EXPECT_TRUE(constantAllows);
    EXPECT_FALSE(timeYieldAllows);
}

TEST(FMTOutputNode, GivenANodeWhenGetOutputIdThenReturnsTheOriginOfItsSource)
{
    // Arrange
    Core::FMTOutputSource source = getInventorySource("? ? ?", "VOLUMETOTAL");
    source.setOutputOrigin(6);

    // Act
    const Core::FMTOutputNode node(source, getConstant(1.0), 1.0);

    // Assert
    EXPECT_EQ(node.getOutputId(), 6);
    EXPECT_FALSE(node.isActionBased());
}

// fillUpEquation replaces the node O<id> by its value in the equation of each output of the graph values.
TEST(FMTOutputNode, GivenTheValuesOfTwoNodesWhenFillUpEquationThenReplacesEachNodeByItsValueInTheEquationOfEachOutput)
{
    // Arrange
    const Core::FMTOutputNode node = getHarvestNode();
    const std::vector<std::string> equation = { "O0", "+", "O1" };
    std::map<std::string, std::vector<std::string>> allEquations;

    // Act
    node.fillUpEquation(allEquations, { { "OA", 5.0 }, { "OB", 7.0 } }, equation, 0);
    node.fillUpEquation(allEquations, { { "OA", 2.0 } }, equation, 1);

    // Assert
    EXPECT_EQ(allEquations.at("OA"), std::vector<std::string>({ "5.000000", "+", "2.000000" }));
    EXPECT_EQ(allEquations.at("OB"), std::vector<std::string>({ "7.000000", "+", "O1" }));
}

TEST(FMTOutputNode, GivenANodeWhenCopiedThenTheCopyHasTheSameValues)
{
    // Arrange
    const Core::FMTOutputNode node = getHarvestNode(2.0, 3.0);

    // Act
    const Core::FMTOutputNode copy(node);
    Core::FMTOutputNode assigned;
    assigned = node;

    // Assert
    EXPECT_TRUE(copy.isSameValues(node));
    EXPECT_TRUE(assigned.isSameValues(node));
    EXPECT_EQ(std::string(assigned), std::string(node));
}
