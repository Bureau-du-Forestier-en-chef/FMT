/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTFunctionCall.h"

#include <gtest/gtest.h>

#include <string>

// call rounds its result to 8 decimals: the expected values below are the exact values rounded so.

TEST(FMTFunctionCall, GivenExpWhenCallThenReturnsTheExponentialRoundedToEightDecimals)
{
    // Arrange
    const Core::FMTFunctionCall exponential("EXP");

    // Act
    const double value = exponential.call(1.0);

    // Assert
    EXPECT_DOUBLE_EQ(value, 2.71828183);
}

TEST(FMTFunctionCall, GivenLnWhenCallThenReturnsTheNaturalLogarithmRoundedToEightDecimals)
{
    // Arrange
    const Core::FMTFunctionCall logarithm("LN");

    // Act
    const double value = logarithm.call(10.0);

    // Assert
    EXPECT_DOUBLE_EQ(value, 2.30258509);
}

// exp(-20) is about 2.06e-9, below the 5e-9 that the rounding to 8 decimals keeps.
TEST(FMTFunctionCall, GivenExpOfAVeryNegativeValueWhenCallThenRoundsTheResultToZero)
{
    // Arrange
    const Core::FMTFunctionCall exponential("EXP");

    // Act
    const double value = exponential.call(-20.0);

    // Assert
    EXPECT_EQ(value, 0.0);
}

TEST(FMTFunctionCall, GivenLnWhenCallOnZeroOrANegativeValueThenReturnsZero)
{
    // Arrange
    const Core::FMTFunctionCall logarithm("LN");

    // Act
    const double ofZero = logarithm.call(0.0);
    const double ofNegative = logarithm.call(-1.0);

    // Assert
    EXPECT_EQ(ofZero, 0.0);
    EXPECT_EQ(ofNegative, 0.0);
}

TEST(FMTFunctionCall, GivenAFunctionNameInLowerCaseWhenConstructedThenIsNotValid)
{
    // Arrange
    const std::string lowerCaseExp = "exp";
    const std::string lowerCaseLn = "ln";

    // Act
    const Core::FMTFunctionCall exponential(lowerCaseExp);
    const Core::FMTFunctionCall logarithm(lowerCaseLn);

    // Assert
    EXPECT_FALSE(exponential.valid());
    EXPECT_FALSE(logarithm.valid());
}

TEST(FMTFunctionCall, GivenAnUnknownFunctionNameWhenConstructedThenIsNotValid)
{
    // Arrange
    const std::string unknownName = "SQRT";

    // Act
    const Core::FMTFunctionCall unknown(unknownName);

    // Assert
    EXPECT_FALSE(unknown.valid());
}

TEST(FMTFunctionCall, GivenAnUnknownFunctionNameWhenCallThenReturnsZero)
{
    // Arrange
    const Core::FMTFunctionCall unknown("SQRT");

    // Act
    const double value = unknown.call(4.0);

    // Assert
    EXPECT_EQ(value, 0.0);
}

// The default function call holds the key 0, which is no FMTfkey: it is neither a function nor FMTfkey::notvalid,
// so that valid() accepts it, while its call returns 0.
TEST(FMTFunctionCall, GivenADefaultFunctionCallWhenValidThenAcceptsIt)
{
    // Arrange
    const Core::FMTFunctionCall defaultCall;

    // Act
    const bool valid = defaultCall.valid();

    // Assert
    EXPECT_TRUE(valid);
}

TEST(FMTFunctionCall, GivenADefaultFunctionCallWhenCallThenReturnsZero)
{
    // Arrange
    const Core::FMTFunctionCall defaultCall;

    // Act
    const double value = defaultCall.call(1.0);

    // Assert
    EXPECT_EQ(value, 0.0);
}

TEST(FMTFunctionCall, GivenAFunctionCallWhenCopiedThenTheCopyCallsTheSameFunction)
{
    // Arrange
    const Core::FMTFunctionCall exponential("EXP");

    // Act
    const Core::FMTFunctionCall copy(exponential);
    Core::FMTFunctionCall assigned("LN");
    assigned = exponential;

    // Assert
    EXPECT_DOUBLE_EQ(copy.call(1.0), 2.71828183);
    EXPECT_DOUBLE_EQ(assigned.call(1.0), 2.71828183);
}
