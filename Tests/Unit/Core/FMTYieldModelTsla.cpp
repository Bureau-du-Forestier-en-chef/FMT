/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTYieldModelTsla.h"
#include "FMTDevelopment.h"
#include "FMTMask.h"
#include "FMTMaskFilter.h"
#include "FMTTheme.h"
#include "FMTYieldRequest.h"
#include "FMTYields.h"
#include "FMTutility.h"

#include "ExceptionChain.h"

#include <gtest/gtest.h>

#include <initializer_list>
#include <map>
#include <memory>
#include <string>
#include <vector>

// The _TSLA yield model gives the time since the last action of a development: FMTYields::generateDefaultYields
// gives every model a handler of the mask ? ? ? whose only yield, _TSLA, this model predicts. It reads the time in the
// graph of the request (FMTGraphVertexToYield), which a unit test does not build: what it computes is covered by
// DoPlanning and Presolve on the scenario TSLA. The cases check what it does without a graph. The themes are those of
// Tests/Unit/Core/FMTTheme.cpp.

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
}

TEST(FMTYieldModelTsla, GivenNothingWhenConstructedThenIsTheTslaModelWithoutSourceYield)
{
    // Arrange and Act
    const Core::FMTYieldModelTsla model;

    // Assert
    EXPECT_EQ(model.getModelName(), "_TSLA");
    EXPECT_EQ(Core::FMTYieldModelTsla::getModelType(), "_TSLA");
    EXPECT_TRUE(model.getModelYields().empty());
    EXPECT_TRUE(model.getPeriodicValues().empty());
}

// The model is not written: a section of yields read back gets it from FMTYields::generateDefaultYields again.
TEST(FMTYieldModelTsla, GivenTheModelWhenConvertedToStringThenWritesNothing)
{
    // Arrange
    const Core::FMTYieldModelTsla model;

    // Act
    const std::string text(model);

    // Assert
    EXPECT_TRUE(text.empty());
}

TEST(FMTYieldModelTsla, GivenTheModelWhenClonedPreSolvedAndPostSolvedThenEachGivesATslaModel)
{
    // Arrange
    const Core::FMTYieldModelTsla model;
    const Core::FMTMaskFilter filter;
    const std::vector<Core::FMTTheme> themes = getThemes();

    // Act
    const std::unique_ptr<Core::FMTYieldModel> clone = model.clone();
    const std::unique_ptr<Core::FMTYieldModel> presolved = model.preSolve(filter, themes);
    const std::unique_ptr<Core::FMTYieldModel> postsolved = model.postSolve(filter, themes);

    // Assert
    for (const Core::FMTYieldModel* copy : { clone.get(), presolved.get(), postsolved.get() })
    {
        ASSERT_NE(copy, nullptr);
        EXPECT_NE(dynamic_cast<const Core::FMTYieldModelTsla*>(copy), nullptr);
        EXPECT_EQ(copy->getModelName(), "_TSLA");
    }
}

TEST(FMTYieldModelTsla, GivenARequestWithoutGraphWhenPredictThenRaisesARangeError)
{
    // Arrange
    const Core::FMTYieldModelTsla model;
    const Core::FMTDevelopment development(Core::FMTMask("UNITE1 PEUPLEMENT1 UTR1", getThemes()), 20, 0, 1);
    const Core::FMTYieldRequest request(development);

    // Act
    const std::vector<Exception::FMTexc> raised = Testing::getRaisedTypes([&]() { model.predict(request); });

    // Assert
    EXPECT_TRUE(Testing::hasType(raised, Exception::FMTexc::FMTrangeerror));
}

// Every development has the yield _TSLA of the default yields; outside a graph, reading it raises.
TEST(FMTYieldModelTsla, GivenTheDefaultYieldsWhenGetTheTslaOfADevelopmentOutsideAGraphThenRaisesARangeError)
{
    // Arrange
    Core::FMTYields yields;
    yields.generateDefaultYields(getThemes());
    yields.update();
    const Core::FMTDevelopment development(Core::FMTMask("UNITE2 FEU UTR3", getThemes()), 5, 0, 1);
    const Core::FMTYieldRequest request(development);

    // Act
    const std::vector<Exception::FMTexc> raised = Testing::getRaisedTypes([&]() { yields.get(request, "_TSLA"); });

    // Assert
    EXPECT_TRUE(yields.isYld("_TSLA"));
    EXPECT_TRUE(Testing::hasType(raised, Exception::FMTexc::FMTrangeerror));
}
