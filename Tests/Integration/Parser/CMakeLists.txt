#[[
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
]]

# The integration tests of the parsers of FMTlib with their real dependency, the Woodstock file formats, labelled
# integration;parser, in the GoogleTest executable IntegrationParser. They read Examples/Models/TWD_land, hence the
# ExamplesModels fixture, and write under tests/IntegrationParser in the build folder. The parsers are part of
# FMTlib, which registergoogletests links: no other library is needed. Each source holds the suite of one parser,
# named after it (Documentation/CodingStandards.md, Test names); every .cpp file of this folder is built at the
# next configuration.

#A function, so that its variables stay out of the variables exported to the install scripts.
function(createparserintegrationtests)
	file(GLOB FMTTESTSsources "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
	registergoogletests(IntegrationParser Integration parser EXAMPLES_MODELS ${FMTTESTSsources})
endfunction(createparserintegrationtests)

createparserintegrationtests()
