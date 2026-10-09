#[[
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
]]

#[[
The tests of FMT, by level (Documentation/CodingStandards.md, Test levels). Included by the root
CMakeLists.txt when WITHOUT_TESTS is off, with include() rather than add_subdirectory() so that the
tests stay in the scope of the root directory, after Excel/ and FMTWrapperCore/ and before
Examples/C++/CMakeLists.txt.

The functions that register the tests, Support/TestRegistration.cmake, are included by the root
CMakeLists.txt itself, before Excel/CMakeLists.txt, which registers its test with them. They can move
here once that test has moved to System/, with the other C++ tests.
]]

# No test may modify Examples/Models: its files are hashed before the tests and compared after them
# (cmake/TestsDataSnapshot.cmake). Every system test and benchmark requires this fixture.
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/tests")
set(FMTTESTSsnapshot "${CMAKE_BINARY_DIR}/tests/ExamplesModels.snapshot")
add_test(
		NAME ExamplesModelsSnapshot
		COMMAND "${CMAKE_COMMAND}" -DMODE=save "-DDATA_DIR=${PROJECT_SOURCE_DIR}/Examples/Models" "-DSNAPSHOT_FILE=${FMTTESTSsnapshot}" -P "${PROJECT_SOURCE_DIR}/cmake/TestsDataSnapshot.cmake"
		)
set_tests_properties(ExamplesModelsSnapshot PROPERTIES FIXTURES_SETUP ExamplesModels)
add_test(
		NAME ExamplesModelsUnchanged
		COMMAND "${CMAKE_COMMAND}" -DMODE=compare "-DDATA_DIR=${PROJECT_SOURCE_DIR}/Examples/Models" "-DSNAPSHOT_FILE=${FMTTESTSsnapshot}" -P "${PROJECT_SOURCE_DIR}/cmake/TestsDataSnapshot.cmake"
		)
set_tests_properties(ExamplesModelsUnchanged PROPERTIES FIXTURES_CLEANUP ExamplesModels)

include("${CMAKE_CURRENT_LIST_DIR}/Unit/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/Integration/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/System/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/Performance/CMakeLists.txt")
