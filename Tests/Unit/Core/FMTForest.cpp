/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTForest.h"
#include "FMTActualDevelopment.h"
#include "FMTCoordinate.h"
#include "FMTDevelopment.h"
#include "FMTLayer.hpp"
#include "FMTMask.h"
#include "FMTMaskFilter.h"
#include "FMTTheme.h"
#include "FMTutility.h"

#include "ExceptionChain.h"

#include <gtest/gtest.h>

#include <boost/dynamic_bitset.hpp>

#include <cstdint>
#include <map>
#include <string>
#include <utility>
#include <vector>

// A forest is the layer of the developments of the cells of a map, as FMTAreaParser::readRasters and
// vectormaptoFMTforest read it: the initial map of a spatially explicit model. Its base,
// FMTLayer<Core::FMTDevelopment>, is exported with it, and is tested here. The extent is that of
// Tests/Unit/Core/FMTLayer.cpp, and the themes those of Tests/Unit/Core/FMTTheme.cpp.

namespace
{
    const std::vector<double> GEOTRANSFORM = { 1000.0, 380.0, 0.0, 5000.0, 0.0, -380.0 };
    const std::string PROJECTION = "LOCAL_CS[\"TWD_land\"]";
    const double CELL_SIZE = 14.44;

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

    Core::FMTDevelopment getDevelopment(const std::string& p_mask, int p_age, int p_lock)
    {
        return Core::FMTDevelopment(Core::FMTMask(p_mask, getThemes()), p_age, p_lock, 0);
    }

    // Three cells: the first row holds two cells of the same development, 10 years old; the second, a development of
    // 20 years locked for 2 years.
    Spatial::FMTForest getForest()
    {
        const std::map<Spatial::FMTCoordinate, Core::FMTDevelopment> mapping = {
            { Spatial::FMTCoordinate(0, 0), getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 10, 0) },
            { Spatial::FMTCoordinate(1, 0), getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 10, 0) },
            { Spatial::FMTCoordinate(0, 1), getDevelopment("UNITE1 PEUPLEMENT2 UTR1", 20, 2) } };
        return Spatial::FMTForest(Spatial::FMTLayer<Core::FMTDevelopment>(mapping, GEOTRANSFORM, 10, 8, PROJECTION,
            CELL_SIZE));
    }

    // The values of the cells of a layer, in the order of the cells.
    template<typename T>
    std::vector<T> getValues(const Spatial::FMTLayer<T>& p_layer)
    {
        std::vector<T> values;
        for (const auto& cell : p_layer)
        {
            values.push_back(cell.second);
        }
        return values;
    }

    // The age, the lock and the period of the development of each cell.
    std::vector<std::vector<int>> getDevelopments(const Spatial::FMTForest& p_forest)
    {
        std::vector<std::vector<int>> developments;
        for (const auto& cell : p_forest)
        {
            developments.push_back({ cell.second.getAge(), cell.second.getLock(), cell.second.getPeriod() });
        }
        return developments;
    }

    std::vector<std::string> getMasks(const Spatial::FMTForest& p_forest)
    {
        std::vector<std::string> masks;
        for (const auto& cell : p_forest)
        {
            masks.push_back(std::string(cell.second.getMask()));
        }
        return masks;
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
        const Core::FMTMask selection = Core::FMTMask("UNITE1 PEUPLEMENT1 UTR1", getThemes()).getUnion(
            Core::FMTMask("UNITE1 PEUPLEMENT2 UTR1", getThemes()));
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

    // The postsolve filter of FMTModel::getPostSolveFilter, as in Tests/Unit/Core/FMTMask.cpp: the bits 3 and 4 of
    // PEUPLEMENT1 and PEUPLEMENT2, and the bits of the developments in the removed themes, UNITE1 and UTR1.
    Core::FMTMaskFilter getPostSolveFilter()
    {
        boost::dynamic_bitset<uint8_t> bits(10, false);
        bits[3] = true;
        bits[4] = true;
        const Core::FMTMask selection(bits);
        return Core::FMTMaskFilter(selection,
            Core::FMTMask("UNITE1 PEUPLEMENT2 UTR1", getThemes()).getPostSolveMask(selection, getThemes()));
    }
}

TEST(FMTForest, GivenNothingWhenDefaultConstructedThenIsAnEmptyForest)
{
    // Arrange and Act
    const Spatial::FMTForest forest;

    // Assert
    EXPECT_TRUE(forest.empty());
    EXPECT_TRUE(forest.getArea().empty());
}

TEST(FMTForest, GivenALayerOfDevelopmentsWhenConstructedCopiedAndAssignedThenHoldItsCellsAndItsExtent)
{
    // Arrange
    const Spatial::FMTForest forest = getForest();
    Spatial::FMTForest assigned;

    // Act
    const Spatial::FMTForest copy(forest);
    assigned = forest;

    // Assert
    EXPECT_EQ(forest.size(), 3u);
    EXPECT_EQ(forest.getXSize(), 10u);
    EXPECT_EQ(forest.getProjection(), PROJECTION);
    EXPECT_TRUE(copy.getMapping() == forest.getMapping());
    EXPECT_TRUE(assigned.getMapping() == forest.getMapping());
    EXPECT_EQ(assigned.getGeoTransform(), GEOTRANSFORM);
}

// The area of a development is the number of its cells times the size of a cell, in the order of their first cell.
TEST(FMTForest, GivenCellsOfTheSameDevelopmentWhenGetAreaThenGivesEachDevelopmentWithTheAreaOfItsCells)
{
    // Arrange
    const Spatial::FMTForest forest = getForest();

    // Act
    const std::vector<Core::FMTActualDevelopment> area = forest.getArea();

    // Assert
    ASSERT_EQ(area.size(), 2u);
    EXPECT_EQ(std::string(area.at(0).getMask()), "UNITE1 PEUPLEMENT1 UTR1");
    EXPECT_DOUBLE_EQ(area.at(0).getArea(), 2.0 * 14.44);
    EXPECT_EQ(std::string(area.at(1).getMask()), "UNITE1 PEUPLEMENT2 UTR1");
    EXPECT_EQ(area.at(1).getLock(), 2);
    EXPECT_DOUBLE_EQ(area.at(1).getArea(), 14.44);
}

// The growth of each development: one year older, a year of lock less, the next period.
TEST(FMTForest, GivenAForestWhenGrowThenGivesAForestWhereEachDevelopmentIsOnePeriodLater)
{
    // Arrange
    const Spatial::FMTForest forest = getForest();

    // Act
    const Spatial::FMTForest grown = forest.grow();

    // Assert
    EXPECT_EQ(getDevelopments(grown), std::vector<std::vector<int>>({ { 11, 0, 1 }, { 11, 0, 1 }, { 21, 1, 1 } }));
    EXPECT_EQ(forest.at(Spatial::FMTCoordinate(0, 1)).getAge(), 20);
    EXPECT_EQ(grown.getGeoTransform(), GEOTRANSFORM);
}

TEST(FMTForest, GivenAForestWhenGetAgeThenGivesALayerOfTheAgeOfEachCellWithTheExtentOfTheForest)
{
    // Arrange
    const Spatial::FMTForest forest = getForest();

    // Act
    const Spatial::FMTLayer<int> ages = forest.getAge();

    // Assert
    EXPECT_EQ(getValues(ages), std::vector<int>({ 10, 10, 20 }));
    EXPECT_EQ(ages.getYSize(), 8u);
    EXPECT_DOUBLE_EQ(ages.getCellSize(), CELL_SIZE);
}

TEST(FMTForest, GivenAForestWhenGetLockThenGivesALayerOfTheLockOfTheLockedCellsOnly)
{
    // Arrange
    const Spatial::FMTForest forest = getForest();

    // Act
    const Spatial::FMTLayer<std::string> locks = forest.getLock();

    // Assert
    EXPECT_EQ(locks.size(), 1u);
    EXPECT_EQ(locks.at(Spatial::FMTCoordinate(0, 1)), "_Lock 2");
    EXPECT_EQ(locks.getProjection(), PROJECTION);
}

TEST(FMTForest, GivenAForestWhenGetThemesThenGivesALayerOfTheAttributesOfEachTheme)
{
    // Arrange
    const Spatial::FMTForest forest = getForest();
    const std::vector<Core::FMTTheme> themes = getThemes();

    // Act
    const std::vector<Spatial::FMTLayer<std::string>> layers = forest.getThemes(themes);

    // Assert
    ASSERT_EQ(layers.size(), 3u);
    EXPECT_EQ(getValues(layers.at(0)), std::vector<std::string>({ "UNITE1", "UNITE1", "UNITE1" }));
    EXPECT_EQ(getValues(layers.at(1)), std::vector<std::string>({ "PEUPLEMENT1", "PEUPLEMENT1", "PEUPLEMENT2" }));
    EXPECT_EQ(getValues(layers.at(2)), std::vector<std::string>({ "UTR1", "UTR1", "UTR1" }));
    EXPECT_EQ(layers.at(2).getXSize(), 10u);
}

TEST(FMTForest, GivenAForestWhenSetPeriodThenEveryDevelopmentIsAtThatPeriod)
{
    // Arrange
    Spatial::FMTForest forest = getForest();

    // Act
    forest.setPeriod(3);

    // Assert
    EXPECT_EQ(getDevelopments(forest), std::vector<std::vector<int>>({ { 10, 0, 3 }, { 10, 0, 3 }, { 20, 2, 3 } }));
}

TEST(FMTForest, GivenAForestWhenGetCopyWithAndWithoutTheDataThenGivesItsCellsOrItsExtentOnly)
{
    // Arrange
    const Spatial::FMTForest forest = getForest();

    // Act
    const Spatial::FMTForest withData = forest.getCopy();
    const Spatial::FMTForest withoutData = forest.getCopy(false);

    // Assert
    EXPECT_TRUE(withData.getMapping() == forest.getMapping());
    EXPECT_TRUE(withoutData.empty());
    EXPECT_EQ(withoutData.getGeoTransform(), GEOTRANSFORM);
    EXPECT_DOUBLE_EQ(withoutData.getCellSize(), CELL_SIZE);
}

// The last theme of each cell comes from the development of the area that has the same attributes in the other themes.
TEST(FMTForest, GivenTheAreaOfTheLastThemeWhenSetLastThemeWithAreaThenEachCellTakesTheAttributeOfItsDevelopment)
{
    // Arrange
    Spatial::FMTForest forest = getForest();
    const std::vector<Core::FMTActualDevelopment> area = {
        Core::FMTActualDevelopment(getDevelopment("UNITE1 PEUPLEMENT1 UTR2", 10, 0), 100.0),
        Core::FMTActualDevelopment(getDevelopment("UNITE1 PEUPLEMENT2 UTR3", 20, 0), 50.0) };

    // Act
    forest.setLastThemeWithArea(area, getThemes());

    // Assert
    EXPECT_EQ(getMasks(forest), std::vector<std::string>({ "UNITE1 PEUPLEMENT1 UTR2", "UNITE1 PEUPLEMENT1 UTR2",
        "UNITE1 PEUPLEMENT2 UTR3" }));
}

TEST(FMTForest, GivenAnAreaWithoutTheAttributesOfACellWhenSetLastThemeWithAreaThenRaisesAnInvalidTheme)
{
    // Arrange
    Spatial::FMTForest forest = getForest();
    const std::vector<Core::FMTActualDevelopment> area = {
        Core::FMTActualDevelopment(getDevelopment("UNITE1 PEUPLEMENT1 UTR2", 10, 0), 100.0) };

    // Act
    const std::vector<Exception::FMTexc> raised = Testing::getRaisedTypes([&]() {
        forest.setLastThemeWithArea(area, getThemes()); });

    // Assert
    EXPECT_TRUE(Testing::hasType(raised, Exception::FMTexc::FMTinvalid_theme));
}

// The presolve and the postsolve of each mask, as FMTMask::preSolve and postSolve do them; ages and locks stay.
TEST(FMTForest, GivenAForestWhenPreSolveThenPostSolveThenEachCellGetsThePresolvedMaskThenItsOwnBack)
{
    // Arrange
    const Spatial::FMTForest forest = getForest();
    const PreSolve preSolve = getPreSolve();

    // Act
    const Spatial::FMTForest presolved = forest.preSolve(preSolve.filter, preSolve.themes);
    const Spatial::FMTForest postsolved = presolved.postSolve(getPostSolveFilter(), getThemes());

    // Assert
    EXPECT_EQ(getMasks(presolved), std::vector<std::string>({ "PEUPLEMENT1", "PEUPLEMENT1", "PEUPLEMENT2" }));
    EXPECT_EQ(getMasks(postsolved), getMasks(forest));
    EXPECT_EQ(getValues(postsolved.getAge()), std::vector<int>({ 10, 10, 20 }));
    EXPECT_EQ(presolved.at(Spatial::FMTCoordinate(0, 1)).getLock(), 2);
}
