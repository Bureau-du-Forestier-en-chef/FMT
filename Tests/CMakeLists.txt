#[[
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
]]

#[[
The tests of FMT, by level (Documentation/CodingStandards.md, Test levels). Included by the root
CMakeLists.txt when WITHOUT_TESTS is off, with include() rather than add_subdirectory() so that the
tests stay in the scope of the root directory, after Excel/ and FMTWrapperCore/: the C++ system tests
link to their libraries, and the include_directories of FMTWrapperCore/ comes first in the include
directories of the targets created after it.
]]

# The functions that register the tests.
include("${CMAKE_CURRENT_LIST_DIR}/Support/TestRegistration.cmake")

# No test may modify Examples/Models: its files are hashed before the tests and compared after them
# (Support/TestsDataSnapshot.cmake). Every system test and benchmark requires this fixture, and every integration
# test that reads these models (registergoogletests with EXAMPLES_MODELS, Support/TestRegistration.cmake).
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/tests")
set(FMTTESTSsnapshot "${CMAKE_BINARY_DIR}/tests/ExamplesModels.snapshot")
add_test(
		NAME ExamplesModelsSnapshot
		COMMAND "${CMAKE_COMMAND}" -DMODE=save "-DDATA_DIR=${PROJECT_SOURCE_DIR}/Examples/Models" "-DSNAPSHOT_FILE=${FMTTESTSsnapshot}" -P "${CMAKE_CURRENT_LIST_DIR}/Support/TestsDataSnapshot.cmake"
		)
set_tests_properties(ExamplesModelsSnapshot PROPERTIES FIXTURES_SETUP ExamplesModels)
add_test(
		NAME ExamplesModelsUnchanged
		COMMAND "${CMAKE_COMMAND}" -DMODE=compare "-DDATA_DIR=${PROJECT_SOURCE_DIR}/Examples/Models" "-DSNAPSHOT_FILE=${FMTTESTSsnapshot}" -P "${CMAKE_CURRENT_LIST_DIR}/Support/TestsDataSnapshot.cmake"
		)
set_tests_properties(ExamplesModelsUnchanged PROPERTIES FIXTURES_CLEANUP ExamplesModels)

# The unit and integration tests run on GoogleTest, which vcpkg.json provides. Without it, as in the MSYS2
# build of the R package, these two levels are not registered, and every other test is.
find_package(GTest CONFIG QUIET)
if (GTest_FOUND)
	include("${CMAKE_CURRENT_LIST_DIR}/Unit/CMakeLists.txt")
	include("${CMAKE_CURRENT_LIST_DIR}/Integration/CMakeLists.txt")
else()
	message(WARNING "GoogleTest was not found: the unit and integration tests are not registered")
endif()
include("${CMAKE_CURRENT_LIST_DIR}/System/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/Performance/CMakeLists.txt")
