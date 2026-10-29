/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "PickleArchive.h"
#include "ParserRoundTrip.h"
#include "FMTLifespans.h"
#include "FMTMask.h"
#include "FMTModel.h"

#include <gtest/gtest.h>

#include <string>
#include <vector>

// FMTLifespans through the archive of the pickle of Python, and through it FMTList, which saves the cache of findSets
// with its data (FMTList::save).

namespace
{
    // The lifespans that findSets gives for a mask, in their order.
    std::vector<int> findLifespans(const Core::FMTLifespans& p_lifespans, const Core::FMTMask& p_mask)
    {
        std::vector<int> lifespans;
        for (const Core::FMTLifespans::const_iterator& lifespan : p_lifespans.findSets(p_mask))
        {
            lifespans.push_back(lifespan->second);
        }
        return lifespans;
    }
}

// Defect 25 (#408) of Tests/ETAT.md: FMTList::save writes its base FMTObject, which FMTList::load does not read.
TEST(FMTLifespans, DISABLED_GivenTheLifespansOfTWDLandWhenPickledThenUnpicklesTheSameLifespans)
{
    // Arrange
    const Core::FMTLifespans lifespans = Testing::readScenario("ROOT").getLifespan();

    // Act
    const Core::FMTLifespans unpickled = Testing::pickleAndUnpickle(lifespans);

    // Assert
    ASSERT_FALSE(lifespans.empty());
    EXPECT_TRUE(unpickled == lifespans);
    EXPECT_EQ(std::string(unpickled), std::string(lifespans));
}

// Defect 25 (#408) of Tests/ETAT.md, as above. The cache of findSets is saved with the lifespans: the unpickled
// lifespans must find the same lifespans for a mask already looked for.
TEST(FMTLifespans, DISABLED_GivenLifespansWhoseCacheOfFindSetsIsFilledWhenPickledThenFindTheSameLifespans)
{
    // Arrange
    const Models::FMTModel model = Testing::readScenario("ROOT");
    const Core::FMTLifespans lifespans = model.getLifespan();
    const Core::FMTMask mask("UNITE1 FEU UTR1", model.getThemes());
    const std::vector<int> expected = findLifespans(lifespans, mask);

    // Act
    const Core::FMTLifespans unpickled = Testing::pickleAndUnpickle(lifespans);

    // Assert
    ASSERT_FALSE(expected.empty());
    EXPECT_EQ(findLifespans(unpickled, mask), expected);
}
