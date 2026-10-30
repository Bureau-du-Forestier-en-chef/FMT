/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTCoordinate.h"

#include <gtest/gtest.h>

#include <boost/functional/hash.hpp>
#include <boost/unordered_set.hpp>

#include <array>
#include <cstdint>
#include <limits>
#include <set>
#include <string>
#include <vector>

// A coordinate is the column x and the row y of a cell of a raster, as FMTAreaParser reads them: x grows to the east and
// y to the south, from the cell 0 0 at the top left. The coordinates are ordered by row, then by column. An envelope
// holds the four corners of a rectangle, the top left first, then the top right, the bottom left and the bottom right.
// FMTCoordinate::within, a member template instantiated in Source/FMTCoordinate.cpp, is not exported
// (Documentation/Architecture.md, Known Architectural Debt): FMTEventContainer uses it, and its suite covers it.

namespace
{
    std::vector<Spatial::FMTCoordinate> getNeighbors(const Spatial::FMTCoordinate& p_coordinate, unsigned int p_first,
        unsigned int p_last)
    {
        std::vector<Spatial::FMTCoordinate> neighbors;
        for (unsigned int id = p_first; id <= p_last; ++id)
        {
            neighbors.push_back(p_coordinate.at(id));
        }
        return neighbors;
    }

    // The envelope of the rectangle from 2 3 to 8 7.
    std::array<Spatial::FMTCoordinate, 4> getEnveloppe()
    {
        return { Spatial::FMTCoordinate(2, 3), Spatial::FMTCoordinate(8, 3), Spatial::FMTCoordinate(2, 7),
            Spatial::FMTCoordinate(8, 7) };
    }
}

TEST(FMTCoordinate, GivenNothingWhenDefaultConstructedThenIsTheFirstCellOfTheRaster)
{
    // Arrange and Act
    const Spatial::FMTCoordinate coordinate;

    // Assert
    EXPECT_EQ(coordinate.getX(), 0);
    EXPECT_EQ(coordinate.getY(), 0);
}

TEST(FMTCoordinate, GivenAColumnAndARowWhenConstructedCopiedAndAssignedThenKeepsThemAndWritesThem)
{
    // Arrange
    const Spatial::FMTCoordinate coordinate(3, 5);
    Spatial::FMTCoordinate assigned;

    // Act
    const Spatial::FMTCoordinate copy(coordinate);
    assigned = coordinate;

    // Assert
    EXPECT_EQ(coordinate.getX(), 3);
    EXPECT_EQ(coordinate.getY(), 5);
    EXPECT_EQ(std::string(coordinate), "X3 Y5");
    EXPECT_TRUE(copy == coordinate);
    EXPECT_TRUE(assigned == coordinate);
}

// The neighbors 0 to 3 share a side, top, right, bottom and left; 4 to 7 share a corner, top right, bottom right,
// bottom left and top left; from 8, the same directions again, one cell further for each turn.
TEST(FMTCoordinate, GivenACoordinateWhenAtTheIdsOfItsNeighborsThenGivesThemSidesFirstThenCornersThenFurther)
{
    // Arrange
    const Spatial::FMTCoordinate coordinate(5, 5);

    // Act
    const std::vector<Spatial::FMTCoordinate> neighbors = getNeighbors(coordinate, 0, 9);
    const Spatial::FMTCoordinate furtherTopLeft = coordinate.at(15);

    // Assert
    EXPECT_EQ(neighbors, std::vector<Spatial::FMTCoordinate>({ Spatial::FMTCoordinate(5, 4), Spatial::FMTCoordinate(6, 5),
        Spatial::FMTCoordinate(5, 6), Spatial::FMTCoordinate(4, 5), Spatial::FMTCoordinate(6, 4),
        Spatial::FMTCoordinate(6, 6), Spatial::FMTCoordinate(4, 6), Spatial::FMTCoordinate(4, 4),
        Spatial::FMTCoordinate(5, 3), Spatial::FMTCoordinate(7, 5) }));
    EXPECT_TRUE(furtherTopLeft == Spatial::FMTCoordinate(3, 3));
}

// A neighbor outside the raster wraps around to the largest coordinate, which no layer of FMT holds: the events count
// it as a cell outside them (FMTEvent::perimeter).
TEST(FMTCoordinate, GivenTheFirstCellOfTheRasterWhenAtItsTopAndLeftNeighborsThenTheyWrapAroundToTheLargestCoordinate)
{
    // Arrange
    const Spatial::FMTCoordinate coordinate(0, 0);
    const uint16_t LARGEST = (std::numeric_limits<uint16_t>::max)();

    // Act
    const Spatial::FMTCoordinate top = coordinate.at(0);
    const Spatial::FMTCoordinate left = coordinate.at(3);

    // Assert
    EXPECT_TRUE(top == Spatial::FMTCoordinate(0, LARGEST));
    EXPECT_TRUE(left == Spatial::FMTCoordinate(LARGEST, 0));
}

TEST(FMTCoordinate, GivenTwoCoordinatesWhenDistanceThenGivesTheEuclideanDistanceInCells)
{
    // Arrange
    const Spatial::FMTCoordinate origin(0, 0);
    const Spatial::FMTCoordinate other(3, 4);

    // Act
    const double distance = origin.distance(other);
    const double reverse = other.distance(origin);
    const double itself = other.distance(other);

    // Assert
    EXPECT_DOUBLE_EQ(distance, 5.0);
    EXPECT_DOUBLE_EQ(reverse, 5.0);
    EXPECT_DOUBLE_EQ(itself, 0.0);
}

TEST(FMTCoordinate, GivenCoordinatesWhenOrderedThenTheRowComesFirstThenTheColumnAndTheOrderAgreesWithEquality)
{
    // Arrange
    const Spatial::FMTCoordinate firstRow(5, 1);
    const Spatial::FMTCoordinate secondRow(1, 2);
    const Spatial::FMTCoordinate secondRowFurther(2, 2);

    // Act
    const std::set<Spatial::FMTCoordinate> coordinates = { secondRowFurther, firstRow, secondRow,
        Spatial::FMTCoordinate(1, 2) };

    // Assert
    EXPECT_EQ(std::vector<Spatial::FMTCoordinate>(coordinates.begin(), coordinates.end()),
        std::vector<Spatial::FMTCoordinate>({ firstRow, secondRow, secondRowFurther }));
    EXPECT_TRUE(firstRow < secondRow);
    EXPECT_FALSE(secondRow < firstRow);
    EXPECT_FALSE(secondRow < Spatial::FMTCoordinate(1, 2));
    EXPECT_FALSE(secondRow == secondRowFurther);
}

TEST(FMTCoordinate, GivenCoordinatesWhenHashedThenEqualCoordinatesHaveTheHashOfBoost)
{
    // Arrange
    const Spatial::FMTCoordinate coordinate(1, 2);
    const Spatial::FMTCoordinate same(1, 2);
    const Spatial::FMTCoordinate swapped(2, 1);

    // Act
    const boost::unordered_set<Spatial::FMTCoordinate> coordinates = { coordinate, same, swapped };

    // Assert
    EXPECT_EQ(coordinate.hash(), same.hash());
    EXPECT_EQ(boost::hash<Spatial::FMTCoordinate>()(coordinate), coordinate.hash());
    EXPECT_NE(coordinate.hash(), swapped.hash());
    EXPECT_EQ(coordinates.size(), 2u);
}

TEST(FMTCoordinate, GivenAnEnveloppeWhenUpEnveloppeWithCoordinatesThenGrowsToHoldThem)
{
    // Arrange
    const Spatial::FMTCoordinate start(5, 5);
    std::array<Spatial::FMTCoordinate, 4> enveloppe = { start, start, start, start };

    // Act
    Spatial::FMTCoordinate(2, 7).upEnveloppe(enveloppe);
    Spatial::FMTCoordinate(8, 3).upEnveloppe(enveloppe);
    Spatial::FMTCoordinate(4, 4).upEnveloppe(enveloppe);

    // Assert
    EXPECT_TRUE(enveloppe == getEnveloppe());
}

TEST(FMTCoordinate, GivenAnEnveloppeWhenGetHeightWidthAndAverageCentroidThenCountsItsCellsAndGivesItsCenter)
{
    // Arrange
    const std::array<Spatial::FMTCoordinate, 4> enveloppe = getEnveloppe();
    const Spatial::FMTCoordinate cell(4, 4);
    const std::array<Spatial::FMTCoordinate, 4> oneCell = { cell, cell, cell, cell };

    // Act
    const size_t height = Spatial::FMTCoordinate::getHeight(enveloppe);
    const size_t width = Spatial::FMTCoordinate::getWidth(enveloppe);
    const Spatial::FMTCoordinate centroid = Spatial::FMTCoordinate::getAverageCentroid(enveloppe);

    // Assert
    EXPECT_EQ(height, 5u);
    EXPECT_EQ(width, 7u);
    EXPECT_TRUE(centroid == Spatial::FMTCoordinate(5, 5));
    EXPECT_EQ(Spatial::FMTCoordinate::getHeight(oneCell), 1u);
    EXPECT_EQ(Spatial::FMTCoordinate::getWidth(oneCell), 1u);
    EXPECT_TRUE(Spatial::FMTCoordinate::getAverageCentroid(oneCell) == cell);
}

// The territory is the four corners of the envelope moved away by the distance; a corner beyond the first row or column
// stops at 0.
TEST(FMTCoordinate, GivenAnEnveloppeWhenGetTerritoryThenGivesItsCornersMovedAwayAndStoppedAtTheEdgeOfTheRaster)
{
    // Arrange
    const std::array<Spatial::FMTCoordinate, 4> enveloppe = getEnveloppe();

    // Act
    const std::set<Spatial::FMTCoordinate> territory = Spatial::FMTCoordinate::getTerritory(enveloppe, 2);
    const std::set<Spatial::FMTCoordinate> atTheEdge = Spatial::FMTCoordinate::getTerritory(enveloppe, 5);

    // Assert
    EXPECT_EQ(territory, std::set<Spatial::FMTCoordinate>({ Spatial::FMTCoordinate(0, 1), Spatial::FMTCoordinate(10, 1),
        Spatial::FMTCoordinate(0, 9), Spatial::FMTCoordinate(10, 9) }));
    EXPECT_EQ(atTheEdge, std::set<Spatial::FMTCoordinate>({ Spatial::FMTCoordinate(0, 0), Spatial::FMTCoordinate(13, 0),
        Spatial::FMTCoordinate(0, 12), Spatial::FMTCoordinate(13, 12) }));
}

// As GDAL defines its geotransform: the origin, the size of a cell along x, the rotation, then the same for y; the
// coordinate is the center of the cell.
TEST(FMTCoordinate, GivenTheGeotransformOfARasterWhenGetSpatialCoordinateThenGivesTheCenterOfTheCell)
{
    // Arrange
    const Spatial::FMTCoordinate coordinate(2, 3);
    const std::vector<double> geotransform = { 1000.0, 20.0, 0.0, 5000.0, 0.0, -20.0 };

    // Act
    const std::vector<double> spatial = coordinate.getSpatialCoordinate(geotransform);

    // Assert
    EXPECT_EQ(spatial, std::vector<double>({ 1050.0, 4930.0 }));
}

// The approximation is the squared distance: closest compares the squares, distance takes their root.
TEST(FMTCoordinate, GivenCoordinatesWhenClosestThenGivesTheNearestAndItsSquaredDistance)
{
    // Arrange
    const std::set<Spatial::FMTCoordinate> coordinates = { Spatial::FMTCoordinate(0, 0), Spatial::FMTCoordinate(5, 5),
        Spatial::FMTCoordinate(2, 1) };
    std::vector<std::set<Spatial::FMTCoordinate>::const_iterator> candidates;
    for (std::set<Spatial::FMTCoordinate>::const_iterator it = coordinates.begin(); it != coordinates.end(); ++it)
    {
        candidates.push_back(it);
    }
    const Spatial::FMTCoordinate from(3, 3);
    double approximation = 0.0;
    double noApproximation = 0.0;

    // Act
    const std::set<Spatial::FMTCoordinate>::const_iterator closest = from.closest(candidates, approximation);
    from.closest({}, noApproximation);

    // Assert
    EXPECT_TRUE(*closest == Spatial::FMTCoordinate(2, 1));
    EXPECT_DOUBLE_EQ(approximation, 5.0);
    EXPECT_EQ(noApproximation, std::numeric_limits<double>::infinity());
}

// Observation, no defect announced: getNeighbors means to leave out the coordinate itself, but compares its column to
// its row (lx == m_x && lx == m_y): a coordinate off the diagonal keeps itself, and one on it, here 5 5, loses its whole
// column. Nothing in FMT calls it, and neither Python nor R exposes it.
TEST(FMTCoordinate, GivenASquareNeighborhoodWhenGetNeighborsThenKeepsTheCoordinateOffTheDiagonalAndLosesTheColumnOnIt)
{
    // Arrange
    const Spatial::FMTCoordinate offDiagonal(5, 7);
    const Spatial::FMTCoordinate onDiagonal(5, 5);

    // Act
    const std::set<Spatial::FMTCoordinate> offNeighbors = offDiagonal.getNeighbors(3);
    const std::set<Spatial::FMTCoordinate> onNeighbors = onDiagonal.getNeighbors(3);

    // Assert
    EXPECT_EQ(offNeighbors.size(), 9u);
    EXPECT_EQ(offNeighbors.count(offDiagonal), 1u);
    EXPECT_EQ(onNeighbors, std::set<Spatial::FMTCoordinate>({ Spatial::FMTCoordinate(4, 4), Spatial::FMTCoordinate(6, 4),
        Spatial::FMTCoordinate(4, 5), Spatial::FMTCoordinate(6, 5), Spatial::FMTCoordinate(4, 6),
        Spatial::FMTCoordinate(6, 6) }));
}

// The neighborhood stops at the first row and column; the circle keeps, on each row, the columns within its radius.
TEST(FMTCoordinate, GivenANeighborhoodAtTheEdgeOrACircleWhenGetNeighborsThenKeepsTheCellsInsideTheRasterAndTheRadius)
{
    // Arrange
    const Spatial::FMTCoordinate atTheEdge(0, 1);
    const Spatial::FMTCoordinate center(5, 7);

    // Act
    const std::set<Spatial::FMTCoordinate> edgeNeighbors = atTheEdge.getNeighbors(3);
    const std::set<Spatial::FMTCoordinate> circle = center.getNeighbors(5, true);

    // Assert
    EXPECT_EQ(edgeNeighbors, std::set<Spatial::FMTCoordinate>({ Spatial::FMTCoordinate(0, 0), Spatial::FMTCoordinate(1, 0),
        Spatial::FMTCoordinate(0, 1), Spatial::FMTCoordinate(1, 1), Spatial::FMTCoordinate(0, 2),
        Spatial::FMTCoordinate(1, 2) }));
    EXPECT_EQ(circle.size(), 13u);
    EXPECT_EQ(circle.count(Spatial::FMTCoordinate(5, 5)), 1u);
    EXPECT_EQ(circle.count(Spatial::FMTCoordinate(3, 7)), 1u);
    EXPECT_EQ(circle.count(Spatial::FMTCoordinate(4, 5)), 0u);
    EXPECT_EQ(circle.count(Spatial::FMTCoordinate(6, 8)), 1u);
}
