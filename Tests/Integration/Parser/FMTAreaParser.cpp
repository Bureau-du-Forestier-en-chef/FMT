/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "ParserRoundTrip.h"
#include "FMTAreaParser.h"
#include "FMTTheme.h"
#include "FMTMask.h"
#include "FMTOperatingArea.h"
#include "FMTOperatingAreaScheme.h"

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <map>
#include <string>
#include <utility>
#include <vector>

// FMTAreaParser::getSchemeNeighbors with GDAL, on a small GeoJSON map that each case writes: the neighbors of an
// operating area are the areas that its buffer intersects, kept when each one finds the other. FMTAreaParser has no
// getSchemeNeighbors without GDAL and OSI: the cases are then skipped.
namespace
{
#if defined(FMTWITHGDAL) && defined(FMTWITHOSI)
    // The buffer around each operating area, in map units: a tenth of the side of the squares of the maps below.
    const double BUFFER_SIZE = 10.0;

    // A unit of a test map, one operating area: its attribute of the only theme, and the corners of its polygon.
    struct MapUnit
    {
        std::string attribute;
        std::vector<std::pair<int, int>> corners;
    };

    // The neighbors of each operating area, by attribute.
    using Neighbors = std::map<std::string, std::vector<std::string>>;

    // The only theme of a test map, with one attribute per unit.
    std::vector<Core::FMTTheme> getThemes(const std::vector<MapUnit>& p_units)
    {
        std::vector<std::string> attributes;
        for (const MapUnit& unit : p_units)
        {
            attributes.push_back(unit.attribute);
        }
        return std::vector<Core::FMTTheme>(1, Core::FMTTheme(attributes, 0, 0, "THEME1"));
    }

    // One operating area per unit. With a neighbors perimeter of 0, any intersection with the buffer makes a
    // neighbor, whatever its share of the buffer.
    std::vector<Heuristics::FMTOperatingAreaScheme> getSchemes(const std::vector<MapUnit>& p_units,
        const std::vector<Core::FMTTheme>& p_themes)
    {
        std::vector<Heuristics::FMTOperatingAreaScheme> schemes;
        for (const MapUnit& unit : p_units)
        {
            const Heuristics::FMTOperatingArea area(
                Core::FMTMask(std::vector<std::string>(1, unit.attribute), p_themes), 0.0);
            schemes.push_back(Heuristics::FMTOperatingAreaScheme(area, 1, 1, 1, 1, 1, 1));
        }
        return schemes;
    }

    // Writes the units as a GeoJSON map in the folder of the test case, with the fields that FMTAreaParser reads:
    // THEME1, AGE and SUPERFICIE. Each polygon is closed on its first corner.
    std::filesystem::path writeMap(const std::vector<MapUnit>& p_units)
    {
        const std::filesystem::path map = Testing::getCaseFolder() / "map.geojson";
        std::ofstream stream(map);
        stream << "{\"type\":\"FeatureCollection\",\"features\":[";
        for (size_t unit = 0; unit < p_units.size(); ++unit)
        {
            const MapUnit& mapUnit = p_units.at(unit);
            stream << (unit > 0 ? "," : "") << "\n{\"type\":\"Feature\",\"properties\":{\"THEME1\":\""
                << mapUnit.attribute << "\",\"AGE\":10,\"SUPERFICIE\":1.0},"
                << "\"geometry\":{\"type\":\"Polygon\",\"coordinates\":[[";
            for (const std::pair<int, int>& corner : mapUnit.corners)
            {
                stream << "[" << corner.first << "," << corner.second << "],";
            }
            stream << "[" << mapUnit.corners.front().first << "," << mapUnit.corners.front().second << "]]]}}";
        }
        stream << "\n]}\n";
        return map;
    }

    Neighbors getNeighbors(const std::vector<Heuristics::FMTOperatingAreaScheme>& p_schemes)
    {
        Neighbors neighbors;
        for (const Heuristics::FMTOperatingAreaScheme& scheme : p_schemes)
        {
            std::vector<std::string>& attributes = neighbors[std::string(scheme.getMask())];
            for (const Core::FMTMask& neighbor : scheme.getNeighbors())
            {
                attributes.push_back(std::string(neighbor));
            }
        }
        return neighbors;
    }
#endif
}

TEST(FMTAreaParser, GivenTwoUnitsSharingASideWhenGetSchemeNeighborsThenTheyAreNeighbors)
{
#if defined(FMTWITHGDAL) && defined(FMTWITHOSI)
    // Arrange
    // Two squares of 100 side by side: the buffer of each one takes a single strip of the other, a polygon.
    const std::vector<MapUnit> units = {
        { "WEST", { { 0, 0 }, { 100, 0 }, { 100, 100 }, { 0, 100 } } },
        { "EAST", { { 100, 0 }, { 200, 0 }, { 200, 100 }, { 100, 100 } } } };
    const std::vector<Core::FMTTheme> themes = getThemes(units);
    const std::filesystem::path map = writeMap(units);
    const Neighbors expected = { { "EAST", { "WEST" } }, { "WEST", { "EAST" } } };
    Parser::FMTAreaParser parser;

    // Act
    const std::vector<Heuristics::FMTOperatingAreaScheme> schemes = parser.getSchemeNeighbors(
        getSchemes(units, themes), themes, map.string(), "AGE", "SUPERFICIE", 1.0, 1.0, "", 0.0, BUFFER_SIZE);

    // Assert
    EXPECT_EQ(getNeighbors(schemes), expected);
#else
    GTEST_SKIP() << "FMT is built without GDAL or OSI";
#endif
}

TEST(FMTAreaParser, GivenTwoUnitsTouchingInTwoSeparatePlacesWhenGetSchemeNeighborsThenTheyAreNeighbors)
{
#if defined(FMTWITHGDAL) && defined(FMTWITHOSI)
    // Arrange
    // An upside-down U on a square of 100: its two legs, 20 wide, stand on the square, and its top is 50 above it.
    // The buffer of each one takes two separate pieces of the other, a multipolygon, whose area was 0 before
    // issue #394.
    const std::vector<MapUnit> units = {
        { "SQUARE", { { 0, 0 }, { 100, 0 }, { 100, 100 }, { 0, 100 } } },
        { "ARCH", { { 0, 100 }, { 20, 100 }, { 20, 150 }, { 80, 150 }, { 80, 100 }, { 100, 100 }, { 100, 200 },
            { 0, 200 } } } };
    const std::vector<Core::FMTTheme> themes = getThemes(units);
    const std::filesystem::path map = writeMap(units);
    const Neighbors expected = { { "ARCH", { "SQUARE" } }, { "SQUARE", { "ARCH" } } };
    Parser::FMTAreaParser parser;

    // Act
    const std::vector<Heuristics::FMTOperatingAreaScheme> schemes = parser.getSchemeNeighbors(
        getSchemes(units, themes), themes, map.string(), "AGE", "SUPERFICIE", 1.0, 1.0, "", 0.0, BUFFER_SIZE);

    // Assert
    EXPECT_EQ(getNeighbors(schemes), expected);
#else
    GTEST_SKIP() << "FMT is built without GDAL or OSI";
#endif
}
