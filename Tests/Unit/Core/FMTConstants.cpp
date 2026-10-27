/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTConstants.h"

#include <gtest/gtest.h>

#include <boost/algorithm/string.hpp>

#include <algorithm>
#include <string>
#include <vector>

// The constants of the constants section, as FMTConstantParser keeps them: a name in capitals, without its #, and one value
// per period. The other sections name a constant with its #. get and getAll, which read the values of a constant, are
// member templates that the DLL does not export (Documentation/Architecture.md, Known Architectural Debt): a test linked
// to FMTlib cannot call them, and these cases read the values through operator std::string. The constants are those of
// the scenario LEVEL1 of TWD_land (CorCour 2, Creguc 100 to 700).

namespace
{
    Core::FMTConstants getConstants()
    {
        Core::FMTConstants constants;
        constants.set("CORCOUR", { 2.0 });
        constants.set("CREGUC", { 100.0, 200.0, 300.0, 400.0, 500.0, 600.0, 700.0 });
        return constants;
    }

    // The lines of p_text, sorted: the constants are kept in an unordered map, whose order may change from a copy to
    // another.
    std::vector<std::string> getSortedLines(const std::string& p_text)
    {
        std::vector<std::string> lines;
        boost::split(lines, p_text, boost::is_any_of("\n"));
        std::sort(lines.begin(), lines.end());
        return lines;
    }

    std::vector<std::string> getWords(const std::string& p_line)
    {
        std::vector<std::string> words;
        boost::split(words, p_line, boost::is_any_of(" "), boost::token_compress_on);
        return words;
    }
}

TEST(FMTConstants, GivenConstantsWhenIsConstantThenTrueForTheNameOfAConstantWrittenWithItsHashOnly)
{
    // Arrange
    const Core::FMTConstants constants = getConstants();

    // Act
    const bool withHash = constants.isConstant("#CORCOUR");
    const bool withoutHash = constants.isConstant("CORCOUR");
    const bool absent = constants.isConstant("#ABSENT");
    const bool number = constants.isConstant("2");

    // Assert
    EXPECT_TRUE(withHash);
    EXPECT_FALSE(withoutHash);
    EXPECT_FALSE(absent);
    EXPECT_FALSE(number);
}

// length needs the #, as FMTOptimizationParser writes it: without it, a constant has no value.
TEST(FMTConstants, GivenConstantsWhenLengthThenCountsTheValuesOfAConstantWrittenWithItsHashOnly)
{
    // Arrange
    const Core::FMTConstants constants = getConstants();

    // Act
    const size_t ofSevenPeriods = constants.length("#CREGUC");
    const size_t ofOnePeriod = constants.length("#CORCOUR");
    const size_t withoutHash = constants.length("CREGUC");
    const size_t absent = constants.length("#ABSENT");

    // Assert
    EXPECT_EQ(ofSevenPeriods, 7u);
    EXPECT_EQ(ofOnePeriod, 1u);
    EXPECT_EQ(withoutHash, 0u);
    EXPECT_EQ(absent, 0u);
}

// set replaces the values of a constant; FMTConstantParser refuses the redefinition itself (FMTconstants_replacement).
TEST(FMTConstants, GivenAConstantSetTwiceWhenLengthAndConvertedToStringThenKeepsTheLastValues)
{
    // Arrange
    Core::FMTConstants constants;
    constants.set("CORCOUR", { 2.0, 3.0 });

    // Act
    constants.set("CORCOUR", { 4.0 });

    // Assert
    EXPECT_EQ(constants.length("#CORCOUR"), 1u);
    EXPECT_EQ(std::string(constants), "CORCOUR 4.000000\n");
}

TEST(FMTConstants, GivenConstantsWhenCopiedThenTheCopyHasTheSameConstants)
{
    // Arrange
    const Core::FMTConstants constants = getConstants();

    // Act
    const Core::FMTConstants copy(constants);
    Core::FMTConstants assigned;
    assigned = constants;

    // Assert
    EXPECT_EQ(copy.length("#CREGUC"), 7u);
    EXPECT_TRUE(assigned.isConstant("#CORCOUR"));
    EXPECT_EQ(getSortedLines(std::string(copy)), getSortedLines(std::string(constants)));
    EXPECT_EQ(getSortedLines(std::string(assigned)), getSortedLines(std::string(constants)));
}

TEST(FMTConstants, GivenNoConstantWhenConvertedToStringThenWritesNothing)
{
    // Arrange
    const Core::FMTConstants constants;

    // Act
    const std::string text(constants);

    // Assert
    EXPECT_TRUE(text.empty());
    EXPECT_FALSE(constants.isConstant("#CORCOUR"));
}

// A value is written with six decimals (std::to_string), a negative one with its sign.
TEST(FMTConstants, GivenAConstantOfOneValueWhenConvertedToStringThenWritesALineOfItsNameAndItsValue)
{
    // Arrange
    Core::FMTConstants rate;
    rate.set("RATE", { 4.5 });
    Core::FMTConstants negative;
    negative.set("NEGATIVE", { -2.25 });

    // Act
    const std::string rateText(rate);
    const std::string negativeText(negative);

    // Assert
    EXPECT_EQ(rateText, "RATE 4.500000\n");
    EXPECT_EQ(negativeText, "NEGATIVE -2.250000\n");
}

// One line per constant, in the order of the unordered map, which a copy may change.
TEST(FMTConstants, GivenTwoConstantsWhenConvertedToStringThenWritesALinePerConstant)
{
    // Arrange
    Core::FMTConstants constants;
    constants.set("CORCOUR", { 2.0 });
    constants.set("RATE", { 4.5 });

    // Act
    const std::string text(constants);

    // Assert
    EXPECT_EQ(getSortedLines(text), std::vector<std::string>({ "", "CORCOUR 2.000000", "RATE 4.500000" }));
    EXPECT_EQ(text.back(), '\n');
}

// Defect 23 (Tests/ETAT.md, section 2.7): operator std::string, which FMTConstantParser::write writes as the constants
// section, writes the values of a constant one after the other, without a separator (CREGUC 100.000000200.000000...),
// which the parser cannot read back. The case expects the name, then one word per value.
TEST(FMTConstants, DISABLED_GivenAConstantOfSeveralValuesWhenConvertedToStringThenWritesEachValueAsAWord)
{
    // Arrange
    Core::FMTConstants constants;
    constants.set("CREGUC", { 100.0, 200.0, 300.0 });

    // Act
    const std::string text(constants);

    // Assert
    const std::vector<std::string> words = getWords(boost::trim_copy(text));
    ASSERT_EQ(words.size(), 4u);
    EXPECT_EQ(words.at(0), "CREGUC");
    EXPECT_DOUBLE_EQ(std::stod(words.at(1)), 100.0);
    EXPECT_DOUBLE_EQ(std::stod(words.at(2)), 200.0);
    EXPECT_DOUBLE_EQ(std::stod(words.at(3)), 300.0);
}
