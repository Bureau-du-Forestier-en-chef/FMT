/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTYieldModelRandom.h"
#include "FMTAgeYieldHandler.h"
#include "FMTDevelopment.h"
#include "FMTMask.h"
#include "FMTMaskFilter.h"
#include "FMTModel.h"
#include "FMTTheme.h"
#include "FMTYieldRequest.h"
#include "FMTYields.h"
#include "FMTutility.h"

#include "ExceptionChain.h"

#include <gtest/gtest.h>

#include <boost/property_tree/ptree.hpp>

#include <map>
#include <memory>
#include <string>
#include <vector>

// The random yield model draws, for a development, the index of one of its source yields, each with a probability
// proportional to the absolute value of that yield for the development: yrandom _PRED(RANDOM,yprob0,yprob1,yprob2)
// in the scenario randomYield of TWD_land. FMTYieldParser builds it from the JSON file of the model, read into a
// property tree, as YieldPredModels/RANDOM/RANDOM.json of Templates; the tree here holds the same keys. The draw takes
// the generator of the model the yield model is given: the cases draw from a distribution where one yield only is
// not zero, whose index is the only possible draw. The themes are those of Tests/Unit/Core/FMTTheme.cpp.

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

    Core::FMTMask getMask(const std::string& p_mask)
    {
        return Core::FMTMask(p_mask, getThemes());
    }

    // The properties that the JSON file of a random model gives.
    boost::property_tree::ptree getProperties(bool p_caching)
    {
        boost::property_tree::ptree properties;
        properties.put("modelFileName", "YieldPredModels/RANDOM/RANDOM.json");
        properties.put("modelType", "RANDOM");
        properties.put("caching", p_caching);
        return properties;
    }

    const std::vector<std::string> SOURCES = { "YPROB0", "YPROB1", "YPROB2" };

    // An age handler of p_mask whose source yields have the values p_weights at every age.
    Core::FMTAgeYieldHandler getWeights(const std::string& p_mask, const std::vector<double>& p_weights)
    {
        Core::FMTAgeYieldHandler handler(getMask(p_mask));
        for (size_t yieldId = 0; yieldId < SOURCES.size(); ++yieldId)
        {
            const double WEIGHT = p_weights.at(yieldId);
            handler.setYieldValues(SOURCES.at(yieldId), { 10, 20, 30 }, { WEIGHT, WEIGHT, WEIGHT });
        }
        return handler;
    }

    // The source yields: YPROB1 alone is not zero for UNITE1, YPROB0 alone for UNITE2.
    Core::FMTYields getYields()
    {
        Core::FMTYields yields;
        yields.pushBackAgeHandler(getMask("UNITE1 ? ?"), getWeights("UNITE1 ? ?", { 0.0, 5.0, 0.0 }));
        yields.pushBackAgeHandler(getMask("UNITE2 ? ?"), getWeights("UNITE2 ? ?", { 3.0, 0.0, 0.0 }));
        yields.update();
        return yields;
    }

    Core::FMTDevelopment getDevelopment(const std::string& p_mask, int p_period)
    {
        return Core::FMTDevelopment(getMask(p_mask), 20, 0, p_period);
    }

    // What the model predicts for p_development: its request is prepared by the yields first, as FMTYields::get does
    // before it calls the model handler.
    std::vector<double> predict(const Core::FMTYieldModelRandom& p_model, const Core::FMTYields& p_yields,
        const Core::FMTDevelopment& p_development)
    {
        const Core::FMTYieldRequest REQUEST(p_development);
        p_yields.get(REQUEST, "YPROB0");
        return p_model.predict(REQUEST);
    }
}

TEST(FMTYieldModelRandom, GivenThePropertiesOfItsFileAndItsSourcesWhenConstructedThenKeepsTheFileNameAndTheSources)
{
    // Arrange
    const boost::property_tree::ptree properties = getProperties(false);

    // Act
    const Core::FMTYieldModelRandom model(properties, SOURCES);

    // Assert
    EXPECT_EQ(model.getModelName(), "YieldPredModels/RANDOM/RANDOM.json");
    EXPECT_EQ(model.getModelYields(), SOURCES);
    EXPECT_EQ(Core::FMTYieldModelRandom::getModelType(), "RANDOM");
}

TEST(FMTYieldModelRandom, GivenADevelopmentWithOneSourceYieldNotZeroWhenPredictThenDrawsTheIndexOfThatYield)
{
    // Arrange
    Models::FMTModel fmtModel;
    Core::FMTYieldModelRandom model(getProperties(false), SOURCES);
    model.setModel(&fmtModel);
    const Core::FMTYields yields = getYields();
    const Core::FMTDevelopment unit1 = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 1);
    const Core::FMTDevelopment unit2 = getDevelopment("UNITE2 PEUPLEMENT1 UTR1", 1);

    // Act
    const std::vector<double> unit1Draw = predict(model, yields, unit1);
    const std::vector<double> unit2Draw = predict(model, yields, unit2);

    // Assert
    EXPECT_EQ(unit1Draw, std::vector<double>({ 1.0 }));
    EXPECT_EQ(unit2Draw, std::vector<double>({ 0.0 }));
}

// _getNormalizedYields takes the absolute value of each source yield: a negative yield weighs as much as its opposite.
TEST(FMTYieldModelRandom, GivenANegativeSourceYieldWhenPredictThenDrawsItsIndexAsIfItWerePositive)
{
    // Arrange
    Models::FMTModel fmtModel;
    Core::FMTYieldModelRandom model(getProperties(false), SOURCES);
    model.setModel(&fmtModel);
    Core::FMTYields yields;
    yields.pushBackAgeHandler(getMask("? ? ?"), getWeights("? ? ?", { 0.0, 0.0, -4.0 }));
    yields.update();
    const Core::FMTDevelopment development = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 1);

    // Act
    const std::vector<double> draw = predict(model, yields, development);

    // Assert
    EXPECT_EQ(draw, std::vector<double>({ 2.0 }));
}

TEST(FMTYieldModelRandom, GivenASourceYieldThatNoHandlerOfTheDevelopmentHasWhenPredictThenRaisesAMissingYield)
{
    // Arrange
    Models::FMTModel fmtModel;
    Core::FMTYieldModelRandom model(getProperties(false), { "YPROB0", "YPROB9" });
    model.setModel(&fmtModel);
    const Core::FMTYields yields = getYields();
    const Core::FMTDevelopment development = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 1);

    // Act
    const std::vector<Exception::FMTexc> raised = Testing::getRaisedTypes([&]() { predict(model, yields, development); });

    // Assert
    EXPECT_TRUE(Testing::hasType(raised, Exception::FMTexc::FMTmissingyield));
}

// With caching, as in YieldPredModels/RANDOM_UNIT, the first draw of a period is the draw of every development of that
// period, until the cache is cleared: the scenario randomYieldUnit draws one value per period for all its developments.
TEST(FMTYieldModelRandom, GivenACachingModelWhenPredictForAnotherDevelopmentOfThePeriodThenGivesTheFirstDrawUntilTheCacheIsCleared)
{
    // Arrange
    Models::FMTModel fmtModel;
    Core::FMTYieldModelRandom model(getProperties(true), SOURCES);
    model.setModel(&fmtModel);
    const Core::FMTYields yields = getYields();
    const Core::FMTDevelopment unit1 = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 1);
    const Core::FMTDevelopment unit2 = getDevelopment("UNITE2 PEUPLEMENT1 UTR1", 1);
    const Core::FMTDevelopment unit2Later = getDevelopment("UNITE2 PEUPLEMENT1 UTR1", 2);

    // Act
    const std::vector<double> unit1Draw = predict(model, yields, unit1);
    const std::vector<double> unit2Draw = predict(model, yields, unit2);
    const std::vector<double> unit2LaterDraw = predict(model, yields, unit2Later);
    model.clearRandomYieldsCache();
    const std::vector<double> unit2DrawAfterClear = predict(model, yields, unit2);

    // Assert
    EXPECT_EQ(unit1Draw, std::vector<double>({ 1.0 }));
    EXPECT_EQ(unit2Draw, std::vector<double>({ 1.0 }));
    EXPECT_EQ(unit2LaterDraw, std::vector<double>({ 0.0 }));
    EXPECT_EQ(unit2DrawAfterClear, std::vector<double>({ 0.0 }));
}

TEST(FMTYieldModelRandom, GivenAModelWithoutCachingWhenPredictForAnotherDevelopmentOfThePeriodThenDrawsFromItsOwnYields)
{
    // Arrange
    Models::FMTModel fmtModel;
    Core::FMTYieldModelRandom model(getProperties(false), SOURCES);
    model.setModel(&fmtModel);
    const Core::FMTYields yields = getYields();
    const Core::FMTDevelopment unit1 = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 1);
    const Core::FMTDevelopment unit2 = getDevelopment("UNITE2 PEUPLEMENT1 UTR1", 1);

    // Act
    predict(model, yields, unit1);
    const std::vector<double> unit2Draw = predict(model, yields, unit2);

    // Assert
    EXPECT_EQ(unit2Draw, std::vector<double>({ 0.0 }));
}

// A clone keeps the model it draws from, as the copies of the yields of a model do.
TEST(FMTYieldModelRandom, GivenAModelWhenClonedThenTheCloneIsARandomModelOfTheSameFileThatDrawsFromTheSameModel)
{
    // Arrange
    Models::FMTModel fmtModel;
    Core::FMTYieldModelRandom model(getProperties(false), SOURCES);
    model.setModel(&fmtModel);
    const Core::FMTYields yields = getYields();
    const Core::FMTDevelopment development = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 1);

    // Act
    const std::unique_ptr<Core::FMTYieldModel> clone = model.clone();
    const Core::FMTYieldModelRandom& random = dynamic_cast<const Core::FMTYieldModelRandom&>(*clone);
    const std::vector<double> draw = predict(random, yields, development);

    // Assert
    EXPECT_EQ(random.getModelName(), model.getModelName());
    EXPECT_EQ(random.getModelYields(), SOURCES);
    EXPECT_EQ(draw, std::vector<double>({ 1.0 }));
}

// The model holds no mask: its presolve and its postsolve are copies.
TEST(FMTYieldModelRandom, GivenAModelWhenPreSolveAndPostSolveThenGivesRandomModelsOfTheSameFileAndSources)
{
    // Arrange
    const Core::FMTYieldModelRandom model(getProperties(true), SOURCES);
    const Core::FMTMaskFilter filter;
    const std::vector<Core::FMTTheme> themes = getThemes();

    // Act
    const std::unique_ptr<Core::FMTYieldModel> presolved = model.preSolve(filter, themes);
    const std::unique_ptr<Core::FMTYieldModel> postsolved = model.postSolve(filter, themes);

    // Assert
    ASSERT_NE(presolved, nullptr);
    ASSERT_NE(postsolved, nullptr);
    EXPECT_NE(dynamic_cast<const Core::FMTYieldModelRandom*>(presolved.get()), nullptr);
    EXPECT_NE(dynamic_cast<const Core::FMTYieldModelRandom*>(postsolved.get()), nullptr);
    EXPECT_EQ(presolved->getModelName(), model.getModelName());
    EXPECT_EQ(postsolved->getModelYields(), SOURCES);
}
