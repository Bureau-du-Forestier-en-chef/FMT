# Snapshot of the example models used by the tests.
#
#   cmake -DMODE=save    -DDATA_DIR=<folder> -DSNAPSHOT_FILE=<file> -P TestsDataSnapshot.cmake
#   cmake -DMODE=compare -DDATA_DIR=<folder> -DSNAPSHOT_FILE=<file> -P TestsDataSnapshot.cmake
#
# save records the SHA256 and the relative path of every file of DATA_DIR in SNAPSHOT_FILE.
# compare computes them again and fails when a file was added, removed or modified since the
# save. Examples/C++/CMakeLists.txt registers both modes as the setup and the cleanup of the
# ExamplesModels fixture, required by every test of the CSV files: no test may write into
# Examples/Models (a test works on a copy, see Testing::copyProject in
# Examples/C++/tests/TestTools.h). The comparison does not use git, so uncommitted scenarios
# are allowed.

cmake_minimum_required(VERSION 3.20)

foreach(REQUIRED_VARIABLE MODE DATA_DIR SNAPSHOT_FILE)
	if (NOT DEFINED ${REQUIRED_VARIABLE})
		message(FATAL_ERROR "TestsDataSnapshot.cmake: ${REQUIRED_VARIABLE} is not defined")
	endif()
endforeach()

if (NOT IS_DIRECTORY "${DATA_DIR}")
	message(FATAL_ERROR "TestsDataSnapshot.cmake: ${DATA_DIR} is not a folder")
endif()

# Returns one "<sha256> <relative path>" entry per file of p_folder, sorted by path.
function(computeSnapshot p_folder p_result)
	file(GLOB_RECURSE FILE_PATHS LIST_DIRECTORIES false RELATIVE "${p_folder}" "${p_folder}/*")
	list(SORT FILE_PATHS)
	set(ENTRIES "")
	foreach(FILE_PATH IN LISTS FILE_PATHS)
		file(SHA256 "${p_folder}/${FILE_PATH}" HASH)
		list(APPEND ENTRIES "${HASH} ${FILE_PATH}")
	endforeach()
	set(${p_result} "${ENTRIES}" PARENT_SCOPE)
endfunction()

computeSnapshot("${DATA_DIR}" CURRENT)
list(LENGTH CURRENT FILE_COUNT)

if (MODE STREQUAL "save")
	string(REPLACE ";" "\n" CONTENT "${CURRENT}")
	file(WRITE "${SNAPSHOT_FILE}" "${CONTENT}\n")
	message(STATUS "${FILE_COUNT} files of ${DATA_DIR} recorded in ${SNAPSHOT_FILE}")
elseif (MODE STREQUAL "compare")
	if (NOT EXISTS "${SNAPSHOT_FILE}")
		message(FATAL_ERROR "No snapshot in ${SNAPSHOT_FILE}: the ExamplesModelsSnapshot test did not run")
	endif()
	file(READ "${SNAPSHOT_FILE}" CONTENT)
	string(STRIP "${CONTENT}" CONTENT)
	string(REPLACE "\n" ";" SAVED "${CONTENT}")
	# A modified file appears on both sides, an added one after only, a removed one before only.
	set(DIFFERENCES "")
	foreach(ENTRY IN LISTS SAVED)
		if (NOT ENTRY IN_LIST CURRENT)
			string(APPEND DIFFERENCES "\n  before the tests: ${ENTRY}")
		endif()
	endforeach()
	foreach(ENTRY IN LISTS CURRENT)
		if (NOT ENTRY IN_LIST SAVED)
			string(APPEND DIFFERENCES "\n  after the tests:  ${ENTRY}")
		endif()
	endforeach()
	if (DIFFERENCES)
		message(FATAL_ERROR "The tests modified ${DATA_DIR}:${DIFFERENCES}\nA test must work on a copy of its project.")
	endif()
	message(STATUS "${FILE_COUNT} files of ${DATA_DIR} unchanged")
else()
	message(FATAL_ERROR "TestsDataSnapshot.cmake: unknown MODE ${MODE} (save or compare)")
endif()
