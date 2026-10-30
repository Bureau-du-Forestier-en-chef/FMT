/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTOutputSource.h"
#include "FMTAction.h"
#include "FMTActualDevelopment.h"
#include "FMTAgeBounds.h"
#include "FMTAgeYieldHandler.h"
#include "FMTDevelopment.h"
#include "FMTDevelopmentPath.h"
#include "FMTMask.h"
#include "FMTMaskFilter.h"
#include "FMTOperator.h"
#include "FMTPerBounds.h"
#include "FMTSpec.h"
#include "FMTTheme.h"
#include "FMTTimeYieldHandler.h"
#include "FMTYields.h"
#include "FMTutility.h"

#include <gtest/gtest.h>

#include <limits>
#include <map>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

// A source of an output, built as FMTOutputParser builds it: the area or a yield of the developments of a mask and of a
// spec, operated by an action (FMTotar::actual) or in inventory (FMTotar::inventory), a constant of each period
// (FMTotar::val), a time yield (FMTotar::timeyld) or a level. Its coefficient is what a development of the graph brings
// to the output. The themes are those of Tests/Unit/Core/FMTTheme.cpp, VOLUMETOTAL that of peuplement1 in TWD_land
// (120 at 7 years, between 100 at 5 years and 150 at 10 years) and PERIODVALUE a time yield of 10, 20 and 30 from
// period 1.

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

    Core::FMTDevelopment getDevelopment(const std::string& p_mask, int p_age, int p_period = 1)
    {
        return Core::FMTDevelopment(getMask(p_mask), p_age, 0, p_period);
    }

    Core::FMTYields getYields()
    {
        Core::FMTAgeYieldHandler ageHandler(getMask("? ? ?"));
        ageHandler.setYieldValues("VOLUMETOTAL", { 0, 5, 10, 15, 20, 25, 30 },
            { 0.0, 100.0, 150.0, 200.0, 250.0, 300.0, 350.0 });
        Core::FMTTimeYieldHandler timeHandler(getMask("? ? ?"));
        timeHandler.setYieldValues("PERIODVALUE", 1, { 10.0, 20.0, 30.0 });
        Core::FMTYields yields;
        yields.pushBackAgeHandler(getMask("? ? ?"), ageHandler);
        yields.pushBackTimeHandler(getMask("? ? ?"), timeHandler);
        yields.update();
        return yields;
    }

    // _AGE >= p_lower AND _AGE <= p_upper of an output source.
    Core::FMTSpec getAgeSpec(int p_lower, int p_upper)
    {
        Core::FMTSpec spec;
        spec.addBounds(Core::FMTAgeBounds(Core::FMTsection::Outputs, p_upper, p_lower));
        return spec;
    }

    // <mask> <action> <yield> of an output, the area when p_yield is empty.
    Core::FMTOutputSource getActionSource(const std::string& p_mask, const std::string& p_action, const std::string& p_yield)
    {
        return Core::FMTOutputSource(Core::FMTSpec(), getMask(p_mask), Core::FMTotar::actual, p_yield, p_action);
    }

    // <mask> _INVENT[(<action>)] <yield> of an output, the area when p_yield is empty.
    Core::FMTOutputSource getInventorySource(const std::string& p_mask, const std::string& p_yield,
        const std::string& p_action = "")
    {
        return Core::FMTOutputSource(Core::FMTSpec(), getMask(p_mask), Core::FMTotar::inventory, p_yield, p_action);
    }

    // An action of the mask ? PROD ?, operable from age 1, of the aggregates p_aggregates.
    Core::FMTAction getAction(const std::string& p_name, const std::vector<std::string>& p_aggregates = {})
    {
        Core::FMTAction action(p_name, true, false);
        action.push_back(getMask("? PROD ?"), getAgeSpec(1, NO_UPPER_BOUND));
        for (const std::string& aggregate : p_aggregates)
        {
            action.pushAggregate(aggregate);
        }
        action.update();
        return action;
    }

    // aFire, then COUPETOTALE and ATHINNING of the aggregate COUPES.
    std::vector<Core::FMTAction> getActions()
    {
        return { getAction("AFIRE"), getAction("COUPETOTALE", { "COUPES" }), getAction("ATHINNING", { "COUPES" }) };
    }

    std::vector<std::string> getNames(const std::vector<const Core::FMTAction*>& p_actions)
    {
        std::vector<std::string> names;
        for (const Core::FMTAction* action : p_actions)
        {
            names.push_back(action->getName());
        }
        return names;
    }
}

TEST(FMTOutputSource, GivenAConstantWhenConstructedThenIsAConstantOfThatValueAndNotAVariable)
{
    // Arrange
    const double value = 5.0;

    // Act
    const Core::FMTOutputSource source(Core::FMTotar::val, value);

    // Assert
    EXPECT_TRUE(source.isConstant());
    EXPECT_FALSE(source.isVariable());
    EXPECT_FALSE(source.isAction());
    EXPECT_DOUBLE_EQ(source.getValue(), value);
    EXPECT_EQ(source.getValues(), std::vector<double>({ value }));
}

// The value of period p is the p-th value: before period 1, the first one; after the last period given, the last one.
TEST(FMTOutputSource, GivenConstantValuesOfPeriodsWhenGetValueThenReadsThePeriodAndKeepsTheFirstBeforeItAndTheLastAfterIt)
{
    // Arrange
    const Core::FMTOutputSource source(Core::FMTotar::val, std::vector<double>({ 10.0, 20.0, 30.0 }));

    // Act
    const std::vector<double> values = { source.getValue(1), source.getValue(3), source.getValue(5), source.getValue(0),
        source.getValue(-2) };

    // Assert
    EXPECT_EQ(values, std::vector<double>({ 10.0, 30.0, 30.0, 10.0, 10.0 }));
}

TEST(FMTOutputSource, GivenATimeYieldSourceWhenConstructedThenKeepsNoValueAndGetValueReturnsOne)
{
    // Arrange
    const double ignored = 7.0;

    // Act
    const Core::FMTOutputSource source(Core::FMTotar::timeyld, ignored, "PERIODVALUE");

    // Assert
    EXPECT_TRUE(source.isTimeYield());
    EXPECT_FALSE(source.isVariable());
    EXPECT_TRUE(source.getValues().empty());
    EXPECT_DOUBLE_EQ(source.getValue(2), 1.0);
    EXPECT_EQ(source.getYield(), "PERIODVALUE");
}

// A default source is a constant without any value: getValue reads before the beginning of its values.
TEST(FMTOutputSource, GivenADefaultSourceWhenGetValueThenRaisesAnOutOfRange)
{
    // Arrange
    const Core::FMTOutputSource source;

    // Act and Assert
    EXPECT_TRUE(source.isConstant());
    EXPECT_THROW(source.getValue(), std::out_of_range);
}

TEST(FMTOutputSource, GivenAnActionSourceWhenConstructedThenIsAVariableOfThatActionAndYieldThatUsesTheOutEdges)
{
    // Arrange
    const std::string action = "COUPETOTALE";

    // Act
    const Core::FMTOutputSource source = getActionSource("? PEUPLEMENT1 ?", action, "VOLUMETOTAL");

    // Assert
    EXPECT_TRUE(source.isVariable());
    EXPECT_TRUE(source.isAction());
    EXPECT_TRUE(source.useOutEdges());
    EXPECT_FALSE(source.useInEdges());
    EXPECT_FALSE(source.isInventory());
    EXPECT_EQ(source.getAction(), action);
    EXPECT_EQ(source.getYield(), "VOLUMETOTAL");
    EXPECT_EQ(std::string(source.getMask()), "? PEUPLEMENT1 ?");
}

// _INVENT without an action is the inventory at the beginning of the next period, read from the in edges; _INVENT(<action>)
// is the inventory of the developments operable to the action.
TEST(FMTOutputSource, GivenInventorySourcesWhenIsNextPeriodThenOnlyTheSourceWithoutActionReadsTheNextPeriod)
{
    // Arrange
    const Core::FMTOutputSource inventory = getInventorySource("? ? ?", "VOLUMETOTAL");
    const Core::FMTOutputSource ofAnAction = getInventorySource("? ? ?", "VOLUMETOTAL", "COUPETOTALE");

    // Act
    const bool inventoryIsNext = inventory.isNextPeriod();
    const bool ofAnActionIsNext = ofAnAction.isNextPeriod();

    // Assert
    EXPECT_TRUE(inventoryIsNext);
    EXPECT_FALSE(ofAnActionIsNext);
    EXPECT_TRUE(inventory.useInEdges());
    EXPECT_TRUE(ofAnAction.useInEdges());
    EXPECT_FALSE(inventory.useOutEdges());
}

// A level of constants keeps its values and its name; a level of an output (_LEVEL) names that output as its action.
TEST(FMTOutputSource, GivenLevelSourcesWhenIsVariableLevelThenOnlyTheLevelOfAnOutputIsVariable)
{
    // Arrange
    const Core::FMTOutputSource constantLevel(Core::FMTotar::level, std::vector<double>({ 4.0, 5.0 }), -1, -1, "MYLEVEL");
    const Core::FMTOutputSource outputLevel(Core::FMTotar::level, 0.0, "", "OVOL");

    // Act
    const bool constantIsVariable = constantLevel.isVariableLevel();
    const bool outputIsVariable = outputLevel.isVariableLevel();

    // Assert
    EXPECT_TRUE(constantLevel.isLevel());
    EXPECT_FALSE(constantIsVariable);
    EXPECT_TRUE(outputIsVariable);
    EXPECT_DOUBLE_EQ(constantLevel.getValue(2), 5.0);
    EXPECT_EQ(std::string(constantLevel), "MYLEVEL");
    EXPECT_EQ(std::string(outputLevel), "OVOL");
}

TEST(FMTOutputSource, GivenASourceWhenUseThenSelectsTheDevelopmentsOfItsMaskThatMeetItsSpec)
{
    // Arrange: ? PEUPLEMENT1 ? _AGE >= 10 AND _AGE <= 20
    const Core::FMTYields yields = getYields();
    const Core::FMTOutputSource source(getAgeSpec(10, 20), getMask("? PEUPLEMENT1 ?"), Core::FMTotar::inventory);

    // Act
    const bool usesWithin = source.use(getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 15), yields);
    const bool usesTooOld = source.use(getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 25), yields);
    const bool usesOtherStand = source.use(getDevelopment("UNITE1 PEUPLEMENT2 UTR1", 15), yields);

    // Assert
    EXPECT_TRUE(usesWithin);
    EXPECT_FALSE(usesTooOld);
    EXPECT_FALSE(usesOtherStand);
}

TEST(FMTOutputSource, GivenAnInventorySourceOfAYieldOrOfTheAreaWhenGetCoefThenReturnsTheYieldOfTheDevelopmentOrOne)
{
    // Arrange
    const Core::FMTYields yields = getYields();
    const Core::FMTDevelopment development = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 7);
    const Core::FMTOutputSource ofAYield = getInventorySource("? ? ?", "VOLUMETOTAL");
    const Core::FMTOutputSource ofTheArea = getInventorySource("? ? ?", "");

    // Act
    const double yieldCoef = ofAYield.getCoef(development, yields);
    const double areaCoef = ofTheArea.getCoef(development, yields);

    // Assert
    EXPECT_DOUBLE_EQ(yieldCoef, 120.0);
    EXPECT_DOUBLE_EQ(areaCoef, 1.0);
}

TEST(FMTOutputSource, GivenAConstantSourceWhenGetCoefThenReturnsItsValueAtThePeriodOfTheDevelopment)
{
    // Arrange
    const Core::FMTYields yields = getYields();
    const Core::FMTOutputSource source(Core::FMTotar::val, std::vector<double>({ 10.0, 20.0 }));

    // Act
    const double atPeriod2 = source.getCoef(getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 7, 2), yields);

    // Assert
    EXPECT_DOUBLE_EQ(atPeriod2, 20.0);
}

TEST(FMTOutputSource, GivenATimeYieldSourceWhenGetCoefThenReturnsTheTimeYieldAtThePeriodOfTheDevelopment)
{
    // Arrange
    const Core::FMTYields yields = getYields();
    const Core::FMTOutputSource source(Core::FMTotar::timeyld, 0.0, "PERIODVALUE");

    // Act
    const double atPeriod2 = source.getCoef(getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 7, 2), yields);

    // Assert
    EXPECT_DOUBLE_EQ(atPeriod2, 20.0);
}

// A constant times a time yield (2 * PERIODVALUE) becomes a time yield of the constant: 2 x 20 at period 2.
TEST(FMTOutputSource, GivenAConstantTimesATimeYieldWhenResetValuesThenTheSourceMultipliesTheTimeYieldByTheConstant)
{
    // Arrange
    const Core::FMTYields yields = getYields();
    Core::FMTOutputSource source(Core::FMTotar::val, 2.0);
    const Core::FMTOutputSource timeYield(Core::FMTotar::timeyld, 0.0, "PERIODVALUE");

    // Act
    source.resetValues(Core::FMTOperator("*"), timeYield);

    // Assert
    EXPECT_TRUE(source.isTimeYield());
    EXPECT_EQ(source.getYield(), "PERIODVALUE");
    EXPECT_EQ(source.getValues(), std::vector<double>({ 2.0 }));
    EXPECT_DOUBLE_EQ(source.getCoef(getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 7, 2), yields), 40.0);
}

TEST(FMTOutputSource, GivenTwoConstantsWhenResetValuesWithAnOperatorThenAppliesItPeriodByPeriod)
{
    // Arrange
    Core::FMTOutputSource product(Core::FMTotar::val, std::vector<double>({ 10.0, 20.0 }));
    Core::FMTOutputSource sum(Core::FMTotar::val, 10.0);

    // Act
    product.resetValues(Core::FMTOperator("*"), Core::FMTOutputSource(Core::FMTotar::val, 2.0));
    sum.resetValues(Core::FMTOperator("+"), Core::FMTOutputSource(Core::FMTotar::val, std::vector<double>({ 1.0, 2.0, 3.0 })));

    // Assert
    EXPECT_EQ(product.getValues(), std::vector<double>({ 20.0, 40.0 }));
    EXPECT_EQ(sum.getValues(), std::vector<double>({ 11.0, 12.0, 13.0 }));
    EXPECT_TRUE(sum.isConstant());
}

// An action that is not partial harvests the yield of the development (Tests/Unit/Core/FMTDevelopment.cpp): an action
// source gives it through the paths of the action.
TEST(FMTOutputSource, GivenAnActionSourceOfAYieldWhenGetCoefThenReturnsTheYieldThatTheActionHarvests)
{
    // Arrange
    const Core::FMTYields yields = getYields();
    const Core::FMTAction action = getAction("COUPETOTALE");
    const Core::FMTDevelopment development = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 7);
    const std::vector<Core::FMTDevelopmentPath> paths = { Core::FMTDevelopmentPath(getMask("UNITE1 PEUPLEMENT1 UTR1"), 0, 0, 1,
        100.0) };
    const Core::FMTOutputSource source = getActionSource("? ? ?", "COUPETOTALE", "VOLUMETOTAL");

    // Act
    const double coef = source.getCoef(development, yields, nullptr, &paths, &action);

    // Assert
    EXPECT_DOUBLE_EQ(coef, 120.0);
}

TEST(FMTOutputSource, GivenSourcesWhenTargetsThenReturnsTheActionOrTheActionsOfTheAggregateItNames)
{
    // Arrange
    const std::vector<Core::FMTAction> actions = getActions();
    const Core::FMTOutputSource ofAnAggregate = getActionSource("? ? ?", "COUPES", "");
    const Core::FMTOutputSource ofAnAction = getActionSource("? ? ?", "AFIRE", "");
    const Core::FMTOutputSource inventory = getInventorySource("? ? ?", "");
    const Core::FMTOutputSource outputLevel(Core::FMTotar::level, 0.0, "", "AFIRE");

    // Act
    const std::vector<std::string> ofTheAggregate = getNames(ofAnAggregate.targets(actions));
    const std::unordered_set<int> aggregateSet = ofAnAggregate.targetsSet(actions);
    const std::vector<std::string> ofTheAction = getNames(ofAnAction.targets(actions));

    // Assert
    EXPECT_EQ(ofTheAggregate, std::vector<std::string>({ "COUPETOTALE", "ATHINNING" }));
    EXPECT_EQ(aggregateSet, std::unordered_set<int>({ 1, 2 }));
    EXPECT_EQ(ofTheAction, std::vector<std::string>({ "AFIRE" }));
    EXPECT_TRUE(inventory.targets(actions).empty());
    EXPECT_TRUE(outputLevel.targets(actions).empty());
}

TEST(FMTOutputSource, GivenSourcesWhenIsNullThenOnlyAZeroConstantOrANullYieldIsNull)
{
    // Arrange
    const Core::FMTYields yields = getYields();

    // Act
    const bool zeroIsNull = Core::FMTOutputSource(Core::FMTotar::val, 0.0).isNull(yields);
    const bool twoIsNull = Core::FMTOutputSource(Core::FMTotar::val, 2.0).isNull(yields);
    const bool areaIsNull = getInventorySource("? ? ?", "").isNull(yields);
    const bool unknownIsNull = getInventorySource("? ? ?", "UNKNOWN").isNull(yields);
    const bool volumeIsNull = getInventorySource("? ? ?", "VOLUMETOTAL").isNull(yields);

    // Assert
    EXPECT_TRUE(zeroIsNull);
    EXPECT_FALSE(twoIsNull);
    EXPECT_FALSE(areaIsNull);
    EXPECT_TRUE(unknownIsNull);
    EXPECT_FALSE(volumeIsNull);
}

// Known behavior (Tests/ETAT.md, section 5.10): equality ignores the yield and the values, which isEqualByValue and the
// hash with the yield compare.
TEST(FMTOutputSource, GivenTwoSourcesThatDifferByTheirYieldOnlyWhenComparedThenTheyAreEqualButNotEqualByValue)
{
    // Arrange
    const Core::FMTOutputSource volume = getActionSource("? ? ?", "COUPETOTALE", "VOLUMETOTAL");
    const Core::FMTOutputSource stems = getActionSource("? ? ?", "COUPETOTALE", "STEMS");

    // Act
    const bool equal = (volume == stems);
    const bool equalByValue = volume.isEqualByValue(stems);

    // Assert
    EXPECT_TRUE(equal);
    EXPECT_FALSE(volume != stems);
    EXPECT_FALSE(equalByValue);
    EXPECT_TRUE(volume.isEqualByValue(getActionSource("? ? ?", "COUPETOTALE", "VOLUMETOTAL")));
    EXPECT_EQ(volume.hash(), stems.hash());
    EXPECT_NE(volume.hash(-1, true), stems.hash(-1, true));
}

// Equality ignores the values, but the hash does not: two equal constants of other values have other hashes. FMT hashes
// sources only through FMTOutputNode::hashForValue (Tests/Unit/Core/FMTOutputNode.cpp), which compares the values too.
TEST(FMTOutputSource, GivenTwoConstantsOfOtherValuesWhenComparedThenTheyAreEqualButTheirHashesDiffer)
{
    // Arrange
    const Core::FMTOutputSource one(Core::FMTotar::val, 1.0);
    const Core::FMTOutputSource two(Core::FMTotar::val, 2.0);

    // Act
    const bool equal = (one == two);

    // Assert
    EXPECT_TRUE(equal);
    EXPECT_NE(one.hash(), two.hash());
}

TEST(FMTOutputSource, GivenEqualSourcesWhenHashThenTheyHaveTheSameHashForEachPeriod)
{
    // Arrange
    const Core::FMTOutputSource first = getActionSource("? PEUPLEMENT1 ?", "COUPETOTALE", "VOLUMETOTAL");
    const Core::FMTOutputSource second = getActionSource("? PEUPLEMENT1 ?", "COUPETOTALE", "VOLUMETOTAL");

    // Act
    const size_t firstAtPeriod3 = first.hash(3, true);
    const size_t secondAtPeriod3 = second.hash(3, true);

    // Assert
    EXPECT_EQ(first.hash(), second.hash());
    EXPECT_EQ(firstAtPeriod3, secondAtPeriod3);
    EXPECT_NE(first.hash(3, true), first.hash(4, true));
    EXPECT_EQ(boost::hash<Core::FMTOutputSource>()(first), first.hash(-1, true));
}

// The order compares the mask, the target, the action, then the spec: two sources are equivalent in the order exactly
// when they are equal.
TEST(FMTOutputSource, GivenSourcesWhenComparedByOrderThenTwoSourcesAreEquivalentExactlyWhenTheyAreEqual)
{
    // Arrange
    const std::vector<Core::FMTOutputSource> sources = {
        getActionSource("? PEUPLEMENT1 ?", "COUPETOTALE", "VOLUMETOTAL"),
        getActionSource("? PEUPLEMENT1 ?", "COUPETOTALE", "STEMS"),
        getActionSource("? PEUPLEMENT2 ?", "COUPETOTALE", "VOLUMETOTAL"),
        getActionSource("? PEUPLEMENT1 ?", "AFIRE", "VOLUMETOTAL"),
        getInventorySource("? PEUPLEMENT1 ?", "VOLUMETOTAL"),
        Core::FMTOutputSource(getAgeSpec(10, 20), getMask("? PEUPLEMENT1 ?"), Core::FMTotar::actual, "VOLUMETOTAL",
            "COUPETOTALE") };
    std::vector<std::string> disagreements;

    // Act
    for (size_t first = 0; first < sources.size(); ++first)
    {
        for (size_t second = 0; second < sources.size(); ++second)
        {
            const bool equivalent = !(sources[first] < sources[second]) && !(sources[second] < sources[first]);
            if (equivalent != (sources[first] == sources[second]))
            {
                disagreements.push_back(std::to_string(first) + "," + std::to_string(second));
            }
        }
    }

    // Assert
    EXPECT_TRUE(disagreements.empty());
    EXPECT_TRUE(sources[0] < sources[2] || sources[2] < sources[0]);
    EXPECT_FALSE(sources[0] < sources[0]);
}

TEST(FMTOutputSource, GivenSourcesWhenConvertedToStringThenWritesTheirLineOfTheOutputSection)
{
    // Arrange
    const Core::FMTOutputSource constants(Core::FMTotar::val, std::vector<double>({ 10.0, 20.5 }));
    const Core::FMTOutputSource ofAnAction = getActionSource("? PEUPLEMENT1 ?", "COUPETOTALE", "VOLUMETOTAL");
    const Core::FMTOutputSource areaOfAnAction = getActionSource("? PEUPLEMENT1 ?", "COUPETOTALE", "");
    const Core::FMTOutputSource inventory = getInventorySource("? ? ?", "");
    const Core::FMTOutputSource inventoryOfAnAction = getInventorySource("? ? ?", "VOLUMETOTAL", "COUPETOTALE");
    const Core::FMTOutputSource timeYield(Core::FMTotar::timeyld, 0.0, "PERIODVALUE");

    // Act
    const std::vector<std::string> lines = { std::string(constants), std::string(ofAnAction), std::string(areaOfAnAction),
        std::string(inventory), std::string(inventoryOfAnAction), std::string(timeYield) };

    // Assert
    EXPECT_EQ(lines, std::vector<std::string>({ "10 20.5", "? PEUPLEMENT1 ? COUPETOTALE VOLUMETOTAL ",
        "? PEUPLEMENT1 ? COUPETOTALE _AREA", "? ? ? _INVENT _AREA", "? ? ? _INVENT(COUPETOTALE) VOLUMETOTAL",
        "PERIODVALUE" }));
}

// trimDouble removes the zeros after the decimal point of what std::to_string writes. Without a decimal point, it removes
// the zeros of an integer too; FMT passes it only numbers written by std::to_string.
TEST(FMTOutputSource, GivenNumbersWithTrailingZerosWhenTrimDoubleThenRemovesTheZerosAfterTheDecimalPoint)
{
    // Arrange
    const std::vector<std::string> numbers = { "1400.000000", "14.030000", "0.000000", "-0.500000", "100" };

    // Act
    std::vector<std::string> trimmed;
    for (const std::string& number : numbers)
    {
        trimmed.push_back(Core::FMTOutputSource::trimDouble(number));
    }

    // Assert
    EXPECT_EQ(trimmed, std::vector<std::string>({ "1400", "14.03", "0", "-0.5", "1" }));
}

TEST(FMTOutputSource, GivenASourceWhenIsSubsetOfThenTrueForASourceOfALargerMaskOfTheSameTargetAndAction)
{
    // Arrange
    const Core::FMTOutputSource source = getActionSource("UNITE1 PEUPLEMENT1 ?", "COUPETOTALE", "VOLUMETOTAL");

    // Act
    const bool ofALargerMask = source.isSubsetOf(getActionSource("? PEUPLEMENT1 ?", "COUPETOTALE", "VOLUMETOTAL"));
    const bool ofASmallerMask = getActionSource("? PEUPLEMENT1 ?", "COUPETOTALE", "VOLUMETOTAL").isSubsetOf(source);
    const bool ofAnotherAction = source.isSubsetOf(getActionSource("? PEUPLEMENT1 ?", "AFIRE", "VOLUMETOTAL"));
    const bool ofTheInventory = source.isSubsetOf(getInventorySource("? PEUPLEMENT1 ?", "VOLUMETOTAL"));
    const bool ofAConstant = Core::FMTOutputSource(Core::FMTotar::val, 1.0).isSubsetOf(Core::FMTOutputSource(Core::FMTotar::val, 1.0));

    // Assert
    EXPECT_TRUE(ofALargerMask);
    EXPECT_FALSE(ofASmallerMask);
    EXPECT_FALSE(ofAnotherAction);
    EXPECT_FALSE(ofTheInventory);
    EXPECT_FALSE(ofAConstant);
}

TEST(FMTOutputSource, GivenAnActionSourceWhenIsSubsetOfWithTheActionsThenAcceptsASourceOfAnAggregateOfItsAction)
{
    // Arrange
    const std::vector<Core::FMTAction> actions = getActions();
    const Core::FMTOutputSource ofTheAction = getActionSource("? PEUPLEMENT1 ?", "COUPETOTALE", "VOLUMETOTAL");
    const Core::FMTOutputSource ofTheAggregate = getActionSource("? ? ?", "COUPES", "VOLUMETOTAL");

    // Act
    const bool withTheActions = ofTheAction.isSubsetOf(ofTheAggregate, actions);
    const bool withoutTheActions = ofTheAction.isSubsetOf(ofTheAggregate);
    const bool inTheAggregate = ofTheAction.isInAggregate(ofTheAggregate, actions);
    const bool fireInTheAggregate = getActionSource("? ? ?", "AFIRE", "").isInAggregate(ofTheAggregate, actions);

    // Assert
    EXPECT_TRUE(withTheActions);
    EXPECT_FALSE(withoutTheActions);
    EXPECT_TRUE(inTheAggregate);
    EXPECT_FALSE(fireInTheAggregate);
}

TEST(FMTOutputSource, GivenSourcesThatDifferByTheirActionOnlyWhenIsSameButDifferentActionThenReturnsTrue)
{
    // Arrange
    const Core::FMTOutputSource clearcut = getActionSource("? PEUPLEMENT1 ?", "COUPETOTALE", "VOLUMETOTAL");

    // Act
    const bool ofFire = clearcut.isSameButDifferentAction(getActionSource("? PEUPLEMENT1 ?", "AFIRE", "VOLUMETOTAL"));
    const bool ofItself = clearcut.isSameButDifferentAction(clearcut);
    const bool ofFireElsewhere = clearcut.isSameButDifferentAction(getActionSource("? PEUPLEMENT2 ?", "AFIRE", "VOLUMETOTAL"));

    // Assert
    EXPECT_TRUE(ofFire);
    EXPECT_FALSE(ofItself);
    EXPECT_FALSE(ofFireElsewhere);
}

// The presolve of Tests/Unit/Core/FMTMask.cpp, which keeps PEUPLEMENT1 and PEUPLEMENT2 of the second theme.
TEST(FMTOutputSource, GivenSourcesWhenPreSolveThenPresolvesTheMaskOfAVariableSourceAndKeepsAConstant)
{
    // Arrange
    const Core::FMTMask selection = getMask("UNITE1 PEUPLEMENT1 UTR1").getUnion(getMask("UNITE1 PEUPLEMENT2 UTR1"));
    Core::FMTMaskFilter filter(selection);
    std::vector<Core::FMTTheme> themes;
    size_t newId = 0;
    size_t newStart = 0;
    for (const Core::FMTTheme& theme : getThemes())
    {
        const Core::FMTTheme presolved = theme.preSolve(filter, newId, newStart);
        if (!presolved.empty())
        {
            themes.push_back(presolved);
        }
    }
    const Core::FMTOutputSource variable = getActionSource("UNITE1 PEUPLEMENT2 UTR1", "COUPETOTALE", "VOLUMETOTAL");
    const Core::FMTOutputSource constant(Core::FMTotar::val, 3.0);

    // Act
    const Core::FMTOutputSource presolvedVariable = variable.preSolve(filter, themes);
    const Core::FMTOutputSource presolvedConstant = constant.preSolve(filter, themes);

    // Assert
    EXPECT_EQ(std::string(presolvedVariable.getMask()), "PEUPLEMENT2");
    EXPECT_EQ(presolvedVariable.getAction(), "COUPETOTALE");
    EXPECT_EQ(presolvedVariable.getYield(), "VOLUMETOTAL");
    EXPECT_TRUE(presolvedConstant.isEqualByValue(constant));
}

// An inventory source of period 0 can be deduced to a constant: the area of its developments times its yield,
// 10 ha x 120 for UNITE1, without the 5 ha of UNITE2.
TEST(FMTOutputSource, GivenAnInventorySourceOfPeriodZeroWhenGetConstantValueThenSumsTheAreaOfItsDevelopmentsTimesItsYield)
{
    // Arrange
    const Core::FMTYields yields = getYields();
    const std::vector<Core::FMTAction> actions = getActions();
    const std::vector<Core::FMTActualDevelopment> area = {
        Core::FMTActualDevelopment(getMask("UNITE1 PEUPLEMENT1 UTR1"), 7, 0, 10.0),
        Core::FMTActualDevelopment(getMask("UNITE2 PEUPLEMENT1 UTR1"), 7, 0, 5.0) };
    Core::FMTOutputSource source = getInventorySource("UNITE1 ? ?", "VOLUMETOTAL");
    source.setBounds(Core::FMTPerBounds(Core::FMTsection::Outputs, 0, 0));

    // Act
    const bool canBeDeduced = source.canBeDeducedToConstant();
    const double value = source.getConstantValue(area, actions, yields);

    // Assert
    EXPECT_TRUE(canBeDeduced);
    EXPECT_FALSE(getInventorySource("UNITE1 ? ?", "VOLUMETOTAL").canBeDeducedToConstant());
    EXPECT_DOUBLE_EQ(value, 1200.0);
}

// The test of getConstantValue reads (use && no action) || operable to an action: a source of an action counts every
// development operable to the action, whatever the mask of the source, here 15 ha x 120. Nothing in FMT calls
// getConstantValue nor canBeDeducedToConstant (Tests/ETAT.md, section 5.16).
TEST(FMTOutputSource, GivenAnInventorySourceOfAnActionWhenGetConstantValueThenCountsTheDevelopmentsOperableToTheActionOutsideItsMask)
{
    // Arrange
    const Core::FMTYields yields = getYields();
    const std::vector<Core::FMTAction> actions = getActions();
    const std::vector<Core::FMTActualDevelopment> area = {
        Core::FMTActualDevelopment(getMask("UNITE1 PEUPLEMENT1 UTR1"), 7, 0, 10.0),
        Core::FMTActualDevelopment(getMask("UNITE2 PEUPLEMENT1 UTR1"), 7, 0, 5.0) };
    Core::FMTOutputSource source = getInventorySource("UNITE1 ? ?", "VOLUMETOTAL", "AFIRE");
    source.setBounds(Core::FMTPerBounds(Core::FMTsection::Outputs, 0, 0));

    // Act
    const double value = source.getConstantValue(area, actions, yields);

    // Assert
    EXPECT_DOUBLE_EQ(value, 1800.0);
}

TEST(FMTOutputSource, GivenASourceWhenSetThenChangesTheValueSetOnly)
{
    // Arrange
    Core::FMTOutputSource source = getActionSource("? PEUPLEMENT1 ?", "COUPETOTALE", "VOLUMETOTAL");

    // Act
    source.setMask(getMask("? PEUPLEMENT2 ?"));
    source.setAction("AFIRE");
    source.setYield("STEMS");
    source.setThemeTarget(1);
    source.setOutputOrigin(4);
    source.setAverage();

    // Assert
    EXPECT_EQ(std::string(source.getMask()), "? PEUPLEMENT2 ?");
    EXPECT_EQ(source.getAction(), "AFIRE");
    EXPECT_EQ(source.getYield(), "STEMS");
    EXPECT_EQ(source.getThemeTarget(), 1);
    EXPECT_EQ(source.getOutputOrigin(), 4);
    EXPECT_TRUE(source.isAverage());
    EXPECT_FALSE(source.isSum());
    EXPECT_TRUE(source.getTarget() == Core::FMTotar::actual);
}
