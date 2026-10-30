/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "PickleArchive.h"
#include "FMTDevelopment.h"
#include "FMTMask.h"
#include "FMTTimeYieldHandler.h"
#include "FMTYieldRequest.h"

#include <gtest/gtest.h>

#include <string>

// FMTTimeYieldHandler through the archive of the pickle of Python, as Python pickles an FMTtimeyieldhandler: by value.
// FMTYieldHandler::operator== compares the mask and the base ages only (issue #383): the handlers are compared by the
// values of their yields.

namespace
{
    // The value of p_yield for a development at period p_period: the handler reads only the period of the request.
    double getAtPeriod(const Core::FMTYieldHandler& p_handler, const std::string& p_yield, int p_period)
    {
        const Core::FMTDevelopment DEVELOPMENT(Core::FMTMask(), 0, 0, p_period);
        const Core::FMTYieldRequest REQUEST(DEVELOPMENT);
        return p_handler.get(p_yield, REQUEST);
    }
}

TEST(FMTTimeYieldHandler, GivenATimeHandlerWhenPickledThenGivesTheSameValuesAtEveryPeriod)
{
    // Arrange: PERIODVALUE of 10, 20 and 30 from the period 1
    Core::FMTTimeYieldHandler handler;
    handler.setYieldValues("PERIODVALUE", 1, { 10.0, 20.0, 30.0 });

    // Act
    const Core::FMTTimeYieldHandler unpickled = Testing::pickleAndUnpickle(handler);

    // Assert
    EXPECT_EQ(unpickled.getYieldNames(), handler.getYieldNames());
    EXPECT_DOUBLE_EQ(getAtPeriod(unpickled, "PERIODVALUE", 1), 10.0);
    EXPECT_DOUBLE_EQ(getAtPeriod(unpickled, "PERIODVALUE", 3), 30.0);
    EXPECT_DOUBLE_EQ(getAtPeriod(unpickled, "PERIODVALUE", 3), getAtPeriod(handler, "PERIODVALUE", 3));
    EXPECT_EQ(std::string(unpickled), std::string(handler));
}
