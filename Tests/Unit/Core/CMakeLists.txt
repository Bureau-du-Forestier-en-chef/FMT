#[[
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
]]

# The unit tests of FMTlib, labelled unit;core, in the GoogleTest executable UnitCore. Each source holds the
# suite of one class, named after that class (Documentation/CodingStandards.md, Test names); every .cpp file
# of this folder is built at the next configuration.

#A function, so that its variables stay out of the variables exported to the install scripts.
function(createcoreunittests)
	file(GLOB FMTTESTSsources "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
	registergoogletests(UnitCore Unit core ${FMTTESTSsources})
endfunction(createcoreunittests)

createcoreunittests()
