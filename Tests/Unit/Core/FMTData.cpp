/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTData.h"
#include "FMTDevelopment.h"
#include "FMTExpression.h"
#include "FMTMask.h"
#include "FMTYieldRequest.h"
#include "FMTutility.h"

#include <gtest/gtest.h>

#include <map>
#include <string>
#include <vector>

// The data are built as FMTYieldParser builds them: the numbers in data, the yields in the sources, and a stacking
// that says, for each argument in its order, whether it is a yield (true) or a number (false). An equation keeps its
// tokens in the sources and its numbers at their position in data, a number having an empty source, as
// FMTYieldParser::_cleanUp rebuilds them.

TEST(FMTData, GivenADefaultDataWhenGetOpThenHasNoOperator)
{
    // Arrange
    const Core::FMTData data;

    // Act
    const Core::FMTyieldparserop op = data.getOp();

    // Assert
    EXPECT_TRUE(op == Core::FMTyieldparserop::FMTnone);
    EXPECT_TRUE(data.data.empty());
}

TEST(FMTData, GivenEqualValuesWithoutOperatorWhenConstantThenReturnsTrue)
{
    // Arrange
    const Core::FMTData data({ 5.0, 5.0, 5.0 }, Core::FMTyieldparserop::FMTnone, {});

    // Act
    const bool constant = data.constant();

    // Assert
    EXPECT_TRUE(constant);
}

TEST(FMTData, GivenDifferentValuesWithoutOperatorWhenConstantThenReturnsFalse)
{
    // Arrange
    const Core::FMTData data({ 5.0, 6.0 }, Core::FMTyieldparserop::FMTnone, {});

    // Act
    const bool constant = data.constant();

    // Assert
    EXPECT_FALSE(constant);
}

TEST(FMTData, GivenAnOperatorWhenConstantThenReturnsFalse)
{
    // Arrange
    const Core::FMTData data({ 4.0 }, Core::FMTyieldparserop::FMTsum, { "A" }, { true, false });

    // Act
    const bool constant = data.constant();

    // Assert
    EXPECT_FALSE(constant);
}

TEST(FMTData, GivenZerosWithoutOperatorWhenNullDataThenReturnsTrue)
{
    // Arrange
    const Core::FMTData data({ 0.0, 0.0 }, Core::FMTyieldparserop::FMTnone, {});

    // Act
    const bool nullData = data.nullData();

    // Assert
    EXPECT_TRUE(nullData);
}

TEST(FMTData, GivenANonZeroValueWithoutOperatorWhenNullDataThenReturnsFalse)
{
    // Arrange
    const Core::FMTData data({ 0.0, 1.0 }, Core::FMTyieldparserop::FMTnone, {});

    // Act
    const bool nullData = data.nullData();

    // Assert
    EXPECT_FALSE(nullData);
}

TEST(FMTData, GivenAnOperatorWhenNullDataThenReturnsFalse)
{
    // Arrange
    const Core::FMTData data({ 0.0 }, Core::FMTyieldparserop::FMTsum, { "A" }, { true, false });

    // Act
    const bool nullData = data.nullData();

    // Assert
    EXPECT_FALSE(nullData);
}

TEST(FMTData, GivenASumWhenGetSourcesThenReturnsEachYield)
{
    // Arrange
    const Core::FMTData data({ 4.0 }, Core::FMTyieldparserop::FMTsum, { "A", "B" }, { true, false, true });

    // Act
    const std::vector<std::string> sources = data.getSourcesCopy();

    // Assert
    EXPECT_EQ(sources, std::vector<std::string>({ "A", "B" }));
}

TEST(FMTData, GivenAnEquationWhenGetSourcesThenSkipsItsOperatorsFunctionsAndParentheses)
{
    // Arrange
    const Core::FMTData data({ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 }, Core::FMTyieldparserop::FMTequation,
        { "LN", "(", "A", ")", "*", "B" });

    // Act
    const std::vector<std::string> sources = data.getSourcesCopy();

    // Assert
    EXPECT_EQ(sources, std::vector<std::string>({ "A", "B" }));
}

TEST(FMTData, GivenYieldsAndNumbersWhenGetValuesThenPointsToEachNumberAtItsPlace)
{
    // Arrange
    const Core::FMTData data({ 4.0, 5.0 }, Core::FMTyieldparserop::FMTsum, { "A", "B" },
        { true, false, true, false });

    // Act
    const std::vector<const double*> values = data.getValues();

    // Assert
    ASSERT_EQ(values.size(), 4u);
    EXPECT_EQ(values.at(0), nullptr);
    EXPECT_EQ(*values.at(1), 4.0);
    EXPECT_EQ(values.at(2), nullptr);
    EXPECT_EQ(*values.at(3), 5.0);
}

TEST(FMTData, GivenASumWhenConvertedToStringThenWritesItsYieldsAndNumbersInTheirOrder)
{
    // Arrange
    const Core::FMTData data({ 4.0 }, Core::FMTyieldparserop::FMTsum, { "A", "B" }, { true, false, true });

    // Act
    const std::string written = std::string(data);

    // Assert
    EXPECT_EQ(written, "_SUM(A,4,B)");
}

// The rate is written as a fraction, which FMTYieldParser reads back as such since the fix of issue #374, with the
// 17 significant digits that boost::lexical_cast gives a double: 0.040000000000000001 reads back as 0.04.
TEST(FMTData, GivenADiscountFactorWhenConvertedToStringThenWritesItsRateYearsAndType)
{
    // Arrange
    const Core::FMTData data({ 0.04, 5.0 }, Core::FMTyieldparserop::FMTdiscountfactor, { "FULL" },
        { false, false, true });

    // Act
    const std::string written = std::string(data);

    // Assert
    EXPECT_EQ(written, "_DISCOUNTFACTOR(0.040000000000000001,5,FULL)");
}

TEST(FMTData, GivenAnEquationWhenConvertedToStringThenWritesItsTokensAndItsNumbers)
{
    // Arrange
    const Core::FMTData data({ 0.0, 0.0, 2.0 }, Core::FMTyieldparserop::FMTequation, { "A", "*", "" });

    // Act
    const std::string written = std::string(data);

    // Assert
    EXPECT_EQ(written, "_EQUATION(A*2)");
}

TEST(FMTData, GivenAnEquationWithANumberWhenToExpressionThenTheExpressionEvaluatesTheEquation)
{
    // Arrange
    const Core::FMTData data({ 0.0, 0.0, 2.0 }, Core::FMTyieldparserop::FMTequation, { "A", "*", "" });
    const std::map<std::string, double> values = { { "A", 3.0 } };

    // Act
    const Core::FMTExpression expression = data.toExpression();

    // Assert
    EXPECT_EQ(expression.getInFix(), std::vector<std::string>({ "A", "*", "2" }));
    EXPECT_DOUBLE_EQ(expression.shuntingYard(values), 6.0);
}

// Characterizes a known behavior (issue #347): toExpression joins the sources of a data whatever its operator.
// FMTYieldParser::_cleanUp rebuilds with it each yield of a *YC section that goes through its branch of the theme
// indexes, so that _SUM(A,B) becomes _EQUATION(AB), which gives the value of its last source.
TEST(FMTData, GivenASumWhenToExpressionThenJoinsItsSourcesWithoutAnOperator)
{
    // Arrange: _SUM(A,B)
    const Core::FMTData sum({}, Core::FMTyieldparserop::FMTsum, { "A", "B" }, { true, true });
    const std::map<std::string, double> values = { { "A", 1.0 }, { "B", 2.0 } };

    // Act
    const Core::FMTExpression expression = sum.toExpression();

    // Assert
    EXPECT_EQ(expression.getInFix(), std::vector<std::string>({ "A", "B" }));
    EXPECT_DOUBLE_EQ(expression.shuntingYard(values), 2.0);
}

TEST(FMTData, GivenTwoDataWithTheSameContentWhenComparedThenAreEqual)
{
    // Arrange
    const Core::FMTData first({ 4.0 }, Core::FMTyieldparserop::FMTsum, { "A" }, { true, false });
    const Core::FMTData second({ 4.0 }, Core::FMTyieldparserop::FMTsum, { "A" }, { true, false });

    // Act
    const bool equal = (first == second);

    // Assert
    EXPECT_TRUE(equal);
}

TEST(FMTData, GivenTwoDataWithDifferentOperatorsWhenComparedThenAreNotEqual)
{
    // Arrange
    const Core::FMTData sum({ 4.0 }, Core::FMTyieldparserop::FMTsum, { "A" }, { true, false });
    const Core::FMTData product({ 4.0 }, Core::FMTyieldparserop::FMTmultiply, { "A" }, { true, false });

    // Act
    const bool equal = (sum == product);

    // Assert
    EXPECT_FALSE(equal);
}

TEST(FMTData, GivenADataWhenCopiedThenTheCopyIsEqual)
{
    // Arrange
    const Core::FMTData original({ 4.0 }, Core::FMTyieldparserop::FMTsum, { "A" }, { true, false });

    // Act
    const Core::FMTData copy(original);
    Core::FMTData assigned;
    assigned = original;

    // Assert
    EXPECT_TRUE(copy == original);
    EXPECT_TRUE(assigned == original);
}

TEST(FMTData, GivenAFactorWhenMultipliedThenMultipliesEachValueOnly)
{
    // Arrange
    const Core::FMTData data({ 1.0, 2.0 }, Core::FMTyieldparserop::FMTsum, { "A" }, { true, false, false });
    const Core::FMTData expected({ 3.0, 6.0 }, Core::FMTyieldparserop::FMTsum, { "A" }, { true, false, false });

    // Act
    const Core::FMTData multiplied = data * 3.0;

    // Assert
    EXPECT_TRUE(multiplied == expected);
}

// The cache keeps a value by development: the request is built from a development alone, as FMTData::get reads
// only the development and the resume mask of the request.
TEST(FMTData, GivenAValueSetForARequestWhenGetThenReturnsIt)
{
    // Arrange
    const Core::FMTData data({ 4.0 }, Core::FMTyieldparserop::FMTsum, { "A" }, { true, false });
    const Core::FMTDevelopment development(Core::FMTMask(), 10, 0, 1);
    const Core::FMTYieldRequest request(development);
    const bool ageOnly = true;
    data.set(42.0, request, ageOnly);

    // Act
    const bool cached = data.cacheValue(request);
    const double value = data.get(request);

    // Assert
    EXPECT_TRUE(cached);
    EXPECT_EQ(value, 42.0);
}

TEST(FMTData, GivenACachedValueWhenClearCacheThenNoLongerHoldsIt)
{
    // Arrange
    Core::FMTData data({ 4.0 }, Core::FMTyieldparserop::FMTsum, { "A" }, { true, false });
    const Core::FMTDevelopment development(Core::FMTMask(), 10, 0, 1);
    const Core::FMTYieldRequest request(development);
    const bool ageOnly = true;
    data.set(42.0, request, ageOnly);

    // Act
    data.clearCache();

    // Assert
    EXPECT_FALSE(data.cacheValue(request));
}

TEST(FMTData, GivenACachedValueWhenCopiedThenTheCopyHoldsNoCachedValue)
{
    // Arrange
    const Core::FMTData data({ 4.0 }, Core::FMTyieldparserop::FMTsum, { "A" }, { true, false });
    const Core::FMTDevelopment development(Core::FMTMask(), 10, 0, 1);
    const Core::FMTYieldRequest request(development);
    const bool ageOnly = true;
    data.set(42.0, request, ageOnly);

    // Act
    const Core::FMTData copy(data);

    // Assert
    EXPECT_FALSE(copy.cacheValue(request));
}
