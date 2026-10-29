#[[
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
]]

# The integration tests of the serialization of FMTlib with its real dependency, Boost.Serialization, labelled
# integration;serialization, in the GoogleTest executable IntegrationSerialization: each case pickles an object through
# the archive of the pickle of Python (PickleArchive.h), then expects the unpickled object to be the same. Objects are
# built in code, or read from Examples/Models/TWD_land, hence the ExamplesModels fixture; the cases write nothing. The
# models of TWD_land are read and compared with the tools of the integration tests of the parsers
# (Tests/Integration/Parser/ParserRoundTrip.h). Each source holds the suite of one class, named after it
# (Documentation/CodingStandards.md, Test names); every .cpp file of this folder is built at the next configuration.

#A function, so that its variables stay out of the variables exported to the install scripts.
function(createserializationintegrationtests)
	file(GLOB FMTTESTSsources "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
	registergoogletests(IntegrationSerialization Integration serialization EXAMPLES_MODELS ${FMTTESTSsources})
	target_include_directories(IntegrationSerialization PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../Parser")
endfunction(createserializationintegrationtests)

createserializationintegrationtests()
