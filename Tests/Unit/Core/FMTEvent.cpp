/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTEvent.h"
#include "FMTCoordinate.h"

#include <gtest/gtest.h>

#include <boost/functional/hash.hpp>
#include <boost/unordered_set.hpp>

#include <array>
#include <cstddef>
#include <cstdint>
#include <set>
#include <string>
#include <utility>
#include <vector>

// An event is a patch of contiguous cells where one action is done at one period: FMTEventContainer builds them as the
// spatially explicit models operate cells, and the spatial constraints (_SIZE, _ADJACENCY) read their sizes and
// distances. Two cells are neighbors when they share a side or a corner (FMTCoordinate::at, ids 0 to 7); the
// perimeter and the borders count the sides only (ids 0 to 3). FMTEvent::within, a member template instantiated in
// Source/FMTEvent.cpp, is not exported (Documentation/Architecture.md, Known Architectural Debt): the suite of
// FMTEventContainer covers it through minimalDistance and evaluateDistance. The coordinates are as in
// Tests/Unit/Core/FMTCoordinate.cpp: x the column, y the row.

namespace
{
    // The event of p_cells, of the action p_action at the period p_period.
    Spatial::FMTEvent getEvent(const std::vector<Spatial::FMTCoordinate>& p_cells, int p_action = 1, int p_period = 2,
        size_t p_family = 0)
    {
        Spatial::FMTEvent event(p_cells.at(0), p_action, p_period, p_family);
        for (const Spatial::FMTCoordinate& cell : p_cells)
        {
            event.insert(cell);
        }
        return event;
    }

    // An L of five cells: down the column 1 from the row 1 to the row 3, then along the row 3 to the column 3.
    std::vector<Spatial::FMTCoordinate> getL()
    {
        return { Spatial::FMTCoordinate(1, 1), Spatial::FMTCoordinate(1, 2), Spatial::FMTCoordinate(1, 3),
            Spatial::FMTCoordinate(2, 3), Spatial::FMTCoordinate(3, 3) };
    }

    // The square of the cells 1 to 3 of the rows 1 to 3.
    std::vector<Spatial::FMTCoordinate> getSquare()
    {
        std::vector<Spatial::FMTCoordinate> cells;
        for (uint16_t y = 1; y <= 3; ++y)
        {
            for (uint16_t x = 1; x <= 3; ++x)
            {
                cells.push_back(Spatial::FMTCoordinate(x, y));
            }
        }
        return cells;
    }

    std::set<Spatial::FMTCoordinate> getCells(const std::vector<std::set<Spatial::FMTCoordinate>::const_iterator>& p_cells)
    {
        std::set<Spatial::FMTCoordinate> cells;
        for (const std::set<Spatial::FMTCoordinate>::const_iterator& cell : p_cells)
        {
            cells.insert(*cell);
        }
        return cells;
    }
}

TEST(FMTEvent, GivenACellAnActionAPeriodAndAFamilyWhenConstructedThenIsAnEventOfThatCell)
{
    // Arrange
    const Spatial::FMTCoordinate cell(2, 3);

    // Act
    const Spatial::FMTEvent event(cell, 1, 4, 7);

    // Assert
    EXPECT_FALSE(event.empty());
    EXPECT_EQ(event.size(), 1u);
    EXPECT_TRUE(event.contain(cell));
    EXPECT_EQ(event.getElements(), std::set<Spatial::FMTCoordinate>({ cell }));
    EXPECT_EQ(event.getActionId(), 1);
    EXPECT_EQ(event.getPeriod(), 4);
    EXPECT_EQ(event.getGraphFamily(), 7u);
}

TEST(FMTEvent, GivenNothingWhenDefaultConstructedThenIsAnEmptyEvent)
{
    // Arrange and Act: the defaulted constructor leaves the action, the period and the family uninitialized, so that a
    // constant event cannot be declared without braces
    Spatial::FMTEvent event;

    // Assert
    EXPECT_TRUE(event.empty());
    EXPECT_EQ(event.size(), 0u);
}

TEST(FMTEvent, GivenAnEventWhenInsertEraseAndMergeCellsThenHoldsEachCellOnce)
{
    // Arrange
    Spatial::FMTEvent event(Spatial::FMTCoordinate(2, 3), 1, 4, 7);
    const Spatial::FMTEvent other = getEvent({ Spatial::FMTCoordinate(5, 5), Spatial::FMTCoordinate(3, 3) });

    // Act
    event.insert(Spatial::FMTCoordinate(3, 3));
    event.insert(Spatial::FMTCoordinate(3, 3));
    event.erase(Spatial::FMTCoordinate(9, 9));
    event.erase(Spatial::FMTCoordinate(2, 3));
    event.merge(other);
    event.setActionId(6);

    // Assert
    EXPECT_EQ(event.getElements(), std::set<Spatial::FMTCoordinate>({ Spatial::FMTCoordinate(3, 3),
        Spatial::FMTCoordinate(5, 5) }));
    EXPECT_FALSE(event.contain(Spatial::FMTCoordinate(2, 3)));
    EXPECT_EQ(event.getActionId(), 6);
    EXPECT_EQ(event.getPeriod(), 4);
}

// The mid position is the cell in the middle of the order of the cells, by row then by column.
TEST(FMTEvent, GivenEventsOfOneThreeAndFourCellsWhenMidPositionThenGivesTheCellInTheMiddleOfTheirOrder)
{
    // Arrange
    const Spatial::FMTEvent one(Spatial::FMTCoordinate(4, 4), 1, 2, 0);
    const Spatial::FMTEvent row = getEvent({ Spatial::FMTCoordinate(0, 0), Spatial::FMTCoordinate(1, 0),
        Spatial::FMTCoordinate(2, 0) });
    const Spatial::FMTEvent square = getEvent({ Spatial::FMTCoordinate(0, 0), Spatial::FMTCoordinate(1, 0),
        Spatial::FMTCoordinate(0, 1), Spatial::FMTCoordinate(1, 1) });

    // Act
    const std::vector<Spatial::FMTCoordinate> mids = { *one.midPosition(), *row.midPosition(), *square.midPosition() };

    // Assert
    EXPECT_EQ(mids, std::vector<Spatial::FMTCoordinate>({ Spatial::FMTCoordinate(4, 4), Spatial::FMTCoordinate(1, 0),
        Spatial::FMTCoordinate(0, 1) }));
}

// The perimeter counts the sides of the cells that touch a cell outside the event, the sides of a hole included.
TEST(FMTEvent, GivenEventsOfDifferentShapesWhenPerimeterThenCountsTheSidesOfTheirCellsOnTheOutside)
{
    // Arrange
    const Spatial::FMTEvent one(Spatial::FMTCoordinate(4, 4), 1, 2, 0);
    const Spatial::FMTEvent pair = getEvent({ Spatial::FMTCoordinate(0, 0), Spatial::FMTCoordinate(1, 0) });
    const Spatial::FMTEvent square = getEvent({ Spatial::FMTCoordinate(0, 0), Spatial::FMTCoordinate(1, 0),
        Spatial::FMTCoordinate(0, 1), Spatial::FMTCoordinate(1, 1) });
    Spatial::FMTEvent ring = getEvent(getSquare());
    ring.erase(Spatial::FMTCoordinate(2, 2));

    // Act
    const std::vector<size_t> perimeters = { one.perimeter(), pair.perimeter(), square.perimeter(), ring.perimeter() };

    // Assert
    EXPECT_EQ(perimeters, std::vector<size_t>({ 4, 6, 8, 16 }));
}

// The envelope holds the borders of the event; the average centroid is the center of the envelope, which the event
// may not hold.
TEST(FMTEvent, GivenAnLShapedEventWhenGetEnveloppeHeightWidthAndAverageCentroidThenDescribeTheRectangleAroundIt)
{
    // Arrange
    const Spatial::FMTEvent event = getEvent(getL());

    // Act
    const std::array<Spatial::FMTCoordinate, 4> enveloppe = event.getEnveloppe();
    const Spatial::FMTCoordinate centroid = event.averageCentroid();

    // Assert
    EXPECT_TRUE(enveloppe == (std::array<Spatial::FMTCoordinate, 4>({ Spatial::FMTCoordinate(1, 1),
        Spatial::FMTCoordinate(3, 1), Spatial::FMTCoordinate(1, 3), Spatial::FMTCoordinate(3, 3) })));
    EXPECT_EQ(event.height(), 3u);
    EXPECT_EQ(event.width(), 3u);
    EXPECT_TRUE(centroid == Spatial::FMTCoordinate(2, 2));
    EXPECT_FALSE(event.contain(centroid));
}

TEST(FMTEvent, GivenAnLShapedEventWhenGetStatsThenWritesItsSizePerimeterHeightAndWidthUnderTheirHeader)
{
    // Arrange
    const Spatial::FMTEvent event = getEvent(getL());

    // Act
    const std::string stats = event.getStats();

    // Assert
    EXPECT_EQ(Spatial::FMTEvent::getStatsHeader(), "Size Perimeter Height Width");
    EXPECT_EQ(stats, "5 12 3 3");
}

// Two events are equal when they have the same period, action and cells; the graph family is not compared.
TEST(FMTEvent, GivenEventsThatDifferByTheirPeriodActionOrCellsWhenComparedThenOnlyThoseOfTheSameAreEqual)
{
    // Arrange
    const Spatial::FMTEvent event = getEvent(getL(), 1, 2, 0);
    const Spatial::FMTEvent otherFamily = getEvent(getL(), 1, 2, 5);
    const Spatial::FMTEvent otherAction = getEvent(getL(), 3, 2, 0);
    const Spatial::FMTEvent otherPeriod = getEvent(getL(), 1, 4, 0);
    const Spatial::FMTEvent mirrored = getEvent({ Spatial::FMTCoordinate(1, 1), Spatial::FMTCoordinate(2, 1),
        Spatial::FMTCoordinate(3, 1), Spatial::FMTCoordinate(3, 2), Spatial::FMTCoordinate(3, 3) }, 1, 2, 0);

    // Act
    const std::vector<bool> equal = { event == otherFamily, event == otherAction, event == otherPeriod,
        event == mirrored };
    const std::vector<bool> different = { event != otherFamily, event != otherAction, event != otherPeriod,
        event != mirrored };

    // Assert
    EXPECT_EQ(equal, std::vector<bool>({ true, false, false, false }));
    EXPECT_EQ(different, std::vector<bool>({ false, true, true, true }));
    EXPECT_TRUE(mirrored.averageCentroid() == event.averageCentroid());
    EXPECT_TRUE(mirrored.getEnveloppe() == event.getEnveloppe());
}

// The order of FMTEventContainer: the period, then the mid position, then the action.
TEST(FMTEvent, GivenEventsWhenOrderedThenThePeriodComesFirstThenTheMidPositionThenTheAction)
{
    // Arrange
    const Spatial::FMTEvent later(Spatial::FMTCoordinate(5, 5), 1, 2, 0);
    const Spatial::FMTEvent earlier(Spatial::FMTCoordinate(9, 9), 0, 1, 0);
    const Spatial::FMTEvent firstAction(Spatial::FMTCoordinate(5, 5), 0, 2, 0);
    const Spatial::FMTEvent furtherRow(Spatial::FMTCoordinate(1, 6), 0, 2, 0);

    // Act
    const std::set<Spatial::FMTEvent> events = { furtherRow, later, earlier, firstAction };

    // Assert
    EXPECT_EQ(std::vector<Spatial::FMTEvent>(events.begin(), events.end()),
        std::vector<Spatial::FMTEvent>({ earlier, firstAction, later, furtherRow }));
    EXPECT_TRUE(earlier < later);
    EXPECT_FALSE(later < earlier);
    EXPECT_FALSE(later < Spatial::FMTEvent(Spatial::FMTCoordinate(5, 5), 1, 2, 3));
}

TEST(FMTEvent, GivenEqualEventsWhenHashedThenTheyHaveTheSameHashAndTheHashOfBoost)
{
    // Arrange
    const Spatial::FMTEvent event = getEvent(getL(), 1, 2, 0);
    const Spatial::FMTEvent same = getEvent(getL(), 1, 2, 5);
    const Spatial::FMTEvent otherAction = getEvent(getL(), 3, 2, 0);

    // Act
    const boost::unordered_set<Spatial::FMTEvent> events = { event, same, otherAction };

    // Assert
    EXPECT_EQ(event.hash(), same.hash());
    EXPECT_EQ(boost::hash<Spatial::FMTEvent>()(event), event.hash());
    EXPECT_EQ(events.size(), 2u);
}

// The distance is that of the closest borders of the two events.
TEST(FMTEvent, GivenTwoEventsWhenDistanceAndGetClosesCoordinatesThenGivesTheirClosestBordersAndTheirDistance)
{
    // Arrange
    const Spatial::FMTEvent event = getEvent({ Spatial::FMTCoordinate(0, 0), Spatial::FMTCoordinate(1, 0) });
    const Spatial::FMTEvent other = getEvent({ Spatial::FMTCoordinate(4, 0), Spatial::FMTCoordinate(5, 0) });
    const Spatial::FMTEvent diagonal(Spatial::FMTCoordinate(4, 4), 1, 2, 0);
    std::set<Spatial::FMTCoordinate>::const_iterator eventCell;
    std::set<Spatial::FMTCoordinate>::const_iterator otherCell;

    // Act
    const double distance = event.distance(other);
    const double diagonalDistance = Spatial::FMTEvent(Spatial::FMTCoordinate(1, 0), 1, 2, 0).distance(diagonal);
    event.getClosesCoordinates(other, eventCell, otherCell);

    // Assert
    EXPECT_DOUBLE_EQ(distance, 3.0);
    EXPECT_DOUBLE_EQ(diagonalDistance, 5.0);
    EXPECT_TRUE(*eventCell == Spatial::FMTCoordinate(1, 0));
    EXPECT_TRUE(*otherCell == Spatial::FMTCoordinate(4, 0));
}

TEST(FMTEvent, GivenASquareWhenGetBordersThenGivesEveryCellButTheCenter)
{
    // Arrange
    const Spatial::FMTEvent square = getEvent(getSquare());

    // Act
    const std::set<Spatial::FMTCoordinate> borders = getCells(square.getBorders());

    // Assert
    EXPECT_EQ(borders.size(), 8u);
    EXPECT_EQ(borders.count(Spatial::FMTCoordinate(2, 2)), 0u);
}

// Each cell outside that touches a side of the event comes once, with the first cell of the event it touches.
TEST(FMTEvent, GivenTwoCellsThatShareACornerWhenGetOutsideBordersPairThenGivesEachOutsideCellOnce)
{
    // Arrange
    const Spatial::FMTEvent event = getEvent({ Spatial::FMTCoordinate(1, 1), Spatial::FMTCoordinate(2, 2) });
    const Spatial::FMTCoordinate first(1, 1);
    const Spatial::FMTCoordinate second(2, 2);

    // Act
    const std::vector<std::pair<Spatial::FMTCoordinate, Spatial::FMTCoordinate>> pairs = event.getOutsideBordersPair();

    // Assert
    EXPECT_EQ(pairs, (std::vector<std::pair<Spatial::FMTCoordinate, Spatial::FMTCoordinate>>({
        { first, Spatial::FMTCoordinate(1, 0) }, { first, Spatial::FMTCoordinate(2, 1) },
        { first, Spatial::FMTCoordinate(1, 2) }, { first, Spatial::FMTCoordinate(0, 1) },
        { second, Spatial::FMTCoordinate(3, 2) }, { second, Spatial::FMTCoordinate(2, 3) } })));
}

TEST(FMTEvent, GivenASquareWhenGetTerritoryThenGivesTheCornersOfItsEnveloppeMovedAwayByTheDistance)
{
    // Arrange
    const Spatial::FMTEvent square = getEvent(getSquare());

    // Act
    const std::set<Spatial::FMTCoordinate> territory = square.getTerritory(1);

    // Assert
    EXPECT_EQ(territory, std::set<Spatial::FMTCoordinate>({ Spatial::FMTCoordinate(0, 0), Spatial::FMTCoordinate(4, 0),
        Spatial::FMTCoordinate(0, 4), Spatial::FMTCoordinate(4, 4) }));
}

// The cells that remain must touch each other by a side or a corner; an event of two cells or less never splits.
TEST(FMTEvent, GivenEventsWhenWillSplitEventWithoutACellThenSplitsOnlyWhenTheOthersNoLongerTouch)
{
    // Arrange
    const Spatial::FMTEvent row = getEvent({ Spatial::FMTCoordinate(0, 0), Spatial::FMTCoordinate(1, 0),
        Spatial::FMTCoordinate(2, 0) });
    const Spatial::FMTEvent diagonal = getEvent({ Spatial::FMTCoordinate(0, 0), Spatial::FMTCoordinate(1, 1),
        Spatial::FMTCoordinate(2, 2) });
    const Spatial::FMTEvent pair = getEvent({ Spatial::FMTCoordinate(0, 0), Spatial::FMTCoordinate(2, 0) });
    const Spatial::FMTEvent square = getEvent(getSquare());

    // Act
    const std::vector<bool> splits = { row.willSplitEvent(Spatial::FMTCoordinate(1, 0)),
        row.willSplitEvent(Spatial::FMTCoordinate(2, 0)), diagonal.willSplitEvent(Spatial::FMTCoordinate(1, 1)),
        pair.willSplitEvent(Spatial::FMTCoordinate(9, 9)), square.willSplitEvent(Spatial::FMTCoordinate(2, 2)),
        square.willSplitEvent(Spatial::FMTCoordinate(9, 9)) };

    // Assert
    EXPECT_EQ(splits, std::vector<bool>({ true, false, true, false, false, false }));
}

// splitEvent gives the parts of cells that touch each other, with the action, the period and the family of the event;
// the event itself is left as it is, unlike what its documentation says.
TEST(FMTEvent, GivenAnEventOfTwoPartsWhenSplitEventThenGivesThePartsAndKeepsTheEvent)
{
    // Arrange
    const Spatial::FMTEvent event = getEvent({ Spatial::FMTCoordinate(0, 0), Spatial::FMTCoordinate(1, 0),
        Spatial::FMTCoordinate(5, 5) }, 3, 4, 6);
    const Spatial::FMTEvent whole = getEvent(getSquare());
    std::vector<Spatial::FMTEvent> parts;
    std::vector<Spatial::FMTEvent> wholeParts;

    // Act
    const bool split = event.splitEvent(parts);
    const bool wholeSplit = whole.splitEvent(wholeParts);

    // Assert
    EXPECT_TRUE(split);
    ASSERT_EQ(parts.size(), 2u);
    EXPECT_EQ(parts.at(0).getElements(), std::set<Spatial::FMTCoordinate>({ Spatial::FMTCoordinate(0, 0),
        Spatial::FMTCoordinate(1, 0) }));
    EXPECT_EQ(parts.at(1).getElements(), std::set<Spatial::FMTCoordinate>({ Spatial::FMTCoordinate(5, 5) }));
    EXPECT_EQ(parts.at(1).getActionId(), 3);
    EXPECT_EQ(parts.at(1).getPeriod(), 4);
    EXPECT_EQ(parts.at(1).getGraphFamily(), 6u);
    EXPECT_EQ(event.size(), 3u);
    EXPECT_FALSE(wholeSplit);
    ASSERT_EQ(wholeParts.size(), 1u);
    EXPECT_EQ(wholeParts.at(0).size(), 9u);
}

// ignit gives the cell where the event starts and sets its action, its period and its family, without inserting the
// cell: spread does.
TEST(FMTEvent, GivenACellOfATerritoryWhenIgnitThenGivesItAsTheActiveCellAndSetsTheEventWithoutTheCell)
{
    // Arrange
    const std::set<Spatial::FMTCoordinate> territory = { Spatial::FMTCoordinate(0, 0), Spatial::FMTCoordinate(1, 0) };
    Spatial::FMTEvent event;
    Spatial::FMTEvent tooSmall;

    // Act
    const std::vector<std::set<Spatial::FMTCoordinate>::const_iterator> actives =
        event.ignit(5, territory.begin(), 2, 3, 4);
    const std::vector<std::set<Spatial::FMTCoordinate>::const_iterator> noActive =
        tooSmall.ignit(0, territory.begin(), 2, 3, 4);

    // Assert
    ASSERT_EQ(actives.size(), 1u);
    EXPECT_TRUE(*actives.at(0) == Spatial::FMTCoordinate(0, 0));
    EXPECT_EQ(event.getActionId(), 2);
    EXPECT_EQ(event.getPeriod(), 3);
    EXPECT_EQ(event.getGraphFamily(), 4u);
    EXPECT_TRUE(event.empty());
    EXPECT_TRUE(noActive.empty());
}

// spread takes the cells of the territory that share a side with the cells taken, from the active cell, up to the
// maximal size; below the minimal size, the event is emptied.
TEST(FMTEvent, GivenATerritoryWhenSpreadThenTakesItsTouchingCellsUpToTheMaximalSizeOrEmptiesTheEventBelowTheMinimalSize)
{
    // Arrange
    const std::set<Spatial::FMTCoordinate> territory = { Spatial::FMTCoordinate(0, 0), Spatial::FMTCoordinate(1, 0),
        Spatial::FMTCoordinate(2, 0), Spatial::FMTCoordinate(5, 5) };
    Spatial::FMTEvent whole;
    Spatial::FMTEvent bounded;
    Spatial::FMTEvent tooSmall;
    const std::vector<std::set<Spatial::FMTCoordinate>::const_iterator> actives = whole.ignit(10, territory.begin(), 2, 3, 4);
    bounded.ignit(10, territory.begin(), 2, 3, 4);
    tooSmall.ignit(10, territory.begin(), 2, 3, 4);

    // Act
    const bool wholeSpread = whole.spread(1, 10, 4, territory, actives);
    const bool boundedSpread = bounded.spread(1, 2, 4, territory, actives);
    const bool tooSmallSpread = tooSmall.spread(4, 10, 4, territory, actives);

    // Assert
    EXPECT_TRUE(wholeSpread);
    EXPECT_EQ(whole.getElements(), std::set<Spatial::FMTCoordinate>({ Spatial::FMTCoordinate(0, 0),
        Spatial::FMTCoordinate(1, 0), Spatial::FMTCoordinate(2, 0) }));
    EXPECT_TRUE(boundedSpread);
    EXPECT_EQ(bounded.getElements(), std::set<Spatial::FMTCoordinate>({ Spatial::FMTCoordinate(0, 0),
        Spatial::FMTCoordinate(1, 0) }));
    EXPECT_FALSE(tooSmallSpread);
    EXPECT_TRUE(tooSmall.empty());
}
