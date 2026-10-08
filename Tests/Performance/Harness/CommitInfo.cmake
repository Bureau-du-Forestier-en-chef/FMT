#[[
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
]]

# Writes OUTPUT_FILE, a header holding the commit of SOURCE_DIR and whether the working tree differs
# from it (modified or untracked files), so that the results of a benchmark name the sources they
# measured. Run at every build by the FMTBenchmarkCommit target (Tests/Performance/CMakeLists.txt):
# the header is only rewritten when its content changes, so that an unchanged commit recompiles
# nothing.
#
#   cmake -DGIT_EXECUTABLE=<git> -DSOURCE_DIR=<sources> -DOUTPUT_FILE=<header> -P CommitInfo.cmake

set(commit "unknown")
set(dirty 0)
if (GIT_EXECUTABLE)
	execute_process(
		COMMAND "${GIT_EXECUTABLE}" rev-parse HEAD
		WORKING_DIRECTORY "${SOURCE_DIR}"
		OUTPUT_VARIABLE gitCommit
		RESULT_VARIABLE gitResult
		OUTPUT_STRIP_TRAILING_WHITESPACE
		ERROR_QUIET
		)
	if (gitResult EQUAL 0)
		set(commit "${gitCommit}")
		execute_process(
			COMMAND "${GIT_EXECUTABLE}" status --porcelain
			WORKING_DIRECTORY "${SOURCE_DIR}"
			OUTPUT_VARIABLE gitStatus
			RESULT_VARIABLE statusResult
			OUTPUT_STRIP_TRAILING_WHITESPACE
			ERROR_QUIET
			)
		if (statusResult EQUAL 0 AND NOT gitStatus STREQUAL "")
			set(dirty 1)
		endif()
	endif()
endif()

set(content "// Written at build time by Tests/Performance/Harness/CommitInfo.cmake: do not edit.\n")
string(APPEND content "#define FMT_BENCHMARK_COMMIT \"${commit}\"\n")
string(APPEND content "#define FMT_BENCHMARK_DIRTY ${dirty}\n")
set(current "")
if (EXISTS "${OUTPUT_FILE}")
	file(READ "${OUTPUT_FILE}" current)
endif()
if (NOT current STREQUAL content)
	file(WRITE "${OUTPUT_FILE}" "${content}")
endif()
