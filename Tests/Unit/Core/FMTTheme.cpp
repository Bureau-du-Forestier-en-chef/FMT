/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTTheme.h"
#include "FMTMask.h"
#include "FMTMaskFilter.h"

#include "ExceptionChain.h"

#include <gtest/gtest.h>

#include <cstddef>
#include <map>
#include <string>
#include <vector>

// The attributes and aggregates of one theme, built as FMTLandscapeParser builds them from a landscape section: one
// name per attribute, empty when the line gives none, the values of each aggregate (attributes or other aggregates),
// the index of the theme from 0, and the bit at which it starts in a mask. The themes are those of TWD_land.

namespace
{
    const std::vector<std::map<std::string, double>> NO_INDEXES;

    // *THEME 1: unite1 to unite3, *AGGREGATE UC (unite1, unite2).
    Core::FMTTheme getUnitTheme()
    {
        return Core::FMTTheme({ "UNITE1", "UNITE2", "UNITE3" }, { "", "", "" }, { "UC" }, { { "UNITE1", "UNITE2" } },
            NO_INDEXES, 0, 0, "");
    }

    // *THEME 2: peuplement1 to peuplement3 and feu, *AGGREGATE prod (the three peuplement) and pourpl (peuplement2,
    // peuplement3).
    Core::FMTTheme getStandTheme()
    {
        return Core::FMTTheme({ "PEUPLEMENT1", "PEUPLEMENT2", "PEUPLEMENT3", "FEU" }, { "", "", "", "" },
            { "PROD", "POURPL" }, { { "PEUPLEMENT1", "PEUPLEMENT2", "PEUPLEMENT3" }, { "PEUPLEMENT2", "PEUPLEMENT3" } },
            NO_INDEXES, 1, 3, "");
    }

    // *THEME 3: UTR1 to UTR3, *AGGREGATE UTA1 (UTR1, UTR2) and UTA2 (UTR3).
    Core::FMTTheme getTerritoryTheme()
    {
        return Core::FMTTheme({ "UTR1", "UTR2", "UTR3" }, { "", "", "" }, { "UTA1", "UTA2" }, { { "UTR1", "UTR2" }, { "UTR3" } },
            NO_INDEXES, 2, 7, "");
    }

    std::vector<Core::FMTTheme> getThemes()
    {
        return { getUnitTheme(), getStandTheme(), getTerritoryTheme() };
    }

    // The third theme with the indexes of the scenario AVERAGEP of TWD_land: UTR1 _INDEX(A=16.36,B=37.80) and so on.
    // The parser gives an attribute with an index an empty name.
    Core::FMTTheme getIndexedTheme()
    {
        return Core::FMTTheme({ "UTR1", "UTR2", "UTR3" }, { "", "", "" }, { "UTA1" }, { { "UTR1", "UTR2" } },
            { { { "A", 16.36 }, { "B", 37.8 } }, { { "A", 19.41 }, { "B", 41.92 } }, { { "A", 11.29 }, { "B", 23.95 } } },
            2, 7, "");
    }

    // A filter that keeps the attributes of p_selected, as FMTModel::preSolve builds it from the union of the masks of
    // its developments.
    Core::FMTMaskFilter getPreSolveFilter(const std::vector<std::string>& p_selected)
    {
        const std::vector<Core::FMTTheme> THEMES = getThemes();
        Core::FMTMask selection(p_selected.at(0), THEMES);
        for (const std::string& mask : p_selected)
        {
            selection = selection.getUnion(Core::FMTMask(mask, THEMES));
        }
        return Core::FMTMaskFilter(selection);
    }
}

TEST(FMTTheme, GivenTheLinesOfALandscapeSectionWhenConstructedThenKeepsItsAttributesAndAggregatesInOrder)
{
    // Arrange: *THEME 2, its attributes, and *AGGREGATE prod and pourpl
    const std::vector<std::string> attributes = { "PEUPLEMENT1", "PEUPLEMENT2", "PEUPLEMENT3", "FEU" };
    const std::vector<std::string> names = { "", "", "", "" };
    const std::vector<std::string> aggregates = { "PROD", "POURPL" };
    const std::vector<std::vector<std::string>> aggregateValues = {
        { "PEUPLEMENT1", "PEUPLEMENT2", "PEUPLEMENT3" }, { "PEUPLEMENT2", "PEUPLEMENT3" } };

    // Act
    const Core::FMTTheme theme(attributes, names, aggregates, aggregateValues, NO_INDEXES, 1, 3, "");

    // Assert
    EXPECT_EQ(theme.getId(), 1u);
    EXPECT_EQ(theme.getStart(), 3u);
    EXPECT_EQ(theme.size(), 4u);
    EXPECT_FALSE(theme.empty());
    EXPECT_EQ(theme.getBaseAttributes(), std::vector<std::string>({ "PEUPLEMENT1", "PEUPLEMENT2", "PEUPLEMENT3", "FEU" }));
    EXPECT_EQ(theme.getAttributeNames(), std::vector<std::string>({ "", "", "", "" }));
    EXPECT_EQ(theme.getAggregates(), std::vector<std::string>({ "PROD", "POURPL" }));
    EXPECT_EQ(theme.getName(), "");
    EXPECT_FALSE(theme.useIndex());
}

TEST(FMTTheme, GivenAttributesOnlyWhenConstructedThenTheThemeHasNoAggregateAndNoAttributeName)
{
    // Arrange
    const std::vector<std::string> attributes = { "A1", "A2" };

    // Act
    const Core::FMTTheme theme(attributes, 0, 0, "STATUS");

    // Assert
    EXPECT_EQ(theme.getName(), "STATUS");
    EXPECT_EQ(theme.size(), 2u);
    EXPECT_TRUE(theme.getAggregates().empty());
    EXPECT_TRUE(theme.getAttributeNames().empty());
}

TEST(FMTTheme, GivenNoIndexesWhenConstructedWithoutThemThenEqualsTheThemeConstructedWithAnEmptyList)
{
    // Arrange
    const Core::FMTTheme withEmptyList = getUnitTheme();

    // Act
    const Core::FMTTheme withoutIndexes({ "UNITE1", "UNITE2", "UNITE3" }, { "", "", "" }, { "UC" }, { { "UNITE1", "UNITE2" } },
        0, 0, "");

    // Assert
    EXPECT_TRUE(withoutIndexes == withEmptyList);
    EXPECT_EQ(withoutIndexes.getAttributes("UC"), std::vector<std::string>({ "UNITE1", "UNITE2" }));
}

TEST(FMTTheme, GivenADefaultThemeWhenSizeThenHasNoAttribute)
{
    // Arrange
    const Core::FMTTheme theme;

    // Act
    const size_t size = theme.size();

    // Assert
    EXPECT_EQ(size, 0u);
    EXPECT_TRUE(theme.empty());
}

TEST(FMTTheme, GivenAnAttributeWhenIsAttributeThenAcceptsItAndItsNegationOnly)
{
    // Arrange
    const Core::FMTTheme theme = getUnitTheme();

    // Act
    const bool acceptsAttribute = theme.isAttribute("UNITE1");
    const bool acceptsNegation = theme.isAttribute("!UNITE1");
    const bool acceptsUnknown = theme.isAttribute("UNITE9");
    const bool acceptsQuestionMark = theme.isAttribute("?");

    // Assert
    EXPECT_TRUE(acceptsAttribute);
    EXPECT_TRUE(acceptsNegation);
    EXPECT_FALSE(acceptsUnknown);
    EXPECT_FALSE(acceptsQuestionMark);
}

// Characterizes the current behavior: isAttribute looks the value up among the attributes and the aggregates
// (_buildAttributeLocations), so that it accepts an aggregate. isValid, which validates the masks of every section,
// relies on it to accept an aggregate.
TEST(FMTTheme, GivenAnAggregateWhenIsAttributeThenReturnsTrue)
{
    // Arrange
    const Core::FMTTheme theme = getUnitTheme();

    // Act
    const bool acceptsAggregate = theme.isAttribute("UC");

    // Assert
    EXPECT_TRUE(acceptsAggregate);
}

TEST(FMTTheme, GivenAnAggregateWhenIsAggregateThenAcceptsItAndItsNegationButNoAttribute)
{
    // Arrange
    const Core::FMTTheme theme = getUnitTheme();

    // Act
    const bool acceptsAggregate = theme.isAggregate("UC");
    const bool acceptsNegation = theme.isAggregate("!UC");
    const bool acceptsAttribute = theme.isAggregate("UNITE1");
    const bool acceptsQuestionMark = theme.isAggregate("?");

    // Assert
    EXPECT_TRUE(acceptsAggregate);
    EXPECT_TRUE(acceptsNegation);
    EXPECT_FALSE(acceptsAttribute);
    EXPECT_FALSE(acceptsQuestionMark);
}

TEST(FMTTheme, GivenAQuestionMarkWhenGetAttributesThenReturnsEveryAttribute)
{
    // Arrange
    const Core::FMTTheme theme = getStandTheme();

    // Act
    const std::vector<std::string> attributes = theme.getAttributes("?");

    // Assert
    EXPECT_EQ(attributes, std::vector<std::string>({ "PEUPLEMENT1", "PEUPLEMENT2", "PEUPLEMENT3", "FEU" }));
}

TEST(FMTTheme, GivenAnAttributeOrAnAggregateWhenGetAttributesThenReturnsItsAttributesInTheOrderOfTheTheme)
{
    // Arrange
    const Core::FMTTheme theme = getStandTheme();

    // Act
    const std::vector<std::string> ofAttribute = theme.getAttributes("FEU");
    const std::vector<std::string> ofProd = theme.getAttributes("PROD");
    const std::vector<std::string> ofPourpl = theme.getAttributes("POURPL");

    // Assert
    EXPECT_EQ(ofAttribute, std::vector<std::string>({ "FEU" }));
    EXPECT_EQ(ofProd, std::vector<std::string>({ "PEUPLEMENT1", "PEUPLEMENT2", "PEUPLEMENT3" }));
    EXPECT_EQ(ofPourpl, std::vector<std::string>({ "PEUPLEMENT2", "PEUPLEMENT3" }));
}

TEST(FMTTheme, GivenANegatedValueWhenGetAttributesThenReturnsTheOtherAttributes)
{
    // Arrange
    const Core::FMTTheme theme = getStandTheme();

    // Act
    const std::vector<std::string> notProd = theme.getAttributes("!PROD");
    const std::vector<std::string> notFire = theme.getAttributes("!FEU");

    // Assert
    EXPECT_EQ(notProd, std::vector<std::string>({ "FEU" }));
    EXPECT_EQ(notFire, std::vector<std::string>({ "PEUPLEMENT1", "PEUPLEMENT2", "PEUPLEMENT3" }));
}

// An aggregate may name another aggregate: its attributes are those of both, and its source is what was written.
TEST(FMTTheme, GivenAnAggregateOfAnAggregateWhenGetAttributesThenReturnsTheAttributesOfBoth)
{
    // Arrange: *AGGREGATE LOW (A1), *AGGREGATE MOST (LOW, A2)
    const Core::FMTTheme theme({ "A1", "A2", "A3" }, { "", "", "" }, { "LOW", "MOST" }, { { "A1" }, { "LOW", "A2" } },
        NO_INDEXES, 0, 0, "");

    // Act
    const std::vector<std::string> attributes = theme.getAttributes("MOST");
    const std::vector<std::string> source = theme.getAttributes("MOST", true);

    // Assert
    EXPECT_EQ(attributes, std::vector<std::string>({ "A1", "A2" }));
    EXPECT_EQ(source, std::vector<std::string>({ "LOW", "A2" }));
}

TEST(FMTTheme, GivenAnUnknownValueWhenGetAttributesThenRaisesAnUndefinedAttribute)
{
    // Arrange
    const Core::FMTTheme theme = getStandTheme();

    // Act
    const std::vector<Exception::FMTexc> raised = Testing::getRaisedTypes([&]()
        {
        theme.getAttributes("PEUPLEMENT9");
        });

    // Assert
    EXPECT_TRUE(Testing::hasType(raised, Exception::FMTexc::FMTundefined_attribute));
}

TEST(FMTTheme, GivenAnAttributeAndAnAggregateWhenInAggregateThenTellsWhetherTheAggregateHoldsTheAttribute)
{
    // Arrange
    Core::FMTTheme theme = getUnitTheme();

    // Act
    const bool holdsFirstUnit = theme.inAggregate("UNITE1", "UC");
    const bool holdsThirdUnit = theme.inAggregate("UNITE3", "UC");

    // Assert
    EXPECT_TRUE(holdsFirstUnit);
    EXPECT_FALSE(holdsThirdUnit);
}

TEST(FMTTheme, GivenAValueWhenIsValidThenAcceptsTheQuestionMarkTheAttributesAndTheAggregates)
{
    // Arrange
    const Core::FMTTheme theme = getUnitTheme();

    // Act
    const bool acceptsQuestionMark = theme.isValid("?");
    const bool acceptsAttribute = theme.isValid("UNITE1");
    const bool acceptsAggregate = theme.isValid("UC");
    const bool acceptsNegation = theme.isValid("!UNITE2");
    const bool acceptsUnknown = theme.isValid("UNITE9");

    // Assert
    EXPECT_TRUE(acceptsQuestionMark);
    EXPECT_TRUE(acceptsAttribute);
    EXPECT_TRUE(acceptsAggregate);
    EXPECT_TRUE(acceptsNegation);
    EXPECT_FALSE(acceptsUnknown);
}

TEST(FMTTheme, GivenAMaskOfOneValuePerThemeWhenValidateThenKeepsIt)
{
    // Arrange
    std::string mask = "UNITE1 PROD UTR1";

    // Act
    const bool valid = Core::FMTTheme::validate(getThemes(), mask);

    // Assert
    EXPECT_TRUE(valid);
    EXPECT_EQ(mask, "UNITE1 PROD UTR1");
}

TEST(FMTTheme, GivenAMaskWithFewerValuesThanThemesWhenValidateThenCompletesItWithQuestionMarks)
{
    // Arrange
    std::string mask = "UNITE1 PEUPLEMENT1";

    // Act
    const bool valid = Core::FMTTheme::validate(getThemes(), mask);

    // Assert
    EXPECT_TRUE(valid);
    EXPECT_EQ(mask, "UNITE1 PEUPLEMENT1 ?");
}

TEST(FMTTheme, GivenAMaskWithMoreValuesThanThemesWhenValidateThenKeepsOneValuePerTheme)
{
    // Arrange
    std::string mask = "UNITE1 PEUPLEMENT1 UTR1 EXTRA";

    // Act
    const bool valid = Core::FMTTheme::validate(getThemes(), mask);

    // Assert
    EXPECT_TRUE(valid);
    EXPECT_EQ(mask, "UNITE1 PEUPLEMENT1 UTR1");
}

TEST(FMTTheme, GivenAMaskSeparatedByTabsWhenValidateThenSeparatesItsValuesBySpaces)
{
    // Arrange
    std::string mask = "UNITE1\tPEUPLEMENT1\t\tUTR1";

    // Act
    const bool valid = Core::FMTTheme::validate(getThemes(), mask);

    // Assert
    EXPECT_TRUE(valid);
    EXPECT_EQ(mask, "UNITE1 PEUPLEMENT1 UTR1");
}

TEST(FMTTheme, GivenAMaskWithAnUnknownValueWhenValidateThenRaisesAnUndefinedAttribute)
{
    // Arrange
    std::string mask = "UNITE1 PEUPLEMENT9 UTR1";

    // Act
    const std::vector<Exception::FMTexc> raised = Testing::getRaisedTypes([&]()
        {
        Core::FMTTheme::validate(getThemes(), mask, " at line 12");
        });

    // Assert
    EXPECT_TRUE(Testing::hasType(raised, Exception::FMTexc::FMTundefined_attribute));
}

TEST(FMTTheme, GivenANamedThemeWithNamedAttributesWhenConvertedToStringThenWritesItsLandscapeSection)
{
    // Arrange
    const Core::FMTTheme theme({ "A1", "A2" }, { "First", "Second" }, { "ALL" }, { { "A1", "A2" } }, NO_INDEXES, 0, 0,
        "STATUS");

    // Act
    const std::string written = std::string(theme);

    // Assert
    EXPECT_EQ(theme.getName(), "STATUS");
    EXPECT_EQ(theme.getAttributeNames(), std::vector<std::string>({ "First", "Second" }));
    EXPECT_EQ(written, "*THEME 1 STATUS\n A1 First\n A2 Second\n*AGGREGATE ALL\n A1\n A2\n");
}

// The theme is numbered from its index plus 1; an unnamed attribute is followed by the space of its empty name.
TEST(FMTTheme, GivenAThemeAsTheParserBuildsItWhenConvertedToStringThenWritesItsAttributesThenItsAggregates)
{
    // Arrange
    const Core::FMTTheme theme = getTerritoryTheme();

    // Act
    const std::string written = std::string(theme);

    // Assert
    EXPECT_EQ(written, "*THEME 3 \n UTR1 \n UTR2 \n UTR3 \n*AGGREGATE UTA1\n UTR1\n UTR2\n*AGGREGATE UTA2\n UTR3\n");
}

TEST(FMTTheme, GivenAnIndexedThemeWhenGetIndexThenReturnsTheValueOfTheIndexForTheAttribute)
{
    // Arrange
    const Core::FMTTheme theme = getIndexedTheme();

    // Act
    const double secondB = theme.getIndex("UTR2", "B");
    const double thirdA = theme.getIndex("UTR3", "A");

    // Assert
    EXPECT_TRUE(theme.useIndex());
    EXPECT_DOUBLE_EQ(secondB, 41.92);
    EXPECT_DOUBLE_EQ(thirdA, 11.29);
}

TEST(FMTTheme, GivenAnIndexedThemeWhenIsIndexThenAcceptsItsIndexesOnly)
{
    // Arrange
    const Core::FMTTheme theme = getIndexedTheme();

    // Act
    const bool hasIndex = theme.isIndex("A");
    const bool hasUnknownIndex = theme.isIndex("Z");
    const bool attributeHasIndex = theme.isIndex("UTR1", "B");
    const bool attributeHasUnknownIndex = theme.isIndex("UTR1", "Z");

    // Assert
    EXPECT_TRUE(hasIndex);
    EXPECT_FALSE(hasUnknownIndex);
    EXPECT_TRUE(attributeHasIndex);
    EXPECT_FALSE(attributeHasUnknownIndex);
}

TEST(FMTTheme, GivenAnIndexThatTheAttributeDoesNotHaveWhenGetIndexThenReturnsZero)
{
    // Arrange
    const Core::FMTTheme theme = getIndexedTheme();

    // Act
    const double value = theme.getIndex("UTR1", "Z");

    // Assert
    EXPECT_EQ(value, 0.0);
}

TEST(FMTTheme, GivenAnUnknownAttributeWhenGetIndexThenRaisesAnUndefinedAttribute)
{
    // Arrange
    const Core::FMTTheme theme = getIndexedTheme();

    // Act
    const std::vector<Exception::FMTexc> raised = Testing::getRaisedTypes([&]()
        {
        theme.getIndex("UTR9", "A");
        });

    // Assert
    EXPECT_TRUE(Testing::hasType(raised, Exception::FMTexc::FMTundefined_attribute));
}

TEST(FMTTheme, GivenAnIndexedThemeWhenConvertedToStringThenWritesTheIndexesOfEachAttribute)
{
    // Arrange
    const Core::FMTTheme theme = getIndexedTheme();

    // Act
    const std::string written = std::string(theme);

    // Assert
    EXPECT_EQ(written, "*THEME 3 \n UTR1  _INDEX(A=16.360000,B=37.800000)\n UTR2  _INDEX(A=19.410000,B=41.920000)\n"
        " UTR3  _INDEX(A=11.290000,B=23.950000)\n*AGGREGATE UTA1\n UTR1\n UTR2\n");
}

TEST(FMTTheme, GivenAThemeWhenCopiedThenTheCopyIsEqualAndFindsTheSameAttributes)
{
    // Arrange
    const Core::FMTTheme original = getStandTheme();

    // Act
    const Core::FMTTheme copy(original);
    Core::FMTTheme assigned;
    assigned = original;

    // Assert
    EXPECT_TRUE(copy == original);
    EXPECT_TRUE(assigned == original);
    EXPECT_EQ(copy.getAttributes("POURPL"), std::vector<std::string>({ "PEUPLEMENT2", "PEUPLEMENT3" }));
    EXPECT_EQ(assigned.getAttributes("PROD"), std::vector<std::string>({ "PEUPLEMENT1", "PEUPLEMENT2", "PEUPLEMENT3" }));
}

TEST(FMTTheme, GivenThemesThatDifferByTheirStartTheirNameOrAnAggregateWhenComparedThenTheyDiffer)
{
    // Arrange
    const Core::FMTTheme theme({ "A1", "A2" }, { "", "" }, { "ALL" }, { { "A1", "A2" } }, NO_INDEXES, 0, 0, "STATUS");
    const Core::FMTTheme otherStart({ "A1", "A2" }, { "", "" }, { "ALL" }, { { "A1", "A2" } }, NO_INDEXES, 0, 3, "STATUS");
    const Core::FMTTheme otherName({ "A1", "A2" }, { "", "" }, { "ALL" }, { { "A1", "A2" } }, NO_INDEXES, 0, 0, "OTHER");
    const Core::FMTTheme otherAggregate({ "A1", "A2" }, { "", "" }, { "FIRST" }, { { "A1" } }, NO_INDEXES, 0, 0, "STATUS");

    // Act
    const bool equalToStart = (theme == otherStart);
    const bool equalToName = (theme == otherName);
    const bool equalToAggregate = (theme == otherAggregate);

    // Assert
    EXPECT_FALSE(equalToStart);
    EXPECT_FALSE(equalToName);
    EXPECT_FALSE(equalToAggregate);
}

// The presolve keeps the attributes of the filter: a theme left with two of them gets the next index and start,
// and keeps the aggregates that still hold one of them.
TEST(FMTTheme, GivenAFilterOfTwoAttributesWhenPreSolveThenKeepsThemWithTheAggregatesThatHoldThem)
{
    // Arrange: the developments of UNITE1 PEUPLEMENT1 UTR1 and UNITE1 PEUPLEMENT2 UTR1
    Core::FMTMaskFilter filter = getPreSolveFilter({ "UNITE1 PEUPLEMENT1 UTR1", "UNITE1 PEUPLEMENT2 UTR1" });
    const Core::FMTTheme theme = getStandTheme();
    size_t newId = 0;
    size_t newStart = 0;

    // Act
    const Core::FMTTheme presolved = theme.preSolve(filter, newId, newStart);

    // Assert
    EXPECT_EQ(presolved.getBaseAttributes(), std::vector<std::string>({ "PEUPLEMENT1", "PEUPLEMENT2" }));
    EXPECT_EQ(presolved.getId(), 0u);
    EXPECT_EQ(presolved.getStart(), 0u);
    EXPECT_EQ(newId, 1u);
    EXPECT_EQ(newStart, 2u);
    EXPECT_EQ(presolved.getAggregates(), std::vector<std::string>({ "PROD", "POURPL" }));
    EXPECT_EQ(presolved.getAttributes("POURPL"), std::vector<std::string>({ "PEUPLEMENT2" }));
}

// A theme left with a single attribute no longer tells developments apart: the presolve returns an empty theme, which
// FMTModel::preSolve removes, and the next theme keeps the index and the start.
TEST(FMTTheme, GivenAFilterOfASingleAttributeWhenPreSolveThenReturnsAnEmptyTheme)
{
    // Arrange
    Core::FMTMaskFilter filter = getPreSolveFilter({ "UNITE1 PEUPLEMENT1 UTR1", "UNITE1 PEUPLEMENT2 UTR1" });
    const Core::FMTTheme theme = getUnitTheme();
    size_t newId = 0;
    size_t newStart = 0;

    // Act
    const Core::FMTTheme presolved = theme.preSolve(filter, newId, newStart);

    // Assert
    EXPECT_TRUE(presolved.empty());
    EXPECT_EQ(newId, 0u);
    EXPECT_EQ(newStart, 0u);
}

TEST(FMTTheme, GivenAnEmptyFilterWhenPreSolveThenRaisesAnInvalidMaskRange)
{
    // Arrange
    Core::FMTMaskFilter filter;
    const Core::FMTTheme theme = getStandTheme();
    size_t newId = 0;
    size_t newStart = 0;

    // Act
    const std::vector<Exception::FMTexc> raised = Testing::getRaisedTypes([&]()
        {
        theme.preSolve(filter, newId, newStart);
        });

    // Assert
    EXPECT_TRUE(Testing::hasType(raised, Exception::FMTexc::FMTinvalid_maskrange));
}

TEST(FMTTheme, GivenAMaskOfTheAttributesOfAnAggregateWhenUpdateFromMaskThenReturnsThatAggregate)
{
    // Arrange
    Core::FMTTheme theme = getStandTheme();
    const Core::FMTMask mask("? POURPL ?", getThemes());

    // Act
    const std::string value = theme.updateFromMask(mask);

    // Assert
    EXPECT_EQ(value, "POURPL");
    EXPECT_EQ(theme.getAggregates(), std::vector<std::string>({ "PROD", "POURPL" }));
}

// A mask of several attributes that neither an aggregate nor a negation names, such as the union of two masks, gets
// a new aggregate named ~FMT<index of the theme>A_<number>, numbered from 1.
TEST(FMTTheme, GivenMasksOfAttributesThatNoAggregateHoldsWhenUpdateFromMaskThenCreatesANumberedAggregateForEach)
{
    // Arrange
    const std::vector<Core::FMTTheme> themes = getThemes();
    Core::FMTTheme theme = getStandTheme();
    const Core::FMTMask firstAndSecond =
        Core::FMTMask("? PEUPLEMENT1 ?", themes).getUnion(Core::FMTMask("? PEUPLEMENT2 ?", themes));
    const Core::FMTMask firstAndThird =
        Core::FMTMask("? PEUPLEMENT1 ?", themes).getUnion(Core::FMTMask("? PEUPLEMENT3 ?", themes));

    // Act
    const std::string first = theme.updateFromMask(firstAndSecond);
    const std::string second = theme.updateFromMask(firstAndThird);

    // Assert
    EXPECT_EQ(first, "~FMT1A_1");
    EXPECT_EQ(second, "~FMT1A_2");
    EXPECT_EQ(theme.getAttributes("~FMT1A_1"), std::vector<std::string>({ "PEUPLEMENT1", "PEUPLEMENT2" }));
    EXPECT_EQ(theme.getAttributes("~FMT1A_2"), std::vector<std::string>({ "PEUPLEMENT1", "PEUPLEMENT3" }));
}

TEST(FMTTheme, GivenAMaskOfOneAttributeWhenUpdateFromMaskThenReturnsTheAttributeWithoutNewAggregate)
{
    // Arrange
    Core::FMTTheme theme = getStandTheme();
    const Core::FMTMask mask("UNITE1 FEU UTR1", getThemes());

    // Act
    const std::string value = theme.updateFromMask(mask);

    // Assert
    EXPECT_EQ(value, "FEU");
    EXPECT_EQ(theme.getAggregates().size(), 2u);
}
