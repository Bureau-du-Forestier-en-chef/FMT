/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTYieldModelUnitCoverage.h"
#include "FMTDevelopment.h"
#include "FMTMask.h"
#include "FMTMaskFilter.h"
#include "FMTTheme.h"
#include "FMTYieldRequest.h"
#include "FMTutility.h"

#include "ExceptionChain.h"
#include "TestTools.h"

#include <gtest/gtest.h>

#include <boost/dynamic_bitset.hpp>
#include <boost/property_tree/ptree.hpp>

#include <cstddef>
#include <cstdint>
#include <exception>
#include <functional>
#include <initializer_list>
#include <map>
#include <memory>
#include <string>
#include <vector>

// The unit coverage model predicts, for the developments of its mask, the inventory of each of its source yields over
// that mask at the period of the area of its model: ysumvolume,yprob0s,yprob1s,yarea
// _PRED(UNIT_COVERAGE,volumetotal,yprob0,yprob1,yreference) in the scenario UNIT_COVERAGE of TWD_land. FMTYieldParser
// builds it from the JSON file of the model, read into a property tree, and the mask of its *YM section. Its
// predictions are outputs of the model it is given, which only a model with a graph computes (FMTSrModel,
// FMTNssModel): they are covered by SimulateNonSpatially on that scenario. The cases check what it does without them.
// The themes are those of Tests/Unit/Core/FMTTheme.cpp.

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

    // The properties that the JSON file of the unit coverage model gives.
    boost::property_tree::ptree getProperties()
    {
        boost::property_tree::ptree properties;
        properties.put("modelFileName", "YieldPredModels/UNIT_COVERAGE/UNIT_COVERAGE.json");
        properties.put("modelType", "UNIT_COVERAGE");
        return properties;
    }

    const std::vector<std::string> SOURCES = { "VOLUMETOTAL", "YPROB0", "YPROB1", "YREFERENCE" };

    // The presolve of Tests/Unit/Core/FMTMask.cpp: the filter of the union of UNITE1 PEUPLEMENT1 UTR1 and
    // UNITE1 PEUPLEMENT2 UTR1, and the themes left with more than one attribute, here PEUPLEMENT1 and PEUPLEMENT2.
    struct PreSolve
    {
        Core::FMTMaskFilter filter;
        std::vector<Core::FMTTheme> themes;
    };

    // The postsolve filter of FMTModel::getPostSolveFilter for UNITE1 PEUPLEMENT2 UTR1, as in Tests/Unit/Core/FMTMask.cpp:
    // the bits 3 and 4 of PEUPLEMENT1 and PEUPLEMENT2, and the bits of the development in the removed themes.
    Core::FMTMaskFilter getPostSolveFilter()
    {
        boost::dynamic_bitset<uint8_t> bits(10, false);
        bits[3] = true;
        bits[4] = true;
        const Core::FMTMask selection(bits);
        return Core::FMTMaskFilter(selection, getMask("UNITE1 PEUPLEMENT2 UTR1").getPostSolveMask(selection, getThemes()));
    }

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

    // The messages of the exceptions that p_call raises, outermost first.
    std::vector<std::string> getRaisedMessages(const std::function<void()>& p_call)
    {
        std::vector<std::string> messages;
        try
        {
            p_call();
        }
        catch (const std::exception& error)
        {
            Testing::visitNested(error, [&messages](const std::exception& p_nested, std::size_t)
                {
                messages.push_back(p_nested.what());
                });
        }
        return messages;
    }
}

TEST(FMTYieldModelUnitCoverage, GivenThePropertiesOfItsFileItsSourcesAndAMaskWhenConstructedThenKeepsTheFileNameAndTheSources)
{
    // Arrange
    const boost::property_tree::ptree properties = getProperties();

    // Act
    const Core::FMTYieldModelUnitCoverage model(properties, SOURCES, getMask("UNITE1 ? ?"));

    // Assert
    EXPECT_EQ(model.getModelName(), "YieldPredModels/UNIT_COVERAGE/UNIT_COVERAGE.json");
    EXPECT_EQ(model.getModelYields(), SOURCES);
    EXPECT_EQ(Core::FMTYieldModelUnitCoverage::getModelType(), "UNIT_COVERAGE");
    EXPECT_TRUE(model.getPeriodicValues().empty());
}

TEST(FMTYieldModelUnitCoverage, GivenAModelWithoutTheModelOfItsOutputsWhenPredictThenRaisesThatNoModelIsAvailable)
{
    // Arrange
    const Core::FMTYieldModelUnitCoverage model(getProperties(), SOURCES, getMask("UNITE1 ? ?"));
    const Core::FMTDevelopment development(getMask("UNITE1 PEUPLEMENT1 UTR1"), 20, 0, 1);
    const Core::FMTYieldRequest request(development);

    // Act
    const std::vector<Exception::FMTexc> types = Testing::getRaisedTypes([&]() { model.predict(request); });
    const std::vector<std::string> messages = getRaisedMessages([&]() { model.predict(request); });

    // Assert
    EXPECT_TRUE(Testing::hasType(types, Exception::FMTexc::FMTfunctionfailed));
    ASSERT_FALSE(messages.empty());
    EXPECT_NE(messages.back().find("No available model"), std::string::npos);
}

TEST(FMTYieldModelUnitCoverage, GivenAModelWhenClonedPreSolvedAndPostSolvedThenEachGivesAUnitCoverageModelOfTheSameFileAndSources)
{
    // Arrange
    const Core::FMTYieldModelUnitCoverage model(getProperties(), SOURCES, getMask("UNITE1 PEUPLEMENT2 UTR1"));
    const PreSolve preSolve = getPreSolve();

    // Act
    const std::unique_ptr<Core::FMTYieldModel> clone = model.clone();
    const std::unique_ptr<Core::FMTYieldModel> presolved = model.preSolve(preSolve.filter, preSolve.themes);
    const std::unique_ptr<Core::FMTYieldModel> postsolved = presolved->postSolve(getPostSolveFilter(), getThemes());

    // Assert
    for (const Core::FMTYieldModel* copy : { clone.get(), presolved.get(), postsolved.get() })
    {
        ASSERT_NE(copy, nullptr);
        EXPECT_NE(dynamic_cast<const Core::FMTYieldModelUnitCoverage*>(copy), nullptr);
        EXPECT_EQ(copy->getModelName(), model.getModelName());
        EXPECT_EQ(copy->getModelYields(), SOURCES);
    }
}
