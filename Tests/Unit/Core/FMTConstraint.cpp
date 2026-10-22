/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTConstraint.h"
#include "FMTMask.h"
#include "FMTOutput.h"
#include "FMTOutputSource.h"
#include "FMTSpec.h"
#include "FMTTheme.h"
#include "FMTYldBounds.h"
#include "FMTutility.h"

#include "ExceptionChain.h"

#include <gtest/gtest.h>

#include <limits>
#include <string>
#include <vector>

// The constraints and objectives of the optimization section, built as FMTOptimizationParser builds them: an output,
// a type, the periods (setLength) and the right-hand side (setRhs, lowest or infinity for an open bound), the
// variation of _EVEN, _NDY and _SEQ in the yield bounds Variation, the goals and the penalties in yield bounds named
// GOAL_<name> and Penalty<operator>_<goal>.

namespace
{
    const double UNBOUNDED_ABOVE = std::numeric_limits<double>::infinity();
    const double UNBOUNDED_BELOW = std::numeric_limits<double>::lowest();
    const int LENGTH = (std::numeric_limits<int>::max)();

    // The output OVOL: the volume harvested by COUPE.
    Core::FMTOutput getOutput()
    {
        const std::vector<Core::FMTTheme> themes = { Core::FMTTheme({ "A1", "A2" }, 0, 0, "THEME1") };
        const Core::FMTOutputSource harvested(Core::FMTSpec(), Core::FMTMask("?", themes), Core::FMTotar::actual,
            "VOLUME", "COUPE");
        return Core::FMTOutput("OVOL", "Volume harvested", "", { harvested }, {});
    }

    // A constraint of type p_type on OVOL, from period p_first to period p_last.
    Core::FMTConstraint getConstraint(Core::FMTconstrainttype p_type, int p_first = 1, int p_last = LENGTH)
    {
        Core::FMTConstraint constraint(p_type, getOutput());
        constraint.setLength(p_first, p_last);
        return constraint;
    }

    // The constraint OVOL >= p_lower, from period 1 to the end of the horizon.
    Core::FMTConstraint getAtLeast(double p_lower)
    {
        Core::FMTConstraint constraint = getConstraint(Core::FMTconstrainttype::FMTstandard);
        constraint.setRhs(p_lower, UNBOUNDED_ABOVE);
        return constraint;
    }

    // The variation of _EVEN(OVOL,<p_lower>%,<p_upper>%), as FMTOptimizationParser adds it.
    void setVariation(Core::FMTConstraint& p_constraint, double p_lower, double p_upper)
    {
        p_constraint.addBounds(Core::FMTYldBounds(Core::FMTsection::Optimize, "Variation", p_upper, p_lower));
    }
}

TEST(FMTConstraint, GivenAConstraintAtLeastAValueWhenGetBoundsThenReturnsTheValueAndInfinity)
{
    // Arrange: OVOL >= 100 1.._LENGTH
    const Core::FMTConstraint constraint = getAtLeast(100.0);
    double lower = 0;
    double upper = 0;

    // Act
    constraint.getBounds(lower, upper);

    // Assert
    EXPECT_EQ(lower, 100.0);
    EXPECT_EQ(upper, UNBOUNDED_ABOVE);
}

TEST(FMTConstraint, GivenAnObjectiveWhenGetBoundsThenReturnsZeros)
{
    // Arrange: _MIN OVOL 1.._LENGTH
    const Core::FMTConstraint objective = getConstraint(Core::FMTconstrainttype::FMTMINobjective);
    double lower = 1;
    double upper = 1;

    // Act
    objective.getBounds(lower, upper);

    // Assert
    EXPECT_EQ(lower, 0.0);
    EXPECT_EQ(upper, 0.0);
}

TEST(FMTConstraint, GivenEachConstraintTypeWhenIsObjectiveThenReturnsTrueForTheFourObjectivesOnly)
{
    // Arrange
    std::vector<int> objectiveTypes;

    // Act
    for (int type = Core::FMTconstrainttype::FMTMAXobjective; type <= Core::FMTconstrainttype::FMTrandomaction; ++type)
    {
        if (getConstraint(static_cast<Core::FMTconstrainttype>(type)).isObjective())
        {
            objectiveTypes.push_back(type);
        }
    }

    // Assert
    EXPECT_EQ(objectiveTypes, std::vector<int>({ Core::FMTconstrainttype::FMTMAXobjective,
        Core::FMTconstrainttype::FMTMINobjective, Core::FMTconstrainttype::FMTMAXMINobjective,
        Core::FMTconstrainttype::FMTMINMAXobjective }));
}

TEST(FMTConstraint, GivenObjectivesWhenSenseThenReturnsMinusOneForTheMaximizations)
{
    // Arrange
    const Core::FMTConstraint maximize = getConstraint(Core::FMTconstrainttype::FMTMAXobjective);
    const Core::FMTConstraint minimize = getConstraint(Core::FMTconstrainttype::FMTMINobjective);
    const Core::FMTConstraint maxMin = getConstraint(Core::FMTconstrainttype::FMTMAXMINobjective);
    const Core::FMTConstraint minMax = getConstraint(Core::FMTconstrainttype::FMTMINMAXobjective);

    // Act
    const std::vector<double> senses = { maximize.sense(), minimize.sense(), maxMin.sense(), minMax.sense() };

    // Assert
    EXPECT_EQ(senses, std::vector<double>({ -1.0, 1.0, -1.0, 1.0 }));
}

TEST(FMTConstraint, GivenMaxMinAndMinMaxObjectivesWhenExtraVariablesThenReturnsTrueForThemOnly)
{
    // Arrange
    const Core::FMTConstraint maxMin = getConstraint(Core::FMTconstrainttype::FMTMAXMINobjective);
    const Core::FMTConstraint minMax = getConstraint(Core::FMTconstrainttype::FMTMINMAXobjective);
    const Core::FMTConstraint maximize = getConstraint(Core::FMTconstrainttype::FMTMAXobjective);

    // Act
    const bool maxMinNeedsOne = maxMin.extraVariables();
    const bool minMaxNeedsOne = minMax.extraVariables();
    const bool maximizeNeedsOne = maximize.extraVariables();

    // Assert
    EXPECT_TRUE(maxMinNeedsOne);
    EXPECT_TRUE(minMaxNeedsOne);
    EXPECT_FALSE(maximizeNeedsOne);
}

TEST(FMTConstraint, GivenEvenFlowNonDecliningAndSequenceWhenAcrossPeriodThenReturnsTrueForThemOnly)
{
    // Arrange
    const Core::FMTConstraint evenFlow = getConstraint(Core::FMTconstrainttype::FMTevenflow);
    const Core::FMTConstraint nonDeclining = getConstraint(Core::FMTconstrainttype::FMTnondeclining);
    const Core::FMTConstraint sequence = getConstraint(Core::FMTconstrainttype::FMTsequence);
    const Core::FMTConstraint standard = getAtLeast(100.0);

    // Act
    const std::vector<bool> acrossPeriod = { evenFlow.acrossPeriod(), nonDeclining.acrossPeriod(), sequence.acrossPeriod(),
        standard.acrossPeriod() };

    // Assert
    EXPECT_EQ(acrossPeriod, std::vector<bool>({ true, true, true, false }));
}

TEST(FMTConstraint, GivenSpatialConstraintsWhenIsSpatialThenReturnsTrueForSizeAdjacencyAndGroup)
{
    // Arrange
    const Core::FMTConstraint size = getConstraint(Core::FMTconstrainttype::FMTspatialsize);
    const Core::FMTConstraint adjacency = getConstraint(Core::FMTconstrainttype::FMTspatialadjacency);
    const Core::FMTConstraint group = getConstraint(Core::FMTconstrainttype::FMTSpatialGroup);
    const Core::FMTConstraint standard = getAtLeast(100.0);

    // Act
    const std::vector<bool> spatial = { size.isSpatial(), adjacency.isSpatial(), group.isSpatial(), standard.isSpatial() };

    // Assert
    EXPECT_EQ(spatial, std::vector<bool>({ true, true, true, false }));
}

TEST(FMTConstraint, GivenAnObjectiveASpatialAndAStandardConstraintWhenGetGroupThenObjectivesComeFirstThenSpatial)
{
    // Arrange
    const Core::FMTConstraint objective = getConstraint(Core::FMTconstrainttype::FMTMINobjective);
    const Core::FMTConstraint spatial = getConstraint(Core::FMTconstrainttype::FMTspatialadjacency);
    const Core::FMTConstraint standard = getAtLeast(100.0);

    // Act
    const std::vector<size_t> groups = { objective.getGroup(), spatial.getGroup(), standard.getGroup() };

    // Assert
    EXPECT_EQ(groups, std::vector<size_t>({ 0, 1, 2 }));
}

TEST(FMTConstraint, GivenARandomActionConstraintWhenIsRandomActionThenReturnsTrue)
{
    // Arrange: _RANDOM(OVOL) >= 1 1.._LENGTH
    Core::FMTConstraint random = getConstraint(Core::FMTconstrainttype::FMTrandomaction);
    random.setRhs(1.0, UNBOUNDED_ABOVE);

    // Act
    const bool isRandom = random.isRandomAction();
    const bool standardIsRandom = getAtLeast(1.0).isRandomAction();

    // Assert
    EXPECT_TRUE(isRandom);
    EXPECT_FALSE(standardIsRandom);
}

TEST(FMTConstraint, GivenAConstraintWhenSetLengthThenSetsItsFirstAndLastPeriods)
{
    // Arrange
    Core::FMTConstraint constraint(Core::FMTconstrainttype::FMTstandard, getOutput());
    Core::FMTConstraint toTheEnd(Core::FMTconstrainttype::FMTstandard, getOutput());

    // Act
    constraint.setLength(2, 4);
    toTheEnd.setLength();

    // Assert
    EXPECT_EQ(constraint.getPeriodLowerBound(), 2);
    EXPECT_EQ(constraint.getPeriodUpperBound(), 4);
    EXPECT_EQ(toTheEnd.getPeriodLowerBound(), 1);
    EXPECT_EQ(toTheEnd.getPeriodUpperBound(), LENGTH);
}

TEST(FMTConstraint, GivenAnEvenFlowWithAVariationWhenGetVariationsThenReturnsItsPercentagesAsFractions)
{
    // Arrange: _EVEN(OVOL,10%,20%)
    Core::FMTConstraint evenFlow = getConstraint(Core::FMTconstrainttype::FMTevenflow);
    setVariation(evenFlow, 10.0, 20.0);
    double lower = 0;
    double upper = 0;

    // Act
    evenFlow.getVariations(lower, upper);

    // Assert
    EXPECT_TRUE(evenFlow.isMultiple());
    EXPECT_DOUBLE_EQ(lower, 0.1);
    EXPECT_DOUBLE_EQ(upper, 0.2);
}

TEST(FMTConstraint, GivenAConstraintWithoutVariationWhenGetVariationsThenReturnsZeros)
{
    // Arrange
    const Core::FMTConstraint constraint = getAtLeast(100.0);
    double lower = 1;
    double upper = 1;

    // Act
    constraint.getVariations(lower, upper);

    // Assert
    EXPECT_FALSE(constraint.isMultiple());
    EXPECT_EQ(lower, 0.0);
    EXPECT_EQ(upper, 0.0);
}

TEST(FMTConstraint, GivenAGoalWhenGetGoalThenReturnsItsNameAndItsWeight)
{
    // Arrange: OVOL >= 100 1.._LENGTH _GOAL(CARIBOU,2.5)
    Core::FMTConstraint constraint = getAtLeast(100.0);
    constraint.setGoal("CARIBOU", 2.5);
    std::string name;
    double weight = 0;

    // Act
    constraint.getGoal(name, weight);

    // Assert
    EXPECT_TRUE(constraint.isGoal());
    EXPECT_EQ(name, "CARIBOU");
    EXPECT_EQ(weight, 2.5);
}

TEST(FMTConstraint, GivenAnObjectiveWhenSetGoalThenAddsNoGoal)
{
    // Arrange
    Core::FMTConstraint objective = getConstraint(Core::FMTconstrainttype::FMTMINobjective);

    // Act
    objective.setGoal("CARIBOU", 2.5);

    // Assert
    EXPECT_FALSE(objective.isGoal());
}

TEST(FMTConstraint, GivenPenaltiesWithAMinusWhenGetPenaltiesThenReturnsTheGoalsInOrderAndAMinusSense)
{
    // Arrange: _MIN OVOL - _PENALTY(RECOLTE,CARIBOU) 1.._LENGTH
    Core::FMTConstraint objective = getConstraint(Core::FMTconstrainttype::FMTMINobjective);
    objective.setPenalties("-", { "RECOLTE", "CARIBOU" });
    double sense = 0;

    // Act
    const std::vector<std::string> penalties = objective.getPenalties(sense);

    // Assert
    EXPECT_TRUE(objective.isGoal());
    EXPECT_EQ(penalties, std::vector<std::string>({ "CARIBOU", "RECOLTE" }));
    EXPECT_EQ(sense, -1.0);
}

// _PENALTY(_ALL) gives the goal ALL, which FMTLpModel::getGoals takes for every goal.
TEST(FMTConstraint, GivenAPenaltyOfAllTheGoalsWhenGetPenaltiesThenReturnsAll)
{
    // Arrange: _MIN OVOL + _PENALTY(_ALL) 1.._LENGTH
    Core::FMTConstraint objective = getConstraint(Core::FMTconstrainttype::FMTMINobjective);
    objective.setPenalties("+", { "_ALL" });
    double sense = 0;

    // Act
    const std::vector<std::string> penalties = objective.getPenalties(sense);

    // Assert
    EXPECT_EQ(penalties, std::vector<std::string>({ "ALL" }));
    EXPECT_EQ(sense, 1.0);
}

TEST(FMTConstraint, GivenAStandardConstraintWhenSetPenaltiesThenAddsNoPenalty)
{
    // Arrange
    Core::FMTConstraint constraint = getAtLeast(100.0);

    // Act
    constraint.setPenalties("+", { "CARIBOU" });

    // Assert
    EXPECT_FALSE(constraint.isGoal());
}

TEST(FMTConstraint, GivenAConstraintAtLeastAValueWhenConvertedToStringThenWritesItsOperatorValueAndPeriods)
{
    // Arrange
    const Core::FMTConstraint constraint = getAtLeast(100.0);

    // Act
    const std::string written = std::string(constraint);

    // Assert
    EXPECT_EQ(written, "OVOL >= 100.000000 1.._LENGTH");
}

TEST(FMTConstraint, GivenAnEqualityOnSomePeriodsWhenConvertedToStringThenWritesTheEqualityAndTheRangeOfPeriods)
{
    // Arrange: OVOL = 50 2..4
    Core::FMTConstraint constraint = getConstraint(Core::FMTconstrainttype::FMTstandard, 2, 4);
    constraint.setRhs(50.0, 50.0);

    // Act
    const std::string written = std::string(constraint);

    // Assert
    EXPECT_EQ(written, "OVOL = 50.000000 2..4");
}

TEST(FMTConstraint, GivenAConstraintAtMostAValueOnOnePeriodWhenConvertedToStringThenWritesThatPeriodAlone)
{
    // Arrange: OVOL <= 200 3
    Core::FMTConstraint constraint = getConstraint(Core::FMTconstrainttype::FMTstandard, 3, 3);
    constraint.setRhs(UNBOUNDED_BELOW, 200.0);

    // Act
    const std::string written = std::string(constraint);

    // Assert
    EXPECT_EQ(written, "OVOL <= 200.000000 3");
}

TEST(FMTConstraint, GivenAMaximizationWhenConvertedToStringThenWritesTheKeywordTheOutputAndThePeriods)
{
    // Arrange
    const Core::FMTConstraint objective = getConstraint(Core::FMTconstrainttype::FMTMAXobjective);

    // Act
    const std::string written = std::string(objective);

    // Assert
    EXPECT_EQ(written, "_MAX OVOL 1.._LENGTH");
}

TEST(FMTConstraint, GivenAMinimizationWithSignedPenaltiesWhenConvertedToStringThenWritesThePenaltiesAfterTheOutput)
{
    // Arrange: _MIN OVOL + _PENALTY(CARIBOU,RECOLTE) 1.._LENGTH
    Core::FMTConstraint objective = getConstraint(Core::FMTconstrainttype::FMTMINobjective);
    objective.setPenalties("+", { "CARIBOU", "RECOLTE" });

    // Act
    const std::string written = std::string(objective);

    // Assert
    EXPECT_EQ(written, "_MIN OVOL+_PENALTY(CARIBOU,RECOLTE) 1.._LENGTH");
}

// Disabled: a penalty without sign is stored as Penalty_<goal>, and operator std::string takes the last letter of
// Penalty for its sign (#379).
TEST(FMTConstraint, DISABLED_GivenAMinimizationOfPenaltiesWithoutSignWhenConvertedToStringThenWritesThemWithoutSign)
{
    // Arrange: _MIN _PENALTY(CARIBOU,RECOLTE) 1.._LENGTH, as the objective of TWD_land
    Core::FMTConstraint objective(Core::FMTconstrainttype::FMTMINobjective, Core::FMTOutput());
    objective.setLength();
    objective.setPenalties("", { "CARIBOU", "RECOLTE" });

    // Act
    const std::string written = std::string(objective);

    // Assert
    EXPECT_EQ(written, "_MIN _PENALTY(CARIBOU,RECOLTE) 1.._LENGTH");
}

TEST(FMTConstraint, GivenAnEvenFlowWithALowerVariationWhenConvertedToStringThenWritesItAsAPercentage)
{
    // Arrange: _EVEN(OVOL,10%) 1.._LENGTH
    Core::FMTConstraint evenFlow = getConstraint(Core::FMTconstrainttype::FMTevenflow);
    setVariation(evenFlow, 10.0, 0.0);

    // Act
    const std::string written = std::string(evenFlow);

    // Assert
    EXPECT_EQ(written, "_EVEN(OVOL,10%) 1.._LENGTH");
}

TEST(FMTConstraint, GivenAnEvenFlowWithBothVariationsWhenConvertedToStringThenWritesBothPercentages)
{
    // Arrange: _EVEN(OVOL,10%,20%) 1.._LENGTH
    Core::FMTConstraint evenFlow = getConstraint(Core::FMTconstrainttype::FMTevenflow);
    setVariation(evenFlow, 10.0, 20.0);

    // Act
    const std::string written = std::string(evenFlow);

    // Assert
    EXPECT_EQ(written, "_EVEN(OVOL,10%,20%) 1.._LENGTH");
}

TEST(FMTConstraint, GivenANonDecliningYieldWithoutVariationWhenConvertedToStringThenWritesTheOutputAlone)
{
    // Arrange: _NDY(OVOL) 1.._LENGTH, whose variation FMTOptimizationParser reads as 0
    Core::FMTConstraint nonDeclining = getConstraint(Core::FMTconstrainttype::FMTnondeclining);
    setVariation(nonDeclining, 0.0, 0.0);

    // Act
    const std::string written = std::string(nonDeclining);

    // Assert
    EXPECT_EQ(written, "_NDY(OVOL) 1.._LENGTH");
}

// Disabled: a _RANDOM(<output>) constraint is written as a standard constraint, without _RANDOM, and read back as one
// (#382).
TEST(FMTConstraint, DISABLED_GivenARandomActionConstraintWhenConvertedToStringThenWritesRandom)
{
    // Arrange: _RANDOM(OVOL) >= 1 1.._LENGTH
    Core::FMTConstraint random = getConstraint(Core::FMTconstrainttype::FMTrandomaction);
    random.setRhs(1.0, UNBOUNDED_ABOVE);

    // Act
    const std::string written = std::string(random);

    // Assert
    EXPECT_NE(written.find("_RANDOM(OVOL)"), std::string::npos) << "written: " << written;
}

TEST(FMTConstraint, GivenTwoConstraintsWhenComparedThenTheyAreEqualOnlyWithTheSameTypeOutputAndBounds)
{
    // Arrange
    const Core::FMTConstraint constraint = getAtLeast(100.0);
    const Core::FMTConstraint same = getAtLeast(100.0);
    const Core::FMTConstraint otherBound = getAtLeast(150.0);
    Core::FMTConstraint otherType = getAtLeast(100.0);
    otherType.setConstraintType(Core::FMTconstrainttype::FMTrandomaction);

    // Act
    const bool equalToSame = (constraint == same);
    const bool equalToOtherBound = (constraint == otherBound);
    const bool equalToOtherType = (constraint == otherType);

    // Assert
    EXPECT_TRUE(equalToSame);
    EXPECT_FALSE(equalToOtherBound);
    EXPECT_FALSE(equalToOtherType);
    EXPECT_TRUE(constraint != otherBound);
}

TEST(FMTConstraint, GivenAConstraintWhenCopiedThenTheCopyIsEqual)
{
    // Arrange
    Core::FMTConstraint original = getAtLeast(100.0);
    original.setGoal("CARIBOU", 2.5);

    // Act
    const Core::FMTConstraint copy(original);
    Core::FMTConstraint assigned;
    assigned = original;

    // Assert
    EXPECT_TRUE(copy == original);
    EXPECT_TRUE(assigned == original);
    EXPECT_EQ(std::string(assigned), std::string(original));
}

TEST(FMTConstraint, GivenEqualConstraintsWhenHashThenTheyHaveTheSameHash)
{
    // Arrange
    const Core::FMTConstraint first = getAtLeast(100.0);
    const Core::FMTConstraint second = getAtLeast(100.0);

    // Act
    const size_t firstHash = first.hash();
    const size_t secondHash = second.hash();

    // Assert
    EXPECT_EQ(firstHash, secondHash);
}

TEST(FMTConstraint, GivenConstraintsThatDifferByTheirBoundWhenHashWithoutTheRightHandSideThenOnlyTheFullHashesDiffer)
{
    // Arrange
    const Core::FMTConstraint lower = getAtLeast(100.0);
    const Core::FMTConstraint higher = getAtLeast(150.0);

    // Act
    const bool sameFullHash = (lower.hash() == higher.hash());
    const bool sameHashWithoutBound = (lower.hash(false) == higher.hash(false));

    // Assert
    EXPECT_FALSE(sameFullHash);
    EXPECT_TRUE(sameHashWithoutBound);
}

// evaluate gives the cost of a constraint for the values of its output at each of its periods, as the spatial
// heuristics read it: the closer to 0 the better, and for an objective the smaller the better.
TEST(FMTConstraint, GivenObjectivesWhenEvaluateThenSumsOrTakesTheExtremeValue)
{
    // Arrange
    const std::vector<double> values = { 1.0, 2.0, 3.0 };
    const Core::FMTConstraint maximize = getConstraint(Core::FMTconstrainttype::FMTMAXobjective);
    const Core::FMTConstraint minimize = getConstraint(Core::FMTconstrainttype::FMTMINobjective);
    const Core::FMTConstraint maxMin = getConstraint(Core::FMTconstrainttype::FMTMAXMINobjective);
    const Core::FMTConstraint minMax = getConstraint(Core::FMTconstrainttype::FMTMINMAXobjective);

    // Act
    const std::vector<double> costs = { maximize.evaluate(values), minimize.evaluate(values), maxMin.evaluate(values),
        minMax.evaluate(values) };

    // Assert
    EXPECT_EQ(costs, std::vector<double>({ -6.0, 6.0, -1.0, 3.0 }));
}

TEST(FMTConstraint, GivenAConstraintAtLeastAValueWhenEvaluateThenSumsTheShortfallOfEachPeriod)
{
    // Arrange: OVOL >= 100, at 120, 80 and 90
    const Core::FMTConstraint constraint = getAtLeast(100.0);

    // Act
    const double cost = constraint.evaluate({ 120.0, 80.0, 90.0 });

    // Assert
    EXPECT_DOUBLE_EQ(cost, 30.0);
}

TEST(FMTConstraint, GivenANonDecliningYieldAndASequenceWhenEvaluateThenSumTheDecreasesAndForTheSequenceTheIncreases)
{
    // Arrange: 100, 80 then 90
    const std::vector<double> values = { 100.0, 80.0, 90.0 };
    Core::FMTConstraint nonDeclining = getConstraint(Core::FMTconstrainttype::FMTnondeclining);
    setVariation(nonDeclining, 0.0, 0.0);
    Core::FMTConstraint sequence = getConstraint(Core::FMTconstrainttype::FMTsequence);
    setVariation(sequence, 0.0, 0.0);

    // Act
    const double nonDecliningCost = nonDeclining.evaluate(values);
    const double sequenceCost = sequence.evaluate(values);

    // Assert
    EXPECT_DOUBLE_EQ(nonDecliningCost, 20.0);
    EXPECT_DOUBLE_EQ(sequenceCost, 30.0);
}

TEST(FMTConstraint, GivenAnEvenFlowWithoutVariationWhenEvaluateThenSumsTheGapsToTheLargestValue)
{
    // Arrange: 100, 80 then 90
    Core::FMTConstraint evenFlow = getConstraint(Core::FMTconstrainttype::FMTevenflow);
    setVariation(evenFlow, 0.0, 0.0);

    // Act
    const double cost = evenFlow.evaluate({ 100.0, 80.0, 90.0 });

    // Assert
    EXPECT_DOUBLE_EQ(cost, 30.0);
}

TEST(FMTConstraint, GivenAnObjectiveOfAnInfiniteValueWhenEvaluateThenRaisesARangeError)
{
    // Arrange
    const Core::FMTConstraint minimize = getConstraint(Core::FMTconstrainttype::FMTMINobjective);

    // Act
    const std::vector<Exception::FMTexc> raised = Testing::getRaisedTypes([&]()
        {
        minimize.evaluate({ 1.0, UNBOUNDED_ABOVE });
        });

    // Assert
    EXPECT_TRUE(Testing::hasType(raised, Exception::FMTexc::FMTrangeerror));
}
