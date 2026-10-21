/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTOperator.h"

#include <gtest/gtest.h>

#include <limits>
#include <string>
#include <vector>

TEST(FMTOperator, GivenMultiplyAndAddWhenPrecedenceThenMultiplyComesFirst)
{
    // Arrange
    const Core::FMTOperator multiply(Core::FMTokey::multiply);
    const Core::FMTOperator add(Core::FMTokey::add);

    // Act
    const int multiplyPrecedence = multiply.precedence();
    const int addPrecedence = add.precedence();

    // Assert
    EXPECT_GT(multiplyPrecedence, addPrecedence);
}

// Characterizes a known behavior (issue #346): ^ has the precedence of * and /, so an expression evaluates them
// from left to right, and 2*3^2 gives 36. Changing it changes the yields of existing models.
TEST(FMTOperator, GivenPowerAndMultiplyWhenPrecedenceThenTheyAreEqual)
{
    // Arrange
    const Core::FMTOperator power(Core::FMTokey::pow);
    const Core::FMTOperator multiply(Core::FMTokey::multiply);

    // Act
    const int powerPrecedence = power.precedence();
    const int multiplyPrecedence = multiply.precedence();

    // Assert
    EXPECT_EQ(powerPrecedence, multiplyPrecedence);
}

TEST(FMTOperator, GivenSubtractAndAddWhenPrecedenceThenTheyAreEqual)
{
    // Arrange
    const Core::FMTOperator subtract(Core::FMTokey::sub);
    const Core::FMTOperator add(Core::FMTokey::add);

    // Act
    const int subtractPrecedence = subtract.precedence();
    const int addPrecedence = add.precedence();

    // Assert
    EXPECT_EQ(subtractPrecedence, addPrecedence);
}

TEST(FMTOperator, GivenACommaAndAddWhenPrecedenceThenTheCommaComesLast)
{
    // Arrange
    const Core::FMTOperator comma(Core::FMTokey::comma);
    const Core::FMTOperator add(Core::FMTokey::add);

    // Act
    const int commaPrecedence = comma.precedence();
    const int addPrecedence = add.precedence();

    // Assert
    EXPECT_LT(commaPrecedence, addPrecedence);
}

TEST(FMTOperator, GivenEachOperatorSymbolWhenConstructedFromItThenGetsTheKeyOfThatOperator)
{
    // Arrange
    const std::vector<std::string> symbols = { "*", "/", "+", "-", ",", "^" };
    const std::vector<Core::FMTokey> expectedKeys = { Core::FMTokey::multiply, Core::FMTokey::divide,
        Core::FMTokey::add, Core::FMTokey::sub, Core::FMTokey::comma, Core::FMTokey::pow };

    // Act
    std::vector<Core::FMTokey> keys;
    for (const std::string& symbol : symbols)
    {
        keys.push_back(Core::FMTOperator(symbol).getKey());
    }

    // Assert
    EXPECT_EQ(keys, expectedKeys);
}

TEST(FMTOperator, GivenEachOperatorSymbolWhenConstructedFromItThenConvertsBackToTheSameSymbol)
{
    // Arrange
    const std::vector<std::string> symbols = { "*", "/", "+", "-", ",", "^" };

    // Act
    std::vector<std::string> converted;
    for (const std::string& symbol : symbols)
    {
        converted.push_back(std::string(Core::FMTOperator(symbol)));
    }

    // Assert
    EXPECT_EQ(converted, symbols);
}

TEST(FMTOperator, GivenAnUnknownSymbolWhenConstructedFromItThenIsNotValid)
{
    // Arrange
    const std::string unknownSymbol = "%";

    // Act
    const Core::FMTOperator unknown(unknownSymbol);

    // Assert
    EXPECT_FALSE(unknown.valid());
    EXPECT_EQ(std::string(unknown), "");
}

// The default operator holds the key 0, which is no FMTokey: it is neither an operator nor FMTokey::notvalid, so
// that valid() accepts it, while its call and its string are those of no operator.
TEST(FMTOperator, GivenADefaultOperatorWhenValidThenAcceptsIt)
{
    // Arrange
    const Core::FMTOperator defaultOperator;

    // Act
    const bool valid = defaultOperator.valid();

    // Assert
    EXPECT_TRUE(valid);
}

TEST(FMTOperator, GivenADefaultOperatorWhenCallThenReturnsZero)
{
    // Arrange
    const Core::FMTOperator defaultOperator;

    // Act
    const double result = defaultOperator.call(7.0, 2.0);

    // Assert
    EXPECT_EQ(result, 0.0);
}

TEST(FMTOperator, GivenTwoValuesWhenCallThenAppliesTheArithmeticOfEachOperator)
{
    // Arrange
    const double left = 7.0;
    const double right = 2.0;

    // Act
    const double sum = Core::FMTOperator(Core::FMTokey::add).call(left, right);
    const double difference = Core::FMTOperator(Core::FMTokey::sub).call(left, right);
    const double product = Core::FMTOperator(Core::FMTokey::multiply).call(left, right);
    const double quotient = Core::FMTOperator(Core::FMTokey::divide).call(left, right);
    const double power = Core::FMTOperator(Core::FMTokey::pow).call(left, right);

    // Assert
    EXPECT_DOUBLE_EQ(sum, 9.0);
    EXPECT_DOUBLE_EQ(difference, 5.0);
    EXPECT_DOUBLE_EQ(product, 14.0);
    EXPECT_DOUBLE_EQ(quotient, 3.5);
    EXPECT_DOUBLE_EQ(power, 49.0);
}

TEST(FMTOperator, GivenACommaOrAnInvalidOperatorWhenCallThenReturnsZero)
{
    // Arrange
    const Core::FMTOperator comma(Core::FMTokey::comma);
    const Core::FMTOperator invalid(Core::FMTokey::notvalid);

    // Act
    const double commaResult = comma.call(7.0, 2.0);
    const double invalidResult = invalid.call(7.0, 2.0);

    // Assert
    EXPECT_EQ(commaResult, 0.0);
    EXPECT_EQ(invalidResult, 0.0);
}

// The division does not guard a zero divisor, unlike _DIVIDE of FMTComplexYieldHandler, which gives 0: an
// equation that divides by a zero yield gives an infinite value.
TEST(FMTOperator, GivenADivisionByZeroWhenCallThenReturnsInfinity)
{
    // Arrange
    const Core::FMTOperator divide(Core::FMTokey::divide);

    // Act
    const double quotient = divide.call(1.0, 0.0);

    // Assert
    EXPECT_EQ(quotient, std::numeric_limits<double>::infinity());
}

TEST(FMTOperator, GivenAnAdditionOrASubtractionWhenReverseThenReturnsTheOtherOne)
{
    // Arrange
    const Core::FMTOperator add(Core::FMTokey::add);
    const Core::FMTOperator subtract(Core::FMTokey::sub);

    // Act
    const Core::FMTOperator reversedAdd = add.reverse();
    const Core::FMTOperator reversedSubtract = subtract.reverse();

    // Assert
    EXPECT_TRUE(reversedAdd == subtract);
    EXPECT_TRUE(reversedSubtract == add);
}

TEST(FMTOperator, GivenAMultiplicationADivisionOrAPowerWhenReverseThenReturnsItUnchanged)
{
    // Arrange
    const Core::FMTOperator multiply(Core::FMTokey::multiply);
    const Core::FMTOperator divide(Core::FMTokey::divide);
    const Core::FMTOperator power(Core::FMTokey::pow);

    // Act
    const Core::FMTOperator reversedMultiply = multiply.reverse();
    const Core::FMTOperator reversedDivide = divide.reverse();
    const Core::FMTOperator reversedPower = power.reverse();

    // Assert
    EXPECT_TRUE(reversedMultiply == multiply);
    EXPECT_TRUE(reversedDivide == divide);
    EXPECT_TRUE(reversedPower == power);
}

TEST(FMTOperator, GivenEachOperatorWhenIsFactorThenOnlyAMultiplicationOrADivisionIsOne)
{
    // Arrange
    const std::vector<Core::FMTokey> keys = { Core::FMTokey::multiply, Core::FMTokey::divide,
        Core::FMTokey::add, Core::FMTokey::sub, Core::FMTokey::comma, Core::FMTokey::pow };
    const std::vector<bool> expectedFactors = { true, true, false, false, false, false };

    // Act
    std::vector<bool> factors;
    for (const Core::FMTokey key : keys)
    {
        factors.push_back(Core::FMTOperator(key).isFactor());
    }

    // Assert
    EXPECT_EQ(factors, expectedFactors);
}

TEST(FMTOperator, GivenEachOperatorWhenIsDivideThenOnlyADivisionIsOne)
{
    // Arrange
    const std::vector<Core::FMTokey> keys = { Core::FMTokey::multiply, Core::FMTokey::divide,
        Core::FMTokey::add, Core::FMTokey::sub, Core::FMTokey::comma, Core::FMTokey::pow };
    const std::vector<bool> expectedDivisions = { false, true, false, false, false, false };

    // Act
    std::vector<bool> divisions;
    for (const Core::FMTokey key : keys)
    {
        divisions.push_back(Core::FMTOperator(key).isDivide());
    }

    // Assert
    EXPECT_EQ(divisions, expectedDivisions);
}

// Characterizes associativity, which no method of FMT calls: _getPostFix of FMTExpression evaluates the operators
// of equal precedence from left to right whatever it returns. It gives RIGHT for the subtraction and the division,
// and LEFT for the power.
TEST(FMTOperator, GivenASubtractionOrADivisionWhenAssociativityThenReturnsRight)
{
    // Arrange
    const Core::FMTOperator subtract(Core::FMTokey::sub);
    const Core::FMTOperator divide(Core::FMTokey::divide);

    // Act
    const std::string subtractAssociativity = subtract.associativity();
    const std::string divideAssociativity = divide.associativity();

    // Assert
    EXPECT_EQ(subtractAssociativity, "RIGHT");
    EXPECT_EQ(divideAssociativity, "RIGHT");
}

TEST(FMTOperator, GivenAnAdditionAMultiplicationAPowerOrACommaWhenAssociativityThenReturnsLeft)
{
    // Arrange
    const std::vector<Core::FMTokey> keys = { Core::FMTokey::add, Core::FMTokey::multiply,
        Core::FMTokey::pow, Core::FMTokey::comma };

    // Act
    std::vector<std::string> associativities;
    for (const Core::FMTokey key : keys)
    {
        associativities.push_back(Core::FMTOperator(key).associativity());
    }

    // Assert
    EXPECT_EQ(associativities, std::vector<std::string>(keys.size(), "LEFT"));
}

TEST(FMTOperator, GivenAnOperatorWhenCopiedThenTheCopyIsEqualAndHasTheSameHash)
{
    // Arrange
    const Core::FMTOperator original(Core::FMTokey::pow);

    // Act
    const Core::FMTOperator copy(original);
    Core::FMTOperator assigned(Core::FMTokey::add);
    assigned = original;

    // Assert
    EXPECT_TRUE(copy == original);
    EXPECT_TRUE(assigned == original);
    EXPECT_EQ(copy.hash(), original.hash());
}

TEST(FMTOperator, GivenTwoDifferentOperatorsWhenComparedThenTheyAreNotEqual)
{
    // Arrange
    const Core::FMTOperator add(Core::FMTokey::add);
    const Core::FMTOperator subtract(Core::FMTokey::sub);

    // Act
    const bool equal = (add == subtract);
    const bool different = (add != subtract);

    // Assert
    EXPECT_FALSE(equal);
    EXPECT_TRUE(different);
}
