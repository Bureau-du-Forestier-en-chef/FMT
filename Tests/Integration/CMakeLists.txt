#[[
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
]]

# Integration tests, labelled integration, on GoogleTest: one component with its real dependency, such as GDAL
# or a solver. None exists yet. The tests of a component go in a folder of their own, whose CMakeLists.txt,
# included here, calls registergoogletests(Integration<Component> Integration <component> <sources>) of
# Tests/Support/TestRegistration.cmake, as Unit/Core/CMakeLists.txt does for the unit tests of FMTlib.
