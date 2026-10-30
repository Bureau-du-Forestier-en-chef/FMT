/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTAction.h"
#include "FMTAgeBounds.h"
#include "FMTMask.h"
#include "FMTMaskFilter.h"
#include "FMTPerBounds.h"
#include "FMTSerie.h"
#include "FMTSpec.h"
#include "FMTTheme.h"
#include "FMTYldBounds.h"
#include "FMTutility.h"

#include "ExceptionChain.h"

#include <gtest/gtest.h>

#include <boost/functional/hash.hpp>

#include <limits>
#include <map>
#include <string>
#include <utility>
#include <vector>

// The actions of TWD_land, built in code as FMTActionParser builds them: *ACTION <name> <Y or N> [_LOCKEXEMPT], then a
// spec per mask of *OPERABLE. FMTModel then calls update, which shrinks the masks and sets the age and period bounds of
// the action; the aggregation of FMTModel::aggregateActions adds actions that are not updated (operator+=). The themes
// are those of Tests/Unit/Core/FMTTheme.cpp.

namespace
{
    const int NO_UPPER_BOUND = (std::numeric_limits<int>::max)();
    const int NO_LOWER_BOUND = (std::numeric_limits<int>::lowest)();

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

    // _AGE >= p_lower AND _AGE <= p_upper
    Core::FMTSpec getAgeSpec(int p_lower, int p_upper)
    {
        Core::FMTSpec spec;
        spec.addBounds(Core::FMTAgeBounds(Core::FMTsection::Action, p_upper, p_lower));
        return spec;
    }

    // _CP >= p_lower AND _CP <= p_upper
    Core::FMTSpec getPeriodSpec(int p_lower, int p_upper)
    {
        Core::FMTSpec spec;
        spec.setBounds(Core::FMTPerBounds(Core::FMTsection::Action, p_upper, p_lower));
        return spec;
    }

    // VOLUMETOTAL >= p_lower
    Core::FMTSpec getVolumeSpec(double p_lower)
    {
        Core::FMTSpec spec;
        spec.addBounds(Core::FMTYldBounds(Core::FMTsection::Action, "VOLUMETOTAL", (std::numeric_limits<double>::max)(),
            p_lower));
        return spec;
    }

    // An action as FMTActionParser leaves it, before the update of FMTModel.
    Core::FMTAction buildAction(const std::string& p_name, bool p_respectLock, bool p_resetAge,
        const std::vector<std::pair<std::string, Core::FMTSpec>>& p_specs)
    {
        Core::FMTAction action(p_name, p_respectLock, p_resetAge);
        for (const std::pair<std::string, Core::FMTSpec>& spec : p_specs)
        {
            action.push_back(getMask(spec.first), spec.second);
        }
        return action;
    }

    Core::FMTAction getAction(const std::string& p_name, bool p_respectLock, bool p_resetAge,
        const std::vector<std::pair<std::string, Core::FMTSpec>>& p_specs)
    {
        Core::FMTAction action = buildAction(p_name, p_respectLock, p_resetAge, p_specs);
        action.update();
        return action;
    }

    // *ACTION COUPEtotale Y: ? prod ? volumetotal >= 100
    Core::FMTAction buildClearcut()
    {
        return buildAction("COUPETOTALE", true, true, { { "? PROD ?", getVolumeSpec(100.0) } });
    }

    // *ACTION aFire N: ? prod ? _AGE >= 1
    Core::FMTAction buildFire()
    {
        return buildAction("AFIRE", true, false, { { "? PROD ?", getAgeSpec(1, NO_UPPER_BOUND) } });
    }

    // *ACTION plantation Y _LOCKEXEMPT: ? prod ? _AGE >= 1 AND _AGE <= 3
    Core::FMTAction buildPlantation()
    {
        return buildAction("PLANTATION", false, true, { { "? PROD ?", getAgeSpec(1, 3) } });
    }

    // *ACTION aThinning N, *PARTIAL aThinning VOLUMETOTAL: ? prod ? _AGE >= 10
    Core::FMTAction buildThinning()
    {
        Core::FMTAction thinning = buildAction("ATHINNING", true, false, { { "? PROD ?", getAgeSpec(10, NO_UPPER_BOUND) } });
        thinning.pushPartials("VOLUMETOTAL");
        return thinning;
    }

    // *ACTION aCaribou N: UC prod ? _AGE >= 8
    Core::FMTAction getCaribou()
    {
        return getAction("ACARIBOU", true, false, { { "UC PROD ?", getAgeSpec(8, NO_UPPER_BOUND) } });
    }

    // *ACTIONSERIES: aCaribou -> COUPEtotale -> plantation, of length 3.
    Core::FMTSerie getSerie()
    {
        return Core::FMTSerie(std::vector<std::string>({ "ACARIBOU", "COUPETOTALE", "PLANTATION" }), 3);
    }

    // The presolve of Tests/Unit/Core/FMTMask.cpp: the filter of the union of UNITE1 PEUPLEMENT1 UTR1 and
    // UNITE1 PEUPLEMENT2 UTR1, and the themes left with more than one attribute, here PEUPLEMENT1 and PEUPLEMENT2.
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

TEST(FMTAction, GivenANameALockAndAnAgeResetWhenConstructedThenKeepsThemWithoutSpecification)
{
    // Arrange
    const std::string name = "COUPETOTALE";

    // Act
    const Core::FMTAction action(name, true, true);

    // Assert
    EXPECT_EQ(action.getName(), name);
    EXPECT_TRUE(action.doRespectLock());
    EXPECT_TRUE(action.isResetAge());
    EXPECT_TRUE(action.empty());
    EXPECT_EQ(action.size(), 0u);
    EXPECT_TRUE(action.getAggregates().empty());
    EXPECT_TRUE(action.getPartials().empty());
    EXPECT_FALSE(action.isPartOfASerie());
    EXPECT_FALSE(action.isInSeries());
    EXPECT_EQ(action.getAgeLowerBound(), 0);
    EXPECT_EQ(action.getAgeUpperBound(), 0);
    EXPECT_EQ(action.getPeriodLowerBound(), 0);
    EXPECT_EQ(action.getPeriodUpperBound(), 0);
}

// A name alone gives the flags of *ACTION <name> N _LOCKEXEMPT: FMTModel::aggregateActions starts from such an action,
// which takes the flags of the first action added to it (operator+=).
TEST(FMTAction, GivenANameOnlyWhenConstructedThenIsLockExemptAndKeepsTheAge)
{
    // Arrange
    const std::string name = "AFIRE";

    // Act
    const Core::FMTAction named(name);
    const Core::FMTAction unnamed;

    // Assert
    EXPECT_EQ(named.getName(), name);
    EXPECT_FALSE(named.doRespectLock());
    EXPECT_FALSE(named.isResetAge());
    EXPECT_EQ(unnamed.getName(), "");
    EXPECT_FALSE(unnamed.doRespectLock());
    EXPECT_FALSE(unnamed.isResetAge());
}

TEST(FMTAction, GivenAgeSpecificationsWhenUpdateThenTakesTheLowestAndTheHighestAgeOfThem)
{
    // Arrange: ? prod ? _AGE >= 1 AND _AGE <= 3, then UC ? ? _AGE >= 8
    Core::FMTAction action = buildAction("ACTION", true, false,
        { { "? PROD ?", getAgeSpec(1, 3) }, { "UC ? ?", getAgeSpec(8, NO_UPPER_BOUND) } });

    // Act
    action.update();

    // Assert
    EXPECT_EQ(action.getAgeLowerBound(), 1);
    EXPECT_EQ(action.getAgeUpperBound(), NO_UPPER_BOUND);
    EXPECT_EQ(action.getPeriodLowerBound(), 0);
    EXPECT_EQ(action.getPeriodUpperBound(), NO_UPPER_BOUND);
}

TEST(FMTAction, GivenPeriodSpecificationsWhenUpdateThenTakesTheFirstAndTheLastPeriodOfThem)
{
    // Arrange: ? prod ? _CP >= 2 AND _CP <= 3, then UC ? ? _CP = 5
    Core::FMTAction action = buildAction("ACTION", true, false,
        { { "? PROD ?", getPeriodSpec(2, 3) }, { "UC ? ?", getPeriodSpec(5, 5) } });

    // Act
    action.update();

    // Assert
    EXPECT_EQ(action.getPeriodLowerBound(), 2);
    EXPECT_EQ(action.getPeriodUpperBound(), 5);
    EXPECT_EQ(action.getAgeLowerBound(), 0);
    EXPECT_EQ(action.getAgeUpperBound(), NO_UPPER_BOUND);
}

TEST(FMTAction, GivenASpecificationWithoutAgeConditionWhenUpdateThenTheActionAcceptsEveryAge)
{
    // Arrange: ? prod ? _AGE >= 8, then ? feu ? volumetotal >= 100
    Core::FMTAction action = buildAction("ACTION", true, false,
        { { "? PROD ?", getAgeSpec(8, NO_UPPER_BOUND) }, { "? FEU ?", getVolumeSpec(100.0) } });

    // Act
    action.update();

    // Assert
    EXPECT_EQ(action.getAgeLowerBound(), 0);
    EXPECT_EQ(action.getAgeUpperBound(), NO_UPPER_BOUND);
}

// Without any spec, update leaves the bounds it starts from: the lower above the upper, so that no age and no period
// fall within them.
TEST(FMTAction, GivenAnActionWithoutSpecificationWhenUpdateThenItsBoundsAllowNoAgeAndNoPeriod)
{
    // Arrange
    Core::FMTAction action("ACTION", true, false);

    // Act
    action.update();

    // Assert
    EXPECT_EQ(action.getAgeLowerBound(), NO_UPPER_BOUND);
    EXPECT_EQ(action.getAgeUpperBound(), 0);
    EXPECT_EQ(action.getPeriodLowerBound(), NO_UPPER_BOUND);
    EXPECT_EQ(action.getPeriodUpperBound(), 0);
}

// #393: for _AGE > 10, FMTParser::bounds returns the bounds from the lowest to the largest integer, which
// FMTActionParser adds as the age bounds of the spec. The action does what these bounds say: it accepts every age, and
// is written without condition. The disabled cases of FMTActionParser (IntegrationParser) wait for the correction.
TEST(FMTAction, GivenAMinimumAgeWrittenWithGreaterThanAsTheParserReadsItWhenUpdateThenTheActionAcceptsEveryAge)
{
    // Arrange: ? prod ? _AGE > 10
    Core::FMTSpec spec;
    spec.addBounds(Core::FMTAgeBounds(Core::FMTsection::Action, NO_UPPER_BOUND, NO_LOWER_BOUND));
    Core::FMTAction action = buildAction("ASTRICT", true, false, { { "? PROD ?", spec } });

    // Act
    action.update();

    // Assert
    EXPECT_EQ(action.getAgeLowerBound(), NO_LOWER_BOUND);
    EXPECT_EQ(action.getAgeUpperBound(), NO_UPPER_BOUND);
    ASSERT_EQ(action.size(), 1u);
    EXPECT_TRUE(action.begin()->second.allowWithoutYield(1, 5, 0));
    EXPECT_EQ(std::string(action), "*ACTION ASTRICT N\n*OPERABLE ASTRICT\n? PROD ?  \n");
}

// Include/FMTAction.h documents a comparison of the names only: the code also compares the specs, the flags and the
// partials, but not the aggregates.
TEST(FMTAction, GivenActionsOfTheSameNameWhenComparedThenTheyAreEqualOnlyWithTheSameSpecificationsFlagsAndPartials)
{
    // Arrange
    const Core::FMTAction action = getAction("COUPETOTALE", true, true, { { "? PROD ?", getVolumeSpec(100.0) } });
    const Core::FMTAction otherSpec = getAction("COUPETOTALE", true, true, { { "? PROD ?", getVolumeSpec(150.0) } });
    const Core::FMTAction lockExempt = getAction("COUPETOTALE", false, true, { { "? PROD ?", getVolumeSpec(100.0) } });
    const Core::FMTAction withoutReset = getAction("COUPETOTALE", true, false, { { "? PROD ?", getVolumeSpec(100.0) } });
    Core::FMTAction withPartial(action);
    withPartial.pushPartials("VOLUMETOTAL");
    Core::FMTAction withAggregate(action);
    withAggregate.pushAggregate("COUPES");

    // Act
    const bool sameValues = (action == getAction("COUPETOTALE", true, true, { { "? PROD ?", getVolumeSpec(100.0) } }));
    const bool sameOtherSpec = (action == otherSpec);
    const bool sameLockExempt = (action == lockExempt);
    const bool sameWithoutReset = (action == withoutReset);
    const bool sameWithPartial = (action == withPartial);
    const bool sameWithAggregate = (action == withAggregate);
    const bool differentOtherSpec = (action != otherSpec);

    // Assert
    EXPECT_TRUE(sameValues);
    EXPECT_FALSE(sameOtherSpec);
    EXPECT_FALSE(sameLockExempt);
    EXPECT_FALSE(sameWithoutReset);
    EXPECT_FALSE(sameWithPartial);
    EXPECT_TRUE(sameWithAggregate);
    EXPECT_TRUE(differentOtherSpec);
}

// Include/FMTAction.h documents an order by the length of the name, then the name: the code compares the names only.
// ACARIBOU is longer than _DEATH, and comes first because A comes before _ in ASCII.
TEST(FMTAction, GivenActionsWhenComparedByOrderThenComparesTheirNamesOnly)
{
    // Arrange
    const Core::FMTAction caribou = getCaribou();
    const Core::FMTAction death("_DEATH", false, false);
    const Core::FMTAction otherCaribou = getAction("ACARIBOU", true, false, { { "? ? ?", getAgeSpec(1, 2) } });

    // Act
    const bool caribouFirst = (caribou < death);
    const bool deathFirst = (death < caribou);
    const bool beforeItself = (caribou < caribou);
    const bool beforeOtherSpec = (caribou < otherCaribou) || (otherCaribou < caribou);

    // Assert
    EXPECT_TRUE(caribouFirst);
    EXPECT_FALSE(deathFirst);
    EXPECT_FALSE(beforeItself);
    EXPECT_FALSE(beforeOtherSpec);
}

TEST(FMTAction, GivenActionsOfTheSameNameWhenHashThenTheirHashesAreEqual)
{
    // Arrange
    const Core::FMTAction clearcut = getAction("COUPETOTALE", true, true, { { "? PROD ?", getVolumeSpec(100.0) } });
    const Core::FMTAction otherClearcut = getAction("COUPETOTALE", false, false, { { "? ? ?", getAgeSpec(1, 2) } });
    const Core::FMTAction fire = getAction("AFIRE", true, true, { { "? PROD ?", getVolumeSpec(100.0) } });

    // Act
    const size_t clearcutHash = clearcut.hash();
    const size_t otherClearcutHash = boost::hash<Core::FMTAction>()(otherClearcut);
    const size_t fireHash = fire.hash();

    // Assert
    EXPECT_EQ(clearcutHash, otherClearcutHash);
    EXPECT_EQ(clearcutHash, boost::hash<std::string>()("COUPETOTALE"));
    EXPECT_NE(clearcutHash, fireHash);
}

TEST(FMTAction, GivenALockExemptActionWhenConvertedToStringThenWritesItsActionAndOperableSections)
{
    // Arrange
    Core::FMTAction plantation = buildPlantation();
    plantation.update();

    // Act
    const std::string written = std::string(plantation);

    // Assert
    EXPECT_EQ(written, "*ACTION PLANTATION Y _LOCKEXEMPT\n*OPERABLE PLANTATION\n? PROD ? _AGE >= 1 AND _AGE <= 3 \n");
}

TEST(FMTAction, GivenAPartialYieldWhenConvertedToStringThenWritesThePartialSection)
{
    // Arrange
    Core::FMTAction thinning = buildThinning();
    thinning.update();

    // Act
    const std::string written = std::string(thinning);

    // Assert
    EXPECT_EQ(written, "*ACTION ATHINNING N\n*OPERABLE ATHINNING\n? PROD ? _AGE >= 10 \n*PARTIAL ATHINNING\nVOLUMETOTAL ");
}

TEST(FMTAction, GivenAPartialYieldWhenPartialThenFindsThePushedYieldOnly)
{
    // Arrange
    const Core::FMTAction thinning = buildThinning();

    // Act
    const bool volumeIsPartial = thinning.partial("VOLUMETOTAL");
    const bool otherIsPartial = thinning.partial("VOLUMERESINEUX");

    // Assert
    EXPECT_TRUE(volumeIsPartial);
    EXPECT_FALSE(otherIsPartial);
    EXPECT_EQ(thinning.getPartials(), std::vector<std::string>({ "VOLUMETOTAL" }));
}

TEST(FMTAction, GivenYieldConditionsWhenUseYieldThenFindsTheYieldsOfItsSpecificationsOnly)
{
    // Arrange
    Core::FMTAction clearcut = buildClearcut();
    clearcut.update();
    Core::FMTAction fire = buildFire();
    fire.update();

    // Act
    const bool clearcutUsesVolume = clearcut.useYield("VOLUMETOTAL");
    const bool clearcutUsesOther = clearcut.useYield("VOLUME");
    const bool fireUsesVolume = fire.useYield("VOLUMETOTAL");

    // Assert
    EXPECT_TRUE(clearcutUsesVolume);
    EXPECT_FALSE(clearcutUsesOther);
    EXPECT_FALSE(fireUsesVolume);
}

TEST(FMTAction, GivenAnAggregateWhenIsPartOfThenMatchesItsNameAndItsAggregates)
{
    // Arrange: *AGGREGATE COUPES, COUPEtotale
    Core::FMTAction clearcut = buildClearcut();
    clearcut.pushAggregate("COUPES");

    // Act
    const bool partOfItsName = clearcut.isPartOf("COUPETOTALE");
    const bool partOfItsAggregate = clearcut.isPartOf("COUPES");
    const bool partOfOtherName = clearcut.isPartOf("AFIRE");

    // Assert
    EXPECT_TRUE(partOfItsName);
    EXPECT_TRUE(partOfItsAggregate);
    EXPECT_FALSE(partOfOtherName);
    EXPECT_EQ(clearcut.getAggregates(), std::vector<std::string>({ "COUPES" }));
}

TEST(FMTAction, GivenAnAggregateWhenSetAggregatesThenReplacesTheAggregates)
{
    // Arrange
    Core::FMTAction clearcut = buildClearcut();
    clearcut.pushAggregate("COUPES");

    // Act
    clearcut.setAggregates({ "RECOLTES" });

    // Assert
    EXPECT_EQ(clearcut.getAggregates(), std::vector<std::string>({ "RECOLTES" }));
    EXPECT_FALSE(clearcut.isPartOf("COUPES"));
}

// FMTModel::aggregateActions: an action of the name of the aggregate, to which the actions of the aggregate are added.
TEST(FMTAction, GivenAnEmptyActionWhenAddedAnActionThenTakesItsFlagsSpecificationsAndAggregates)
{
    // Arrange
    Core::FMTAction aggregate("COUPES");
    Core::FMTAction clearcut = buildClearcut();
    clearcut.pushAggregate("RECOLTES");

    // Act
    aggregate += clearcut;

    // Assert
    EXPECT_EQ(aggregate.getName(), "COUPES");
    EXPECT_TRUE(aggregate.doRespectLock());
    EXPECT_TRUE(aggregate.isResetAge());
    EXPECT_EQ(aggregate.size(), 1u);
    EXPECT_EQ(aggregate.getAggregates(), std::vector<std::string>({ "RECOLTES" }));
}

TEST(FMTAction, GivenAnAgeResetActionWhenAddedAnActionWithoutResetThenKeepsTheResetAndTakesItsPartials)
{
    // Arrange
    Core::FMTAction aggregate("COUPES");
    aggregate += buildClearcut();

    // Act
    aggregate += buildThinning();

    // Assert
    EXPECT_TRUE(aggregate.isResetAge());
    EXPECT_EQ(aggregate.getPartials(), std::vector<std::string>({ "VOLUMETOTAL" }));
    EXPECT_EQ(aggregate.size(), 2u);
}

// The partials are lost with a warning (FMTignore), not an error: the documentation of operator+= announces an
// exception for actions of different age resets.
TEST(FMTAction, GivenAnActionWithPartialsWhenAddedAnAgeResetActionThenLosesItsPartialsAndResetsTheAge)
{
    // Arrange
    Core::FMTAction aggregate("ECLAIRCIES");
    aggregate += buildThinning();

    // Act
    aggregate += buildClearcut();

    // Assert
    EXPECT_TRUE(aggregate.isResetAge());
    EXPECT_TRUE(aggregate.getPartials().empty());
    EXPECT_EQ(aggregate.size(), 2u);
}

TEST(FMTAction, GivenALockExemptActionWhenAddedThenTheResultIsLockExempt)
{
    // Arrange
    Core::FMTAction aggregate("TRAVAUX");
    aggregate += buildClearcut();

    // Act
    aggregate += buildPlantation();

    // Assert
    EXPECT_FALSE(aggregate.doRespectLock());
    EXPECT_EQ(aggregate.size(), 2u);
}

TEST(FMTAction, GivenAnUpdatedActionWhenAddedThenRaisesAnInvalidAction)
{
    // Arrange
    Core::FMTAction aggregate("COUPES");
    Core::FMTAction clearcut = buildClearcut();
    clearcut.update();

    // Act
    const std::vector<Exception::FMTexc> raised = Testing::getRaisedTypes([&]()
        {
        aggregate += clearcut;
        });

    // Assert
    EXPECT_TRUE(Testing::hasType(raised, Exception::FMTexc::FMTinvalid_action));
}

// FMTModel::aggregateActions unshrinks each action before adding it.
TEST(FMTAction, GivenAnUpdatedActionUnshrunkWhenAddedThenAppendsItsSpecifications)
{
    // Arrange
    Core::FMTAction aggregate("COUPES");
    Core::FMTAction clearcut = buildClearcut();
    clearcut.update();
    clearcut.unShrink(getThemes());

    // Act
    aggregate += clearcut;

    // Assert
    EXPECT_EQ(aggregate.size(), 1u);
    EXPECT_TRUE(aggregate.begin()->first == getMask("? PROD ?"));
}

TEST(FMTAction, GivenSpecificationsOfPeriodZeroOnlyWhenNotUseThenReturnsTrue)
{
    // Arrange: _CP = 0, _CP = 1, no period, and no spec
    const Core::FMTAction atPeriodZero = buildAction("ACTION", true, false, { { "? ? ?", getPeriodSpec(0, 0) } });
    const Core::FMTAction atPeriodOne = buildAction("ACTION", true, false, { { "? ? ?", getPeriodSpec(1, 1) } });
    const Core::FMTAction withoutPeriod = buildFire();
    const Core::FMTAction withoutSpecification("ACTION", true, false);

    // Act
    const std::vector<bool> notUsed = { atPeriodZero.notUse(), atPeriodOne.notUse(), withoutPeriod.notUse(),
        withoutSpecification.notUse() };

    // Assert
    EXPECT_EQ(notUsed, std::vector<bool>({ true, false, false, true }));
}

TEST(FMTAction, GivenMasksWhenSplitThenGivesOneActionPerMaskNamedAfterIt)
{
    // Arrange
    Core::FMTAction clearcut = buildClearcut();
    clearcut.pushAggregate("RECOLTES");
    clearcut.update();

    // Act
    const std::vector<Core::FMTAction> split = clearcut.split({ getMask("UNITE1 ? ?"), getMask("UNITE3 ? ?") }, getThemes());

    // Assert
    ASSERT_EQ(split.size(), 2u);
    EXPECT_EQ(split.at(0).getName(), "COUPETOTALE_UNITE1");
    EXPECT_EQ(split.at(1).getName(), "COUPETOTALE_UNITE3");
    EXPECT_EQ(split.at(0).getAggregates(), std::vector<std::string>({ "COUPETOTALE", "RECOLTES" }));
    EXPECT_TRUE(split.at(0).doRespectLock());
    EXPECT_TRUE(split.at(0).isResetAge());
    EXPECT_EQ(split.at(0).findSets(getMask("UNITE1 PEUPLEMENT1 UTR1")).size(), 1u);
    EXPECT_EQ(split.at(0).findSets(getMask("UNITE3 PEUPLEMENT1 UTR1")).size(), 0u);
    EXPECT_EQ(split.at(1).findSets(getMask("UNITE3 PEUPLEMENT1 UTR1")).size(), 1u);
}

TEST(FMTAction, GivenAMaskOutsideTheSpecificationsWhenSplitThenItsActionHasNoSpecification)
{
    // Arrange: UC prod ?, split on UNITE3
    const Core::FMTAction caribou = getCaribou();

    // Act
    const std::vector<Core::FMTAction> split = caribou.split({ getMask("UNITE3 ? ?") }, getThemes());

    // Assert
    ASSERT_EQ(split.size(), 1u);
    EXPECT_EQ(split.at(0).getName(), "ACARIBOU_UNITE3");
    EXPECT_TRUE(split.at(0).empty());
}

// The spec of UNITE3 ? ? cannot be presolved, the presolve keeping UNITE1 only: the spec of ? prod ? is left, its mask
// becomes ? in the presolved themes, and update gives the action its age bounds.
TEST(FMTAction, GivenAPreSolveFilterWhenPreSolveThenKeepsTheSpecificationsOfThePresolvedDevelopmentsOnly)
{
    // Arrange: ? prod ? _AGE >= 1 AND _AGE <= 3, then UNITE3 ? ? _AGE >= 8
    const PreSolve preSolve = getPreSolve();
    std::vector<Core::FMTTheme> newThemes = preSolve.themes;
    const Core::FMTAction action = getAction("ACTION", true, false,
        { { "? PROD ?", getAgeSpec(1, 3) }, { "UNITE3 ? ?", getAgeSpec(8, NO_UPPER_BOUND) } });

    // Act
    const Core::FMTAction presolved = action.preSolve(preSolve.filter, getThemes(), newThemes);

    // Assert
    ASSERT_EQ(presolved.size(), 1u);
    EXPECT_EQ(std::string(presolved.begin()->first), "?");
    EXPECT_EQ(presolved.getAgeLowerBound(), 1);
    EXPECT_EQ(presolved.getAgeUpperBound(), 3);
    EXPECT_EQ(action.size(), 2u);
}

// The GCBM action of an action is an aggregate ~GCBM:<id>:<name>.
TEST(FMTAction, GivenAGcbmAggregateWhenGetGcbmActionIdAndNameThenReadsThemFromTheAggregate)
{
    // Arrange
    Core::FMTAction fire = buildFire();
    fire.pushAggregate("PERTURBATIONS");
    fire.pushAggregate("~GCBM:10:Wildfire");

    // Act
    const int id = fire.getGCBMActionId();
    const std::string name = fire.getGCBMActionName();

    // Assert
    EXPECT_EQ(id, FMTGCBMWILDFIREID);
    EXPECT_EQ(name, "Wildfire");
}

// The missing aggregate gives a warning, FMTempty_action (Include/FMTEmptyAction.h); reading the id in the empty
// definition then raises.
TEST(FMTAction, GivenNoGcbmAggregateWhenGetGcbmActionIdThenRaisesAFunctionFailed)
{
    // Arrange
    const Core::FMTAction fire = buildFire();

    // Act
    const std::vector<Exception::FMTexc> raised = Testing::getRaisedTypes([&]()
        {
        fire.getGCBMActionId();
        });

    // Assert
    ASSERT_FALSE(raised.empty());
    EXPECT_EQ(raised.front(), Exception::FMTexc::FMTfunctionfailed);
    EXPECT_FALSE(Testing::hasType(raised, Exception::FMTexc::FMTempty_action));
}

// An action keeps the part of each serie that ends with it: COUPEtotale keeps aCaribou -> COUPEtotale.
TEST(FMTAction, GivenASerieWhenSetSeriesThenKeepsThePartOfTheSerieThatEndsWithTheAction)
{
    // Arrange
    Core::FMTAction clearcut = buildClearcut();
    clearcut.update();

    // Act
    clearcut.setSeries({ getSerie() });

    // Assert
    EXPECT_TRUE(clearcut.isPartOfASerie());
    EXPECT_TRUE(clearcut.isInSeries());
    ASSERT_EQ(clearcut.getSeries().size(), 1u);
    EXPECT_EQ(clearcut.getSeries().at(0).getActions(), std::vector<std::string>({ "ACARIBOU", "COUPETOTALE" }));
    EXPECT_EQ(clearcut.getSeries().at(0).getLength(), 3);
    EXPECT_EQ(clearcut.getLargestSerieSize(), 2u);
}

TEST(FMTAction, GivenTheFirstActionOfASerieWhenSetSeriesThenIsInSeriesWithoutASerieOfItsOwn)
{
    // Arrange
    Core::FMTAction caribou = getCaribou();
    Core::FMTAction fire = buildFire();

    // Act
    caribou.setSeries({ getSerie() });
    fire.setSeries({ getSerie() });

    // Assert
    EXPECT_TRUE(caribou.isInSeries());
    EXPECT_FALSE(caribou.isPartOfASerie());
    EXPECT_EQ(caribou.getLargestSerieSize(), 0u);
    EXPECT_FALSE(fire.isInSeries());
    EXPECT_FALSE(fire.isPartOfASerie());
}

// The serie mask is the sequence of the last actions of a development.
TEST(FMTAction, GivenASerieWhenIsAllowedInSerieThenAllowsTheActionAfterTheActionsThatPrecedeItOnly)
{
    // Arrange
    Core::FMTAction clearcut = buildClearcut();
    clearcut.setSeries({ getSerie() });

    // Act
    const bool afterCaribou = clearcut.isAllowedInSerie({ "ACARIBOU" });
    const bool afterItself = clearcut.isAllowedInSerie({ "ACARIBOU", "COUPETOTALE" });
    const bool afterFire = clearcut.isAllowedInSerie({ "AFIRE" });
    const bool withoutHistory = clearcut.isAllowedInSerie({});

    // Assert
    EXPECT_TRUE(afterCaribou);
    EXPECT_FALSE(afterItself);
    EXPECT_FALSE(afterFire);
    EXPECT_FALSE(withoutHistory);
    EXPECT_NE(clearcut.getSerie({ "ACARIBOU" }), nullptr);
    EXPECT_EQ(clearcut.getSerie({ "AFIRE" }), nullptr);
}
