/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTExpression.h"

#include <gtest/gtest.h>

#include <map>
#include <stdexcept>
#include <string>
#include <vector>

// An expression is a list of tokens, as FMTData::toExpression gives them: numbers, variables, operators, function
// names and parentheses. shuntingYard replaces each variable by its value in the mapping, then evaluates the tokens.

TEST(FMTExpression, GivenADefaultExpressionWhenShuntingYardThenReturnsZero)
{
    // Arrange
    const Core::FMTExpression expression;
    const std::map<std::string, double> noValues;

    // Act
    const double value = expression.shuntingYard(noValues);

    // Assert
    EXPECT_EQ(value, 0.0);
}

TEST(FMTExpression, GivenTokensWhenGetInFixThenReturnsThemInTheirOrder)
{
    // Arrange
    const std::vector<std::string> tokens = { "A", "*", "(", "2", "+", "B", ")" };
    const Core::FMTExpression expression(tokens);

    // Act
    const std::vector<std::string> inFix = expression.getInFix();

    // Assert
    EXPECT_EQ(inFix, tokens);
}

TEST(FMTExpression, GivenTokensWhenConvertedToStringThenPutsASpaceBeforeEachToken)
{
    // Arrange
    const Core::FMTExpression expression({ "A", "+", "1" });

    // Act
    const std::string written = std::string(expression);

    // Assert
    EXPECT_EQ(written, " A + 1");
}

TEST(FMTExpression, GivenAnExpressionWhenCopiedThenTheCopyHasTheSameTokens)
{
    // Arrange
    const Core::FMTExpression original({ "A", "+", "1" });

    // Act
    const Core::FMTExpression copy(original);
    Core::FMTExpression assigned({ "B" });
    assigned = original;

    // Assert
    EXPECT_EQ(copy.getInFix(), original.getInFix());
    EXPECT_EQ(assigned.getInFix(), original.getInFix());
}

TEST(FMTExpression, GivenAnAdditionThenAMultiplicationWhenShuntingYardThenMultipliesFirst)
{
    // Arrange
    const Core::FMTExpression expression({ "2", "+", "3", "*", "4" });
    const std::map<std::string, double> noValues;

    // Act
    const double value = expression.shuntingYard(noValues);

    // Assert
    EXPECT_DOUBLE_EQ(value, 14.0);
}

TEST(FMTExpression, GivenParenthesesWhenShuntingYardThenEvaluatesThemFirst)
{
    // Arrange
    const Core::FMTExpression expression({ "(", "2", "+", "3", ")", "*", "4" });
    const std::map<std::string, double> noValues;

    // Act
    const double value = expression.shuntingYard(noValues);

    // Assert
    EXPECT_DOUBLE_EQ(value, 20.0);
}

TEST(FMTExpression, GivenSuccessiveSubtractionsWhenShuntingYardThenEvaluatesThemFromLeftToRight)
{
    // Arrange
    const Core::FMTExpression expression({ "10", "-", "4", "-", "3" });
    const std::map<std::string, double> noValues;

    // Act
    const double value = expression.shuntingYard(noValues);

    // Assert
    EXPECT_DOUBLE_EQ(value, 3.0);
}

TEST(FMTExpression, GivenSuccessiveDivisionsWhenShuntingYardThenEvaluatesThemFromLeftToRight)
{
    // Arrange
    const Core::FMTExpression expression({ "100", "/", "10", "/", "2" });
    const std::map<std::string, double> noValues;

    // Act
    const double value = expression.shuntingYard(noValues);

    // Assert
    EXPECT_DOUBLE_EQ(value, 5.0);
}

// Characterizes a known behavior (issue #346): ^ has the precedence of * and /, and _getPostFix evaluates the
// operators of equal precedence from left to right, so 2*3^2 gives (2*3)^2 = 36 and 12/2^2 gives (12/2)^2 = 36,
// where the usual precedence gives 18 and 3. Changing it changes the yields of existing models.
TEST(FMTExpression, GivenAPowerAfterAMultiplicationWhenShuntingYardThenEvaluatesThemFromLeftToRight)
{
    // Arrange
    const Core::FMTExpression expression({ "2", "*", "3", "^", "2" });
    const std::map<std::string, double> noValues;

    // Act
    const double value = expression.shuntingYard(noValues);

    // Assert
    EXPECT_DOUBLE_EQ(value, 36.0);
}

// Same known behavior (issue #346).
TEST(FMTExpression, GivenAPowerAfterADivisionWhenShuntingYardThenEvaluatesThemFromLeftToRight)
{
    // Arrange
    const Core::FMTExpression expression({ "12", "/", "2", "^", "2" });
    const std::map<std::string, double> noValues;

    // Act
    const double value = expression.shuntingYard(noValues);

    // Assert
    EXPECT_DOUBLE_EQ(value, 36.0);
}

// Same mechanism as issue #346: two powers are evaluated from left to right, so 2^3^2 gives (2^3)^2 = 64, where the
// usual right associativity of ^ gives 2^9 = 512.
TEST(FMTExpression, GivenTwoPowersWhenShuntingYardThenEvaluatesThemFromLeftToRight)
{
    // Arrange
    const Core::FMTExpression expression({ "2", "^", "3", "^", "2" });
    const std::map<std::string, double> noValues;

    // Act
    const double value = expression.shuntingYard(noValues);

    // Assert
    EXPECT_DOUBLE_EQ(value, 64.0);
}

// The parentheses get around issue #346: 2*(3^2) gives 18.
TEST(FMTExpression, GivenParenthesesAroundAPowerWhenShuntingYardThenAppliesThePowerFirst)
{
    // Arrange
    const Core::FMTExpression expression({ "2", "*", "(", "3", "^", "2", ")" });
    const std::map<std::string, double> noValues;

    // Act
    const double value = expression.shuntingYard(noValues);

    // Assert
    EXPECT_DOUBLE_EQ(value, 18.0);
}

TEST(FMTExpression, GivenALeadingMinusWhenShuntingYardThenNegatesTheFirstValue)
{
    // Arrange
    const Core::FMTExpression expression({ "-", "5", "+", "2" });
    const std::map<std::string, double> noValues;

    // Act
    const double value = expression.shuntingYard(noValues);

    // Assert
    EXPECT_DOUBLE_EQ(value, -3.0);
}

// Reveals defect 11 of section 2.7 of Tests/ETAT.md, issue to open: a minus that follows an operator is taken for a
// subtraction. _getPostFix gives 2 * 3 - in postfix, and _evaluatePostFix then multiplies 2 by a missing left
// operand, 0, before subtracting 3: 2*-3 gives -3 instead of -6.
TEST(FMTExpression, DISABLED_GivenAMinusAfterAMultiplicationWhenShuntingYardThenNegatesTheValueThatFollows)
{
    // Arrange
    const Core::FMTExpression expression({ "2", "*", "-", "3" });
    const std::map<std::string, double> noValues;

    // Act
    const double value = expression.shuntingYard(noValues);

    // Assert
    EXPECT_DOUBLE_EQ(value, -6.0);
}

TEST(FMTExpression, GivenVariablesWhenShuntingYardThenReplacesThemByTheirValues)
{
    // Arrange
    const Core::FMTExpression expression({ "A", "*", "2", "+", "B" });
    const std::map<std::string, double> values = { { "A", 3.0 }, { "B", 1.0 } };

    // Act
    const double value = expression.shuntingYard(values);

    // Assert
    EXPECT_DOUBLE_EQ(value, 7.0);
}

// A variable without a value stays a name, which std::stod cannot read.
TEST(FMTExpression, GivenAVariableWithoutValueWhenShuntingYardThenThrowsAnInvalidArgument)
{
    // Arrange
    const Core::FMTExpression expression({ "A", "+", "B" });
    const std::map<std::string, double> valueOfBOnly = { { "B", 1.0 } };

    // Act and Assert
    EXPECT_THROW(expression.shuntingYard(valueOfBOnly), std::invalid_argument);
}

TEST(FMTExpression, GivenAFunctionCallWhenShuntingYardThenAppliesTheFunctionToItsArgument)
{
    // Arrange
    const Core::FMTExpression expression({ "2", "*", "EXP", "(", "1", ")" });
    const std::map<std::string, double> noValues;

    // Act
    const double value = expression.shuntingYard(noValues);

    // Assert
    EXPECT_DOUBLE_EQ(value, 2.0 * 2.71828183);
}

TEST(FMTExpression, GivenAFunctionOfAnExpressionWhenShuntingYardThenEvaluatesItsArgumentFirst)
{
    // Arrange
    const Core::FMTExpression expression({ "LN", "(", "A", "*", "5", ")" });
    const std::map<std::string, double> values = { { "A", 2.0 } };

    // Act
    const double value = expression.shuntingYard(values);

    // Assert
    EXPECT_DOUBLE_EQ(value, 2.30258509);
}

TEST(FMTExpression, GivenVariablesNumbersAndOperatorsWhenGetVariablesThenReturnsTheVariables)
{
    // Arrange
    const Core::FMTExpression expression({ "A", "*", "2.5", "+", "B" });

    // Act
    const std::vector<std::string> variables = expression.getVariables();

    // Assert
    EXPECT_EQ(variables, std::vector<std::string>({ "A", "B" }));
}

// Characterizes getVariables, which no method of FMT calls: it skips the numbers, the operators and the function
// names, but not the parentheses.
TEST(FMTExpression, GivenParenthesesWhenGetVariablesThenReturnsThemWithTheVariables)
{
    // Arrange
    const Core::FMTExpression expression({ "LN", "(", "A", ")", "+", "2" });

    // Act
    const std::vector<std::string> variables = expression.getVariables();

    // Assert
    EXPECT_EQ(variables, std::vector<std::string>({ "(", "A", ")" }));
}

// simplify serves the equations of the optimization section (FMTOptimizationParser), linear in their variables:
// the factor of a variable is the value of the expression with that variable at 1 and the others at 0, minus its
// value with every variable at 0, and the right-hand side RHS is minus that value.
TEST(FMTExpression, GivenALinearExpressionWhenSimplifyThenGivesTheFactorOfEachVariableAndTheRightHandSide)
{
    // Arrange
    const Core::FMTExpression expression({ "2", "*", "A", "+", "3", "*", "B", "-", "5" });
    std::map<std::string, double> values;
    const std::map<std::string, double> expectedValues = { { "A", 2.0 }, { "B", 3.0 }, { "RHS", 5.0 } };

    // Act
    expression.simplify(values);

    // Assert
    EXPECT_EQ(values, expectedValues);
}

TEST(FMTExpression, GivenALinearExpressionWhenSimplifyThenReturnsEachVariableTimesItsFactorThenTheConstant)
{
    // Arrange
    const Core::FMTExpression expression({ "2", "*", "A", "+", "3", "*", "B", "-", "5" });
    std::map<std::string, double> values;

    // Act
    const Core::FMTExpression simplified = expression.simplify(values);

    // Assert
    EXPECT_EQ(std::string(simplified), " A * 2.000000 + B * 3.000000 - 5.000000");
}
