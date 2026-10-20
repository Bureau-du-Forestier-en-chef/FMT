#[[
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
]]

# Integration tests, labelled integration, on GoogleTest: one component with its real dependency, such as GDAL,
# a solver or the Woodstock file formats. The tests of a component go in a folder of their own, whose
# CMakeLists.txt, included here, calls registergoogletests(Integration<Component> Integration <component> <sources>)
# of Tests/Support/TestRegistration.cmake, as Unit/Core/CMakeLists.txt does for the unit tests of FMTlib.
# Parser/ tests the parsers of FMTlib with the files they write and read back.
include("${CMAKE_CURRENT_LIST_DIR}/Parser/CMakeLists.txt")
