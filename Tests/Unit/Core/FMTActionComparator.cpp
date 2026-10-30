/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTAction.h"
#include "FMTAgeBounds.h"
#include "FMTMask.h"
#include "FMTSpec.h"
#include "FMTTheme.h"
#include "FMTutility.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <iterator>
#include <limits>
#include <string>
#include <unordered_set>
#include <vector>

// FMTActionComparator finds an action by its name, or by an aggregate it belongs to (*AGGREGATE of the action
// section), as FMTModel and the parsers search the actions of a model. The actions are those of
// Tests/Unit/Core/FMTAction.cpp, on one theme.

namespace
{
    std::vector<Core::FMTTheme> getThemes()
    {
        return { Core::FMTTheme({ "A1", "A2" }, 0, 0, "THEME1") };
    }

    // An action of the mask ?, operable from age 1, and of the aggregates p_aggregates.
    Core::FMTAction getAction(const std::string& p_name, const std::vector<std::string>& p_aggregates = {})
    {
        Core::FMTAction action(p_name, true, false);
        Core::FMTSpec spec;
        spec.addBounds(Core::FMTAgeBounds(Core::FMTsection::Action, (std::numeric_limits<int>::max)(), 1));
        action.push_back(Core::FMTMask("?", getThemes()), spec);
        for (const std::string& aggregate : p_aggregates)
        {
            action.pushAggregate(aggregate);
        }
        action.update();
        return action;
    }

    // aFire, then COUPEtotale and aThinning of the aggregate COUPES, then plantation.
    std::vector<Core::FMTAction> getActions()
    {
        return { getAction("AFIRE"), getAction("COUPETOTALE", { "COUPES" }), getAction("ATHINNING", { "COUPES" }),
            getAction("PLANTATION") };
    }
}

TEST(FMTActionComparator, GivenANameWhenComparedThenMatchesTheActionOfThatNameOnly)
{
    // Arrange
    const Core::FMTActionComparator comparator("AFIRE");

    // Act
    const bool matchesFire = comparator(getAction("AFIRE"));
    const bool matchesClearcut = comparator(getAction("COUPETOTALE"));
    const bool matchesLowerCase = Core::FMTActionComparator("afire")(getAction("AFIRE"));

    // Assert
    EXPECT_TRUE(matchesFire);
    EXPECT_FALSE(matchesClearcut);
    EXPECT_FALSE(matchesLowerCase);
}

TEST(FMTActionComparator, GivenAnAggregateWhenComparedThenMatchesItsActionsOnlyWhenCheckingTheAggregates)
{
    // Arrange
    const Core::FMTAction clearcut = getAction("COUPETOTALE", { "COUPES" });

    // Act
    const bool byAggregateWithoutCheck = Core::FMTActionComparator("COUPES")(clearcut);
    const bool byAggregateWithCheck = Core::FMTActionComparator("COUPES", true)(clearcut);
    const bool byNameWithCheck = Core::FMTActionComparator("COUPETOTALE", true)(clearcut);

    // Assert
    EXPECT_FALSE(byAggregateWithoutCheck);
    EXPECT_TRUE(byAggregateWithCheck);
    EXPECT_TRUE(byNameWithCheck);
}

TEST(FMTActionComparator, GivenActionsWhenFindIfWithAComparatorThenFindsTheActionOfThatName)
{
    // Arrange
    const std::vector<Core::FMTAction> actions = getActions();

    // Act
    const std::vector<Core::FMTAction>::const_iterator thinning =
        std::find_if(actions.begin(), actions.end(), Core::FMTActionComparator("ATHINNING"));
    const std::vector<Core::FMTAction>::const_iterator salvage =
        std::find_if(actions.begin(), actions.end(), Core::FMTActionComparator("ARECUP"));

    // Assert
    EXPECT_EQ(std::distance(actions.begin(), thinning), 2);
    EXPECT_TRUE(salvage == actions.end());
}

// getAllAggregates always looks at the aggregates, whether the comparator checks them or not; with aggregateonly, it
// looks at the aggregates only.
TEST(FMTActionComparator, GivenAnAggregateWhenGetAllAggregatesThenReturnsTheActionsOfTheAggregateInTheirOrder)
{
    // Arrange
    const std::vector<Core::FMTAction> actions = getActions();

    // Act
    const std::vector<const Core::FMTAction*> ofAggregate = Core::FMTActionComparator("COUPES").getAllAggregates(actions);
    const std::vector<const Core::FMTAction*> ofName = Core::FMTActionComparator("AFIRE").getAllAggregates(actions);
    const std::vector<const Core::FMTAction*> ofNameAggregatesOnly =
        Core::FMTActionComparator("AFIRE").getAllAggregates(actions, true);

    // Assert
    EXPECT_EQ(ofAggregate, std::vector<const Core::FMTAction*>({ &actions.at(1), &actions.at(2) }));
    EXPECT_EQ(ofName, std::vector<const Core::FMTAction*>({ &actions.at(0) }));
    EXPECT_TRUE(ofNameAggregatesOnly.empty());
}

TEST(FMTActionComparator, GivenAnAggregateWhenGetAllAggregatesSetThenReturnsThePositionsOfItsActions)
{
    // Arrange
    const std::vector<Core::FMTAction> actions = getActions();

    // Act
    const std::unordered_set<int> ofAggregate = Core::FMTActionComparator("COUPES").getAllAggregatesSet(actions);
    const std::unordered_set<int> ofUnknown = Core::FMTActionComparator("ARECUP").getAllAggregatesSet(actions);

    // Assert
    EXPECT_EQ(ofAggregate, std::unordered_set<int>({ 1, 2 }));
    EXPECT_TRUE(ofUnknown.empty());
}
