/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTMaskFilter.h"
#include "FMTMask.h"
#include "FMTTheme.h"
#include "FMTutility.h"

#include <gtest/gtest.h>

#include <boost/dynamic_bitset.hpp>
#include <boost/functional/hash.hpp>

#include <cstdint>
#include <map>
#include <string>
#include <vector>

// FMTMaskFilter reduces masks. Built from the masks of a list (FMTList::shrink), it gives the resume mask of a
// development (Tests/Unit/Core/FMTYieldRequest.cpp): the bits that every mask of the list has are set, the others are
// those of the development. Built from a selection (FMTModel::preSolve, Tests/Unit/Core/FMTMask.cpp), it keeps the bits
// of the selection, then the presolve of the themes records in it the bits they keep. The themes are those of
// Tests/Unit/Core/FMTTheme.cpp: UNITE1 to UNITE3 are the bits 0 to 2, PEUPLEMENT1 to FEU the bits 3 to 6, UTR1 to UTR3
// the bits 7 to 9; a mask writes its bits the highest first.

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

    // A mask of the bits p_bits only, without name, as FMTModel::getSelectedMask builds one.
    Core::FMTMask getBitsMask(const std::vector<size_t>& p_bits)
    {
        boost::dynamic_bitset<uint8_t> bits(10, false);
        for (const size_t bit : p_bits)
        {
            bits[bit] = true;
        }
        return Core::FMTMask(bits);
    }

    // The filter of the masks of a list.
    Core::FMTMaskFilter getListFilter(const std::vector<std::string>& p_masks)
    {
        std::vector<Core::FMTMask> masks;
        for (const std::string& mask : p_masks)
        {
            masks.push_back(getMask(mask));
        }
        return Core::FMTMaskFilter(masks);
    }
}

TEST(FMTMaskFilter, GivenADefaultFilterWhenFilterThenReturnsTheMaskUnchanged)
{
    // Arrange
    const Core::FMTMaskFilter filter;
    const Core::FMTMask development = getMask("UNITE1 PEUPLEMENT1 UTR1");

    // Act
    const Core::FMTMask filtered = filter.filter(development);

    // Assert
    EXPECT_TRUE(filter.empty());
    EXPECT_TRUE(filter.emptyFlipped());
    EXPECT_TRUE(filtered == development);
    EXPECT_EQ(std::string(filtered), "UNITE1 PEUPLEMENT1 UTR1");
}

// The masks UNITE1 ? ?, ? ? ? and UNITE2 ? ? all have the bits of the second and third themes, and no bit of the first
// one: the filter sets the bits 3 to 9 and keeps UNITE1 of the development. The filtered mask keeps the name of the
// development.
TEST(FMTMaskFilter, GivenTheMasksOfAListWhenFilterThenSetsTheBitsThatEveryMaskHasAndKeepsTheOthersOfTheDevelopment)
{
    // Arrange
    const Core::FMTMaskFilter filter = getListFilter({ "UNITE1 ? ?", "? ? ?", "UNITE2 ? ?" });

    // Act
    const Core::FMTMask filtered = filter.filter(getMask("UNITE1 PEUPLEMENT1 UTR1"));

    // Assert
    EXPECT_FALSE(filter.empty());
    EXPECT_FALSE(filter.emptyFlipped());
    EXPECT_EQ(filtered.getBitsString(), "1111111001");
    EXPECT_EQ(std::string(filtered), "UNITE1 PEUPLEMENT1 UTR1");
}

// What FMTList::findSets relies on: a development is in a mask of the list exactly when its filtered mask is in the
// filtered mask of the list.
TEST(FMTMaskFilter, GivenTheMasksOfAListWhenFilterThenAFilteredDevelopmentIsInAFilteredMaskExactlyWhenTheDevelopmentIsInTheMask)
{
    // Arrange
    const std::vector<std::string> listMasks = { "UNITE1 PEUPLEMENT1 ?", "? PEUPLEMENT2 UTR3", "UC PROD ?" };
    const std::vector<std::string> developments = { "UNITE1 PEUPLEMENT1 UTR1", "UNITE2 PEUPLEMENT2 UTR3",
        "UNITE3 PEUPLEMENT2 UTR3", "UNITE2 PEUPLEMENT3 UTR2", "UNITE3 FEU UTR1", "UNITE1 PEUPLEMENT2 UTR1" };
    const Core::FMTMaskFilter filter = getListFilter(listMasks);
    std::vector<std::string> disagreements;

    // Act
    for (const std::string& development : developments)
    {
        for (const std::string& listMask : listMasks)
        {
            const bool inMask = getMask(development).isSubsetOf(getMask(listMask));
            const bool filteredInMask = filter.filter(getMask(development)).isSubsetOf(filter.filter(getMask(listMask)));
            if (inMask != filteredInMask)
            {
                disagreements.push_back(development + " in " + listMask);
            }
        }
    }

    // Assert
    EXPECT_TRUE(disagreements.empty());
    EXPECT_TRUE(filter.filter(getMask("UNITE1 PEUPLEMENT1 UTR1")).isSubsetOf(filter.filter(getMask("UC PROD ?"))));
}

// When every mask of the list has every bit, no bit is left to keep, and the filter is meant to keep them all. It builds
// its selection by boost::dynamic_bitset<uint8_t>(size, true), whose second argument is the integer value of the first
// bits, not a value for every bit: the selection is the bit 0 only, UNITE1. A development of UNITE1 then has the resume
// mask of the bit 0, any other the empty one. FMTList::findSets stays right, since every mask of such a list selects
// every development.
TEST(FMTMaskFilter, GivenMasksThatAllHaveEveryBitWhenFilterThenKeepsTheFirstBitOfTheDevelopmentOnly)
{
    // Arrange
    const Core::FMTMaskFilter filter = getListFilter({ "? ? ?", "? ? ?" });
    const Core::FMTMask filteredList = filter.filter(getMask("? ? ?"));

    // Act
    const Core::FMTMask ofUnit1 = filter.filter(getMask("UNITE1 PEUPLEMENT1 UTR1"));
    const Core::FMTMask ofUnit2 = filter.filter(getMask("UNITE2 PEUPLEMENT3 UTR2"));

    // Assert
    EXPECT_FALSE(filter.empty());
    EXPECT_EQ(ofUnit1.getBitsString(), "0000000001");
    EXPECT_EQ(ofUnit2.getBitsString(), "0000000000");
    EXPECT_TRUE(ofUnit1.isSubsetOf(filteredList));
    EXPECT_TRUE(ofUnit2.isSubsetOf(filteredList));
}

// The selection of UNITE1 PEUPLEMENT1 UTR1 and UNITE1 PEUPLEMENT2 UTR1 is the bits 0, 3, 4 and 7.
TEST(FMTMaskFilter, GivenASelectionWhenFilterThenKeepsTheBitsOfTheMaskThatTheSelectionHas)
{
    // Arrange
    const Core::FMTMaskFilter filter(getMask("UNITE1 PEUPLEMENT1 UTR1").getUnion(getMask("UNITE1 PEUPLEMENT2 UTR1")));

    // Act
    const Core::FMTMask everything = filter.filter(getMask("? ? ?"));
    const Core::FMTMask stand = filter.filter(getMask("UNITE2 PEUPLEMENT1 UTR1"));

    // Assert
    EXPECT_EQ(everything.getBitsString(), "0010011001");
    EXPECT_EQ(stand.getBitsString(), "0010001000");
    EXPECT_FALSE(filter.emptyFlipped());
}

TEST(FMTMaskFilter, GivenASelectionAndABufferWhenFilterThenKeepsTheSelectedBitsAndAddsTheBitsOfTheBuffer)
{
    // Arrange: the selection PEUPLEMENT1 and PEUPLEMENT2, the buffer UNITE1 and UTR1
    const Core::FMTMaskFilter filter(getBitsMask({ 3, 4 }), getBitsMask({ 0, 7 }));

    // Act
    const Core::FMTMask filtered = filter.filter(getMask("UNITE2 PEUPLEMENT2 UTR3"));

    // Assert
    EXPECT_EQ(filtered.getBitsString(), "0010010001");
}

TEST(FMTMaskFilter, GivenASelectionWhenGetSelectedThemesThenReturnsTheThemesOfItsBits)
{
    // Arrange: UNITE1, PEUPLEMENT1 and PEUPLEMENT2
    const std::vector<Core::FMTTheme> themes = getThemes();
    const Core::FMTMaskFilter filter(getBitsMask({ 0, 3, 4 }));

    // Act
    const std::vector<const Core::FMTTheme*> selected = filter.getSelectedThemes(themes);

    // Assert
    ASSERT_EQ(selected.size(), 2u);
    EXPECT_EQ(selected.at(0), &themes.at(0));
    EXPECT_EQ(selected.at(1), &themes.at(1));
}

// A mask can be presolved when it has a selected attribute in each selected theme.
TEST(FMTMaskFilter, GivenASelectionWhenCanPreSolveThenRefusesAMaskWithoutASelectedAttributeInASelectedTheme)
{
    // Arrange: UNITE1, PEUPLEMENT1 and PEUPLEMENT2
    const std::vector<Core::FMTTheme> themes = getThemes();
    const Core::FMTMaskFilter filter(getBitsMask({ 0, 3, 4 }));
    const std::vector<const Core::FMTTheme*> selected = filter.getSelectedThemes(themes);

    // Act
    const bool productive = filter.canPreSolve(getMask("UNITE1 PROD ?"), selected);
    const bool thirdStand = filter.canPreSolve(getMask("UNITE1 PEUPLEMENT3 UTR1"), selected);
    const bool secondUnit = filter.canPreSolve(getMask("UNITE2 PEUPLEMENT1 UTR1"), selected);

    // Assert
    EXPECT_TRUE(productive);
    EXPECT_FALSE(thirdStand);
    EXPECT_FALSE(secondUnit);
}

// The presolve of the themes keeps PEUPLEMENT1 and PEUPLEMENT2 (Tests/Unit/Core/FMTMask.cpp), which the presolved filter
// selects, for the next pass of FMTModel::preSolve.
TEST(FMTMaskFilter, GivenTheThemesPresolvedWithAFilterWhenPreSolveTheFilterThenSelectsTheAttributesThatTheyKeep)
{
    // Arrange
    Core::FMTMaskFilter filter(getMask("UNITE1 PEUPLEMENT1 UTR1").getUnion(getMask("UNITE1 PEUPLEMENT2 UTR1")));
    std::vector<Core::FMTTheme> presolvedThemes;
    size_t newId = 0;
    size_t newStart = 0;
    for (const Core::FMTTheme& theme : getThemes())
    {
        const Core::FMTTheme presolved = theme.preSolve(filter, newId, newStart);
        if (!presolved.empty())
        {
            presolvedThemes.push_back(presolved);
        }
    }

    // Act
    const Core::FMTMaskFilter presolvedFilter = filter.preSolve(presolvedThemes);

    // Assert
    ASSERT_EQ(presolvedThemes.size(), 1u);
    EXPECT_EQ(presolvedFilter.filter(Core::FMTMask("?", presolvedThemes)).getBitsString(), "11");
    EXPECT_EQ(presolvedFilter.filter(Core::FMTMask("PEUPLEMENT2", presolvedThemes)).getBitsString(), "10");
}

TEST(FMTMaskFilter, GivenTwoFiltersWhenSwapThenEachFiltersAsTheOtherDid)
{
    // Arrange
    Core::FMTMaskFilter first(getBitsMask({ 0, 3 }));
    Core::FMTMaskFilter second(getBitsMask({ 7, 8, 9 }));
    const Core::FMTMask everything = getMask("? ? ?");

    // Act
    first.swap(second);

    // Assert
    EXPECT_EQ(first.filter(everything).getBitsString(), "1110000000");
    EXPECT_EQ(second.filter(everything).getBitsString(), "0000001001");
}

TEST(FMTMaskFilter, GivenAFilterWhenCopiedThenTheCopyFiltersTheSameWayAndHasTheSameHash)
{
    // Arrange
    const Core::FMTMaskFilter filter = getListFilter({ "UNITE1 ? ?", "? ? ?", "UNITE2 ? ?" });
    const Core::FMTMask development = getMask("UNITE1 PEUPLEMENT1 UTR1");

    // Act
    const Core::FMTMaskFilter copy(filter);
    Core::FMTMaskFilter assigned;
    assigned = filter;

    // Assert
    EXPECT_TRUE(copy.filter(development) == filter.filter(development));
    EXPECT_TRUE(assigned.filter(development) == filter.filter(development));
    EXPECT_EQ(copy.hash(), filter.hash());
    EXPECT_EQ(boost::hash<Core::FMTMaskFilter>()(assigned), filter.hash());
}

// The hash combines the selection and the buffer by a XOR: a filter whose buffer is its selection, as
// FMTModel::getPreSolveFilter builds them, hashes to 0, and swapping the selection and the buffer keeps the hash. Nothing
// in FMT hashes a filter today.
TEST(FMTMaskFilter, GivenAFilterWhoseBufferIsItsSelectionWhenHashThenReturnsZero)
{
    // Arrange
    const Core::FMTMask selection = getBitsMask({ 0, 3, 4, 7 });
    const Core::FMTMask buffer = getBitsMask({ 1, 5 });

    // Act
    const size_t sameHash = Core::FMTMaskFilter(selection, selection).hash();
    const size_t forward = Core::FMTMaskFilter(selection, buffer).hash();
    const size_t backward = Core::FMTMaskFilter(buffer, selection).hash();

    // Assert
    EXPECT_EQ(sameHash, 0u);
    EXPECT_EQ(forward, backward);
}
