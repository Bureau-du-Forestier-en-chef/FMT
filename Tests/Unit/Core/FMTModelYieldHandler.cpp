/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTModelYieldHandler.h"
#include "FMTAgeYieldHandler.h"
#include "FMTDevelopment.h"
#include "FMTMask.h"
#include "FMTMaskFilter.h"
#include "FMTModel.h"
#include "FMTTheme.h"
#include "FMTTimeYieldHandler.h"
#include "FMTYieldModelRandom.h"
#include "FMTYieldModelTsla.h"
#include "FMTYieldRequest.h"
#include "FMTYields.h"
#include "FMTutility.h"

#include "ExceptionChain.h"
#include "GivenYieldModel.h"

#include <gtest/gtest.h>

#include <boost/property_tree/ptree.hpp>

#include <map>
#include <memory>
#include <string>
#include <vector>

// A model handler is the handler of a *YM section: its yields are predictions of yield models. FMTYieldParser builds
// it for each section, pushes a clone of each model it reads (pushBackModel) and gives each yield of a _PRED its
// model and its position among the predictions of that model (setYield); FMTYields::generateDefaultYields gives every
// model one, of the mask ? ? ?, for _TSLA. The models are those of Tests/Unit/Core/GivenYieldModel.h, which predict
// given values, and the random model of Tests/Unit/Core/FMTYieldModelRandom.cpp, which draws from the generator of
// the model it is set to. The themes are those of Tests/Unit/Core/FMTTheme.cpp.

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

    // A model of the file YieldPredModels/<p_folder>/<p_folder>.json that predicts p_predictions from VOLUMETOTAL.
    std::unique_ptr<Core::FMTYieldModel> getModel(const std::string& p_folder, const std::vector<double>& p_predictions,
        const std::vector<std::vector<double>>& p_periodicValues = std::vector<std::vector<double>>())
    {
        return std::unique_ptr<Core::FMTYieldModel>(new Testing::GivenYieldModel(
            "YieldPredModels/" + p_folder + "/" + p_folder + ".json", { "VOLUMETOTAL" }, p_predictions, p_periodicValues));
    }

    // The handler of UNITE1 ? ?: the model GIVEN predicts AREA and STEMS, 10 and 20; the model OTHER predicts VOLUME, 300.
    Core::FMTModelYieldHandler getHandler()
    {
        Core::FMTModelYieldHandler handler(getMask("UNITE1 ? ?"));
        handler.pushBackModel(getModel("GIVEN", { 10.0, 20.0 }));
        handler.setYield(0, 0, "AREA");
        handler.setYield(0, 1, "STEMS");
        handler.pushBackModel(getModel("OTHER", { 300.0 }));
        handler.setYield(1, 0, "VOLUME");
        return handler;
    }

    Core::FMTDevelopment getDevelopment(const std::string& p_mask, int p_period)
    {
        return Core::FMTDevelopment(getMask(p_mask), 20, 0, p_period);
    }

    // The value of p_yield for p_development, read by the handler directly.
    double getValue(const Core::FMTModelYieldHandler& p_handler, const std::string& p_yield,
        const Core::FMTDevelopment& p_development)
    {
        const Core::FMTYieldRequest REQUEST(p_development);
        return p_handler.get(p_yield, REQUEST);
    }

    // The random model of Tests/Unit/Core/FMTYieldModelRandom.cpp, of the sources YPROB0 and YPROB1.
    std::unique_ptr<Core::FMTYieldModel> getRandomModel(bool p_caching)
    {
        boost::property_tree::ptree properties;
        properties.put("modelFileName", "YieldPredModels/RANDOM/RANDOM.json");
        properties.put("modelType", "RANDOM");
        properties.put("caching", p_caching);
        return std::unique_ptr<Core::FMTYieldModel>(new Core::FMTYieldModelRandom(properties, { "YPROB0", "YPROB1" }));
    }

    // The sources of the random model: YPROB1 alone is not zero for UNITE1, YPROB0 alone for UNITE2.
    Core::FMTYields getWeights()
    {
        Core::FMTYields yields;
        Core::FMTAgeYieldHandler unit1(getMask("UNITE1 ? ?"));
        unit1.setYieldValues("YPROB0", { 10, 30 }, { 0.0, 0.0 });
        unit1.setYieldValues("YPROB1", { 10, 30 }, { 5.0, 5.0 });
        Core::FMTAgeYieldHandler unit2(getMask("UNITE2 ? ?"));
        unit2.setYieldValues("YPROB0", { 10, 30 }, { 3.0, 3.0 });
        unit2.setYieldValues("YPROB1", { 10, 30 }, { 0.0, 0.0 });
        yields.pushBackAgeHandler(getMask("UNITE1 ? ?"), unit1);
        yields.pushBackAgeHandler(getMask("UNITE2 ? ?"), unit2);
        yields.update();
        return yields;
    }

    // The draw of the random yield YRANDOM of p_handler for p_development, its request prepared by the sources.
    double getDraw(const Core::FMTModelYieldHandler& p_handler, const Core::FMTYields& p_weights,
        const Core::FMTDevelopment& p_development)
    {
        const Core::FMTYieldRequest REQUEST(p_development);
        p_weights.get(REQUEST, "YPROB0");
        return p_handler.get("YRANDOM", REQUEST);
    }

    // The presolve of Tests/Unit/Core/FMTMask.cpp: the filter of the union of UNITE1 PEUPLEMENT1 UTR1 and
    // UNITE1 PEUPLEMENT2 UTR1, and the themes left with more than one attribute, here PEUPLEMENT1 and PEUPLEMENT2.
    struct PreSolve
    {
        Core::FMTMaskFilter filter;
        std::vector<Core::FMTTheme> themes;
    };

    PreSolve getPreSolve()
    {
        PreSolve preSolve;
        const Core::FMTMask selection = getMask("UNITE1 PEUPLEMENT1 UTR1").getUnion(getMask("UNITE1 PEUPLEMENT2 UTR1"));
        preSolve.filter = Core::FMTMaskFilter(selection);
        size_t newId = 0;
        size_t newStart = 0;
        for (const Core::FMTTheme& theme : getThemes())
        {
            const Core::FMTTheme presolved = theme.preSolve(preSolve.filter, newId, newStart);
            if (!presolved.empty())
            {
                preSolve.themes.push_back(presolved);
            }
        }
        return preSolve;
    }
}

TEST(FMTModelYieldHandler, GivenNothingWhenDefaultConstructedThenIsAModelHandlerWithoutModelNorYield)
{
    // Arrange and Act
    const Core::FMTModelYieldHandler handler;

    // Assert
    EXPECT_EQ(handler.getType(), Core::FMTyldtype::FMTmodelyld);
    EXPECT_TRUE(handler.empty());
    EXPECT_EQ(handler.size(), 0u);
    EXPECT_TRUE(handler.getYieldNames().empty());
    EXPECT_FALSE(handler.containsYield("AREA"));
    EXPECT_TRUE(handler.getModelsNameByIndex().empty());
}

TEST(FMTModelYieldHandler, GivenAMaskWhenConstructedThenKeepsIt)
{
    // Arrange
    const Core::FMTMask mask = getMask("UNITE1 ? ?");

    // Act
    const Core::FMTModelYieldHandler handler(mask);

    // Assert
    EXPECT_EQ(std::string(handler.getMask()), "UNITE1 ? ?");
    EXPECT_TRUE(handler.empty());
}

// size counts the yields, not the models. The names are in the order of cmpYieldString: the shortest first.
TEST(FMTModelYieldHandler, GivenModelsAndTheirYieldsWhenPushBackModelAndSetYieldThenHasTheYieldsInTheOrderOfTheirNames)
{
    // Arrange and Act
    const Core::FMTModelYieldHandler handler = getHandler();

    // Assert
    EXPECT_FALSE(handler.empty());
    EXPECT_EQ(handler.size(), 3u);
    EXPECT_EQ(handler.getYieldNames(), std::vector<std::string>({ "AREA", "STEMS", "VOLUME" }));
    EXPECT_TRUE(handler.containsYield("STEMS"));
    EXPECT_FALSE(handler.containsYield("VOLUMETOTAL"));
    EXPECT_FALSE(handler.isNullYield("AREA"));
    EXPECT_EQ(handler.getModelsNameByIndex(), (std::map<std::string, size_t>({
        { "YieldPredModels/GIVEN/GIVEN.json", 0 }, { "YieldPredModels/OTHER/OTHER.json", 1 } })));
}

TEST(FMTModelYieldHandler, GivenTheHandlerInTheYieldsWhenGetThenGivesThePredictionAtThePositionOfEachYieldInItsModel)
{
    // Arrange
    Core::FMTYields yields;
    yields.push_back(getMask("UNITE1 ? ?"), getHandler().clone());
    yields.update();
    const Core::FMTDevelopment development = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 1);
    const Core::FMTYieldRequest request(development);

    // Act
    const std::vector<double> values = { yields.get(request, "AREA"), yields.get(request, "STEMS"),
        yields.get(request, "VOLUME") };

    // Assert
    EXPECT_EQ(values, std::vector<double>({ 10.0, 20.0, 300.0 }));
}

TEST(FMTModelYieldHandler, GivenAYieldBeyondThePredictionsOfItsModelWhenGetThenRaisesAnUnhandledError)
{
    // Arrange
    Core::FMTModelYieldHandler handler = getHandler();
    handler.setYield(1, 1, "BEYOND");
    const Core::FMTDevelopment development = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 1);

    // Act
    const std::vector<Exception::FMTexc> raised = Testing::getRaisedTypes([&]() { getValue(handler, "BEYOND", development); });

    // Assert
    EXPECT_TRUE(Testing::hasType(raised, Exception::FMTexc::FMTunhandlederror));
}

TEST(FMTModelYieldHandler, GivenAYieldThatTheHandlerDoesNotHaveWhenGetThenRaisesAnUnhandledError)
{
    // Arrange
    const Core::FMTModelYieldHandler handler = getHandler();
    const Core::FMTDevelopment development = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 1);

    // Act
    const std::vector<Exception::FMTexc> raised = Testing::getRaisedTypes([&]() { getValue(handler, "STEM", development); });

    // Assert
    EXPECT_TRUE(Testing::hasType(raised, Exception::FMTexc::FMTunhandlederror));
}

// The copy clones the models: it predicts after the handler it was copied from is gone.
TEST(FMTModelYieldHandler, GivenAHandlerWhenCopyConstructedThenTheCopyPredictsItsYieldsWithModelsOfItsOwn)
{
    // Arrange
    std::unique_ptr<Core::FMTModelYieldHandler> handler(new Core::FMTModelYieldHandler(getHandler()));
    const Core::FMTDevelopment development = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 1);

    // Act
    const Core::FMTModelYieldHandler copy(*handler);
    handler.reset();
    const std::vector<double> values = { getValue(copy, "AREA", development), getValue(copy, "VOLUME", development) };

    // Assert
    EXPECT_EQ(std::string(copy.getMask()), "UNITE1 ? ?");
    EXPECT_EQ(copy.getYieldNames(), std::vector<std::string>({ "AREA", "STEMS", "VOLUME" }));
    EXPECT_EQ(values, std::vector<double>({ 10.0, 300.0 }));
}

TEST(FMTModelYieldHandler, GivenAHandlerWhenClonedThenTheCloneIsAModelHandlerOfTheSameYieldsAndPredictions)
{
    // Arrange
    const Core::FMTModelYieldHandler handler = getHandler();
    const Core::FMTDevelopment development = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 1);
    const Core::FMTYieldRequest request(development);

    // Act
    const std::unique_ptr<Core::FMTYieldHandler> clone = handler.clone();
    const double stems = clone->get("STEMS", request);

    // Assert
    EXPECT_EQ(clone->getType(), Core::FMTyldtype::FMTmodelyld);
    EXPECT_EQ(clone->getYieldNames(), handler.getYieldNames());
    EXPECT_DOUBLE_EQ(stems, 20.0);
}

TEST(FMTModelYieldHandler, GivenAHandlerWithoutModelWhenAssignedThenGetsTheModelsAndTheYieldsOfTheOther)
{
    // Arrange
    const Core::FMTModelYieldHandler handler = getHandler();
    Core::FMTModelYieldHandler target(getMask("UNITE2 ? ?"));
    const Core::FMTDevelopment development = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 1);

    // Act
    target = handler;
    const double volume = getValue(target, "VOLUME", development);

    // Assert
    EXPECT_EQ(std::string(target.getMask()), "UNITE1 ? ?");
    EXPECT_EQ(target.getModelsNameByIndex(), handler.getModelsNameByIndex());
    EXPECT_DOUBLE_EQ(volume, 300.0);
}

// Observation, no defect announced: operator= adds the models of the other handler after its own instead of replacing
// them, and takes the positions of its yields. A handler that had a model reads the yields of the other from its former
// model. Nothing in FMT assigns a model handler: the yields clone their handlers.
TEST(FMTModelYieldHandler, GivenAHandlerWithAModelWhenAssignedThenKeepsItsModelBeforeThoseOfTheOther)
{
    // Arrange
    const Core::FMTModelYieldHandler handler = getHandler();
    Core::FMTModelYieldHandler target(getMask("UNITE1 ? ?"));
    target.pushBackModel(getModel("FORMER", { 1.0, 2.0 }));
    target.setYield(0, 0, "AREA");
    const Core::FMTDevelopment development = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 1);

    // Act
    target = handler;
    const double area = getValue(target, "AREA", development);

    // Assert
    EXPECT_EQ(target.getModelsNameByIndex(), (std::map<std::string, size_t>({ { "YieldPredModels/FORMER/FORMER.json", 0 },
        { "YieldPredModels/GIVEN/GIVEN.json", 1 }, { "YieldPredModels/OTHER/OTHER.json", 2 } })));
    EXPECT_DOUBLE_EQ(area, 1.0);
}

// operator== compares what FMTYieldHandler::operator== compares, the mask and the base ages: two handlers of the same
// mask are equal whatever their models (the base ages of a model handler stay empty). As for the age handlers
// (Tests/Unit/Core/FMTYieldHandler.cpp, #383), a correction of #383 that compares the handlers by their operator==
// would take them for equal.
TEST(FMTModelYieldHandler, GivenTwoHandlersOfTheSameMaskWithOtherModelsWhenComparedThenTheyAreEqual)
{
    // Arrange
    const Core::FMTModelYieldHandler handler = getHandler();
    Core::FMTModelYieldHandler other(getMask("UNITE1 ? ?"));
    other.pushBackModel(getModel("FORMER", { 1.0 }));
    other.setYield(0, 0, "AREA");
    Core::FMTModelYieldHandler otherMask(getMask("UNITE2 ? ?"));
    otherMask.pushBackModel(getModel("GIVEN", { 10.0, 20.0 }));
    otherMask.setYield(0, 0, "AREA");

    // Act
    const bool sameMask = (handler == other);
    const bool differentMask = (handler == otherMask);

    // Assert
    EXPECT_TRUE(sameMask);
    EXPECT_FALSE(differentMask);
}

TEST(FMTModelYieldHandler, GivenAHandlerOfARandomModelWhenSetModelThenItsModelDrawsWithTheGeneratorOfThatModel)
{
    // Arrange
    Models::FMTModel fmtModel;
    Core::FMTModelYieldHandler handler(getMask("? ? ?"));
    handler.pushBackModel(getRandomModel(false));
    handler.setYield(0, 0, "YRANDOM");
    const Core::FMTYields weights = getWeights();
    const Core::FMTDevelopment unit1 = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 1);
    const Core::FMTDevelopment unit2 = getDevelopment("UNITE2 PEUPLEMENT1 UTR1", 1);

    // Act
    handler.setModel(&fmtModel);
    const std::vector<double> draws = { getDraw(handler, weights, unit1), getDraw(handler, weights, unit2) };

    // Assert
    EXPECT_EQ(draws, std::vector<double>({ 1.0, 0.0 }));
}

TEST(FMTModelYieldHandler, GivenAHandlerOfACachingRandomModelWhenClearRandomYieldsCacheThenItsModelDrawsAgain)
{
    // Arrange
    Models::FMTModel fmtModel;
    Core::FMTModelYieldHandler handler(getMask("? ? ?"));
    handler.pushBackModel(getRandomModel(true));
    handler.setYield(0, 0, "YRANDOM");
    handler.setModel(&fmtModel);
    const Core::FMTYields weights = getWeights();
    const Core::FMTDevelopment unit1 = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 1);
    const Core::FMTDevelopment unit2 = getDevelopment("UNITE2 PEUPLEMENT1 UTR1", 1);
    const double unit1Draw = getDraw(handler, weights, unit1);
    const double unit2Cached = getDraw(handler, weights, unit2);

    // Act
    handler.clearRandomYieldsCache();
    const double unit2Draw = getDraw(handler, weights, unit2);

    // Assert
    EXPECT_DOUBLE_EQ(unit1Draw, 1.0);
    EXPECT_DOUBLE_EQ(unit2Cached, 1.0);
    EXPECT_DOUBLE_EQ(unit2Draw, 0.0);
}

// The periodic values of the model of each yield, from period 0, become the values of a time handler of the mask.
TEST(FMTModelYieldHandler, GivenModelsWithPeriodicValuesWhenToTimeHandlerThenGivesATimeHandlerOfTheseValues)
{
    // Arrange
    Core::FMTModelYieldHandler handler(getMask("UNITE1 ? ?"));
    handler.pushBackModel(getModel("GIVEN", { 10.0, 20.0 }, { { 1.0, 2.0, 3.0 }, { 4.0, 5.0, 6.0 } }));
    handler.setYield(0, 0, "AREA");
    handler.setYield(0, 1, "STEMS");
    const Core::FMTDevelopment development = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 1);
    const Core::FMTYieldRequest request(development);

    // Act
    const Core::FMTTimeYieldHandler timeHandler = handler.toTimeHandler();

    // Assert
    EXPECT_EQ(std::string(timeHandler.getMask()), "UNITE1 ? ?");
    EXPECT_EQ(timeHandler.getYieldNames(), std::vector<std::string>({ "AREA", "STEMS" }));
    EXPECT_DOUBLE_EQ(timeHandler.get("AREA", request), 2.0);
    EXPECT_DOUBLE_EQ(timeHandler.get("STEMS", request), 5.0);
}

TEST(FMTModelYieldHandler, GivenAModelWithoutPeriodicValuesWhenToTimeHandlerThenGivesAnEmptyTimeHandlerOfTheMask)
{
    // Arrange
    Core::FMTModelYieldHandler handler(getMask("UNITE1 ? ?"));
    handler.pushBackModel(getModel("PERIODIC", { 1.0 }, { { 1.0, 2.0 } }));
    handler.setYield(0, 0, "AREA");
    handler.pushBackModel(getModel("GIVEN", { 10.0 }));
    handler.setYield(1, 0, "STEMS");

    // Act
    const Core::FMTTimeYieldHandler timeHandler = handler.toTimeHandler();

    // Assert
    EXPECT_TRUE(timeHandler.empty());
    EXPECT_EQ(std::string(timeHandler.getMask()), "UNITE1 ? ?");
}

// A handler is written as its *YM section: one line per model, its yields then its prediction, followed by an empty
// line, since the string of the model ends with its own line break; the parser skips the empty lines.
TEST(FMTModelYieldHandler, GivenModelsWithoutPeriodicValuesWhenConvertedToStringThenWritesTheirYieldsAndTheirPredictions)
{
    // Arrange
    const Core::FMTModelYieldHandler handler = getHandler();

    // Act
    const std::string text(handler);

    // Assert
    EXPECT_EQ(text, "*YM UNITE1 ? ?\nAREA,STEMS _PRED(GIVEN,VOLUMETOTAL)\n\nVOLUME _PRED(OTHER,VOLUMETOTAL)\n\n");
}

// The handler of models that all have periodic values is written as their time handler, with the values of
// std::to_string without their trailing zeros.
TEST(FMTModelYieldHandler, GivenModelsWithPeriodicValuesWhenConvertedToStringThenWritesTheirTimeHandler)
{
    // Arrange
    Core::FMTModelYieldHandler handler(getMask("UNITE1 ? ?"));
    handler.pushBackModel(getModel("GIVEN", { 10.0, 20.0 }, { { 1.0, 2.0, 3.0 }, { 4.0, 5.5, 6.0 } }));
    handler.setYield(0, 0, "AREA");
    handler.setYield(0, 1, "STEMS");

    // Act
    const std::string text(handler);

    // Assert
    EXPECT_EQ(text, "*YT UNITE1 ? ?\nAREA 0 1 2 3 \nSTEMS 0 4 5.5 6 \n");
}

// The _TSLA handler of the default yields writes nothing: FMTYields::generateDefaultYields gives it again at the
// next read.
TEST(FMTModelYieldHandler, GivenTheTslaHandlerOfTheDefaultYieldsWhenConvertedToStringThenWritesNothing)
{
    // Arrange
    Core::FMTModelYieldHandler handler(getMask("? ? ?"));
    const std::unique_ptr<Core::FMTYieldModel> tsla(new Core::FMTYieldModelTsla());
    handler.pushBackModel(tsla);
    handler.setYield(0, 0, tsla->getModelName());

    // Act
    const std::string text(handler);

    // Assert
    EXPECT_TRUE(text.empty());
    EXPECT_EQ(handler.getYieldNames(), std::vector<std::string>({ "_TSLA" }));
}

// Observation, no defect announced: the presolve presolves the models but not the mask of the handler, unlike
// FMTYieldHandler::preSolve (Tests/Unit/Core/FMTYieldHandler.cpp), which gives the age handler the mask PEUPLEMENT2.
// FMTYields selects the handlers by the masks of its list, which FMTYields::preSolve presolves: the values are not
// touched, only what reads the mask of the handler, its string and toTimeHandler.
TEST(FMTModelYieldHandler, GivenAHandlerWhenPreSolveThenTheCopyKeepsTheMaskAndTheYieldsOfTheHandler)
{
    // Arrange
    const PreSolve preSolve = getPreSolve();
    Core::FMTModelYieldHandler handler(getMask("UNITE1 PEUPLEMENT2 UTR1"));
    handler.pushBackModel(getModel("GIVEN", { 10.0, 20.0 }));
    handler.setYield(0, 0, "AREA");
    const Core::FMTDevelopment development = getDevelopment("UNITE1 PEUPLEMENT2 UTR1", 1);
    const Core::FMTYieldRequest request(development);

    // Act
    const std::unique_ptr<Core::FMTYieldHandler> presolved = handler.preSolve(preSolve.filter, preSolve.themes);
    const double area = presolved->get("AREA", request);

    // Assert
    EXPECT_EQ(presolved->getType(), Core::FMTyldtype::FMTmodelyld);
    EXPECT_EQ(std::string(presolved->getMask()), "UNITE1 PEUPLEMENT2 UTR1");
    EXPECT_DOUBLE_EQ(area, 10.0);
}
