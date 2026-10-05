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
	"${FMTPERFORMANCEdir}/Harness/Benchmark.cpp"
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
	"${FMTPERFORMANCEdir}/Harness/DatasetFingerprint.h"
	"${FMTPERFORMANCEdir}/Harness/DatasetFingerprint.cpp"
	"${FMTPERFORMANCEdir}/Harness/JsonWriter.h"
	"${FMTPERFORMANCEdir}/Harness/JsonWriter.cpp"
	"${FMTPERFORMANCEdir}/Harness/MemoryMonitor.h"
	"${FMTPERFORMANCEdir}/Harness/MemoryMonitor.cpp"
	"${FMTPERFORMANCEdir}/Harness/ProcessorPolicy.h"
	"${FMTPERFORMANCEdir}/Harness/ProcessorPolicy.cpp"
	"${FMTPERFORMANCEdir}/Harness/RunSettings.h"
	"${FMTPERFORMANCEdir}/Harness/RunSettings.cpp"
	)
if (WIN32)
	target_sources(FMTBenchmarkHarness PRIVATE "${FMTPERFORMANCEdir}/Harness/AllocationMonitorWindows.cpp")
	# psapi reads the memory of the process, bcrypt computes the fingerprints of the datasets.
	target_link_libraries(FMTBenchmarkHarness PUBLIC psapi bcrypt)
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
	"${FMTPERFORMANCEdir}/DefinedBenchmarks.h"
	"${FMTPERFORMANCEdir}/DefinedBenchmarks.cpp"
	"${FMTPERFORMANCEdir}/FlowBenchmarks.h"
	"${FMTPERFORMANCEdir}/FlowBenchmarks.cpp"
	"${FMTPERFORMANCEdir}/MaskBenchmarks.h"
	"${FMTPERFORMANCEdir}/MaskBenchmarks.cpp"
	"${FMTPERFORMANCEdir}/ParserBenchmarks.h"
	"${FMTPERFORMANCEdir}/ParserBenchmarks.cpp"
	"${FMTPERFORMANCEdir}/PerfYields.h"
	"${FMTPERFORMANCEdir}/PerfYields.cpp"
	"${FMTPERFORMANCEdir}/QuietFmt.h"
	"${FMTPERFORMANCEdir}/QuietFmt.cpp"
	"${FMTPERFORMANCEdir}/YieldBenchmarks.h"
	"${FMTPERFORMANCEdir}/YieldBenchmarks.cpp"
	)
target_include_directories(FMTPerformanceTests PRIVATE "${FMTPERFORMANCEdir}" "${FMTPERFORMANCEroot}/Examples/C++/tests")
target_link_libraries(FMTPerformanceTests PRIVATE FMTBenchmarkHarness)

# One ctest test per row of performance.csv:
# <executable>;<benchmark>;<result>;<allocations>;<retained bytes>;<peak MB>.
# The executable reads its expected result and bounds from the same file at run time: a changed
# value needs no reconfiguration, a new row does. Every test is labelled performance, allocation
# when its row bounds the allocations, and memory when it bounds the memory a call keeps or the
# peak memory of the process. ctest runs them in their short mode; with FMT_BENCHMARK_MODE=full in
# the environment, they measure (run them without -j).
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
		set(retainedBound "")
		if (fieldCount GREATER 4)
			list(GET fields 4 retainedBound)
		endif()
		set(peakBound "")
		if (fieldCount GREATER 5)
			list(GET fields 5 peakBound)
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
		if (NOT retainedBound STREQUAL "" OR NOT peakBound STREQUAL "")
			list(APPEND labels "memory")
		endif()
		#77: a check needs what the platform lacks, such as the allocation monitor outside Windows
		set_tests_properties("${testname}" PROPERTIES
				SKIP_RETURN_CODE 77
				FIXTURES_REQUIRED ExamplesModels
				LABELS "${labels}"
				)
	endforeach()
	# The private benchmarks: rows of performance-private.csv, a local file that git ignores, on
	# models outside the source tree. Each row gives the arguments of its benchmark after its
	# bounds; the first argument is the primary file of the model. That model must be on T:\, like
	# those of the private tests: the name of the test holds its path, which keeps the test out of
	# the base suite (-E "T:/"). The tests are labelled bfec-perf, and only that.
	set(FMTPERFORMANCEprivate "${FMTPERFORMANCEdir}/performance-private.csv")
	if (EXISTS "${FMTPERFORMANCEprivate}")
		file(READ "${FMTPERFORMANCEprivate}" FMTPERFORMANCEprivaterows)
		string(REPLACE "\r" "" FMTPERFORMANCEprivaterows "${FMTPERFORMANCEprivaterows}")
		string(REPLACE ";" "," FMTPERFORMANCEprivaterows "${FMTPERFORMANCEprivaterows}")
		string(REPLACE "\n" ";" FMTPERFORMANCEprivaterows "${FMTPERFORMANCEprivaterows}")
		foreach(row ${FMTPERFORMANCEprivaterows})
			string(REPLACE "," ";" fields "${row}")
			list(GET fields 0 executable)
			if (executable STREQUAL "TEST")#the header of the file
				continue()
			endif()
			if (NOT TARGET "${executable}")
				message(WARNING "performance-private.csv: no target named \"${executable}\", this row is not registered in ctest")
				continue()
			endif()
			list(GET fields 1 benchmark)
			list(LENGTH fields fieldCount)
			set(arguments "")
			if (fieldCount GREATER 6)
				list(GET fields 6 arguments)
			endif()
			string(REPLACE "|" ";" arguments "${arguments}")
			list(LENGTH arguments argumentCount)
			set(model "")
			if (argumentCount GREATER 0)
				list(GET arguments 0 model)
			endif()
			string(REPLACE "\\" "/" model "${model}")
			if (NOT model MATCHES "^[Tt]:/")
				message(WARNING "performance-private.csv: ${benchmark} does not read a model on T:/, this row is not registered in ctest")
				continue()
			endif()
			set(testname "${executable}.${benchmark}|${model}")
			string(REPLACE " " "_" testname "${testname}")
			add_test(
					NAME "${testname}"
					COMMAND ${executable} --benchmark ${benchmark}
						--expectations "${FMTPERFORMANCEprivate}"
						--output "${FMTPERFORMANCEresults}/${benchmark}.json"
					WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}"
					)
			set_tests_properties("${testname}" PROPERTIES
					SKIP_RETURN_CODE 77
					LABELS "bfec-perf"
					)
		endforeach()
	endif()
endif (NOT WITHOUT_TESTING)
