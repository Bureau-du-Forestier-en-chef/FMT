/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTLayer.hpp"
#include "FMTCoordinate.h"

#include <gtest/gtest.h>

#include <map>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

// FMTLayer is a class template: a map of the cells of a raster to a value, with the extent of the raster. It is tested
// through the instantiations of FMT that this source can build alone: the layers of strings of FMTForest::getThemes
// and getLock, of ages of FMTForest::getAge, and of output values of FMTSpatialSchedule::getSpatialOutput. The layer
// of developments, the base of FMTForest, is exported with FMTForest and is in its suite. The extent is that of a
// raster of 10 by 8 cells of 380 m, of 14.44 ha each, as the maps of TWD_land.

namespace
{
    const std::vector<double> GEOTRANSFORM = { 1000.0, 380.0, 0.0, 5000.0, 0.0, -380.0 };
    const std::string PROJECTION = "LOCAL_CS[\"TWD_land\"]";
    const double CELL_SIZE = 14.44;

    // Three cells: two of A in the first row, one of B in the second.
    Spatial::FMTLayer<std::string> getThemeLayer()
    {
        const std::map<Spatial::FMTCoordinate, std::string> mapping = { { Spatial::FMTCoordinate(1, 0), "A" },
            { Spatial::FMTCoordinate(0, 1), "B" }, { Spatial::FMTCoordinate(0, 0), "A" } };
        return Spatial::FMTLayer<std::string>(mapping, GEOTRANSFORM, 10, 8, PROJECTION, CELL_SIZE);
    }

    std::vector<std::pair<Spatial::FMTCoordinate, std::string>> getCells(const Spatial::FMTLayer<std::string>& p_layer)
    {
        return std::vector<std::pair<Spatial::FMTCoordinate, std::string>>(p_layer.begin(), p_layer.end());
    }

    void expectTheExtent(const Spatial::FMTLayer<int>& p_layer)
    {
        EXPECT_EQ(p_layer.getGeoTransform(), GEOTRANSFORM);
        EXPECT_EQ(p_layer.getXSize(), 10u);
        EXPECT_EQ(p_layer.getYSize(), 8u);
        EXPECT_EQ(p_layer.getProjection(), PROJECTION);
        EXPECT_DOUBLE_EQ(p_layer.getCellSize(), CELL_SIZE);
    }
}

TEST(FMTLayer, GivenNothingWhenDefaultConstructedThenIsAnEmptyLayerWithoutExtent)
{
    // Arrange and Act
    const Spatial::FMTLayer<int> layer;

    // Assert
    EXPECT_TRUE(layer.empty());
    EXPECT_EQ(layer.size(), 0u);
    EXPECT_TRUE(layer.begin() == layer.end());
    EXPECT_TRUE(layer.getGeoTransform().empty());
    EXPECT_EQ(layer.getXSize(), 0u);
    EXPECT_EQ(layer.getYSize(), 0u);
    EXPECT_TRUE(layer.getProjection().empty());
    EXPECT_DOUBLE_EQ(layer.area(), 0.0);
}

TEST(FMTLayer, GivenAnExtentWhenConstructedThenKeepsItWithoutCell)
{
    // Arrange and Act
    const Spatial::FMTLayer<int> layer(GEOTRANSFORM, 10, 8, PROJECTION, CELL_SIZE);

    // Assert
    expectTheExtent(layer);
    EXPECT_TRUE(layer.empty());
}

// The cells are in the order of the coordinates: by row, then by column.
TEST(FMTLayer, GivenAMappingAndAnExtentWhenConstructedThenHoldsTheCellsInTheOrderOfTheirCoordinates)
{
    // Arrange and Act
    const Spatial::FMTLayer<std::string> layer = getThemeLayer();

    // Assert
    EXPECT_EQ(layer.size(), 3u);
    EXPECT_FALSE(layer.empty());
    EXPECT_EQ(getCells(layer), (std::vector<std::pair<Spatial::FMTCoordinate, std::string>>({
        { Spatial::FMTCoordinate(0, 0), "A" }, { Spatial::FMTCoordinate(1, 0), "A" },
        { Spatial::FMTCoordinate(0, 1), "B" } })));
    EXPECT_EQ(layer.getMapping().size(), 3u);
    EXPECT_EQ(layer.at(Spatial::FMTCoordinate(0, 1)), "B");
}

TEST(FMTLayer, GivenACoordinateOutsideTheLayerWhenAtAndFindThenAtRaisesAndFindGivesTheEnd)
{
    // Arrange
    const Spatial::FMTLayer<std::string> layer = getThemeLayer();
    const Spatial::FMTCoordinate outside(9, 7);

    // Act
    const Spatial::FMTLayer<std::string>::const_iterator found = layer.find(outside);

    // Assert
    EXPECT_TRUE(found == layer.end());
    EXPECT_THROW(layer.at(outside), std::out_of_range);
}

TEST(FMTLayer, GivenACoordinateWhenTheAccessOperatorSetsItsValueThenTheLayerHoldsItAndFindsIt)
{
    // Arrange
    Spatial::FMTLayer<int> layer(GEOTRANSFORM, 10, 8, PROJECTION, CELL_SIZE);
    const Spatial::FMTCoordinate cell(4, 2);

    // Act
    layer[cell] = 35;
    ++layer[cell];

    // Assert
    EXPECT_EQ(layer.size(), 1u);
    EXPECT_EQ(layer.at(cell), 36);
    EXPECT_EQ(layer.find(cell)->second, 36);
}

// The area is the number of cells times the size of a cell.
TEST(FMTLayer, GivenALayerOfThreeCellsWhenAreaThenGivesThreeTimesTheSizeOfACell)
{
    // Arrange
    const Spatial::FMTLayer<std::string> layer = getThemeLayer();

    // Act
    const double area = layer.area();

    // Assert
    EXPECT_DOUBLE_EQ(area, 3.0 * 14.44);
}

TEST(FMTLayer, GivenALayerWhenGetAttributesThenGivesEachValueOnceInTheOrderOfTheCells)
{
    // Arrange
    const Spatial::FMTLayer<std::string> layer = getThemeLayer();

    // Act
    const std::vector<std::string> attributes = layer.getAttributes();

    // Assert
    EXPECT_EQ(attributes, std::vector<std::string>({ "A", "B" }));
}

TEST(FMTLayer, GivenALayerWhenCopiedAndAssignedThenTheCopiesHoldTheCellsAndTheExtentApartFromIt)
{
    // Arrange
    const Spatial::FMTLayer<std::string> layer = getThemeLayer();
    Spatial::FMTLayer<std::string> assigned;

    // Act
    Spatial::FMTLayer<std::string> copy(layer);
    assigned = layer;
    copy[Spatial::FMTCoordinate(0, 0)] = "C";

    // Assert
    EXPECT_EQ(copy.at(Spatial::FMTCoordinate(0, 0)), "C");
    EXPECT_EQ(layer.at(Spatial::FMTCoordinate(0, 0)), "A");
    EXPECT_EQ(getCells(assigned), getCells(layer));
    EXPECT_EQ(assigned.getGeoTransform(), GEOTRANSFORM);
    EXPECT_DOUBLE_EQ(assigned.getCellSize(), CELL_SIZE);
}

TEST(FMTLayer, GivenALayerWhenMoveConstructedThenTheNewLayerHoldsItsCellsAndItsExtent)
{
    // Arrange
    Spatial::FMTLayer<std::string> layer = getThemeLayer();

    // Act
    const Spatial::FMTLayer<std::string> moved(std::move(layer));

    // Assert
    EXPECT_EQ(moved.size(), 3u);
    EXPECT_EQ(moved.getProjection(), PROJECTION);
    EXPECT_EQ(moved.getXSize(), 10u);
}

TEST(FMTLayer, GivenTwoLayersWhenSwapThenEachHoldsTheCellsAndTheExtentOfTheOther)
{
    // Arrange
    Spatial::FMTLayer<std::string> layer = getThemeLayer();
    Spatial::FMTLayer<std::string> other({ { Spatial::FMTCoordinate(2, 2), "Z" } }, { 0.0, 1.0, 0.0, 0.0, 0.0, -1.0 }, 3,
        3, "", 1.0);

    // Act
    layer.swap(other);

    // Assert
    EXPECT_EQ(layer.size(), 1u);
    EXPECT_EQ(layer.getXSize(), 3u);
    EXPECT_DOUBLE_EQ(layer.getCellSize(), 1.0);
    EXPECT_EQ(other.size(), 3u);
    EXPECT_EQ(other.getProjection(), PROJECTION);
    EXPECT_EQ(other.getGeoTransform(), GEOTRANSFORM);
}

// copyExtent gives a layer of another type, as FMTForest::getAge and getThemes build theirs from the forest.
TEST(FMTLayer, GivenALayerWhenCopyExtentAndSetExtentFromThenTheOtherLayerGetsItsExtentWithoutItsCells)
{
    // Arrange
    const Spatial::FMTLayer<std::string> layer = getThemeLayer();
    Spatial::FMTLayer<std::string> target({ { Spatial::FMTCoordinate(2, 2), "Z" } }, {}, 0, 0, "", 0.0);

    // Act
    const Spatial::FMTLayer<int> ages = layer.copyExtent<int>();
    target.setExtentFrom(layer);

    // Assert
    expectTheExtent(ages);
    EXPECT_TRUE(ages.empty());
    EXPECT_EQ(target.size(), 1u);
    EXPECT_EQ(target.at(Spatial::FMTCoordinate(2, 2)), "Z");
    EXPECT_EQ(target.getGeoTransform(), GEOTRANSFORM);
    EXPECT_EQ(target.getYSize(), 8u);
}

// The layers of strings join the values of the cells that both have, with a dash, and drop the others; the layers of
// other types are left as they are.
TEST(FMTLayer, GivenTwoLayersOfStringsWhenAddedThenKeepsTheCellsOfBothWithTheirValuesJoined)
{
    // Arrange
    Spatial::FMTLayer<std::string> layer = getThemeLayer();
    const Spatial::FMTLayer<std::string> other({ { Spatial::FMTCoordinate(0, 0), "UTR1" },
        { Spatial::FMTCoordinate(0, 1), "UTR2" }, { Spatial::FMTCoordinate(5, 5), "UTR3" } }, GEOTRANSFORM, 10, 8,
        PROJECTION, CELL_SIZE);
    Spatial::FMTLayer<int> ages({ { Spatial::FMTCoordinate(0, 0), 10 } }, GEOTRANSFORM, 10, 8, PROJECTION, CELL_SIZE);
    const Spatial::FMTLayer<int> otherAges({ { Spatial::FMTCoordinate(0, 0), 5 } }, GEOTRANSFORM, 10, 8, PROJECTION,
        CELL_SIZE);

    // Act
    layer += other;
    ages += otherAges;

    // Assert
    EXPECT_EQ(getCells(layer), (std::vector<std::pair<Spatial::FMTCoordinate, std::string>>({
        { Spatial::FMTCoordinate(0, 0), "A-UTR1" }, { Spatial::FMTCoordinate(0, 1), "B-UTR2" } })));
    EXPECT_EQ(ages.size(), 1u);
    EXPECT_EQ(ages.at(Spatial::FMTCoordinate(0, 0)), 10);
}

// replace takes the values of the cells of a range that the layer has. Read in the code, not tested: for a cell that
// the layer does not have, it writes through the end of its own map, since it compares the cell it finds with the end
// of the range, not with its own end. Nothing in FMT calls it.
TEST(FMTLayer, GivenTheCellsOfAnotherLayerThatTheLayerHasWhenReplaceThenTakesTheirValues)
{
    // Arrange
    Spatial::FMTLayer<double> outputs({ { Spatial::FMTCoordinate(0, 0), 1.0 }, { Spatial::FMTCoordinate(1, 0), 2.0 },
        { Spatial::FMTCoordinate(0, 1), 3.0 } }, GEOTRANSFORM, 10, 8, PROJECTION, CELL_SIZE);
    const Spatial::FMTLayer<double> others({ { Spatial::FMTCoordinate(1, 0), 20.0 },
        { Spatial::FMTCoordinate(0, 1), 30.0 } }, GEOTRANSFORM, 10, 8, PROJECTION, CELL_SIZE);

    // Act
    outputs.replace(others.begin(), others.end());

    // Assert
    EXPECT_EQ(outputs.getMapping(), (std::map<Spatial::FMTCoordinate, double>({ { Spatial::FMTCoordinate(0, 0), 1.0 },
        { Spatial::FMTCoordinate(1, 0), 20.0 }, { Spatial::FMTCoordinate(0, 1), 30.0 } })));
}
