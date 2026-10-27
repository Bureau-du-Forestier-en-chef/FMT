/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

// FMTLookup.h uses std::string and std::vector without including them (Tests/ETAT.md, section 7).
#include <string>
#include <vector>

#include "FMTLookup.h"
#include "FMTDevelopment.h"
#include "FMTMask.h"
#include "FMTTheme.h"
#include "FMTutility.h"

#include <gtest/gtest.h>

#include <boost/functional/hash.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/unordered_set.hpp>

#include <cstddef>
#include <map>

// FMTLookup is a template: it is tested through its instantiations in FMT. FMTGraph keeps the developments of a period in
// a set of FMTLookup<vertex descriptor, FMTDevelopment> (FMTGraph::getDevsSet): a lookup kept holds the vertex and the
// address of its development, a lookup to look for holds the address of a development only, and both compare and hash
// the development they point to (FMTGraph::containsDevelopment). The vertex descriptor is that of the adjacency list of
// FMTGraph (vecS, vecS, bidirectionalS). FMTLookup.h also specializes the hash of FMTLookup<std::vector<size_t>,
// std::string>, which nothing in FMT uses. The themes are those of Tests/Unit/Core/FMTTheme.cpp.

namespace
{
    typedef boost::graph_traits<boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS>>::vertex_descriptor
        VertexDescriptor;
    typedef Core::FMTLookup<VertexDescriptor, Core::FMTDevelopment> VertexLookup;

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

    Core::FMTDevelopment getDevelopment(const std::string& p_mask, int p_age)
    {
        return Core::FMTDevelopment(Core::FMTMask(p_mask, getThemes()), p_age, 0, 1);
    }
}

TEST(FMTLookup, GivenTheDevelopmentsOfAPeriodInAHashedSetWhenFindALookupOfAnEqualDevelopmentThenGivesTheVertexKeptWithIt)
{
    // Arrange: the vertices 4 and 7 of the graph
    const Core::FMTDevelopment first = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 10);
    const Core::FMTDevelopment second = getDevelopment("UNITE2 PEUPLEMENT1 UTR1", 10);
    boost::unordered_set<VertexLookup> developments;
    developments.insert(VertexLookup(4, first));
    developments.insert(VertexLookup(7, second));
    const Core::FMTDevelopment searched = getDevelopment("UNITE2 PEUPLEMENT1 UTR1", 10);

    // Act
    const boost::unordered_set<VertexLookup>::const_iterator found = developments.find(VertexLookup(searched));

    // Assert
    ASSERT_TRUE(found != developments.end());
    EXPECT_EQ(found->memoryobject, 7u);
    EXPECT_EQ(found->pointerobject, &second);
}

TEST(FMTLookup, GivenTheDevelopmentsOfAPeriodInAHashedSetWhenFindALookupOfAnotherDevelopmentThenFindsNothing)
{
    // Arrange
    const Core::FMTDevelopment kept = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 10);
    boost::unordered_set<VertexLookup> developments;
    developments.insert(VertexLookup(4, kept));
    const Core::FMTDevelopment older = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 11);

    // Act
    const bool found = (developments.find(VertexLookup(older)) != developments.end());

    // Assert
    EXPECT_FALSE(found);
}

TEST(FMTLookup, GivenLookupsWhenComparedThenTheyCompareTheirDevelopmentsWhateverTheirVertices)
{
    // Arrange
    const Core::FMTDevelopment young = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 10);
    const Core::FMTDevelopment sameYoung = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 10);
    const Core::FMTDevelopment old = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 20);
    const VertexLookup first(1, young);
    const VertexLookup second(2, sameYoung);
    const VertexLookup third(1, old);

    // Act
    const bool firstEqualsSecond = (first == second);
    const bool firstEqualsThird = (first == third);
    const bool firstBeforeSecond = (first < second);
    const bool firstBeforeThird = (first < third);

    // Assert
    EXPECT_TRUE(firstEqualsSecond);
    EXPECT_FALSE(firstEqualsThird);
    EXPECT_FALSE(firstBeforeSecond);
    EXPECT_EQ(firstBeforeThird, young < old);
    EXPECT_EQ(third < first, old < young);
}

TEST(FMTLookup, GivenLookupsOfEqualDevelopmentsWhenHashThenTheyHaveTheHashOfTheirDevelopment)
{
    // Arrange
    const Core::FMTDevelopment development = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 10);
    const Core::FMTDevelopment sameDevelopment = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 10);

    // Act
    const size_t keptHash = boost::hash<VertexLookup>()(VertexLookup(3, development));
    const size_t searchedHash = boost::hash<VertexLookup>()(VertexLookup(sameDevelopment));

    // Assert
    EXPECT_EQ(keptHash, searchedHash);
    EXPECT_EQ(keptHash, development.hash());
}

// A lookup keeps the address of its development, not a copy: it sees what becomes of it, and so does its copy.
TEST(FMTLookup, GivenALookupWhenItsDevelopmentChangesThenTheLookupAndItsCopySeeTheChange)
{
    // Arrange
    Core::FMTDevelopment development = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 10);
    const VertexLookup lookup(5, development);
    const VertexLookup copy(lookup);
    VertexLookup assigned;
    assigned = lookup;
    const Core::FMTDevelopment older = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 15);

    // Act
    development.setAge(15);

    // Assert
    EXPECT_TRUE(lookup == VertexLookup(older));
    EXPECT_TRUE(copy == VertexLookup(older));
    EXPECT_EQ(assigned.memoryobject, 5u);
    EXPECT_EQ(assigned.pointerobject, &development);
}

// A lookup without development (a null pointer, as a value-initialized lookup has) is equal to no lookup, and comes before
// every lookup, itself included: the order is not strict on it. FMT always builds its lookups from a development.
TEST(FMTLookup, GivenALookupWithoutDevelopmentWhenComparedThenItIsEqualToNoneAndComesBeforeEveryLookupItselfIncluded)
{
    // Arrange
    const Core::FMTDevelopment development = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 10);
    const VertexLookup none{};
    const VertexLookup some(development);

    // Act
    const bool noneBeforeSome = (none < some);
    const bool someBeforeNone = (some < none);
    const bool noneBeforeItself = (none < none);

    // Assert
    EXPECT_EQ(none.pointerobject, nullptr);
    EXPECT_TRUE(noneBeforeSome);
    EXPECT_FALSE(someBeforeNone);
    EXPECT_TRUE(noneBeforeItself);
    EXPECT_FALSE(none == none);
    EXPECT_FALSE(none == some);
}

TEST(FMTLookup, GivenLookupsOfEqualNamesWhenHashedAsNamesThenTheyHaveTheHashOfTheNameAndFindEachOther)
{
    // Arrange
    typedef Core::FMTLookup<std::vector<size_t>, std::string> NameLookup;
    const std::string name = "COUPETOTALE";
    const std::string sameName = "COUPETOTALE";
    boost::unordered_set<NameLookup> names;
    names.insert(NameLookup({ 1, 2 }, name));

    // Act
    const size_t hash = boost::hash<NameLookup>()(NameLookup(sameName));
    const boost::unordered_set<NameLookup>::const_iterator found = names.find(NameLookup(sameName));

    // Assert
    EXPECT_EQ(hash, boost::hash<std::string>()(name));
    ASSERT_TRUE(found != names.end());
    EXPECT_EQ(found->memoryobject, std::vector<size_t>({ 1, 2 }));
}
