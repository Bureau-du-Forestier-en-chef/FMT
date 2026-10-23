/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTMask.h"
#include "FMTMaskFilter.h"
#include "FMTTheme.h"

#include "ExceptionChain.h"

#include <gtest/gtest.h>

#include <boost/dynamic_bitset.hpp>
#include <boost/functional/hash.hpp>
#include <boost/unordered_set.hpp>

#include <cstddef>
#include <map>
#include <string>
#include <utility>
#include <vector>

// The masks of TWD_land, built from a string and the themes as the parsers build them (Tests/Unit/Core/FMTTheme.cpp):
// one bit per attribute, theme after theme. The bits of UNITE1 to UNITE3 are 0 to 2, those of PEUPLEMENT1 to
// PEUPLEMENT3 and FEU 3 to 6, those of UTR1 to UTR3 7 to 9. getBitsString writes the last bit first.

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

    // The presolve of FMTModel::preSolve for the developments of UNITE1 PEUPLEMENT1 UTR1 and UNITE1 PEUPLEMENT2 UTR1:
    // the filter of the union of their masks, and the themes left with more than one attribute, here PEUPLEMENT1 and
    // PEUPLEMENT2 of the second theme.
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

TEST(FMTMask, GivenAMaskStringAndThemesWhenConstructedThenSetsTheBitOfEachAttribute)
{
    // Arrange
    const std::vector<Core::FMTTheme> themes = getThemes();

    // Act
    const Core::FMTMask mask("UNITE1 PEUPLEMENT2 UTR3", themes);

    // Assert
    EXPECT_EQ(mask.size(), 10u);
    EXPECT_EQ(mask.count(), 3u);
    EXPECT_EQ(mask.getBitsString(), "1000010001");
    EXPECT_EQ(std::string(mask), "UNITE1 PEUPLEMENT2 UTR3");
    EXPECT_TRUE(static_cast<bool>(mask));
    EXPECT_FALSE(mask.empty());
}

TEST(FMTMask, GivenAQuestionMarkAndAggregatesWhenConstructedThenSetsTheBitsOfAllTheirAttributes)
{
    // Arrange
    const std::vector<Core::FMTTheme> themes = getThemes();

    // Act
    const Core::FMTMask mask("? PROD UTA1", themes);

    // Assert
    EXPECT_EQ(mask.count(), 8u);
    EXPECT_EQ(mask.getBitsString(), "0110111111");
}

TEST(FMTMask, GivenNegatedValuesWhenConstructedThenSetsTheBitsOfTheOtherAttributes)
{
    // Arrange
    const std::vector<Core::FMTTheme> themes = getThemes();

    // Act
    const Core::FMTMask mask("!UNITE1 !PROD ?", themes);

    // Assert
    EXPECT_EQ(mask.count(), 6u);
    EXPECT_EQ(mask.getBitsString(), "1111000110");
}

TEST(FMTMask, GivenTheValueOfEachThemeWhenConstructedThenEqualsTheMaskOfTheirString)
{
    // Arrange
    const Core::FMTMask fromString = getMask("UNITE1 PEUPLEMENT2 UTR3");

    // Act
    const Core::FMTMask fromValues({ "UNITE1", "PEUPLEMENT2", "UTR3" }, getThemes());

    // Assert
    EXPECT_TRUE(fromValues == fromString);
    EXPECT_EQ(std::string(fromValues), "UNITE1 PEUPLEMENT2 UTR3");
}

// Characterizes the current behavior: a mask shorter than the themes is completed with the first attribute of each
// missing theme, where FMTTheme::validate, which the parsers call before they build a mask, completes it with ?.
TEST(FMTMask, GivenFewerValuesThanThemesWhenConstructedThenCompletesWithTheFirstAttributeOfEachMissingTheme)
{
    // Arrange
    const std::vector<Core::FMTTheme> themes = getThemes();

    // Act
    const Core::FMTMask mask("UNITE1 PEUPLEMENT1", themes);

    // Assert
    EXPECT_EQ(std::string(mask), "UNITE1 PEUPLEMENT1 UTR1");
    EXPECT_TRUE(mask == getMask("UNITE1 PEUPLEMENT1 UTR1"));
}

TEST(FMTMask, GivenAnUnknownAttributeWhenConstructedThenRaisesAnUndefinedAttribute)
{
    // Arrange
    const std::vector<Core::FMTTheme> themes = getThemes();

    // Act
    const std::vector<Exception::FMTexc> raised = Testing::getRaisedTypes([&]()
        {
        Core::FMTMask("UNITE9 ? ?", themes);
        });

    // Assert
    EXPECT_TRUE(Testing::hasType(raised, Exception::FMTexc::FMTundefined_attribute));
}

TEST(FMTMask, GivenThemesOnlyWhenConstructedThenSelectsNoAttribute)
{
    // Arrange
    const std::vector<Core::FMTTheme> themes = getThemes();

    // Act
    const Core::FMTMask mask(themes);

    // Assert
    EXPECT_EQ(mask.size(), 10u);
    EXPECT_EQ(mask.count(), 0u);
    EXPECT_EQ(std::string(mask), "!? !? !?");
}

TEST(FMTMask, GivenADefaultMaskWhenTestedThenIsEmptyAndFalse)
{
    // Arrange
    const Core::FMTMask mask;

    // Act
    const bool asBool = static_cast<bool>(mask);

    // Assert
    EXPECT_FALSE(asBool);
    EXPECT_TRUE(mask.empty());
    EXPECT_EQ(mask.size(), 0u);
    EXPECT_EQ(std::string(mask), "");
}

TEST(FMTMask, GivenAMaskWhenCopiedThenTheCopyHasTheSameBitsAndName)
{
    // Arrange
    const Core::FMTMask original = getMask("UC PROD UTR2");

    // Act
    const Core::FMTMask copy(original);
    Core::FMTMask assigned;
    assigned = original;

    // Assert
    EXPECT_TRUE(copy == original);
    EXPECT_TRUE(assigned == original);
    EXPECT_EQ(std::string(copy), "UC PROD UTR2");
    EXPECT_EQ(assigned.getBitsString(), original.getBitsString());
}

TEST(FMTMask, GivenAMaskWhenMovedThenTheTargetHoldsItsBitsAndName)
{
    // Arrange
    Core::FMTMask original = getMask("UC PROD UTR2");
    const Core::FMTMask expected(original);

    // Act
    const Core::FMTMask moved(std::move(original));

    // Assert
    EXPECT_TRUE(moved == expected);
    EXPECT_EQ(std::string(moved), "UC PROD UTR2");
}

// Two masks are compared by their bits only: UTA2 holds UTR3 alone, so that both masks are equal.
TEST(FMTMask, GivenTwoNamesOfTheSameAttributesWhenComparedThenTheMasksAreEqual)
{
    // Arrange
    const Core::FMTMask aggregate = getMask("? PROD UTA2");
    const Core::FMTMask attribute = getMask("? PROD UTR3");

    // Act
    const bool equal = (aggregate == attribute);
    const bool different = (aggregate != attribute);

    // Assert
    EXPECT_TRUE(equal);
    EXPECT_FALSE(different);
    EXPECT_NE(std::string(aggregate), std::string(attribute));
}

TEST(FMTMask, GivenMasksOfDifferentSizesWhenComparedThenTheyDifferAndTheSmallerIsLess)
{
    // Arrange
    const Core::FMTMask oneTheme(std::vector<Core::FMTTheme>(1, getThemes().at(0)));
    const Core::FMTMask threeThemes(getThemes());

    // Act
    const bool equal = (oneTheme == threeThemes);
    const bool oneThemeIsLess = (oneTheme < threeThemes);
    const bool threeThemesIsLess = (threeThemes < oneTheme);

    // Assert
    EXPECT_FALSE(equal);
    EXPECT_TRUE(oneThemeIsLess);
    EXPECT_FALSE(threeThemesIsLess);
}

TEST(FMTMask, GivenTwoDifferentMasksOfTheSameSizeWhenComparedThenExactlyOneIsLess)
{
    // Arrange
    const Core::FMTMask first = getMask("UNITE1 PEUPLEMENT1 UTR1");
    const Core::FMTMask second = getMask("UNITE2 PEUPLEMENT1 UTR1");

    // Act
    const bool firstIsLess = (first < second);
    const bool secondIsLess = (second < first);
    const bool firstIsLessThanItself = (first < first);

    // Assert
    EXPECT_NE(firstIsLess, secondIsLess);
    EXPECT_FALSE(firstIsLessThanItself);
}

TEST(FMTMask, GivenEqualMasksWhenHashThenTheyHaveTheSameHash)
{
    // Arrange
    const Core::FMTMask aggregate = getMask("? PROD UTA2");
    const Core::FMTMask attribute = getMask("? PROD UTR3");

    // Act
    const size_t aggregateHash = aggregate.hash();
    const size_t attributeHash = boost::hash<Core::FMTMask>()(attribute);

    // Assert
    EXPECT_EQ(aggregateHash, attributeHash);
}

TEST(FMTMask, GivenMasksThatDifferByOneAttributeWhenHashThenTheirHashesDiffer)
{
    // Arrange
    const Core::FMTMask first = getMask("UNITE1 PEUPLEMENT1 UTR1");
    const Core::FMTMask second = getMask("UNITE1 PEUPLEMENT1 UTR2");

    // Act
    const size_t firstHash = first.hash();
    const size_t secondHash = second.hash();

    // Assert
    EXPECT_NE(firstHash, secondHash);
}

TEST(FMTMask, GivenMasksInAHashedSetWhenFindThenFindsTheMaskOfTheSameAttributes)
{
    // Arrange
    boost::unordered_set<Core::FMTMask> masks;
    masks.insert(getMask("UNITE1 PEUPLEMENT1 UTR1"));
    masks.insert(getMask("? PROD UTA2"));

    // Act
    const bool findsDevelopment = masks.find(getMask("UNITE1 PEUPLEMENT1 UTR1")) != masks.end();
    const bool findsSameBits = masks.find(getMask("? PROD UTR3")) != masks.end();
    const bool findsOther = masks.find(getMask("UNITE2 PEUPLEMENT1 UTR1")) != masks.end();

    // Assert
    EXPECT_TRUE(findsDevelopment);
    EXPECT_TRUE(findsSameBits);
    EXPECT_FALSE(findsOther);
}

TEST(FMTMask, GivenTheMaskOfADevelopmentWhenIsSubsetOfThenIsASubsetOfTheMasksThatSelectIt)
{
    // Arrange
    const Core::FMTMask development = getMask("UNITE1 PEUPLEMENT1 UTR1");
    const Core::FMTMask burnt = getMask("UNITE1 FEU UTR1");
    const Core::FMTMask productive = getMask("? PROD ?");

    // Act
    const bool developmentInProductive = development.isSubsetOf(productive);
    const bool burntInProductive = burnt.isSubsetOf(productive);
    const bool productiveInDevelopment = productive.isSubsetOf(development);
    const bool developmentInItself = development.isSubsetOf(development);

    // Assert
    EXPECT_TRUE(developmentInProductive);
    EXPECT_FALSE(burntInProductive);
    EXPECT_FALSE(productiveInDevelopment);
    EXPECT_TRUE(developmentInItself);
}

TEST(FMTMask, GivenTheGenericMaskWhenIsSubsetOfThenEveryMaskIsASubsetOfIt)
{
    // Arrange
    const Core::FMTMask generic = getMask("? ? ?");

    // Act
    const bool developmentInGeneric = getMask("UNITE3 FEU UTR2").isSubsetOf(generic);
    const bool aggregateInGeneric = getMask("UC POURPL UTA1").isSubsetOf(generic);
    const bool genericInDevelopment = generic.isSubsetOf(getMask("UNITE3 FEU UTR2"));

    // Assert
    EXPECT_TRUE(developmentInGeneric);
    EXPECT_TRUE(aggregateInGeneric);
    EXPECT_FALSE(genericInDevelopment);
    EXPECT_EQ(generic.count(), generic.size());
    EXPECT_EQ(generic.get(getThemes()), "? ? ?");
}

// The bits are kept in blocks of 8: the last block of a mask of 10 bits holds 2 of them and is never full.
TEST(FMTMask, GivenMasksWhenGetNonFullBlocksThenReturnsTheBlocksThatMissABit)
{
    // Arrange
    const Core::FMTMask generic = getMask("? ? ?");
    const Core::FMTMask development = getMask("UNITE1 PEUPLEMENT1 UTR1");

    // Act
    const std::vector<size_t> genericBlocks = generic.getNonFullBlocks();
    const std::vector<size_t> developmentBlocks = development.getNonFullBlocks();

    // Assert
    EXPECT_EQ(genericBlocks, std::vector<size_t>({ 1 }));
    EXPECT_EQ(developmentBlocks, std::vector<size_t>({ 0, 1 }));
}

// A full block of the larger mask cannot exclude anything: the test of its non full blocks answers as the whole mask.
TEST(FMTMask, GivenTheNonFullBlocksOfAMaskWhenIsSubsetOfThenAnswersAsTheWholeMask)
{
    // Arrange: ? ? UTR1 sets the 8 bits of its first block
    const Core::FMTMask selection = getMask("? ? UTR1");
    const std::vector<size_t> blocks = selection.getNonFullBlocks();
    const Core::FMTMask inside = getMask("UNITE2 FEU UTR1");
    const Core::FMTMask outside = getMask("UNITE2 FEU UTR2");

    // Act
    const bool insideByBlocks = inside.isSubsetOf(selection, blocks);
    const bool outsideByBlocks = outside.isSubsetOf(selection, blocks);

    // Assert
    EXPECT_EQ(blocks, std::vector<size_t>({ 1 }));
    EXPECT_TRUE(insideByBlocks);
    EXPECT_FALSE(outsideByBlocks);
    EXPECT_EQ(insideByBlocks, inside.isSubsetOf(selection));
    EXPECT_EQ(outsideByBlocks, outside.isSubsetOf(selection));
}

// The intersection has the bits of both masks and no name, until update gives it one.
TEST(FMTMask, GivenTwoMasksWhenGetIntersectThenKeepsTheBitsOfBoth)
{
    // Arrange
    const Core::FMTMask productive = getMask("? PROD ?");
    const Core::FMTMask other = getMask("? POURPL UTA1");

    // Act
    Core::FMTMask intersect = productive.getIntersect(other);
    const std::string nameBeforeUpdate = std::string(intersect);
    intersect.update(getThemes());

    // Assert
    EXPECT_TRUE(intersect == getMask("? POURPL UTA1"));
    EXPECT_EQ(nameBeforeUpdate, "");
    EXPECT_EQ(std::string(intersect), "? POURPL UTA1");
    EXPECT_TRUE(productive.getBitsetIntersect(other) == intersect.getBitsetReference());
}

TEST(FMTMask, GivenTwoMasksWhenSetIntersectThenKeepsTheBitsOfBothInPlace)
{
    // Arrange
    Core::FMTMask mask = getMask("UC PROD ?");

    // Act
    mask.setIntersect(getMask("? POURPL UTA2"));

    // Assert
    EXPECT_TRUE(mask == getMask("UC POURPL UTA2"));
}

TEST(FMTMask, GivenTwoMasksWhenGetUnionThenKeepsTheBitsOfEither)
{
    // Arrange
    const Core::FMTMask first = getMask("UNITE1 PEUPLEMENT1 UTR1");
    Core::FMTMask second = getMask("UNITE2 PEUPLEMENT1 UTR1");

    // Act
    const Core::FMTMask merged = first.getUnion(second);
    second.unionWith(first);

    // Assert
    EXPECT_TRUE(merged == getMask("UC PEUPLEMENT1 UTR1"));
    EXPECT_TRUE(second == merged);
    EXPECT_EQ(std::string(second), "");
}

TEST(FMTMask, GivenAMaskWhenGetThenReturnsTheValueOfEachTheme)
{
    // Arrange
    const std::vector<Core::FMTTheme> themes = getThemes();
    const Core::FMTMask mask = getMask("UNITE1 POURPL ?");

    // Act
    const std::string unit = mask.get(themes.at(0));
    const std::string stand = mask.get(themes.at(1));
    const std::string territory = mask.get(themes.at(2));

    // Assert
    EXPECT_EQ(unit, "UNITE1");
    EXPECT_EQ(stand, "POURPL");
    EXPECT_EQ(territory, "?");
    EXPECT_EQ(mask.get(themes), "UNITE1 POURPL ?");
    EXPECT_EQ(mask.getSubsetCount(themes.at(1)), 2u);
}

// The value of a theme is read back from its bits: an aggregate of a single attribute gives that attribute, and the
// bits of all but one attribute give the negation of the missing one.
TEST(FMTMask, GivenTheBitsOfOneAttributeOrOfAllButOneWhenGetThenReturnsTheAttributeOrItsNegation)
{
    // Arrange
    const std::vector<Core::FMTTheme> themes = getThemes();
    const Core::FMTMask aggregate = getMask("? PROD UTA2");
    const Core::FMTMask negation = getMask("!UNITE1 ? ?");

    // Act
    const std::string fromAggregate = aggregate.get(themes);
    const std::string fromNegation = negation.get(themes.at(0));

    // Assert
    EXPECT_EQ(fromAggregate, "? PROD UTR3");
    EXPECT_EQ(std::string(aggregate), "? PROD UTA2");
    EXPECT_EQ(fromNegation, "!UNITE1");
}

TEST(FMTMask, GivenAttributesThatNoAggregateHoldsWhenGetThenRaisesAnUndefinedAttribute)
{
    // Arrange: PEUPLEMENT1 and PEUPLEMENT2, which neither an aggregate nor a negation names
    const std::vector<Core::FMTTheme> themes = getThemes();
    const Core::FMTMask mask = getMask("? PEUPLEMENT1 ?").getUnion(getMask("? PEUPLEMENT2 ?"));

    // Act
    const std::vector<Exception::FMTexc> raised = Testing::getRaisedTypes([&]()
        {
        mask.get(themes.at(1));
        });

    // Assert
    EXPECT_TRUE(Testing::hasType(raised, Exception::FMTexc::FMTundefined_attribute));
}

TEST(FMTMask, GivenTheMaskOfADevelopmentWhenGetAttributeThenReturnsTheAttributeOfTheTheme)
{
    // Arrange
    const std::vector<Core::FMTTheme> themes = getThemes();
    const Core::FMTMask mask = getMask("UNITE2 FEU UTR3");

    // Act
    const std::string& stand = mask.getAttribute(themes.at(1));
    const std::string& territory = mask.getAttribute(themes.at(2));

    // Assert
    EXPECT_EQ(stand, "FEU");
    EXPECT_EQ(territory, "UTR3");
}

TEST(FMTMask, GivenAnAggregateWhenDecomposeThenReturnsOneMaskPerAttribute)
{
    // Arrange
    const std::vector<Core::FMTTheme> themes = getThemes();
    const Core::FMTMask mask = getMask("UNITE1 PROD ?");

    // Act
    const std::vector<Core::FMTMask> masks = mask.decompose(themes.at(1));

    // Assert
    ASSERT_EQ(masks.size(), 3u);
    EXPECT_EQ(std::string(masks.at(0)), "UNITE1 PEUPLEMENT1 ?");
    EXPECT_EQ(std::string(masks.at(1)), "UNITE1 PEUPLEMENT2 ?");
    EXPECT_EQ(std::string(masks.at(2)), "UNITE1 PEUPLEMENT3 ?");
    EXPECT_TRUE(masks.at(2) == getMask("UNITE1 PEUPLEMENT3 ?"));
}

TEST(FMTMask, GivenAThemeAndAValueWhenSetThenChangesTheValueOfThatThemeOnly)
{
    // Arrange
    const std::vector<Core::FMTTheme> themes = getThemes();
    Core::FMTMask mask = getMask("UNITE1 PEUPLEMENT1 UTR1");

    // Act
    mask.set(themes.at(1), "FEU");

    // Assert
    EXPECT_EQ(std::string(mask), "UNITE1 FEU UTR1");
    EXPECT_TRUE(mask == getMask("UNITE1 FEU UTR1"));
}

// A static theme is one whose value is neither an attribute nor an aggregate: a question mark.
TEST(FMTMask, GivenAMaskWhenGetStaticThemesThenReturnsTheThemesOfAQuestionMark)
{
    // Arrange
    const std::vector<Core::FMTTheme> themes = getThemes();
    const std::vector<const Core::FMTTheme*> pointers = { &themes.at(0), &themes.at(1), &themes.at(2) };
    const Core::FMTMask mask = getMask("? PROD UTR1");

    // Act
    const std::vector<const Core::FMTTheme*> staticThemes = mask.getStaticThemes(pointers);

    // Assert
    ASSERT_EQ(staticThemes.size(), 1u);
    EXPECT_EQ(staticThemes.at(0), &themes.at(0));
}

TEST(FMTMask, GivenAMaskOfOneThemeWhenGetSelectedThemesThenReturnsThatThemeOnly)
{
    // Arrange: the bits of UNITE1 only
    const std::vector<Core::FMTTheme> themes = getThemes();
    Core::FMTMask mask(themes);
    mask.set(themes.at(0), "UNITE1");

    // Act
    const std::vector<const Core::FMTTheme*> selected = mask.getSelectedThemes(themes);

    // Assert
    ASSERT_EQ(selected.size(), 1u);
    EXPECT_EQ(selected.at(0), &themes.at(0));
    EXPECT_EQ(std::string(mask), "UNITE1 !? !?");
}

TEST(FMTMask, GivenTwoMasksWhenIsNotThemesSubsetThenTellsWhetherOneThemeHasNoCommonAttribute)
{
    // Arrange
    const std::vector<Core::FMTTheme> themes = getThemes();
    const Core::FMTMask development = getMask("UNITE1 PEUPLEMENT1 UTR1");

    // Act
    const bool notInSecondUnit = development.isNotThemesSubset(getMask("UNITE2 ? ?"), themes);
    const bool notInUnits = development.isNotThemesSubset(getMask("UC ? ?"), themes);

    // Assert
    EXPECT_TRUE(notInSecondUnit);
    EXPECT_FALSE(notInUnits);
}

// removeAggregates clears the bits of each theme whose value is an aggregate or a question mark, or the question marks
// only.
TEST(FMTMask, GivenAMaskWithAggregatesWhenRemoveAggregatesThenClearsTheBitsOfTheirThemes)
{
    // Arrange
    const std::vector<Core::FMTTheme> themes = getThemes();
    const Core::FMTMask mask = getMask("? PROD UTR1");

    // Act
    const Core::FMTMask withoutAggregates = mask.removeAggregates(themes);
    const Core::FMTMask withoutQuestionMarks = mask.removeAggregates(themes, true);

    // Assert
    EXPECT_EQ(withoutAggregates.getBitsString(), "0010000000");
    EXPECT_EQ(withoutQuestionMarks.getBitsString(), "0010111000");
    EXPECT_EQ(std::string(withoutAggregates), "");
}

TEST(FMTMask, GivenAMaskWithQuestionMarksAndAggregatesWhenRefineThenTakesTheMorePreciseValueOfEachTheme)
{
    // Arrange
    const std::vector<Core::FMTTheme> themes = getThemes();
    const Core::FMTMask mask = getMask("UNITE1 PROD ?");

    // Act
    const Core::FMTMask refined = mask.refine(getMask("UC PEUPLEMENT2 UTR3"), themes);

    // Assert
    EXPECT_EQ(std::string(refined), "UNITE1 PEUPLEMENT2 UTR3");
    EXPECT_TRUE(refined == getMask("UNITE1 PEUPLEMENT2 UTR3"));
}

TEST(FMTMask, GivenTheGenericMaskWhenRefineThenReturnsTheOtherMask)
{
    // Arrange
    const Core::FMTMask generic = getMask("? ? ?");
    const Core::FMTMask other = getMask("UC PEUPLEMENT2 UTA1");

    // Act
    const Core::FMTMask refined = generic.refine(other, getThemes());

    // Assert
    EXPECT_TRUE(refined == other);
    EXPECT_EQ(std::string(refined), "UC PEUPLEMENT2 UTA1");
}

TEST(FMTMask, GivenIndexesWhenResumeThenKeepsTheBitsAtThoseIndexes)
{
    // Arrange
    const Core::FMTMask mask = getMask("UNITE1 PEUPLEMENT1 UTR1");
    boost::dynamic_bitset<uint8_t> selection(10, false);
    selection[1] = true;
    selection[3] = true;

    // Act
    const Core::FMTMask ofSetBits = mask.resume(std::vector<size_t>({ 0, 3, 7 }));
    const Core::FMTMask ofOtherBits = mask.resume(std::vector<size_t>({ 1, 4, 8 }));
    const Core::FMTMask ofSelection = mask.resume(selection);

    // Assert
    EXPECT_EQ(ofSetBits.getBitsString(), "111");
    EXPECT_EQ(ofOtherBits.getBitsString(), "000");
    EXPECT_EQ(ofSelection.getBitsString(), "10");
    EXPECT_EQ(std::string(ofSetBits), "UNITE1 PEUPLEMENT1 UTR1");
}

TEST(FMTMask, GivenAnotherMaskWhenSetExclusiveBitsThenKeepsTheAttributesOfTheThemeThatTheOtherMaskDoesNotHold)
{
    // Arrange
    const std::vector<Core::FMTTheme> themes = getThemes();
    Core::FMTMask mask = getMask("? PROD ?");

    // Act
    mask.setExclusiveBits(getMask("? POURPL ?"), themes.at(1));

    // Assert
    EXPECT_TRUE(mask == getMask("? PEUPLEMENT1 ?"));
    EXPECT_EQ(std::string(mask), "");
}

TEST(FMTMask, GivenBitsWhenAppendThenAddsThemAfterTheBitsOfTheMask)
{
    // Arrange
    Core::FMTMask mask = getMask("UNITE1 PEUPLEMENT1 UTR1");
    boost::dynamic_bitset<uint8_t> bits(3, false);
    bits[0] = true;
    bits[2] = true;

    // Act
    mask.append(bits);

    // Assert
    EXPECT_EQ(mask.size(), 13u);
    EXPECT_EQ(mask.getBitsString(), "1010010001001");
}

TEST(FMTMask, GivenTwoMasksWhenSwapThenEachHoldsTheBitsAndNameOfTheOther)
{
    // Arrange
    Core::FMTMask first = getMask("UNITE1 PEUPLEMENT1 UTR1");
    Core::FMTMask second = getMask("UC PROD ?");

    // Act
    first.swap(second);

    // Assert
    EXPECT_EQ(std::string(first), "UC PROD ?");
    EXPECT_EQ(std::string(second), "UNITE1 PEUPLEMENT1 UTR1");
    EXPECT_TRUE(second == getMask("UNITE1 PEUPLEMENT1 UTR1"));
}

TEST(FMTMask, GivenAMaskWhenClearThenIsEmpty)
{
    // Arrange
    Core::FMTMask mask = getMask("UNITE1 PEUPLEMENT1 UTR1");

    // Act
    mask.clear();

    // Assert
    EXPECT_TRUE(mask.empty());
    EXPECT_EQ(std::string(mask), "");
}

// The presolved mask keeps the bits of the themes left with more than one attribute, and is named after them.
TEST(FMTMask, GivenAPreSolveFilterWhenPreSolveThenKeepsTheBitsOfThePresolvedThemes)
{
    // Arrange
    const PreSolve preSolve = getPreSolve();
    const Core::FMTMask development = getMask("UNITE1 PEUPLEMENT2 UTR1");
    const Core::FMTMask productive = getMask("? PROD ?");

    // Act
    const Core::FMTMask presolvedDevelopment = development.preSolve(preSolve.filter, preSolve.themes);
    const Core::FMTMask presolvedProductive = productive.preSolve(preSolve.filter, preSolve.themes);

    // Assert
    ASSERT_EQ(preSolve.themes.size(), 1u);
    EXPECT_EQ(presolvedDevelopment.size(), 2u);
    EXPECT_EQ(presolvedDevelopment.getBitsString(), "10");
    EXPECT_EQ(std::string(presolvedDevelopment), "PEUPLEMENT2");
    EXPECT_EQ(std::string(presolvedProductive), "?");
}

// A mask can be presolved when each presolved theme keeps one of its attributes; the attributes of the removed themes
// are not looked at.
TEST(FMTMask, GivenAPreSolveFilterWhenCanPreSolveThenRefusesTheMasksWithoutAnAttributeOfAPresolvedTheme)
{
    // Arrange
    const PreSolve preSolve = getPreSolve();

    // Act
    const bool keepsFirstStand = getMask("UNITE3 PEUPLEMENT1 UTR3").canPreSolve(preSolve.filter, preSolve.themes);
    const bool keepsThirdStand = getMask("UNITE1 PEUPLEMENT3 UTR1").canPreSolve(preSolve.filter, preSolve.themes);

    // Assert
    EXPECT_TRUE(keepsFirstStand);
    EXPECT_FALSE(keepsThirdStand);
}

// The postsolve filter of FMTModel::getPostSolveFilter: the bits of the presolved attributes in the original masks,
// and the bits of the development in the themes that the presolve removed.
TEST(FMTMask, GivenAPresolvedMaskWhenPostSolveThenRestoresTheMaskOfTheDevelopment)
{
    // Arrange: PEUPLEMENT1 and PEUPLEMENT2 are the bits 3 and 4
    const std::vector<Core::FMTTheme> themes = getThemes();
    const PreSolve preSolve = getPreSolve();
    const Core::FMTMask development = getMask("UNITE1 PEUPLEMENT2 UTR1");
    const Core::FMTMask presolved = development.preSolve(preSolve.filter, preSolve.themes);
    const Core::FMTMask selection = getBitsMask({ 3, 4 });
    const Core::FMTMask removedThemes = development.getPostSolveMask(selection, themes);

    // Act
    const Core::FMTMask postsolved = presolved.postSolve(Core::FMTMaskFilter(selection, removedThemes), themes);

    // Assert
    EXPECT_EQ(removedThemes.getBitsString(), "0010000001");
    EXPECT_TRUE(postsolved == development);
    EXPECT_EQ(std::string(postsolved), "UNITE1 PEUPLEMENT2 UTR1");
}
