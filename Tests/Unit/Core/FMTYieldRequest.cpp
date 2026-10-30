/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTYieldRequest.h"
#include "FMTAgeYieldHandler.h"
#include "FMTDevelopment.h"
#include "FMTMask.h"
#include "FMTTheme.h"
#include "FMTYields.h"
#include "FMTutility.h"

#include "ExceptionChain.h"

#include <gtest/gtest.h>

#include <map>
#include <string>
#include <vector>

// A yield request is what FMTYields::get reads a yield through: the address of a development and, from its first use by
// FMTYields (_updateData), the resume mask of the development (its mask through the filter of the yields) and the
// handlers whose mask selects it (Documentation/Architecture.md, Known Architectural Debt). The themes are those of
// Tests/Unit/Core/FMTTheme.cpp. A request keeps the address of its development, which must outlive it.

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

    Core::FMTDevelopment getDevelopment(const std::string& p_mask, int p_age)
    {
        return Core::FMTDevelopment(getMask(p_mask), p_age, 0, 1);
    }

    // An age handler of the mask p_mask with p_yield, of p_values at the ages 10, 20 and 30.
    Core::FMTAgeYieldHandler getAgeHandler(const std::string& p_mask, const std::string& p_yield,
        const std::vector<double>& p_values)
    {
        Core::FMTAgeYieldHandler handler(getMask(p_mask));
        handler.setYieldValues(p_yield, { 10, 20, 30 }, p_values);
        return handler;
    }

    // Three sections: VOLUME of UNITE1 (1000, 2000 and 2400), VOLUME of every development (100, 200 and 240), and STEMS
    // of UNITE2 (50, 60 and 70). The first handler that selects a development gives its value. No mask names a
    // single attribute of the second and third themes, so that the resume mask of a development keeps only its
    // attribute of the first theme.
    Core::FMTYields getYields()
    {
        Core::FMTYields yields;
        yields.pushBackAgeHandler(getMask("UNITE1 ? ?"), getAgeHandler("UNITE1 ? ?", "VOLUME", { 1000.0, 2000.0, 2400.0 }));
        yields.pushBackAgeHandler(getMask("? ? ?"), getAgeHandler("? ? ?", "VOLUME", { 100.0, 200.0, 240.0 }));
        yields.pushBackAgeHandler(getMask("UNITE2 ? ?"), getAgeHandler("UNITE2 ? ?", "STEMS", { 50.0, 60.0, 70.0 }));
        yields.update();
        return yields;
    }
}

TEST(FMTYieldRequest, GivenADevelopmentWhenConstructedThenReadsItThroughItsAddressWithoutGraphNorHandlers)
{
    // Arrange
    const Core::FMTDevelopment development = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 20);

    // Act
    const Core::FMTYieldRequest request(development);

    // Assert
    EXPECT_EQ(&request.getDevelopment(), &development);
    EXPECT_EQ(request.getVertexGraphInfo(), nullptr);
    EXPECT_TRUE(request.getDatas().empty());
    EXPECT_TRUE(request.getResumeMask().empty());
}

// The bits of the resume mask, the highest first: the seven bits of the second and third themes, which every mask of
// the yields covers, then UNITE3, UNITE2 and UNITE1.
TEST(FMTYieldRequest, GivenARequestWhenYieldsGetThenKeepsTheMaskOfTheDevelopmentThroughTheFilterOfTheYields)
{
    // Arrange
    const Core::FMTYields yields = getYields();
    const Core::FMTDevelopment development = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 20);
    const Core::FMTYieldRequest request(development);

    // Act
    yields.get(request, "VOLUME");

    // Assert
    EXPECT_TRUE(request.getResumeMask() == yields.filterMask(development.getMask()));
    EXPECT_EQ(request.getResumeMask().getBitsString(), "1111111001");
}

TEST(FMTYieldRequest, GivenDevelopmentsThatDifferOnlyByThemesThatNoMaskNamesWhenYieldsGetThenTheyHaveTheSameResumeMask)
{
    // Arrange
    const Core::FMTYields yields = getYields();
    const Core::FMTDevelopment first = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 20);
    const Core::FMTDevelopment second = getDevelopment("UNITE1 PEUPLEMENT3 UTR2", 20);
    const Core::FMTDevelopment otherUnit = getDevelopment("UNITE2 PEUPLEMENT1 UTR1", 20);
    const Core::FMTYieldRequest firstRequest(first);
    const Core::FMTYieldRequest secondRequest(second);
    const Core::FMTYieldRequest otherUnitRequest(otherUnit);

    // Act
    yields.get(firstRequest, "VOLUME");
    yields.get(secondRequest, "VOLUME");
    yields.get(otherUnitRequest, "VOLUME");

    // Assert
    EXPECT_TRUE(firstRequest.getResumeMask() == secondRequest.getResumeMask());
    EXPECT_FALSE(firstRequest.getResumeMask() == otherUnitRequest.getResumeMask());
}

TEST(FMTYieldRequest, GivenARequestPreparedByYieldsWhenGetDatasThenListsTheHandlersThatSelectTheDevelopmentInTheirOrder)
{
    // Arrange
    const Core::FMTYields yields = getYields();
    const Core::FMTDevelopment ofUnit1 = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 20);
    const Core::FMTDevelopment ofUnit2 = getDevelopment("UNITE2 PEUPLEMENT1 UTR1", 20);
    const Core::FMTYieldRequest unit1Request(ofUnit1);
    const Core::FMTYieldRequest unit2Request(ofUnit2);
    yields.get(unit1Request, "VOLUME");
    yields.get(unit2Request, "VOLUME");

    // Act
    const std::vector<Core::FMTYieldRequest::const_iterator> unit1Handlers = unit1Request.getDatas();
    const std::vector<Core::FMTYieldRequest::const_iterator> unit2Handlers = unit2Request.getDatas();

    // Assert
    ASSERT_EQ(unit1Handlers.size(), 2u);
    EXPECT_TRUE(unit1Handlers[0] == yields.begin());
    EXPECT_TRUE(unit1Handlers[1] == yields.begin() + 1);
    ASSERT_EQ(unit2Handlers.size(), 2u);
    EXPECT_TRUE(unit2Handlers[0] == yields.begin() + 1);
    EXPECT_TRUE(unit2Handlers[1] == yields.begin() + 2);
}

TEST(FMTYieldRequest, GivenAPreparedRequestWhenGetFirstSeenThenGivesTheFirstHandlerOfTheYieldOrTheEndOfTheYields)
{
    // Arrange
    const Core::FMTYields yields = getYields();
    const Core::FMTDevelopment development = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 20);
    const Core::FMTYieldRequest request(development);
    yields.get(request, "VOLUME");

    // Act
    const Core::FMTYieldRequest::const_iterator volume = request.getFirstSeen("VOLUME");
    const Core::FMTYieldRequest::const_iterator stems = request.getFirstSeen("STEMS");
    const Core::FMTYieldRequest::const_iterator unknown = request.getFirstSeen("UNKNOWN");

    // Assert
    EXPECT_TRUE(volume == yields.begin());
    EXPECT_TRUE(stems == yields.begin() + 2);
    EXPECT_TRUE(unknown == yields.end());
}

TEST(FMTYieldRequest, GivenAPreparedRequestWhenTheAgeOfItsDevelopmentChangesThenTheNextGetReadsTheNewAge)
{
    // Arrange
    const Core::FMTYields yields = getYields();
    Core::FMTDevelopment development = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 10);
    const Core::FMTYieldRequest request(development);
    const double atTen = yields.get(request, "VOLUME");

    // Act
    development.setAge(20);
    const double atTwenty = yields.get(request, "VOLUME");

    // Assert
    EXPECT_DOUBLE_EQ(atTen, 1000.0);
    EXPECT_DOUBLE_EQ(atTwenty, 2000.0);
}

// Known behavior (Documentation/Architecture.md, Known Architectural Debt): the request takes its resume mask and its
// handlers once, at its first use. The development of UNITE2 that the development of UNITE1 became still gets the
// VOLUME of UNITE1, where a new request gets 200.
TEST(FMTYieldRequest, GivenAPreparedRequestWhenTheMaskOfItsDevelopmentChangesThenKeepsTheHandlersOfTheFirstMask)
{
    // Arrange
    const Core::FMTYields yields = getYields();
    Core::FMTDevelopment development = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 20);
    const Core::FMTYieldRequest request(development);
    const double beforeChange = yields.get(request, "VOLUME");
    const Core::FMTDevelopment ofUnit2 = getDevelopment("UNITE2 PEUPLEMENT1 UTR1", 20);

    // Act
    development.setMask(getMask("UNITE2 PEUPLEMENT1 UTR1"));
    const double afterChange = yields.get(request, "VOLUME");
    const double ofANewRequest = yields.get(Core::FMTYieldRequest(ofUnit2), "VOLUME");

    // Assert
    EXPECT_DOUBLE_EQ(beforeChange, 2000.0);
    EXPECT_DOUBLE_EQ(afterChange, 2000.0);
    EXPECT_DOUBLE_EQ(ofANewRequest, 200.0);
}

TEST(FMTYieldRequest, GivenAPreviousRequestWhenConstructedForADevelopmentOfTheSameMaskThenKeepsItsHandlersAndReadsTheNewDevelopment)
{
    // Arrange
    const Core::FMTYields yields = getYields();
    const Core::FMTDevelopment young = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 10);
    const Core::FMTYieldRequest previous(young);
    yields.get(previous, "VOLUME");
    const Core::FMTDevelopment grown = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 20);

    // Act
    const Core::FMTYieldRequest request(grown, previous);
    const double value = yields.get(request, "VOLUME");

    // Assert
    EXPECT_EQ(&request.getDevelopment(), &grown);
    EXPECT_TRUE(request.getDatas() == previous.getDatas());
    EXPECT_TRUE(request.getResumeMask() == previous.getResumeMask());
    EXPECT_EQ(request.getVertexGraphInfo(), nullptr);
    EXPECT_DOUBLE_EQ(value, 2000.0);
}

TEST(FMTYieldRequest, GivenAPreviousRequestWhenConstructedForADevelopmentOfAnotherMaskThenRaisesAFunctionFailed)
{
    // Arrange
    const Core::FMTYields yields = getYields();
    const Core::FMTDevelopment ofUnit1 = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 10);
    const Core::FMTYieldRequest previous(ofUnit1);
    yields.get(previous, "VOLUME");
    const Core::FMTDevelopment ofUnit2 = getDevelopment("UNITE2 PEUPLEMENT1 UTR1", 10);

    // Act
    const std::vector<Exception::FMTexc> raised = Testing::getRaisedTypes([&]()
        {
        const Core::FMTYieldRequest request(ofUnit2, previous);
        });

    // Assert
    ASSERT_FALSE(raised.empty());
    EXPECT_EQ(raised.front(), Exception::FMTexc::FMTfunctionfailed);
}

TEST(FMTYieldRequest, GivenAPreparedRequestWhenCopiedThenTheCopyReadsTheSameDevelopmentThroughTheSameHandlers)
{
    // Arrange
    const Core::FMTYields yields = getYields();
    const Core::FMTDevelopment development = getDevelopment("UNITE1 PEUPLEMENT1 UTR1", 20);
    const Core::FMTYieldRequest request(development);
    yields.get(request, "VOLUME");

    // Act
    const Core::FMTYieldRequest copy(request);
    Core::FMTYieldRequest assigned;
    assigned = request;

    // Assert
    EXPECT_EQ(&copy.getDevelopment(), &development);
    EXPECT_TRUE(copy.getDatas() == request.getDatas());
    EXPECT_TRUE(copy.getResumeMask() == request.getResumeMask());
    EXPECT_EQ(&assigned.getDevelopment(), &development);
    EXPECT_DOUBLE_EQ(yields.get(copy, "VOLUME"), 2000.0);
    EXPECT_DOUBLE_EQ(yields.get(assigned, "VOLUME"), 2000.0);
}

TEST(FMTYieldRequest, GivenADevelopmentThatNoHandlerSelectsWhenYieldsGetThenTheRequestHasItsResumeMaskButNoHandler)
{
    // Arrange: VOLUME of UNITE1 only
    Core::FMTYields yields;
    yields.pushBackAgeHandler(getMask("UNITE1 ? ?"), getAgeHandler("UNITE1 ? ?", "VOLUME", { 1000.0, 2000.0, 2400.0 }));
    yields.update();
    const Core::FMTDevelopment development = getDevelopment("UNITE3 PEUPLEMENT1 UTR1", 20);
    const Core::FMTYieldRequest request(development);

    // Act
    const std::vector<Exception::FMTexc> raised = Testing::getRaisedTypes([&]()
        {
        yields.get(request, "VOLUME");
        });

    // Assert
    EXPECT_TRUE(Testing::hasType(raised, Exception::FMTexc::FMTmissingyield));
    EXPECT_FALSE(request.getResumeMask().empty());
    EXPECT_TRUE(request.getDatas().empty());
}
