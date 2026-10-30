/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "PickleArchive.h"
#include "FMTAgeYieldHandler.h"
#include "FMTDevelopment.h"
#include "FMTMask.h"
#include "FMTYieldRequest.h"

#include <gtest/gtest.h>

#include <string>
#include <vector>

// FMTAgeYieldHandler through the archive of the pickle of Python, as Python pickles an FMTageyieldhandler: by value.
// FMTYieldHandler::operator== compares the mask and the base ages only (issue #383): the handlers are compared by the
// values of their yields.

namespace
{
    // The value of p_yield for a development of age p_age: the handler reads only the age of the request.
    double getAtAge(const Core::FMTYieldHandler& p_handler, const std::string& p_yield, int p_age)
    {
        const Core::FMTDevelopment DEVELOPMENT(Core::FMTMask(), p_age, 0);
        const Core::FMTYieldRequest REQUEST(DEVELOPMENT);
        return p_handler.get(p_yield, REQUEST);
    }
}

TEST(FMTAgeYieldHandler, GivenAnAgeHandlerWhenPickledThenGivesTheSameValuesAtEveryAge)
{
    // Arrange: VOLUME of 100, 200 and 240 at the base ages 10, 20 and 30, STEMS of 50, 60 and 70
    Core::FMTAgeYieldHandler handler;
    handler.setYieldValues("VOLUME", { 10, 20, 30 }, { 100.0, 200.0, 240.0 });
    handler.setYieldValues("STEMS", { 10, 20, 30 }, { 50.0, 60.0, 70.0 });

    // Act
    const Core::FMTAgeYieldHandler unpickled = Testing::pickleAndUnpickle(handler);

    // Assert
    EXPECT_EQ(unpickled.getYieldNames(), handler.getYieldNames());
    EXPECT_EQ(unpickled.getBases(), handler.getBases());
    EXPECT_DOUBLE_EQ(getAtAge(unpickled, "VOLUME", 15), 150.0);
    EXPECT_DOUBLE_EQ(getAtAge(unpickled, "VOLUME", 30), 240.0);
    EXPECT_DOUBLE_EQ(getAtAge(unpickled, "STEMS", 20), 60.0);
    EXPECT_EQ(std::string(unpickled), std::string(handler));
}

TEST(FMTAgeYieldHandler, GivenAnAgeHandlerWithoutDataWhenPickledThenUnpicklesAHandlerWithoutYield)
{
    // Arrange
    const Core::FMTAgeYieldHandler handler;

    // Act
    const Core::FMTAgeYieldHandler unpickled = Testing::pickleAndUnpickle(handler);

    // Assert
    EXPECT_TRUE(unpickled.getYieldNames().empty());
    EXPECT_TRUE(unpickled.getBases().empty());
}
