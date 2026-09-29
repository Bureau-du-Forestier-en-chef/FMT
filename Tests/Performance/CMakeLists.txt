#[[
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
]]

# Performance and allocation benchmarks (issue #349), described in Documentation/PerformanceTesting.md.
# Included by the root CMakeLists.txt with the tests, after Examples/C++/CMakeLists.txt, whose
# createexecutable function and ExamplesModels fixture it uses.

set(FMTPERFORMANCEdir "${CMAKE_CURRENT_LIST_DIR}")
get_filename_component(FMTPERFORMANCEroot "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
# Apart from tests/performance, where the results go: Windows ignores the case of the names.
set(FMTPERFORMANCEgenerated "${CMAKE_BINARY_DIR}/generated/FMTBenchmarkHarness")

# The commit of the sources, read at every build so that no result names a stale one.
find_package(Git QUIET)
add_custom_target(FMTBenchmarkCommit
	COMMAND "${CMAKE_COMMAND}" "-DGIT_EXECUTABLE=${GIT_EXECUTABLE}" "-DSOURCE_DIR=${FMTPERFORMANCEroot}"
		"-DOUTPUT_FILE=${FMTPERFORMANCEgenerated}/BenchmarkCommit.h" -P "${FMTPERFORMANCEdir}/Harness/CommitInfo.cmake"
	BYPRODUCTS "${FMTPERFORMANCEgenerated}/BenchmarkCommit.h"
	COMMENT "Reading the commit of the benchmarked sources"
	)

# The harness: measures, counts allocations, checks and writes the results of any benchmark.
add_library(FMTBenchmarkHarness STATIC
	"${FMTPERFORMANCEdir}/Harness/AllocationMonitor.h"
	"${FMTPERFORMANCEdir}/Harness/Benchmark.h"
	"${FMTPERFORMANCEdir}/Harness/BenchmarkEnvironment.h"
	"${FMTPERFORMANCEdir}/Harness/BenchmarkEnvironment.cpp"
	"${FMTPERFORMANCEdir}/Harness/BenchmarkOptions.h"
	"${FMTPERFORMANCEdir}/Harness/BenchmarkOptions.cpp"
	"${FMTPERFORMANCEdir}/Harness/BenchmarkResult.h"
	"${FMTPERFORMANCEdir}/Harness/BenchmarkResult.cpp"
	"${FMTPERFORMANCEdir}/Harness/BenchmarkRunner.h"
	"${FMTPERFORMANCEdir}/Harness/BenchmarkRunner.cpp"
	"${FMTPERFORMANCEdir}/Harness/BenchmarkSuite.h"
	"${FMTPERFORMANCEdir}/Harness/BenchmarkSuite.cpp"
	"${FMTPERFORMANCEdir}/Harness/JsonWriter.h"
	"${FMTPERFORMANCEdir}/Harness/JsonWriter.cpp"
	"${FMTPERFORMANCEdir}/Harness/MemoryMonitor.h"
	"${FMTPERFORMANCEdir}/Harness/MemoryMonitor.cpp"
	)
if (WIN32)
	target_sources(FMTBenchmarkHarness PRIVATE "${FMTPERFORMANCEdir}/Harness/AllocationMonitorWindows.cpp")
	target_link_libraries(FMTBenchmarkHarness PUBLIC psapi)
else()
	target_sources(FMTBenchmarkHarness PRIVATE "${FMTPERFORMANCEdir}/Harness/AllocationMonitorPortable.cpp")
endif()
add_dependencies(FMTBenchmarkHarness FMTBenchmarkCommit)
target_include_directories(FMTBenchmarkHarness
	PUBLIC "${FMTPERFORMANCEdir}/Harness"
	PRIVATE "${FMTPERFORMANCEgenerated}" ${FMTINCLUDEDIR} ${Boost_INCLUDE_DIRS}
	)
target_compile_definitions(FMTBenchmarkHarness PRIVATE "-DFMTLIBIMPORT" "FMT_BENCHMARK_CONFIGURATION=\"$<CONFIG>\"")
target_link_libraries(FMTBenchmarkHarness PUBLIC FMTlib)
set_property(TARGET FMTBenchmarkHarness PROPERTY CXX_STANDARD 17)
set_property(TARGET FMTBenchmarkHarness PROPERTY CXX_STANDARD_REQUIRED ON)

# The benchmarks of FMT, built like the examples.
createexecutable("${FMTPERFORMANCEdir}/FMTPerformanceTests.cpp" FMTPERFORMANCEexecutable)
target_sources(FMTPerformanceTests PRIVATE
	"${FMTPERFORMANCEdir}/ComplexYieldBenchmarks.h"
	"${FMTPERFORMANCEdir}/ComplexYieldBenchmarks.cpp"
	)
target_include_directories(FMTPerformanceTests PRIVATE "${FMTPERFORMANCEdir}" "${FMTPERFORMANCEroot}/Examples/C++/tests")
target_link_libraries(FMTPerformanceTests PRIVATE FMTBenchmarkHarness)

# One ctest test per row of performance.csv: <executable>;<benchmark>;<result>;<allocations>.
# The executable reads its expected result and allocation bound from the same file at run time: a
# changed value needs no reconfiguration, a new row does. Every test is labelled performance, and
# allocation when its row bounds the allocations. ctest runs them in their short mode; with
# FMT_BENCHMARK_MODE=full in the environment, they measure (run them without -j).
if (NOT WITHOUT_TESTING)
	enable_testing()
	set(FMTPERFORMANCEcsv "${FMTPERFORMANCEdir}/performance.csv")
	set(FMTPERFORMANCEresults "${CMAKE_BINARY_DIR}/tests/performance")
	file(MAKE_DIRECTORY "${FMTPERFORMANCEresults}")
	file(READ "${FMTPERFORMANCEcsv}" FMTPERFORMANCErows)
	string(REPLACE "\r" "" FMTPERFORMANCErows "${FMTPERFORMANCErows}")
	string(REPLACE ";" "," FMTPERFORMANCErows "${FMTPERFORMANCErows}")
	string(REPLACE "\n" ";" FMTPERFORMANCErows "${FMTPERFORMANCErows}")
	foreach(row ${FMTPERFORMANCErows})
		string(REPLACE "," ";" fields "${row}")
		list(GET fields 0 executable)
		if (executable STREQUAL "TEST")#the header of the file
			continue()
		endif()
		if (NOT TARGET "${executable}")
			message(WARNING "performance.csv: no target named \"${executable}\", this row is not registered in ctest")
			continue()
		endif()
		list(GET fields 1 benchmark)
		list(LENGTH fields fieldCount)
		set(bound "")
		if (fieldCount GREATER 3)
			list(GET fields 3 bound)
		endif()
		set(testname "${executable}.${benchmark}")
		add_test(
				NAME "${testname}"
				COMMAND ${executable} --benchmark ${benchmark}
					--model "${FMTPERFORMANCEroot}/Examples/Models/TWD_land/TWD_land.pri"
					--expectations "${FMTPERFORMANCEcsv}"
					--output "${FMTPERFORMANCEresults}/${benchmark}.json"
				WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}"
				)
		set(labels "performance")
		if (NOT bound STREQUAL "")
			list(APPEND labels "allocation")
		endif()
		#77: a check needs what the platform lacks, such as the allocation monitor outside Windows
		set_tests_properties("${testname}" PROPERTIES
				SKIP_RETURN_CODE 77
				FIXTURES_REQUIRED ExamplesModels
				LABELS "${labels}"
				)
	endforeach()
endif (NOT WITHOUT_TESTING)
