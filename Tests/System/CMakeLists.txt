#[[
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
]]

# System tests, labelled system: Cpp/ holds the C++ tests, registered by the rows of its CSV files, Python/
# and R/ the pytest and testthat suites, one test per file, each registered where the install builds its
# package.
include("${CMAKE_CURRENT_LIST_DIR}/Cpp/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/Python/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/R/CMakeLists.txt")
