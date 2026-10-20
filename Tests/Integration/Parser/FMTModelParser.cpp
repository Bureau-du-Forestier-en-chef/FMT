/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "ParserRoundTrip.h"
#include "FMTModelParser.h"
#include "FMTModel.h"
#include "FMTConstraint.h"

#include <gtest/gtest.h>

#include <filesystem>
#include <string>
#include <vector>

// FMTModelParser::write and writeToProject with the files they write, read back by FMTModelParser::readproject.
// A model written then read back must be the model written, and writing it again must give the same files: a
// round trip that needs no reference value (Testing::writeRereadAndRewrite, in ParserRoundTrip.h). Each case reads
// one scenario of TWD_land, chosen for the sections it defines.
//
// The disabled cases fail on a behavior of FMTlib, named above each of them.

// Disabled: the yield YBM_RTM_F, a _SUM in a block with a numeric equation, is written _EQUATION(YBM_RTM_BOGYBM_RTM_MEH)
// and read back as 0 (#347), and the objective _MIN _PENALTY(...) is written _MIN y_PENALTY(...) (#379, see
// GivenAPenaltyObjectiveWithoutSignWhenWriteThenRereadsTheSameObjective).
TEST(FMTModelParser, DISABLED_GivenTheRootModelWhenWriteThenRereadsTheSameModelAndRewritesTheSameFiles)
{
    // Arrange
    const Models::FMTModel model = Testing::readScenario("ROOT");
    const std::filesystem::path folder = Testing::getCaseFolder();

    // Act
    const Testing::ModelRoundTrip roundTrip = Testing::writeRereadAndRewrite(model, folder);

    // Assert
    Testing::expectSameModel(model, roundTrip.reread);
    Testing::expectSameFiles(roundTrip.firstFiles, roundTrip.secondFiles);
}

TEST(FMTModelParser, GivenYieldsOnNegatedMasksWhenWriteThenRereadsTheSameModelAndRewritesTheSameFiles)
{
    // Arrange
    const Models::FMTModel model = Testing::readScenario("NOT_MASK");
    const std::filesystem::path folder = Testing::getCaseFolder();

    // Act
    const Testing::ModelRoundTrip roundTrip = Testing::writeRereadAndRewrite(model, folder);

    // Assert
    Testing::expectSameModel(model, roundTrip.reread);
    Testing::expectSameFiles(roundTrip.firstFiles, roundTrip.secondFiles);
}

TEST(FMTModelParser, GivenEquationsInYieldsOutputsAndConstraintsWhenWriteThenRereadsTheSameModelAndRewritesTheSameFiles)
{
    // Arrange
    const Models::FMTModel model = Testing::readScenario("equation");
    const std::filesystem::path folder = Testing::getCaseFolder();

    // Act
    const Testing::ModelRoundTrip roundTrip = Testing::writeRereadAndRewrite(model, folder);

    // Assert
    Testing::expectSameModel(model, roundTrip.reread);
    Testing::expectSameFiles(roundTrip.firstFiles, roundTrip.secondFiles);
}

TEST(FMTModelParser, GivenItsOwnLandscapeActionsAndTimeYieldsWhenWriteThenRereadsTheSameModelAndRewritesTheSameFiles)
{
    // Arrange
    const Models::FMTModel model = Testing::readScenario("COS");
    const std::filesystem::path folder = Testing::getCaseFolder();

    // Act
    const Testing::ModelRoundTrip roundTrip = Testing::writeRereadAndRewrite(model, folder);

    // Assert
    Testing::expectSameModel(model, roundTrip.reread);
    Testing::expectSameFiles(roundTrip.firstFiles, roundTrip.secondFiles);
}

TEST(FMTModelParser, GivenAMeanAnnualIncrementYieldWhenWriteThenRereadsTheSameModelAndRewritesTheSameFiles)
{
    // Arrange
    const Models::FMTModel model = Testing::readScenario("mai");
    const std::filesystem::path folder = Testing::getCaseFolder();

    // Act
    const Testing::ModelRoundTrip roundTrip = Testing::writeRereadAndRewrite(model, folder);

    // Assert
    Testing::expectSameModel(model, roundTrip.reread);
    Testing::expectSameFiles(roundTrip.firstFiles, roundTrip.secondFiles);
}

TEST(FMTModelParser, GivenAShiftedYieldWhenWriteThenRereadsTheSameModelAndRewritesTheSameFiles)
{
    // Arrange
    const Models::FMTModel model = Testing::readScenario("Shift");
    const std::filesystem::path folder = Testing::getCaseFolder();

    // Act
    const Testing::ModelRoundTrip roundTrip = Testing::writeRereadAndRewrite(model, folder);

    // Assert
    Testing::expectSameModel(model, roundTrip.reread);
    Testing::expectSameFiles(roundTrip.firstFiles, roundTrip.secondFiles);
}

TEST(FMTModelParser, GivenADistanceYieldAndARatioOfInventoryOutputsWhenWriteThenRereadsTheSameModelAndRewritesTheSameFiles)
{
    // Arrange
    const Models::FMTModel model = Testing::readScenario("DISTANCE");
    const std::filesystem::path folder = Testing::getCaseFolder();

    // Act
    const Testing::ModelRoundTrip roundTrip = Testing::writeRereadAndRewrite(model, folder);

    // Assert
    Testing::expectSameModel(model, roundTrip.reread);
    Testing::expectSameFiles(roundTrip.firstFiles, roundTrip.secondFiles);
}

// Disabled: a _RANDOM(<output>) constraint is written without _RANDOM, and read back as a standard constraint
// (#382).
TEST(FMTModelParser, DISABLED_GivenRandomActionConstraintsWhenWriteThenRereadsTheSameModelAndRewritesTheSameFiles)
{
    // Arrange
    const Models::FMTModel model = Testing::readScenario("COMPLEX_NSS");
    const std::filesystem::path folder = Testing::getCaseFolder();

    // Act
    const Testing::ModelRoundTrip roundTrip = Testing::writeRereadAndRewrite(model, folder);

    // Assert
    Testing::expectSameModel(model, roundTrip.reread);
    Testing::expectSameFiles(roundTrip.firstFiles, roundTrip.secondFiles);
}

// The scenario transitions takes the yields and the objective of the root, which do not survive a round trip (#347
// and #379): this case compares its transitions only.
TEST(FMTModelParser, GivenSplitTargetsAgesLocksAndSourceConditionsWhenWriteThenRereadsTheSameTransitions)
{
    // Arrange
    const Models::FMTModel model = Testing::readScenario("transitions");
    const std::filesystem::path folder = Testing::getCaseFolder();

    // Act
    const Testing::ModelRoundTrip roundTrip = Testing::writeRereadAndRewrite(model, folder);

    // Assert
    EXPECT_TRUE(model.getTransitions() == roundTrip.reread.getTransitions());
    EXPECT_EQ(Testing::getFirstDifference(roundTrip.firstFiles.at(".trn"), roundTrip.secondFiles.at(".trn")), "");
}

// The scenario stdconstraints takes the yields of the root (#347): this case compares its constraints only.
TEST(FMTModelParser, GivenThousandsSeparatorsAndListsOfPeriodsWhenWriteThenRereadsTheSameConstraints)
{
    // Arrange
    const Models::FMTModel model = Testing::readScenario("stdconstraints");
    const std::filesystem::path folder = Testing::getCaseFolder();

    // Act
    const Testing::ModelRoundTrip roundTrip = Testing::writeRereadAndRewrite(model, folder);

    // Assert
    EXPECT_TRUE(model.getConstraints() == roundTrip.reread.getConstraints());
    EXPECT_EQ(Testing::getFirstDifference(roundTrip.firstFiles.at(".opt"), roundTrip.secondFiles.at(".opt")), "");
}

// Disabled: the objective _MIN _PENALTY(caribou,recolte) of the root is written _MIN y_PENALTY(CARIBOU,RECOLTE), the
// last letter of Penalty taking the place of the missing sign (FMTConstraint::operator std::string, #379).
TEST(FMTModelParser, DISABLED_GivenAPenaltyObjectiveWithoutSignWhenWriteThenRereadsTheSameObjective)
{
    // Arrange
    const Models::FMTModel model = Testing::readScenario("ROOT");
    const std::filesystem::path folder = Testing::getCaseFolder();

    // Act
    const Testing::ModelRoundTrip roundTrip = Testing::writeRereadAndRewrite(model, folder);

    // Assert
    EXPECT_TRUE(model.getConstraints() == roundTrip.reread.getConstraints());
    EXPECT_EQ(Testing::getFirstDifference(roundTrip.firstFiles.at(".opt"), roundTrip.secondFiles.at(".opt")), "");
}

// The scenario ForLoops takes the yields of the root (#347): this case compares its outputs only.
TEST(FMTModelParser, GivenOutputsBuiltByForLoopsWhenWriteThenRereadsTheSameOutputs)
{
    // Arrange
    const Models::FMTModel model = Testing::readScenario("ForLoops");
    const std::filesystem::path folder = Testing::getCaseFolder();

    // Act
    const Testing::ModelRoundTrip roundTrip = Testing::writeRereadAndRewrite(model, folder);

    // Assert
    EXPECT_TRUE(model.getOutputs() == roundTrip.reread.getOutputs());
    EXPECT_EQ(Testing::getFirstDifference(roundTrip.firstFiles.at(".out"), roundTrip.secondFiles.at(".out")), "");
}

// Disabled: an output that mixes inventory and action sources is written as hidden outputs ~<name><n>IN and
// ~<name><n>OUT, which a read takes as outputs of the model and a second write repeats (#381).
TEST(FMTModelParser, DISABLED_GivenARatioOfAnInventoryOutputToAnActionOutputWhenWriteThenRereadsTheSameOutputs)
{
    // Arrange
    const Models::FMTModel model = Testing::readScenario("TSLA");
    const std::filesystem::path folder = Testing::getCaseFolder();

    // Act
    const Testing::ModelRoundTrip roundTrip = Testing::writeRereadAndRewrite(model, folder);

    // Assert
    EXPECT_TRUE(model.getOutputs() == roundTrip.reread.getOutputs());
    EXPECT_EQ(Testing::getFirstDifference(roundTrip.firstFiles.at(".out"), roundTrip.secondFiles.at(".out")), "");
}

// Disabled: a _DISCOUNTFACTOR is written as a series of its parameters, DISCOUNTFULL 1 0.04 5, so that the model
// read back has a rate of 0.04 at period 1 and 5 at period 2 (FMTTimeYieldHandler::operator std::string, #380).
// The scenario timeyields takes the yield YBM_RTM_F and the objective of the root (#347 and #379): this case
// compares the discount factors only.
TEST(FMTModelParser, DISABLED_GivenDiscountFactorsWhenWriteThenRereadsTheirValues)
{
    // Arrange
    const Models::FMTModel model = Testing::readScenario("timeyields");
    const std::filesystem::path folder = Testing::getCaseFolder();

    // Act
    const Testing::ModelRoundTrip roundTrip = Testing::writeRereadAndRewrite(model, folder);

    // Assert
    Testing::expectSameYieldValues(model, roundTrip.reread, "DISCOUNT");
}

// Disabled: a *YT block whose only yield is a _DISCOUNTFACTOR has no base, and its writing throws (invalid vector
// subscript, FMTTimeYieldHandler::operator std::string, #380).
TEST(FMTModelParser, DISABLED_GivenADiscountRateFromAConstantWhenWriteThenWritesTheModel)
{
    // Arrange
    const Models::FMTModel model = Testing::readScenario("discountconstant");
    const std::filesystem::path folder = Testing::getCaseFolder();
    Parser::FMTModelParser parser;
    parser.setDefaultExceptionHandler();

    // Act and Assert
    EXPECT_NO_THROW(parser.write(model, folder.string() + "/"));
}

TEST(FMTModelParser, GivenNoPrimaryFileWhenWriteToProjectThenWritesTheSameFilesAsWrite)
{
    // Arrange
    const Models::FMTModel model = Testing::readScenario("NOT_MASK");
    const std::filesystem::path folder = Testing::getCaseFolder();
    const std::filesystem::path projectFolder = folder / "project";
    std::filesystem::create_directories(projectFolder);
    const Testing::WrittenFiles writtenFiles = Testing::writeModel(model, folder / "write");
    Parser::FMTModelParser parser;
    parser.setDefaultExceptionHandler();

    // Act
    parser.writeToProject((projectFolder / (model.getName() + ".pri")).string(), model);

    // Assert
    Testing::expectSameFiles(writtenFiles, Testing::getWrittenFiles(projectFolder));
}

TEST(FMTModelParser, GivenAProjectWhenWriteToProjectThenRereadsTheScenarioAsTheModelWritten)
{
    // Arrange
    const Models::FMTModel root = Testing::readScenario("NOT_MASK");
    const Models::FMTModel scenario = Testing::readScenario("equation");
    const std::filesystem::path folder = Testing::getCaseFolder();
    const std::string primaryFile = (folder / (root.getName() + ".pri")).string();
    Parser::FMTModelParser parser;
    parser.setDefaultExceptionHandler();
    parser.writeToProject(primaryFile, root);

    // Act
    parser.writeToProject(primaryFile, scenario);
    const Models::FMTModel reread = Testing::readScenario(scenario.getName(), primaryFile);

    // Assert
    Testing::expectSameModel(scenario, reread);
    Testing::expectSameFiles(Testing::writeModel(scenario, folder / "scenario"), Testing::writeModel(reread, folder / "reread"));
}

// Disabled: writeToProject writes the sections of a scenario that differ from the root, and the yields always
// differ, since FMTYields::operator== compares the addresses of their handlers: the yields are written too (#383).
TEST(FMTModelParser, DISABLED_GivenAScenarioThatOnlyChangesItsObjectiveWhenWriteToProjectThenWritesItsOptimizeAndScheduleSectionsOnly)
{
    // Arrange
    const Models::FMTModel root = Testing::readScenario("NOT_MASK");
    Models::FMTModel scenario(root);
    scenario.setName("objective");
    Core::FMTConstraint objective = root.getConstraints().at(0);
    objective.setLength(1, 5);
    scenario.setConstraints(std::vector<Core::FMTConstraint>(1, objective));
    const std::filesystem::path folder = Testing::getCaseFolder();
    const std::string primaryFile = (folder / (root.getName() + ".pri")).string();
    Parser::FMTModelParser parser;
    parser.setDefaultExceptionHandler();
    parser.writeToProject(primaryFile, root);
    const std::vector<std::string> expectedSections = { "._opt", "._seq" };

    // Act
    parser.writeToProject(primaryFile, scenario);

    // Assert
    EXPECT_EQ(Testing::getExtensions(Testing::getWrittenFiles(folder / "Scenarios" / scenario.getName())), expectedSections);
}
