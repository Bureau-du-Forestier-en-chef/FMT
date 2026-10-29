/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "PickleArchive.h"
#include "FMTAgeYieldHandler.h"
#include "FMTComplexYieldHandler.h"
#include "FMTData.h"
#include "FMTDevelopment.h"
#include "FMTMask.h"
#include "FMTTheme.h"
#include "FMTYieldRequest.h"
#include "FMTYields.h"
#include "FMTutility.h"

#include <gtest/gtest.h>

#include <string>
#include <vector>

// FMTComplexYieldHandler and its FMTData through the archive of the pickle of Python, by value: FMTYields saves its
// handlers through a pointer, which does not save (Tests/Integration/Serialization/FMTYields.cpp). A complex yield is
// computed only through FMTYields::get: the unpickled handler joins yields built in code, as in
// Tests/Unit/Core/FMTComplexYieldHandler.cpp. The names of its yields are used by no other case of the process, since
// the cache of the complex yields is shared by the process (defect 22, issue #403).

namespace
{
    std::vector<Core::FMTTheme> getThemes()
    {
        return { Core::FMTTheme({ "A1", "A2" }, 0, 0, "THEME1") };
    }

    // The age yield PICKLEDVOLUME (100, 200 and 240 at the ages 10, 20 and 30), then p_complex.
    Core::FMTYields getYields(const Core::FMTComplexYieldHandler& p_complex)
    {
        const Core::FMTMask ALL("?", getThemes());
        Core::FMTAgeYieldHandler ageYields(ALL);
        ageYields.setYieldValues("PICKLEDVOLUME", { 10, 20, 30 }, { 100.0, 200.0, 240.0 });
        Core::FMTYields yields;
        yields.pushBackAgeHandler(ALL, ageYields);
        yields.push_back(ALL, p_complex.clone());
        yields.update();
        return yields;
    }

    double getYield(const Core::FMTYields& p_yields, const std::string& p_yield, int p_age)
    {
        const Core::FMTDevelopment DEVELOPMENT(Core::FMTMask("A1", getThemes()), p_age, 0, 1);
        const Core::FMTYieldRequest REQUEST(DEVELOPMENT);
        return p_yields.get(REQUEST, p_yield);
    }
}

TEST(FMTComplexYieldHandler, GivenAComplexHandlerWhenPickledThenComputesTheSameYields)
{
    // Arrange: PICKLEDHALF _MULTIPLY(PICKLEDVOLUME,0.5) and PICKLEDSUM _SUM(PICKLEDVOLUME,4)
    Core::FMTComplexYieldHandler handler(Core::FMTMask("?", getThemes()));
    handler.pushData("PICKLEDHALF",
        Core::FMTData({ 0.5 }, Core::FMTyieldparserop::FMTmultiply, { "PICKLEDVOLUME" }, { true, false }));
    handler.pushData("PICKLEDSUM",
        Core::FMTData({ 4.0 }, Core::FMTyieldparserop::FMTsum, { "PICKLEDVOLUME" }, { true, false }));

    // Act
    const Core::FMTComplexYieldHandler unpickled = Testing::pickleAndUnpickle(handler);

    // Assert
    const Core::FMTYields yields = getYields(unpickled);
    EXPECT_EQ(unpickled.getYieldNames(), handler.getYieldNames());
    EXPECT_EQ(std::string(unpickled), std::string(handler));
    EXPECT_DOUBLE_EQ(getYield(yields, "PICKLEDHALF", 20), 100.0);
    EXPECT_DOUBLE_EQ(getYield(yields, "PICKLEDSUM", 20), 204.0);
}
