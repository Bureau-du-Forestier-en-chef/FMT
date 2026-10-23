/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTOutput.h"
#include "FMTMask.h"
#include "FMTOperator.h"
#include "FMTOutputSource.h"
#include "FMTSpec.h"
#include "FMTTheme.h"
#include "FMTutility.h"

#include <gtest/gtest.h>

#include <string>
#include <vector>

// The outputs of the output section, built as FMTOutputParser builds them: sources (the area or a yield of the
// developments of a mask, operated by an action or in inventory, a constant, a level) and the operators between them,
// one fewer than the sources.

namespace
{
    std::vector<Core::FMTTheme> getThemes()
    {
        return { Core::FMTTheme({ "A1", "A2" }, 0, 0, "THEME1") };
    }

    // The yield p_yield of the developments of every attribute that the action p_action operates.
    Core::FMTOutputSource getActionSource(const std::string& p_action, const std::string& p_yield)
    {
        return Core::FMTOutputSource(Core::FMTSpec(), Core::FMTMask("?", getThemes()), Core::FMTotar::actual, p_yield,
            p_action);
    }

    // The yield p_yield of the developments of every attribute in inventory.
    Core::FMTOutputSource getInventorySource(const std::string& p_yield)
    {
        return Core::FMTOutputSource(Core::FMTSpec(), Core::FMTMask("?", getThemes()), Core::FMTotar::inventory, p_yield);
    }

    Core::FMTOutputSource getConstant(double p_value)
    {
        return Core::FMTOutputSource(Core::FMTotar::val, p_value);
    }

    // OVOL: the volume harvested by COUPE.
    Core::FMTOutput getVolumeOutput()
    {
        return Core::FMTOutput("OVOL", "Volume harvested", "HARVEST", { getActionSource("COUPE", "VOLUME") }, {});
    }

    // OSTEMS: the stems harvested by COUPE.
    Core::FMTOutput getStemsOutput()
    {
        return Core::FMTOutput("OSTEMS", "Stems harvested", "HARVEST", { getActionSource("COUPE", "STEMS") }, {});
    }
}

TEST(FMTOutput, GivenAnOutputWhenGetItsNameDescriptionAndGroupThenReturnsThem)
{
    // Arrange
    const Core::FMTOutput output = getVolumeOutput();

    // Act
    const std::vector<std::string> texts = { output.getName(), output.getDescription(), output.getGroup() };

    // Assert
    EXPECT_EQ(texts, std::vector<std::string>({ "OVOL", "Volume harvested", "HARVEST" }));
}

TEST(FMTOutput, GivenOutputsWithAndWithoutSourcesWhenEmptyThenReturnsTrueForTheOutputWithoutSources)
{
    // Arrange
    const Core::FMTOutput named("OVOL");
    const Core::FMTOutput output = getVolumeOutput();

    // Act
    const bool namedIsEmpty = named.empty();
    const bool outputIsEmpty = output.empty();

    // Assert
    EXPECT_TRUE(namedIsEmpty);
    EXPECT_FALSE(outputIsEmpty);
    EXPECT_EQ(output.size(), 1u);
}

TEST(FMTOutput, GivenAnOutputWhenCopiedThenTheCopyIsEqual)
{
    // Arrange
    const Core::FMTOutput original = getVolumeOutput();

    // Act
    const Core::FMTOutput copy(original);
    Core::FMTOutput assigned;
    assigned = original;

    // Assert
    EXPECT_TRUE(copy == original);
    EXPECT_EQ(copy.getGroup(), "HARVEST");
    EXPECT_TRUE(assigned == original);
    EXPECT_EQ(std::string(assigned), std::string(original));
}

TEST(FMTOutput, GivenTwoOutputsWhenComparedThenTheyAreEqualOnlyWithTheSameNameDescriptionSourcesAndOperators)
{
    // Arrange
    const Core::FMTOutput output = getVolumeOutput();
    const Core::FMTOutput otherName("OVOLUME", "Volume harvested", "HARVEST", { getActionSource("COUPE", "VOLUME") }, {});
    const Core::FMTOutput otherDescription("OVOL", "Volume", "HARVEST", { getActionSource("COUPE", "VOLUME") }, {});
    const Core::FMTOutput otherAction("OVOL", "Volume harvested", "HARVEST", { getActionSource("ECLAIRCIE", "VOLUME") }, {});

    // Act
    const bool equalToSame = (output == getVolumeOutput());
    const bool equalToOtherName = (output == otherName);
    const bool equalToOtherDescription = (output == otherDescription);
    const bool equalToOtherAction = (output == otherAction);

    // Assert
    EXPECT_TRUE(equalToSame);
    EXPECT_FALSE(equalToOtherName);
    EXPECT_FALSE(equalToOtherDescription);
    EXPECT_FALSE(equalToOtherAction);
    EXPECT_TRUE(output != otherName);
}

TEST(FMTOutput, GivenEqualOutputsWhenHashThenTheyHaveTheSameHash)
{
    // Arrange
    const Core::FMTOutput first = getVolumeOutput();
    const Core::FMTOutput second = getVolumeOutput();

    // Act
    const size_t firstHash = first.hash();
    const size_t secondHash = second.hash();

    // Assert
    EXPECT_EQ(firstHash, secondHash);
}

TEST(FMTOutput, GivenOutputsOfTwoYieldsWhenHashThenTheirHashesDiffer)
{
    // Arrange
    const Core::FMTOutput volume = getVolumeOutput();
    const Core::FMTOutput stems("OVOL", "Volume harvested", "HARVEST", { getActionSource("COUPE", "STEMS") }, {});

    // Act
    const size_t volumeHash = volume.hash();
    const size_t stemsHash = stems.hash();

    // Assert
    EXPECT_NE(volumeHash, stemsHash);
}

TEST(FMTOutput, GivenTwoOutputsWhenAddedThenJoinsTheirNamesAndTheirSourcesWithAPlus)
{
    // Arrange
    Core::FMTOutput output = getVolumeOutput();

    // Act
    output += getStemsOutput();

    // Assert
    EXPECT_EQ(output.getName(), "OVOL + OSTEMS");
    EXPECT_EQ(output.getDescription(), "Volume harvested+Stems harvested");
    ASSERT_EQ(output.size(), 2u);
    EXPECT_EQ(output.getSources().at(1).getYield(), "STEMS");
    EXPECT_EQ(output.getOpes(), std::vector<Core::FMTOperator>({ Core::FMTOperator("+") }));
}

TEST(FMTOutput, GivenAnOutputOfASumWhenSubtractedThenReversesTheOperatorsOfTheSum)
{
    // Arrange: OVOL - (OVOL + OSTEMS)
    Core::FMTOutput output = getVolumeOutput();
    Core::FMTOutput sum = getVolumeOutput();
    sum += getStemsOutput();

    // Act
    output -= sum;

    // Assert
    EXPECT_EQ(output.getName(), "OVOL - OVOL + OSTEMS");
    EXPECT_EQ(output.size(), 3u);
    EXPECT_EQ(output.getOpes(), std::vector<Core::FMTOperator>({ Core::FMTOperator("-"), Core::FMTOperator("-") }));
}

TEST(FMTOutput, GivenAnOutputWhenMultipliedByAConstantThenMultipliesEachOfItsVariableSources)
{
    // Arrange
    Core::FMTOutput output = getVolumeOutput();

    // Act
    output *= getConstant(2.0);

    // Assert
    EXPECT_EQ(output.getName(), "2 * OVOL");
    ASSERT_EQ(output.size(), 2u);
    EXPECT_EQ(output.getSources().at(1).getValue(), 2.0);
    EXPECT_EQ(output.getOpes(), std::vector<Core::FMTOperator>({ Core::FMTOperator("*") }));
}

TEST(FMTOutput, GivenAnOutputWhenMultipliedByOneThenKeepsItsName)
{
    // Arrange
    Core::FMTOutput output = getVolumeOutput();

    // Act
    output *= getConstant(1.0);

    // Assert
    EXPECT_EQ(output.getName(), "OVOL");
}

TEST(FMTOutput, GivenAnOutputWhenDividedByAConstantThenDividesEachOfItsVariableSources)
{
    // Arrange
    Core::FMTOutput output = getVolumeOutput();

    // Act
    output /= getConstant(2.0);

    // Assert
    EXPECT_EQ(output.getName(), "OVOL/2");
    ASSERT_EQ(output.size(), 2u);
    EXPECT_EQ(output.getOpes(), std::vector<Core::FMTOperator>({ Core::FMTOperator("/") }));
}

TEST(FMTOutput, GivenAnOutputOfConstantsWhenGetConstantValueThenComputesItWithItsOperators)
{
    // Arrange: 2 + 3 * 4
    const Core::FMTOutput output("CONSTANT", "", "", { getConstant(2.0), getConstant(3.0), getConstant(4.0) },
        { Core::FMTOperator("+"), Core::FMTOperator("*") });

    // Act
    const double value = output.getConstantValue();

    // Assert
    EXPECT_TRUE(output.isConstant());
    EXPECT_DOUBLE_EQ(value, 14.0);
}

TEST(FMTOutput, GivenAnOutputWithAVariableSourceWhenIsConstantThenReturnsFalseAndItsConstantValueIsZero)
{
    // Arrange
    const Core::FMTOutput output = getVolumeOutput();

    // Act
    const bool constant = output.isConstant();
    const double value = output.getConstantValue();

    // Assert
    EXPECT_FALSE(constant);
    EXPECT_EQ(value, 0.0);
}

TEST(FMTOutput, GivenValuesAndOperatorsWhenShuntingYardThenEvaluatesThemWithThePrecedenceOfTheOperators)
{
    // Arrange: 2 + 3 * 4 - 6 / 3
    const Core::FMTOutput output;
    const std::vector<double> values = { 2.0, 3.0, 4.0, 6.0, 3.0 };
    const std::vector<Core::FMTOperator> operators = { Core::FMTOperator("+"), Core::FMTOperator("*"),
        Core::FMTOperator("-"), Core::FMTOperator("/") };

    // Act
    const double value = output.shuntingYard(values, operators);

    // Assert
    EXPECT_DOUBLE_EQ(value, 12.0);
}

TEST(FMTOutput, GivenAProductOfTwoVariableSourcesWhenIsLinearThenReturnsFalse)
{
    // Arrange: VOLUME of COUPE * STEMS of COUPE, and VOLUME of COUPE * 2
    const Core::FMTOutput product("PRODUCT", "", "", { getActionSource("COUPE", "VOLUME"), getActionSource("COUPE", "STEMS") },
        { Core::FMTOperator("*") });
    const Core::FMTOutput scaled("SCALED", "", "", { getActionSource("COUPE", "VOLUME"), getConstant(2.0) },
        { Core::FMTOperator("*") });

    // Act
    const bool productIsLinear = product.isLinear();
    const bool scaledIsLinear = scaled.isLinear();

    // Assert
    EXPECT_FALSE(productIsLinear);
    EXPECT_TRUE(scaledIsLinear);
}

TEST(FMTOutput, GivenActionAndInventorySourcesWhenIsActionBasedAndIsInventoryThenTellsWhichSourcesItHas)
{
    // Arrange
    const Core::FMTOutput harvested = getVolumeOutput();
    const Core::FMTOutput standing("OSTANDING", "", "", { getInventorySource("VOLUME") }, {});

    // Act
    const std::vector<bool> kinds = { harvested.isActionBased(), harvested.isInventory(), standing.isActionBased(),
        standing.isInventory() };

    // Assert
    EXPECT_EQ(kinds, std::vector<bool>({ true, false, false, true }));
}

TEST(FMTOutput, GivenAConstantLevelWhenIsLevelThenItIsOnlyAConstantLevel)
{
    // Arrange: *LEVEL LIMIT 100 200
    const Core::FMTOutput level("LIMIT", "", "", { Core::FMTOutputSource(Core::FMTotar::level, { 100.0, 200.0 }) }, {});

    // Act
    const std::vector<bool> kinds = { level.isLevel(), level.isOnlyLevel(), level.isConstantLevel(), level.containsLevel() };

    // Assert
    EXPECT_EQ(kinds, std::vector<bool>({ true, true, true, true }));
    EXPECT_FALSE(getVolumeOutput().isLevel());
}

// The section of an output in a .out file: *OUTPUT, its name and its description, then *SOURCE and its sources, each
// source followed by two spaces.
TEST(FMTOutput, GivenAnOutputOfOneSourceWhenConvertedToStringThenWritesItsOutputAndSourceLines)
{
    // Arrange
    const Core::FMTOutput output = getVolumeOutput();

    // Act
    const std::string written = std::string(output);

    // Assert
    EXPECT_EQ(written, "*OUTPUT OVOL Volume harvested\n*SOURCE ? COUPE VOLUME  \n");
}

TEST(FMTOutput, GivenASumOfTwoSourcesWhenConvertedToStringThenWritesTheOperatorAtTheEndOfTheFirstSourceLine)
{
    // Arrange
    Core::FMTOutput output = getVolumeOutput();
    output += getStemsOutput();

    // Act
    const std::string written = std::string(output);

    // Assert
    EXPECT_EQ(written, "*OUTPUT OVOL + OSTEMS Volume harvested+Stems harvested\n*SOURCE ? COUPE VOLUME  +\n? COUPE STEMS  \n");
}

TEST(FMTOutput, GivenAnInventorySourceWhenConvertedToStringThenWritesInvent)
{
    // Arrange
    const Core::FMTOutput output("OSTANDING", "Standing volume", "", { getInventorySource("VOLUME") }, {});

    // Act
    const std::string written = std::string(output);

    // Assert
    EXPECT_EQ(written, "*OUTPUT OSTANDING Standing volume\n*SOURCE ? _INVENT VOLUME \n");
}

TEST(FMTOutput, GivenAConstantLevelWhenConvertedToStringThenWritesALevelAndItsValues)
{
    // Arrange
    const Core::FMTOutput level("LIMIT", "Limit", "", { Core::FMTOutputSource(Core::FMTotar::level, { 100.0, 200.5 }) }, {});

    // Act
    const std::string written = std::string(level);

    // Assert
    EXPECT_EQ(written, "*LEVEL LIMIT Limit\n*SOURCE 100 200.5\n");
}
