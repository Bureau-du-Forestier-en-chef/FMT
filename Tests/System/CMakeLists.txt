#[[
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
]]

# System tests, labelled system: Cpp/ receives the C++ tests in phase 6 of the test reorganization (#350),
# Python/ and R/ hold the pytest and testthat suites, one test per file, each registered where the install
# builds its package.
include("${CMAKE_CURRENT_LIST_DIR}/Python/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/R/CMakeLists.txt")
