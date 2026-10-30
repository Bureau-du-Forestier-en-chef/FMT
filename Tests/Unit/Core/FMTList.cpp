/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTList.hpp"
#include "FMTAction.h"
#include "FMTAgeBounds.h"
#include "FMTLifespans.h"
#include "FMTMask.h"
#include "FMTSpec.h"
#include "FMTTheme.h"
#include "FMTutility.h"

#include "ExceptionChain.h"

#include <gtest/gtest.h>

#include <limits>
#include <map>
#include <string>
#include <vector>

// FMTList is a class template, tested through the lists of FMT: FMTList<int> of FMTLifespans and FMTList<FMTSpec> of
// FMTAction, which FMTlib exports. Each element is a mask and its value, in the order of the section; update shrinks
// the masks to the bits that the list uses (FMTMaskFilter), and findSets gives the elements whose mask holds a
// development, through a cache by filtered mask. The themes are those of Tests/Unit/Core/FMTTheme.cpp.

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

    // ? feu ? 0, then ? ? ? 80, as FMTLifespansParser reads them, before the update of FMTModel.
    Core::FMTLifespans buildLifespans()
    {
        Core::FMTLifespans lifespans;
        lifespans.push_back(getMask("? FEU ?"), 0);
        lifespans.push_back(getMask("? ? ?"), 80);
        return lifespans;
    }

    // The values of the list, in their order.
    std::vector<int> getValues(const Core::FMTList<int>& p_list)
    {
        std::vector<int> values;
        for (const auto& element : p_list)
        {
            values.push_back(element.second);
        }
        return values;
    }

    // The values of the elements whose mask holds the development of the mask p_mask, in their order.
    std::vector<int> findValues(const Core::FMTList<int>& p_list, const std::string& p_mask)
    {
        std::vector<int> values;
        for (const Core::FMTList<int>::const_iterator element : p_list.findSets(getMask(p_mask)))
        {
            values.push_back(element->second);
        }
        return values;
    }
}

TEST(FMTList, GivenAListWithoutElementWhenConstructedThenIsEmptyAndCanShrink)
{
    // Arrange and Act
    const Core::FMTLifespans lifespans;

    // Assert
    EXPECT_TRUE(lifespans.empty());
    EXPECT_EQ(lifespans.size(), 0u);
    EXPECT_TRUE(lifespans.canShrink());
}

TEST(FMTList, GivenElementsWhenPushFrontAndInsertThenPutsThemAtTheirPlace)
{
    // Arrange
    Core::FMTLifespans lifespans;
    lifespans.push_back(getMask("? ? ?"), 80);

    // Act
    lifespans.push_front(getMask("? FEU ?"), 0);
    lifespans.insert(1, getMask("UNITE3 ? ?"), 60);

    // Assert
    EXPECT_EQ(getValues(lifespans), std::vector<int>({ 0, 60, 80 }));
    EXPECT_TRUE((lifespans.begin() + 1)->first == getMask("UNITE3 ? ?"));
}

TEST(FMTList, GivenElementsWhenEraseAndPopBackThenRemovesThem)
{
    // Arrange
    Core::FMTLifespans lifespans = buildLifespans();
    lifespans.insert(1, getMask("UNITE3 ? ?"), 60);

    // Act
    lifespans.erase(1);
    const std::vector<int> afterErase = getValues(lifespans);
    lifespans.pop_back();

    // Assert
    EXPECT_EQ(afterErase, std::vector<int>({ 0, 80 }));
    EXPECT_EQ(getValues(lifespans), std::vector<int>({ 0 }));
}

TEST(FMTList, GivenAListNotUpdatedWhenFindSetsThenGivesTheElementsWhoseMaskHoldsTheDevelopmentInTheirOrder)
{
    // Arrange
    const Core::FMTLifespans lifespans = buildLifespans();

    // Act
    const std::vector<int> ofBurnt = findValues(lifespans, "UNITE1 FEU UTR1");
    const std::vector<int> ofProductive = findValues(lifespans, "UNITE1 PEUPLEMENT1 UTR1");

    // Assert
    EXPECT_EQ(ofBurnt, std::vector<int>({ 0, 80 }));
    EXPECT_EQ(ofProductive, std::vector<int>({ 80 }));
}

TEST(FMTList, GivenAListWhenUpdateThenCannotShrinkAgainAndStillFindsTheElementsOfADevelopment)
{
    // Arrange
    Core::FMTLifespans lifespans = buildLifespans();

    // Act
    lifespans.update();

    // Assert
    EXPECT_FALSE(lifespans.canShrink());
    EXPECT_EQ(findValues(lifespans, "UNITE1 FEU UTR1"), std::vector<int>({ 0, 80 }));
    EXPECT_EQ(findValues(lifespans, "UNITE1 PEUPLEMENT1 UTR1"), std::vector<int>({ 80 }));
}

TEST(FMTList, GivenAnUpdatedListWhenFindSetsTwiceThenGivesTheSameElements)
{
    // Arrange
    Core::FMTLifespans lifespans = buildLifespans();
    lifespans.update();
    const Core::FMTMask burnt = getMask("UNITE1 FEU UTR1");

    // Act
    const std::vector<Core::FMTLifespans::const_iterator> first = lifespans.findSets(burnt);
    const std::vector<Core::FMTLifespans::const_iterator> second = lifespans.findSets(burnt);

    // Assert
    EXPECT_EQ(first, second);
    EXPECT_EQ(first.size(), 2u);
}

// The masks of the lifespans use the second theme only: masks that differ in the other themes have the same key, and
// share the cache of findSets.
TEST(FMTList, GivenAnUpdatedListWhenFilterMaskThenMasksThatDifferOnlyInThemesItDoesNotUseHaveTheSameKey)
{
    // Arrange
    Core::FMTLifespans lifespans = buildLifespans();
    lifespans.update();

    // Act
    const Core::FMTMask first = lifespans.filterMask(getMask("UNITE1 PEUPLEMENT1 UTR1"));
    const Core::FMTMask otherUnit = lifespans.filterMask(getMask("UNITE3 PEUPLEMENT1 UTR3"));
    const Core::FMTMask otherStand = lifespans.filterMask(getMask("UNITE1 PEUPLEMENT2 UTR1"));

    // Assert
    EXPECT_TRUE(first == otherUnit);
    EXPECT_FALSE(first == otherStand);
    EXPECT_EQ(std::string(first), "UNITE1 PEUPLEMENT1 UTR1");
}

TEST(FMTList, GivenAnUpdatedListWhenUnShrinkThenRebuildsTheMasksFromTheirNamesAndCanShrinkAgain)
{
    // Arrange
    Core::FMTLifespans lifespans = buildLifespans();
    lifespans.update();

    // Act
    lifespans.unShrink(getThemes());

    // Assert
    EXPECT_TRUE(lifespans.canShrink());
    EXPECT_TRUE(lifespans.begin()->first == getMask("? FEU ?"));
    EXPECT_TRUE((lifespans.begin() + 1)->first == getMask("? ? ?"));
}

TEST(FMTList, GivenAnUpdatedListWhenCopiedThenTheCopyKeepsItsFilter)
{
    // Arrange
    Core::FMTLifespans lifespans = buildLifespans();
    lifespans.update();

    // Act
    const Core::FMTLifespans copy(lifespans);

    // Assert
    EXPECT_FALSE(copy.canShrink());
    EXPECT_EQ(findValues(copy, "UNITE1 FEU UTR1"), std::vector<int>({ 0, 80 }));
}

TEST(FMTList, GivenTwoListsNotUpdatedWhenAddedThenAppendsTheElementsOfTheOther)
{
    // Arrange
    Core::FMTLifespans lifespans = buildLifespans();
    Core::FMTLifespans other;
    other.push_back(getMask("UNITE3 ? ?"), 60);

    // Act
    lifespans += other;

    // Assert
    EXPECT_EQ(getValues(lifespans), std::vector<int>({ 0, 80, 60 }));
}

TEST(FMTList, GivenAnUpdatedListWhenAddedThenRaisesAnInvalidAction)
{
    // Arrange
    Core::FMTLifespans lifespans = buildLifespans();
    Core::FMTLifespans updated = buildLifespans();
    updated.update();

    // Act
    const std::vector<Exception::FMTexc> raised = Testing::getRaisedTypes([&]()
        {
        lifespans += updated;
        });

    // Assert
    EXPECT_TRUE(Testing::hasType(raised, Exception::FMTexc::FMTinvalid_action));
}

TEST(FMTList, GivenListsWhenComparedThenTheyAreEqualOnlyWithTheSameElementsInTheSameOrder)
{
    // Arrange
    const Core::FMTLifespans lifespans = buildLifespans();
    Core::FMTLifespans reversed;
    reversed.push_back(getMask("? ? ?"), 80);
    reversed.push_back(getMask("? FEU ?"), 0);

    // Act
    const bool same = (lifespans == buildLifespans());
    const bool sameReversed = (lifespans == reversed);

    // Assert
    EXPECT_TRUE(same);
    EXPECT_FALSE(sameReversed);
}

// clear keeps the filter of the update: the cleared list cannot be added to another.
TEST(FMTList, GivenAnUpdatedListWhenClearThenIsEmptyAndStillCannotShrink)
{
    // Arrange
    Core::FMTLifespans lifespans = buildLifespans();
    lifespans.update();

    // Act
    lifespans.clear();

    // Assert
    EXPECT_TRUE(lifespans.empty());
    EXPECT_FALSE(lifespans.canShrink());
}

TEST(FMTList, GivenMasksWhenGetUnionThenReturnsTheUnionOfTheMasksOfTheList)
{
    // Arrange: an action of UC prod ? and ? feu ?
    Core::FMTAction action("ACTION", true, false);
    Core::FMTSpec spec;
    spec.addBounds(Core::FMTAgeBounds(Core::FMTsection::Action, (std::numeric_limits<int>::max)(), 1));
    action.push_back(getMask("UC PROD ?"), spec);
    action.push_back(getMask("? FEU ?"), spec);

    // Act
    const Core::FMTMask together = action.getUnion(getThemes());

    // Assert
    EXPECT_TRUE(together == getMask("UC PROD ?").getUnion(getMask("? FEU ?")));
}

// Include/FMTList.hpp documents push_back of a list as an append; the code copies the elements of the other list in
// place of those of this list, then shrinks it. Nothing in FMT calls it (Tests/ETAT.md, section 5.15).
TEST(FMTList, GivenAListWhenPushBackAnotherListThenTheListHoldsTheElementsOfTheOtherOnly)
{
    // Arrange
    Core::FMTLifespans lifespans = buildLifespans();
    Core::FMTLifespans other;
    other.push_back(getMask("UNITE3 ? ?"), 60);

    // Act
    lifespans.push_back(other);

    // Assert
    EXPECT_EQ(getValues(lifespans), std::vector<int>({ 60 }));
    EXPECT_FALSE(lifespans.canShrink());
}
