/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTLockBounds.h"
#include "FMTutility.h"

#include <gtest/gtest.h>

#include <limits>
#include <string>

// The bounds of a lock, as the parsers build them: the lock of a transition target (_LOCK <n>, FMTTransitionParser),
// and the locked inventory of an output source (_INVLOCK, from lock 1 up, FMTOutputParser).

TEST(FMTLockBounds, GivenLockBoundsWhenCopiedThenTheCopyIsEqual)
{
    // Arrange
    const Core::FMTLockBounds original(Core::FMTsection::Transition, Core::FMTkwor::Target, 2, 2);

    // Act
    const Core::FMTLockBounds copy(original);
    Core::FMTLockBounds assigned;
    assigned = original;

    // Assert
    EXPECT_TRUE(copy == original);
    EXPECT_TRUE(assigned == original);
    EXPECT_EQ(assigned.getLower(), 2);
}

TEST(FMTLockBounds, GivenLocksThatDifferWhenComparedThenTheyDiffer)
{
    // Arrange
    const Core::FMTLockBounds first(Core::FMTsection::Transition, Core::FMTkwor::Target, 1, 1);
    const Core::FMTLockBounds second(Core::FMTsection::Transition, Core::FMTkwor::Target, 2, 2);

    // Act
    const bool equal = (first == second);

    // Assert
    EXPECT_FALSE(equal);
}

TEST(FMTLockBounds, GivenTheLockOfATransitionTargetWhenConvertedToStringThenWritesTheLockKeyword)
{
    // Arrange
    const Core::FMTLockBounds bounds(Core::FMTsection::Transition, Core::FMTkwor::Target, 2, 2);

    // Act
    const std::string written = std::string(bounds);

    // Assert
    EXPECT_EQ(written, "_LOCK 2");
}

TEST(FMTLockBounds, GivenTheLockedInventoryOfAnOutputSourceWhenConvertedToStringThenWritesInvLock)
{
    // Arrange
    const Core::FMTLockBounds bounds(Core::FMTsection::Outputs, Core::FMTkwor::Source,
        (std::numeric_limits<int>::max)(), 1);

    // Act
    const std::string written = std::string(bounds);

    // Assert
    EXPECT_EQ(written, "_INVLOCK");
}

TEST(FMTLockBounds, GivenTheLockedInventoryOfAnOutputSourceWhenInThenAcceptsTheLockedDevelopmentsOnly)
{
    // Arrange
    const Core::FMTLockBounds bounds(Core::FMTsection::Outputs, Core::FMTkwor::Source,
        (std::numeric_limits<int>::max)(), 1);

    // Act
    const bool acceptsUnlocked = bounds.in(0);
    const bool acceptsLocked = bounds.in(3);

    // Assert
    EXPECT_FALSE(acceptsUnlocked);
    EXPECT_TRUE(acceptsLocked);
}
