/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTYieldModel.h"
#include "FMTDevelopment.h"
#include "FMTMask.h"
#include "FMTMaskFilter.h"
#include "FMTModel.h"
#include "FMTTheme.h"
#include "FMTYieldModelRandom.h"
#include "FMTYieldModelTsla.h"
#include "FMTYieldModelUnitCoverage.h"
#include "FMTYieldRequest.h"
#include "FMTutility.h"

#include "GivenYieldModel.h"

#include <gtest/gtest.h>

#include <boost/property_tree/ptree.hpp>

#include <map>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

// FMTYieldModel is the abstract base of the yield models: it holds the name of the file of the model and its source
// yields, the model it predicts for, and the defaults of its derived classes. The cases check the base through the
// models of FMT that are exported (FMTYieldModelRandom, FMTYieldModelTsla, FMTYieldModelUnitCoverage), and through the
// model of Tests/Unit/Core/GivenYieldModel.h for the defaults that these three redefine. The models of ONNX files and
// the decision trees are not exported (Documentation/Architecture.md, Known Architectural Debt).

namespace
{
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

    // The properties that the JSON file of a random model gives. The files of Templates/YieldPredModels write the name
    // with backslashes; the cases write it with slashes, which every platform reads as separators.
    boost::property_tree::ptree getRandomProperties()
    {
        boost::property_tree::ptree properties;
        properties.put("modelFileName", "YieldPredModels/RANDOM/RANDOM.json");
        properties.put("modelType", "RANDOM");
        properties.put("caching", false);
        return properties;
    }

    const std::vector<std::string> SOURCES = { "YPROB0", "YPROB1", "YPROB2" };
}

// The string of a model is its call in the yield section: the name of the folder of its file, then its source yields.
TEST(FMTYieldModel, GivenARandomModelWhenConvertedToStringThenWritesThePredictionOfTheFolderOfItsFileWithItsSources)
{
    // Arrange
    const Core::FMTYieldModelRandom model(getRandomProperties(), SOURCES);

    // Act
    const std::string text(model);

    // Assert
    EXPECT_EQ(text, " _PRED(RANDOM,YPROB0,YPROB1,YPROB2)\n");
}

TEST(FMTYieldModel, GivenAModelWhenCopyConstructedThenTheCopyHasItsFileNameItsSourcesAndItsModel)
{
    // Arrange
    Models::FMTModel fmtModel;
    Testing::GivenYieldModel model("YieldPredModels/GIVEN/GIVEN.json", { "VOLUMETOTAL" }, { 1.0 });
    model.setModel(&fmtModel);

    // Act
    const Testing::GivenYieldModel copy(model);

    // Assert
    EXPECT_EQ(copy.getModelName(), "YieldPredModels/GIVEN/GIVEN.json");
    EXPECT_EQ(copy.getModelYields(), std::vector<std::string>({ "VOLUMETOTAL" }));
    EXPECT_EQ(copy.getModel(), &fmtModel);
}

TEST(FMTYieldModel, GivenAModelWithoutModelWhenSetModelThenPredictsForThatModel)
{
    // Arrange
    Models::FMTModel fmtModel;
    Testing::GivenYieldModel model("YieldPredModels/GIVEN/GIVEN.json", { "VOLUMETOTAL" }, { 1.0 });
    const Models::FMTModel* before = model.getModel();

    // Act
    model.setModel(&fmtModel);

    // Assert
    EXPECT_EQ(before, nullptr);
    EXPECT_EQ(model.getModel(), &fmtModel);
}

// A model that does not redefine them is cloned by its presolve and its postsolve.
TEST(FMTYieldModel, GivenAModelThatKeepsTheDefaultsWhenPreSolveAndPostSolveThenGivesClonesThatPredictTheSame)
{
    // Arrange
    const Testing::GivenYieldModel model("YieldPredModels/GIVEN/GIVEN.json", { "VOLUMETOTAL" }, { 10.0, 20.0 });
    const Core::FMTMaskFilter filter;
    const std::vector<Core::FMTTheme> themes = getThemes();
    const Core::FMTDevelopment development(Core::FMTMask("UNITE1 PEUPLEMENT1 UTR1", themes), 20, 0, 1);
    const Core::FMTYieldRequest request(development);

    // Act
    const std::unique_ptr<Core::FMTYieldModel> presolved = model.preSolve(filter, themes);
    const std::unique_ptr<Core::FMTYieldModel> postsolved = model.postSolve(filter, themes);
    const std::vector<double> presolvedPredictions = presolved->predict(request);
    const std::vector<double> postsolvedPredictions = postsolved->predict(request);

    // Assert
    EXPECT_NE(dynamic_cast<const Testing::GivenYieldModel*>(presolved.get()), nullptr);
    EXPECT_NE(dynamic_cast<const Testing::GivenYieldModel*>(postsolved.get()), nullptr);
    EXPECT_EQ(presolvedPredictions, std::vector<double>({ 10.0, 20.0 }));
    EXPECT_EQ(postsolvedPredictions, std::vector<double>({ 10.0, 20.0 }));
    EXPECT_EQ(postsolved->getModelName(), "YieldPredModels/GIVEN/GIVEN.json");
}

// No exported model of FMT has periodic values: none can become a time yield (FMTModelYieldHandler::toTimeHandler).
TEST(FMTYieldModel, GivenTheExportedModelsOfFMTWhenGetPeriodicValuesThenNoneHasAny)
{
    // Arrange
    const Core::FMTYieldModelRandom random(getRandomProperties(), SOURCES);
    const Core::FMTYieldModelTsla tsla;
    const Core::FMTYieldModelUnitCoverage coverage(getRandomProperties(), SOURCES,
        Core::FMTMask("UNITE1 ? ?", getThemes()));

    // Act
    const std::vector<std::vector<std::vector<double>>> values = { random.getPeriodicValues(), tsla.getPeriodicValues(),
        coverage.getPeriodicValues() };

    // Assert
    EXPECT_EQ(values, std::vector<std::vector<std::vector<double>>>(3));
}

// FMTYieldModelTsla declares its copy assignment as defaulted, but the base holds a constant member, the key of the
// name in the JSON file: the compiler deletes the assignment of every yield model. Nothing in FMT assigns a yield model;
// the handlers clone them.
TEST(FMTYieldModel, GivenTheExportedModelsOfFMTWhenTheirCopyAssignmentIsCheckedThenNoneIsCopyAssignable)
{
    // Arrange and Act
    const std::vector<bool> assignable = { std::is_copy_assignable<Core::FMTYieldModel>::value,
        std::is_copy_assignable<Core::FMTYieldModelRandom>::value, std::is_copy_assignable<Core::FMTYieldModelTsla>::value,
        std::is_copy_assignable<Core::FMTYieldModelUnitCoverage>::value };
    const std::vector<bool> copyConstructible = {
        std::is_copy_constructible<Core::FMTYieldModelRandom>::value,
        std::is_copy_constructible<Core::FMTYieldModelTsla>::value,
        std::is_copy_constructible<Core::FMTYieldModelUnitCoverage>::value };

    // Assert
    EXPECT_EQ(assignable, std::vector<bool>(4, false));
    EXPECT_EQ(copyConstructible, std::vector<bool>(3, true));
}
