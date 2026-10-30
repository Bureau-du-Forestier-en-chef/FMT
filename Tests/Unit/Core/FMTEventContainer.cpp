/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTEventContainer.h"
#include "FMTBindingSpatialAction.h"
#include "FMTCoordinate.h"
#include "FMTEvent.h"
#include "FMTEventRelation.h"

#include <gtest/gtest.h>

#include <boost/unordered_set.hpp>

#include <limits>
#include <set>
#include <utility>
#include <vector>

// The container of the events of a spatially explicit model: FMTSpatialSchedule adds the cells it operates
// (addAction, addActions), removes them (eraseCoordinate, or getEventsToErase then addUpdate), and reads the events of
// a period and of actions for the spatial constraints. The events are ordered by period, mid position and action
// (FMTEvent::operator<): the searches by coordinates take the events whose mid position is between two coordinates in
// that order, by row then by column, which holds every event of the rows between them. The rules of the actions,
// Spatial::FMTBindingSpatialAction, are not exported: only the binding of its default constructor, defined in its
// header, can be built, and its values are 0, which makes one event of each cell. getDispersion and
// getDispersionConflicts read the spatial graphs of a model, which a unit test does not build.

namespace
{
    // The event of p_cells, of the action p_action at the period p_period.
    Spatial::FMTEvent getEvent(const std::vector<Spatial::FMTCoordinate>& p_cells, int p_action, int p_period)
    {
        Spatial::FMTEvent event(p_cells.at(0), p_action, p_period, 0);
        for (const Spatial::FMTCoordinate& cell : p_cells)
        {
            event.insert(cell);
        }
        return event;
    }

    // At the period 1, a row of three cells and a cell of the action 0, a cell of the action 1; at the period 2, a
    // pair of cells of the action 0. Their mid positions: 1 0, 7 2, 5 5 and 9 8.
    const Spatial::FMTEvent ROW = getEvent({ Spatial::FMTCoordinate(0, 0), Spatial::FMTCoordinate(1, 0),
        Spatial::FMTCoordinate(2, 0) }, 0, 1);
    const Spatial::FMTEvent CELL = getEvent({ Spatial::FMTCoordinate(7, 2) }, 0, 1);
    const Spatial::FMTEvent OTHER_ACTION = getEvent({ Spatial::FMTCoordinate(5, 5) }, 1, 1);
    const Spatial::FMTEvent LATER = getEvent({ Spatial::FMTCoordinate(8, 8), Spatial::FMTCoordinate(9, 8) }, 0, 2);

    Spatial::FMTEventContainer getContainer()
    {
        Spatial::FMTEventContainer container;
        container.insert(LATER);
        container.insert(OTHER_ACTION);
        container.insert(ROW);
        container.insert(CELL);
        return container;
    }

    std::vector<Spatial::FMTEvent> getEvents(const Spatial::FMTEventContainer& p_container)
    {
        return std::vector<Spatial::FMTEvent>(p_container.begin(), p_container.end());
    }

    std::vector<Spatial::FMTEvent> getEvents(const std::vector<Spatial::FMTEventContainer::const_iterator>& p_events)
    {
        std::vector<Spatial::FMTEvent> events;
        for (const Spatial::FMTEventContainer::const_iterator& event : p_events)
        {
            events.push_back(*event);
        }
        return events;
    }

    std::vector<Spatial::FMTEvent> getEvents(
        const std::pair<Spatial::FMTEventContainer::const_iterator, Spatial::FMTEventContainer::const_iterator>& p_bounds)
    {
        return std::vector<Spatial::FMTEvent>(p_bounds.first, p_bounds.second);
    }

    // The cells of each event, in the order of the container.
    std::vector<std::set<Spatial::FMTCoordinate>> getCells(const Spatial::FMTEventContainer& p_container)
    {
        std::vector<std::set<Spatial::FMTCoordinate>> cells;
        for (const Spatial::FMTEvent& event : p_container)
        {
            cells.push_back(event.getElements());
        }
        return cells;
    }

    // A row of three cells and a cell, of the action 0 at the period 1.
    Spatial::FMTEventContainer getRowAndCell()
    {
        Spatial::FMTEventContainer container;
        container.insert(ROW);
        container.insert(getEvent({ Spatial::FMTCoordinate(5, 5) }, 0, 1));
        return container;
    }

    // Two cells of the action 0 at the period 1, 5 cells apart, and a third far from both.
    Spatial::FMTEventContainer getDistantCells()
    {
        Spatial::FMTEventContainer container;
        container.insert(getEvent({ Spatial::FMTCoordinate(0, 0) }, 0, 1));
        container.insert(getEvent({ Spatial::FMTCoordinate(3, 4) }, 0, 1));
        container.insert(getEvent({ Spatial::FMTCoordinate(20, 20) }, 0, 1));
        return container;
    }

    const Spatial::FMTBindingSpatialAction NO_BINDING = Spatial::FMTBindingSpatialAction();
}

TEST(FMTEventContainer, GivenNothingWhenDefaultConstructedThenHoldsNoEvent)
{
    // Arrange and Act
    const Spatial::FMTEventContainer container;

    // Assert
    EXPECT_TRUE(container.empty());
    EXPECT_EQ(container.size(), 0u);
    EXPECT_TRUE(container.begin() == container.end());
}

TEST(FMTEventContainer, GivenEventsWhenInsertedThenHoldsThemByPeriodThenMidPositionThenAction)
{
    // Arrange and Act
    const Spatial::FMTEventContainer container = getContainer();

    // Assert
    EXPECT_EQ(container.size(), 4u);
    EXPECT_EQ(getEvents(container), std::vector<Spatial::FMTEvent>({ ROW, CELL, OTHER_ACTION, LATER }));
    EXPECT_EQ(container.firstPeriod(), 1);
    EXPECT_EQ(container.lastPeriod(), 2);
}

TEST(FMTEventContainer, GivenEventsWhenFindEraseAndClearThenFindsThemRemovesThemAndEmptiesTheContainer)
{
    // Arrange
    Spatial::FMTEventContainer container = getContainer();
    Spatial::FMTEventContainer cleared = getContainer();

    // Act
    const bool foundCell = (container.find(CELL) != container.end());
    container.erase(CELL);
    const bool foundAfterErase = (container.find(CELL) != container.end());
    const Spatial::FMTEventContainer::const_iterator next = container.erase(container.find(ROW));
    cleared.clear();

    // Assert
    EXPECT_TRUE(foundCell);
    EXPECT_FALSE(foundAfterErase);
    EXPECT_TRUE(*next == OTHER_ACTION);
    EXPECT_EQ(getEvents(container), std::vector<Spatial::FMTEvent>({ OTHER_ACTION, LATER }));
    EXPECT_TRUE(cleared.empty());
}

TEST(FMTEventContainer, GivenContainersWhenCopiedComparedSwappedAndMergedThenHoldTheExpectedEvents)
{
    // Arrange
    const Spatial::FMTEventContainer container = getContainer();
    Spatial::FMTEventContainer first;
    first.insert(ROW);
    Spatial::FMTEventContainer second;
    second.insert(LATER);
    second.insert(CELL);
    Spatial::FMTEventContainer merged;
    merged.insert(ROW);
    merged.insert(CELL);

    // Act
    const Spatial::FMTEventContainer copy(container);
    first.swap(second);
    merged.merge(getContainer());

    // Assert
    EXPECT_TRUE(copy == container);
    EXPECT_FALSE(first == container);
    EXPECT_EQ(getEvents(first), std::vector<Spatial::FMTEvent>({ CELL, LATER }));
    EXPECT_EQ(getEvents(second), std::vector<Spatial::FMTEvent>({ ROW }));
    EXPECT_TRUE(merged == container);
}

TEST(FMTEventContainer, GivenEventsWhenGetBoundsOfAPeriodThenGivesTheEventsOfThatPeriodOnly)
{
    // Arrange
    const Spatial::FMTEventContainer container = getContainer();

    // Act
    const std::vector<Spatial::FMTEvent> first = getEvents(container.getBounds(1));
    const std::vector<Spatial::FMTEvent> second = getEvents(container.getBounds(2));
    const std::vector<Spatial::FMTEvent> none = getEvents(container.getBounds(3));

    // Assert
    EXPECT_EQ(first, std::vector<Spatial::FMTEvent>({ ROW, CELL, OTHER_ACTION }));
    EXPECT_EQ(second, std::vector<Spatial::FMTEvent>({ LATER }));
    EXPECT_TRUE(none.empty());
}

TEST(FMTEventContainer, GivenEventsWhenGetEventsOfAPeriodAndOfActionsThenGivesTheEventsOfTheseActionsAtThatPeriod)
{
    // Arrange
    const Spatial::FMTEventContainer container = getContainer();

    // Act
    const std::vector<Spatial::FMTEvent> action0 = getEvents(container.getEvents(1, 0));
    const std::vector<Spatial::FMTEvent> action1 = getEvents(container.getEvents(1, 1));
    const std::vector<Spatial::FMTEvent> laterAction0 = getEvents(container.getEvents(2, 0));
    const std::vector<Spatial::FMTEvent> byIds = getEvents(container.getEvents(1, std::vector<int>({ 0, 1 })));
    const std::vector<Spatial::FMTEvent> used = getEvents(container.getEvents(1, std::vector<bool>({ false, true })));
    const std::vector<Spatial::FMTEvent> rule = getEvents(container.getEventsOf(0, 1, 2));

    // Assert
    EXPECT_EQ(action0, std::vector<Spatial::FMTEvent>({ ROW, CELL }));
    EXPECT_EQ(action1, std::vector<Spatial::FMTEvent>({ OTHER_ACTION }));
    EXPECT_EQ(laterAction0, std::vector<Spatial::FMTEvent>({ LATER }));
    EXPECT_EQ(byIds, std::vector<Spatial::FMTEvent>({ ROW, CELL, OTHER_ACTION }));
    EXPECT_EQ(used, std::vector<Spatial::FMTEvent>({ OTHER_ACTION }));
    EXPECT_EQ(rule, std::vector<Spatial::FMTEvent>({ ROW, CELL, LATER }));
}

TEST(FMTEventContainer, GivenEventsWhenGetEventsOfACellThenGivesTheEventsOfThePeriodThatHoldIt)
{
    // Arrange
    const Spatial::FMTEventContainer container = getContainer();

    // Act
    const std::vector<Spatial::FMTEvent> holding = getEvents(container.getEvents(1, Spatial::FMTCoordinate(2, 0)));
    const std::vector<Spatial::FMTEvent> otherPeriod = getEvents(container.getEvents(2, Spatial::FMTCoordinate(2, 0)));
    const std::vector<Spatial::FMTEvent> outside = getEvents(container.getEvents(1, Spatial::FMTCoordinate(9, 9)));

    // Assert
    EXPECT_EQ(holding, std::vector<Spatial::FMTEvent>({ ROW }));
    EXPECT_TRUE(otherPeriod.empty());
    EXPECT_TRUE(outside.empty());
}

// Observation, no defect announced: find of a period and a cell gives the first event that holds the cell only when
// more than one does (eventsfound.size() > 1); for one event, it gives the end. Nothing in FMT calls it.
TEST(FMTEventContainer, GivenOneEventThatHoldsACellWhenFindOfThePeriodAndTheCellThenGivesTheEnd)
{
    // Arrange
    const Spatial::FMTEventContainer container = getContainer();
    Spatial::FMTEventContainer overlapping = getContainer();
    overlapping.insert(getEvent({ Spatial::FMTCoordinate(1, 0) }, 1, 1));

    // Act
    const Spatial::FMTEventContainer::const_iterator alone = container.find(1, Spatial::FMTCoordinate(1, 0));
    const Spatial::FMTEventContainer::const_iterator first = overlapping.find(1, Spatial::FMTCoordinate(1, 0));

    // Assert
    EXPECT_TRUE(alone == container.end());
    ASSERT_TRUE(first != overlapping.end());
    EXPECT_TRUE(*first == ROW);
}

// Between two coordinates, by row then by column: 0 0 and 6 1 hold the mid position 1 0 of the row, not 7 2.
TEST(FMTEventContainer, GivenEventsWhenGetEventsBetweenTwoCoordinatesThenGivesTheEventsWhoseMidPositionIsBetweenThem)
{
    // Arrange
    const Spatial::FMTEventContainer container = getContainer();
    const Spatial::FMTCoordinate lower(0, 0);
    const Spatial::FMTCoordinate upper(6, 1);
    const Spatial::FMTCoordinate allTheRows(9, 9);

    // Act
    const std::vector<Spatial::FMTEvent> action0 = getEvents(container.getEvents(1, 0, lower, upper));
    const std::vector<Spatial::FMTEvent> used = getEvents(container.getEvents(1, std::vector<bool>({ true, true }),
        lower, allTheRows));
    const std::vector<std::vector<Spatial::FMTEventContainer::const_iterator>> multiple =
        container.getMultipleEvents(1, { 0, 1 }, lower, allTheRows);
    const std::vector<Spatial::FMTEvent> territory = getEvents(container.getEvents(1,
        std::set<Spatial::FMTCoordinate>({ lower, Spatial::FMTCoordinate(7, 2) })));
    const std::vector<Spatial::FMTEvent> noTerritory = getEvents(container.getEvents(1, std::set<Spatial::FMTCoordinate>()));

    // Assert
    EXPECT_EQ(action0, std::vector<Spatial::FMTEvent>({ ROW }));
    EXPECT_EQ(used, std::vector<Spatial::FMTEvent>({ ROW, CELL, OTHER_ACTION }));
    ASSERT_EQ(multiple.size(), 2u);
    EXPECT_EQ(getEvents(multiple.at(0)), std::vector<Spatial::FMTEvent>({ ROW, CELL }));
    EXPECT_EQ(getEvents(multiple.at(1)), std::vector<Spatial::FMTEvent>({ OTHER_ACTION }));
    EXPECT_EQ(territory, std::vector<Spatial::FMTEvent>({ ROW, CELL }));
    EXPECT_TRUE(noTerritory.empty());
}

// The subset of the periods and of the cells around coordinates, the buffer added to each side.
TEST(FMTEventContainer, GivenEventsWhenGetContainerAroundACellThenGivesTheEventsOfThePeriodsWithinTheBuffer)
{
    // Arrange
    const Spatial::FMTEventContainer container = getContainer();
    const std::vector<Spatial::FMTCoordinate> cells = { Spatial::FMTCoordinate(1, 0) };

    // Act
    const Spatial::FMTEventContainer noBuffer = container.getContainer(cells, 1, 2, 0);
    const Spatial::FMTEventContainer buffer = container.getContainer(cells, 1, 2, 5);
    const Spatial::FMTEventContainer none = container.getContainer({}, 1, 2, 5);

    // Assert
    EXPECT_EQ(getEvents(noBuffer), std::vector<Spatial::FMTEvent>({ ROW }));
    EXPECT_EQ(getEvents(buffer), std::vector<Spatial::FMTEvent>({ ROW, CELL, OTHER_ACTION }));
    EXPECT_TRUE(none.empty());
}

// With the binding of value 0, each cell added is an event of its own, beside the event of its neighbor.
TEST(FMTEventContainer, GivenTheBindingOfTheDefaultConstructorWhenAddActionOnTwoNeighborsThenEachIsAnEventOfOneCell)
{
    // Arrange
    Spatial::FMTEventContainer container;

    // Act
    container.addAction(Spatial::FMTCoordinate(3, 3), 1, 0, NO_BINDING, 5);
    container.addAction(Spatial::FMTCoordinate(4, 3), 1, 0, NO_BINDING, 5);
    container.addActions(Spatial::FMTCoordinate(3, 4), 2, { 0 }, { NO_BINDING }, 6);

    // Assert
    EXPECT_EQ(getCells(container), std::vector<std::set<Spatial::FMTCoordinate>>({ { Spatial::FMTCoordinate(3, 3) },
        { Spatial::FMTCoordinate(4, 3) }, { Spatial::FMTCoordinate(3, 4) } }));
    EXPECT_EQ(container.begin()->getActionId(), 0);
    EXPECT_EQ(container.begin()->getGraphFamily(), 5u);
    EXPECT_EQ(container.lastPeriod(), 2);
}

TEST(FMTEventContainer, GivenTheBindingOfTheDefaultConstructorWhenGetEventsToAddThenGivesTheNewEventOfTheCellAndNothingToRecompute)
{
    // Arrange
    const Spatial::FMTEventContainer container = getRowAndCell();
    Spatial::FMTEventContainer newEvents;

    // Act
    const Spatial::FMTEventContainer toCompute = container.getEventsToAdd(Spatial::FMTCoordinate(3, 0), 1, 0, NO_BINDING,
        newEvents, 0);

    // Assert
    EXPECT_TRUE(toCompute.empty());
    EXPECT_EQ(getCells(newEvents), std::vector<std::set<Spatial::FMTCoordinate>>({ { Spatial::FMTCoordinate(3, 0) } }));
    EXPECT_EQ(container.size(), 2u);
}

// Erasing a cell from the events of the targeted actions: an event that loses its middle splits in two, an event of
// one cell disappears.
TEST(FMTEventContainer, GivenEventsWhenEraseCoordinateThenTheEventsThatHoldItLoseItAndSplitWhereItJoinedThem)
{
    // Arrange
    Spatial::FMTEventContainer middle = getRowAndCell();
    Spatial::FMTEventContainer end = getRowAndCell();
    Spatial::FMTEventContainer single = getRowAndCell();
    Spatial::FMTEventContainer notTargeted = getRowAndCell();

    // Act
    middle.eraseCoordinate(Spatial::FMTCoordinate(1, 0), 1, { { true } });
    end.eraseCoordinate(Spatial::FMTCoordinate(2, 0), 1, { { true } });
    single.eraseCoordinate(Spatial::FMTCoordinate(5, 5), 1, { { true } });
    notTargeted.eraseCoordinate(Spatial::FMTCoordinate(1, 0), 1, std::vector<std::vector<bool>>(1));

    // Assert
    EXPECT_EQ(getCells(middle), std::vector<std::set<Spatial::FMTCoordinate>>({ { Spatial::FMTCoordinate(0, 0) },
        { Spatial::FMTCoordinate(2, 0) }, { Spatial::FMTCoordinate(5, 5) } }));
    EXPECT_EQ(getCells(end), std::vector<std::set<Spatial::FMTCoordinate>>({ { Spatial::FMTCoordinate(0, 0),
        Spatial::FMTCoordinate(1, 0) }, { Spatial::FMTCoordinate(5, 5) } }));
    EXPECT_EQ(getCells(single), std::vector<std::set<Spatial::FMTCoordinate>>({ ROW.getElements() }));
    EXPECT_TRUE(notTargeted == getRowAndCell());
}

// getEventsToErase then addUpdate give the events that eraseCoordinate gives, without changing the container before
// addUpdate.
TEST(FMTEventContainer, GivenEventsWhenGetEventsToEraseThenAddUpdateThenGivesTheEventsOfEraseCoordinate)
{
    // Arrange
    Spatial::FMTEventContainer container = getRowAndCell();
    Spatial::FMTEventContainer erased = getRowAndCell();
    erased.eraseCoordinate(Spatial::FMTCoordinate(1, 0), 1, { { true } });
    Spatial::FMTEventContainer newEvents;

    // Act
    const Spatial::FMTEventContainer toErase = container.getEventsToErase(1, { { true } }, Spatial::FMTCoordinate(1, 0),
        2, newEvents);
    const bool unchanged = (container == getRowAndCell());
    const Spatial::FMTEventContainer added = container.addUpdate(newEvents, toErase);

    // Assert
    EXPECT_TRUE(unchanged);
    EXPECT_EQ(getEvents(toErase), std::vector<Spatial::FMTEvent>({ ROW }));
    EXPECT_TRUE(added == newEvents);
    EXPECT_EQ(added.size(), 2u);
    EXPECT_TRUE(container == erased);
}

// The smallest distance to another event of the actions within the distance given, or that distance plus one.
TEST(FMTEventContainer, GivenEventsWhenMinimalDistanceThenGivesTheDistanceOfTheClosestEventWithinTheDistanceOrOneMore)
{
    // Arrange
    const Spatial::FMTEventContainer container = getDistantCells();
    const Spatial::FMTEvent origin = getEvent({ Spatial::FMTCoordinate(0, 0) }, 0, 1);

    // Act
    const double within = container.minimalDistance(origin, 10, 1, { 0 });
    const double tooFar = container.minimalDistance(origin, 3, 1, { 0 });
    const double otherAction = container.minimalDistance(origin, 10, 1, { 1 });

    // Assert
    EXPECT_DOUBLE_EQ(within, 5.0);
    EXPECT_DOUBLE_EQ(tooFar, 4.0);
    EXPECT_DOUBLE_EQ(otherAction, 11.0);
}

// The penalty of an event closer than the lower distance is what it lacks; of an event farther than the upper
// distance, what it exceeds: the cell 3 4 is at 5 cells from 0 0. The relations are meant to count each pair of
// events once; read in the code, not tested: FMTEventRelation::operator== compares the cell of the first event to the
// cell of the second event of the other relation (m_fromcoord == rhs.m_tocoord), unlike its hash, so that what the
// set finds depends on its buckets. Nothing in FMT calls evaluateDistance.
TEST(FMTEventContainer, GivenEventsWhenEvaluateDistanceThenAddsWhatTheyLackOrExceedOfTheDistanceRange)
{
    // Arrange
    Spatial::FMTEventContainer container;
    container.insert(getEvent({ Spatial::FMTCoordinate(0, 0) }, 0, 1));
    container.insert(getEvent({ Spatial::FMTCoordinate(3, 4) }, 0, 1));
    const Spatial::FMTEvent& origin = *container.find(getEvent({ Spatial::FMTCoordinate(0, 0) }, 0, 1));
    const double INFINITE = std::numeric_limits<double>::infinity();
    boost::unordered_set<Spatial::FMTEventRelation> tooCloseRelations;
    boost::unordered_set<Spatial::FMTEventRelation> tooFarRelations;

    // Act
    const double tooClose = container.evaluateDistance(origin, 6.0, INFINITE, 1, { true }, tooCloseRelations);
    const double tooFar = container.evaluateDistance(origin, -INFINITE, 4.0, 1, { true }, tooFarRelations);

    // Assert
    EXPECT_DOUBLE_EQ(tooClose, 1.0);
    EXPECT_DOUBLE_EQ(tooFar, 1.0);
}

// The size penalty of each event of the actions of the period, below the lower bound or above the upper bound.
TEST(FMTEventContainer, GivenEventsOfOneAndThreeCellsWhenEvaluateSizeThenAddsWhatTheyLackOrExceedOfTheBound)
{
    // Arrange
    const Spatial::FMTEventContainer container = getRowAndCell();

    // Act
    const double lower = container.evaluateSize({ true }, 1, 3, 10, true);
    const double upper = container.evaluateSize({ true }, 1, 0, 2, false);
    const double otherPeriod = container.evaluateSize({ true }, 2, 3, 10, true);

    // Assert
    EXPECT_DOUBLE_EQ(lower, 2.0);
    EXPECT_DOUBLE_EQ(upper, 1.0);
    EXPECT_DOUBLE_EQ(otherPeriod, 0.0);
}
