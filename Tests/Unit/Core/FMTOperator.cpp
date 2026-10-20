/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTOperator.h"

#include <gtest/gtest.h>

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
