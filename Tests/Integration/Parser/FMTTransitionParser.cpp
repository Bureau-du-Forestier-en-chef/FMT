/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "ParserRoundTrip.h"
#include "FMTDevelopment.h"
#include "FMTFork.h"
#include "FMTGCBMTransition.h"
#include "FMTMask.h"
#include "FMTModel.h"
#include "FMTTransition.h"
#include "FMTTransitionParser.h"
#include "FMTYields.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <map>
#include <string>
#include <vector>

// FMTTransitionParser, through the transition section of a scenario of TWD_land read by FMTModelParser, and the GCBM
// transitions that writeGCBM writes and readGCBM reads back.

// Characterizes a known behavior (issue #358): FMTParser::_setSpec reads only @AGE(min..max) and @YLD(yield,min..max)
// in a transition source; it drops the action form _AGE >= 10 without a message, and the source then applies to every
// development of its mask. The scenario INVALID_sourcecondition has such a source, which the disabled row of
// ReadInvalidScenario in Tests/System/Cpp/knownbugs.csv expects to be refused.
TEST(FMTTransitionParser, GivenASourceConditionInTheActionFormWhenReadThenAppliesTheSourceToEveryAge)
{
    // Arrange: *CASE arecup, *SOURCE ? feu ? _AGE >= 10
    const Models::FMTModel model = Testing::readScenario("INVALID_sourcecondition");
    const std::vector<Core::FMTTransition> transitions = model.getTransitions();
    const std::vector<Core::FMTTransition>::const_iterator salvage = std::find_if(transitions.begin(), transitions.end(),
        [](const Core::FMTTransition& p_transition) { return p_transition.getName() == "ARECUP"; });
    ASSERT_NE(salvage, transitions.end());
    const Core::FMTYields yields = model.getYields();
    const Core::FMTDevelopment youngBurnt(Core::FMTMask("UNITE1 FEU UTR1", model.getThemes()), 5, 0, 1);

    // Act
    const Core::FMTFork* fork = salvage->getFork(youngBurnt, yields);

    // Assert
    ASSERT_NE(fork, nullptr);
    EXPECT_TRUE(fork->emptyAge());
}

// writeGCBM and readGCBM: the GCBM transitions of a period, which FMTSpatialSchedule::getGCBMtransitions gives and the
// SES of the wrapper core writes in one file per period. writeGCBM writes by hand the text archive of Boost of library
// version 17, which readGCBM reads with boost::archive::text_iarchive: a round trip must give back the transitions.
namespace
{
    // GCBM transitions as FMTSpatialSchedule::getGCBMtransitions gives them: one per stack of actions and attributes,
    // named after them, each with the final attribute of every growth theme, named THEME<id + 1>.
    std::vector<Core::FMTGCBMTransition> getGCBMTransitions()
    {
        return std::vector<Core::FMTGCBMTransition>({
            Core::FMTGCBMTransition(11, { { "THEME1", "UNITE1" }, { "THEME3", "UTR1" } }, "COUPETOTALE-UNITE1-UTR1"),
            Core::FMTGCBMTransition(10, { { "THEME1", "UNITE2" }, { "THEME3", "UTR2" } }, "COUPETOTALE-UNITE2-UTR2"),
            Core::FMTGCBMTransition(0, { { "THEME1", "UNITE3" }, { "THEME3", "UTR3" } }, "FEU-UNITE3-UTR3") });
    }

    // Writes GCBM transitions with writeGCBM in the folder of the test case, then reads them back with readGCBM.
    std::vector<Core::FMTGCBMTransition> writeAndReadGCBM(const std::vector<Core::FMTGCBMTransition>& p_transitions)
    {
        const std::string file = (Testing::getCaseFolder() / "transition1.txt").string();
        Parser::FMTTransitionParser parser;
        parser.setDefaultExceptionHandler();
        parser.writeGCBM(p_transitions, file);
        return parser.readGCBM(file);
    }
}

TEST(FMTTransitionParser, GivenGCBMTransitionsWhenWrittenByWriteGCBMThenReadGCBMRereadsThem)
{
    // Arrange: three transitions, since writeGCBM writes the first one, the second one and the next ones differently
    const std::vector<Core::FMTGCBMTransition> transitions = getGCBMTransitions();

    // Act
    const std::vector<Core::FMTGCBMTransition> reread = writeAndReadGCBM(transitions);

    // Assert
    ASSERT_EQ(reread.size(), 3u);
    EXPECT_TRUE(reread == transitions);
    EXPECT_EQ(reread.at(2).name, "FEU-UNITE3-UTR3");
    EXPECT_EQ(reread.at(2).ageafter, 0);
}

TEST(FMTTransitionParser, GivenGCBMTransitionsWithSpacesOrAnEmptyNameWhenWrittenByWriteGCBMThenReadGCBMRereadsThem)
{
    // Arrange: a text archive writes each string after its length, so that a space is part of the string
    const std::vector<Core::FMTGCBMTransition> transitions({
        Core::FMTGCBMTransition(5, { { "THEME1", "UNITE 1" } }, "COUPE TOTALE"),
        Core::FMTGCBMTransition(6, { { "THEME1", "UNITE2" } }, ""),
        Core::FMTGCBMTransition(7, { { "THEME1", "" } }, "FEU") });

    // Act
    const std::vector<Core::FMTGCBMTransition> reread = writeAndReadGCBM(transitions);

    // Assert
    ASSERT_EQ(reread.size(), 3u);
    EXPECT_TRUE(reread == transitions);
    EXPECT_EQ(reread.at(0).name, "COUPE TOTALE");
    EXPECT_EQ(reread.at(0).themes.at("THEME1"), "UNITE 1");
    EXPECT_EQ(reread.at(1).name, "");
}

// Next to the disabled case of a first transition without theme: a transition without theme after the first one is
// read back.
TEST(FMTTransitionParser, GivenAGCBMTransitionWithoutThemeAfterTheFirstWhenWrittenByWriteGCBMThenReadGCBMRereadsIt)
{
    // Arrange
    std::vector<Core::FMTGCBMTransition> transitions = getGCBMTransitions();
    transitions.push_back(Core::FMTGCBMTransition(3, std::map<std::string, std::string>(), "FEU"));

    // Act
    const std::vector<Core::FMTGCBMTransition> reread = writeAndReadGCBM(transitions);

    // Assert
    ASSERT_EQ(reread.size(), 4u);
    EXPECT_TRUE(reread == transitions);
    EXPECT_TRUE(reread.at(3).themes.empty());
}

// Defect 24 of Tests/ETAT.md (section 2.7): writeGCBM writes nothing for an empty list, a period without any
// disturbance, and readGCBM throws on the empty file (input stream error) instead of reading back an empty list.
TEST(FMTTransitionParser, DISABLED_GivenNoGCBMTransitionWhenWrittenByWriteGCBMThenReadGCBMRereadsAnEmptyList)
{
    // Arrange
    const std::vector<Core::FMTGCBMTransition> transitions;

    // Act
    const std::vector<Core::FMTGCBMTransition> reread = writeAndReadGCBM(transitions);

    // Assert
    EXPECT_TRUE(reread.empty());
}

// Defect 24 of Tests/ETAT.md (section 2.7): writeGCBM always writes the class information of the pair of a theme after
// the themes of the first transition; when the first transition has no theme, Boost reads it as the first theme of the
// next transition, and readGCBM throws (input stream error).
TEST(FMTTransitionParser, DISABLED_GivenAFirstGCBMTransitionWithoutThemeWhenWrittenByWriteGCBMThenReadGCBMRereadsIt)
{
    // Arrange
    std::vector<Core::FMTGCBMTransition> transitions = getGCBMTransitions();
    transitions.insert(transitions.begin(), Core::FMTGCBMTransition(3, std::map<std::string, std::string>(), "FEU"));

    // Act
    const std::vector<Core::FMTGCBMTransition> reread = writeAndReadGCBM(transitions);

    // Assert
    ASSERT_EQ(reread.size(), 4u);
    EXPECT_TRUE(reread == transitions);
}
