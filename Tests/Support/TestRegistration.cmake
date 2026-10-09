#[[
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
]]

#[[
The functions that register the tests with ctest, included by the root CMakeLists.txt before the first
of them is called, by Excel/CMakeLists.txt:

- getcsvtestcase builds the case part of the name of a CSV row;
- registersystemtest registers one system test;
- registercsvtests registers the rows of CSV files, one system test per row.

A test name is given once. registersystemtest keeps the origin of each name in a GLOBAL property, so
that the check covers every registration, the CSV rows and Excel/CMakeLists.txt alike, and a name
given twice stops the configuration with the origin of both tests.
]]

#Case part of the ctest name of a CSV row, built from its first two arguments joined by |:
#../../../../Examples/Models/<M>/<M>.pri becomes <M>, an absolute path becomes <drive>:/.../<folder>/<file>
#(so the private rows keep T:/ in their name), spaces become _, and a case longer than 60 characters
#keeps its first 51 followed by ~ and 8 characters of its SHA1. Empty when the row has no argument.
function(getcsvtestcase primarylocation scenario testcase)
	set(arguments "")
	foreach(argument IN ITEMS "${primarylocation}" "${scenario}")
		if (NOT argument STREQUAL "")
			list(APPEND arguments "${argument}")
		endif()
	endforeach()
	string(REPLACE "|" ";" fields "${arguments}")
	set(reducedfields "")
	foreach(field IN LISTS fields)
		if (field MATCHES "^\\.\\./\\.\\./\\.\\./\\.\\./Examples/Models/([^/]+)/([^/]+)\\.pri$")
			if ("${CMAKE_MATCH_1}" STREQUAL "${CMAKE_MATCH_2}")
				set(field "${CMAKE_MATCH_1}")
			endif()
		elseif (field MATCHES "^([A-Za-z]:/)(.*/)?([^/]+)/([^/]*)$")
			set(field "${CMAKE_MATCH_1}.../${CMAKE_MATCH_3}")
			if (NOT CMAKE_MATCH_4 STREQUAL "")
				string(APPEND field "/${CMAKE_MATCH_4}")
			endif()
		endif()
		list(APPEND reducedfields "${field}")
	endforeach()
	string(REPLACE ";" "|" case "${reducedfields}")
	string(REPLACE " " "_" case "${case}")
	string(LENGTH "${case}" caselength)
	if (caselength GREATER 60)
		string(SHA1 casehash "${case}")
		string(SUBSTRING "${casehash}" 0 8 casehash)
		string(SUBSTRING "${case}" 0 51 case)
		string(APPEND case "~${casehash}")
	endif()
	set(${testcase} "${case}" PARENT_SCOPE)
endfunction()

#Registers the system test <testname>, which runs the command given after <origin>: an executable target
#and its arguments, run from the folder of the executables. The test is labelled system and cpp, skipped
#when it returns 77 (Testing::skip in Examples/C++/tests/TestTools.h), and requires the ExamplesModels
#fixture of Tests/CMakeLists.txt. <origin> says where the test comes from, such as
#"basetests.csv, line 12": the configuration stops, naming both origins, when <testname> is already given.
function(registersystemtest testname origin)
	get_property(alreadygiven GLOBAL PROPERTY "FMTTESTSorigin.${testname}" SET)
	if (alreadygiven)
		get_property(registeredorigin GLOBAL PROPERTY "FMTTESTSorigin.${testname}")
		message(FATAL_ERROR "${origin}: the test name \"${testname}\" is already given to ${registeredorigin}")
	endif(alreadygiven)
	set_property(GLOBAL PROPERTY "FMTTESTSorigin.${testname}" "${origin}")
	add_test(
			NAME "${testname}"
			COMMAND ${ARGN}
			WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}"
			)
	set_tests_properties("${testname}" PROPERTIES
			LABELS "system;cpp"
			SKIP_RETURN_CODE 77
			FIXTURES_REQUIRED ExamplesModels
			)
endfunction(registersystemtest testname origin)

#Registers the rows of the CSV files <csvfiles>, one system test per row. A row reads
#<target>;<argument 1>;<argument 2>;<argument 3>: the test runs <target> with its arguments, an empty one
#left out, and is named System.<target>, followed by .<case> when the row has arguments (getcsvtestcase).
#- The first row of a file, TEST, is its header; an empty row is skipped.
#- A row whose target does not exist is not registered, with a warning: every target a row may name must
#  exist before the call.
#- The rows of a target listed in <sharedoutputs> write into the same output files: they never run at the
#  same time.
#- The rows of knownbugs.csv reveal a known bug: they are registered, but not run until it is fixed.
#- Each registered target gets its output folder, tests/<target> in the build folder.
function(registercsvtests csvfiles sharedoutputs)
	foreach(testfile IN LISTS csvfiles)
		get_filename_component(testfilename "${testfile}" NAME)
		file(READ "${testfile}" data)
		string(REPLACE "\r" "" data "${data}")
		string(REPLACE ";" "," data "${data}")
		string(REPLACE "\n" ";" data "${data}")
		set(linenumber 0)
		foreach(line IN LISTS data)
			math(EXPR linenumber "${linenumber} + 1")
			if (line)
				string(REPLACE "," ";" line "${line}")
				list(GET line 0 testname)
				list(GET line 1 primarylocation)
				list(GET line 2 scenario)
				list(GET line 3 dblvalue)
				if (TARGET "${testname}")#Got the executable!
					file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/tests/${testname}")
					getcsvtestcase("${primarylocation}" "${scenario}" testcase)
					set(fullname "System.${testname}")
					if (NOT testcase STREQUAL "")
						string(APPEND fullname ".${testcase}")
					endif()
					registersystemtest("${fullname}" "${testfilename}, line ${linenumber}"
						${testname} ${primarylocation} ${scenario} ${dblvalue})
					if (testname IN_LIST sharedoutputs)
						set_tests_properties("${fullname}" PROPERTIES RESOURCE_LOCK "${testname}")
					endif()
					if (testfilename STREQUAL "knownbugs.csv")
						set_tests_properties("${fullname}" PROPERTIES DISABLED TRUE)
					endif()
				elseif (NOT testname STREQUAL "TEST")#TEST is the header of the files
					message(WARNING "${testfilename}: no target named \"${testname}\", this row is not registered in ctest")
				endif(TARGET "${testname}")
			endif(line)
		endforeach()
	endforeach()
endfunction(registercsvtests csvfiles sharedoutputs)
