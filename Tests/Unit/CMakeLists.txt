#[[
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
]]

# Unit tests, labelled unit, on GoogleTest: one folder per component, each building one executable whose
# cases registergoogletests registers (Tests/Support/TestRegistration.cmake). Core/ tests the core classes of
# FMTlib, Graph/ its graphs and Spatial/ its spatial classes.
include("${CMAKE_CURRENT_LIST_DIR}/Core/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/Graph/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/Spatial/CMakeLists.txt")
