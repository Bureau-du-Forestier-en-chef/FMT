#[[
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
]]

#[[
The functions that register the tests with ctest, included by Tests/CMakeLists.txt before any test is
registered:

- getcsvtestcase builds the case part of the name of a CSV row from its arguments;
- checkcsvtestnameparts checks the given and then columns of a CSV row, which name its test by hand;
- getrenamedtests reads the former names of the test executables;
- registersystemtest registers one system test;
- registercsvtests registers the rows of CSV files, one system test per row;
- registergoogletests creates a GoogleTest executable of the unit or integration tests and registers its
  tests, one ctest test per case.

A test name is given once. registersystemtest keeps the origin of each name in a GLOBAL property, so
that the check covers every registration, the CSV rows, the Excel test and the Python and R tests alike,
and a name given twice stops the configuration with the origin of both tests.
]]

include(GoogleTest)

#Case part of the ctest name of a CSV row, built from its first two arguments joined by |:
#../../../../Examples/Models/<M>/<M>.pri becomes <M>, an absolute path becomes <drive>:/.../<folder>/<file>,
#<file> empty when the path ends with / (so the private rows keep T:/ in their name), spaces become _, and a
#case longer than 60 characters keeps its first 51 followed by ~ and 8 characters of its SHA1. Empty when
#the row has no argument.
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
		elseif (field MATCHES "^([A-Za-z]:/)(.*/)?([^/]+)/[^/]*$")
			set(reducedfield "${CMAKE_MATCH_1}.../${CMAKE_MATCH_3}/")
			#The file is not read from a group of the expression: if(MATCHES) sets no CMAKE_MATCH_<n> for a group
			#that matches an empty string, as the file of a path that ends with /, and the variable would keep the
			#value of an earlier match or of the calling scope. string(REGEX MATCH) sets the groups again: it
			#comes after them.
			string(REGEX MATCH "[^/]+$" file "${field}")
			set(field "${reducedfield}${file}")
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

#Stops the configuration when <given> and <then>, the given and then columns of a CSV row, cannot name its test:
#the two go together, and each is in PascalCase, ^[A-Z][A-Za-z0-9]*$. <origin> names the row, such as
#"basetests.csv, line 12".
function(checkcsvtestnameparts origin given then)
	if (then STREQUAL "")
		message(FATAL_ERROR "${origin}: the given and then columns go together, and this row has a given without a then")
	elseif (given STREQUAL "")
		message(FATAL_ERROR "${origin}: the given and then columns go together, and this row has a then without a given")
	endif()
	foreach(column IN ITEMS given then)
		if (NOT "${${column}}" MATCHES "^[A-Z][A-Za-z0-9]*$")
			message(FATAL_ERROR "${origin}: the ${column} column, \"${${column}}\", is not in PascalCase (^[A-Z][A-Za-z0-9]*$)")
		endif()
	endforeach()
endfunction()

#Reads Tests/Support/RenamedTests.csv, the table of the test executables renamed under #350 (former;new),
#into <formernames> and <newnames>, two lists in the same order.
function(getrenamedtests formernames newnames)
	file(READ "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/RenamedTests.csv" data)
	string(REPLACE "\r" "" data "${data}")
	string(REPLACE ";" "," data "${data}")
	string(REPLACE "\n" ";" data "${data}")
	set(former "")
	set(new "")
	foreach(line IN LISTS data)
		if (line AND NOT line STREQUAL "former,new")#former;new is the header of the file
			string(REPLACE "," ";" line "${line}")
			list(GET line 0 formername)
			list(GET line 1 newname)
			list(APPEND former "${formername}")
			list(APPEND new "${newname}")
		endif()
	endforeach()
	set(${formernames} "${former}" PARENT_SCOPE)
	set(${newnames} "${new}" PARENT_SCOPE)
endfunction()

#Registers the system test <testname>, which runs the command given after <origin>: an executable target
#and its arguments, or an interpreter, its test runner and their arguments, run from the folder of the
#executables. The test is labelled system and with its interface: cpp, unless the command is preceded by
#INTERFACE <interface>, such as INTERFACE python. When the command runs a target whose FMTTESTScomponent
#property is set, such as wrappercore for the executables of Tests/System/Cpp/FMTWrapperCore, the test is
#also labelled with that component. It is skipped when it returns 77 (Testing::skip in
#Tests/Support/TestTools.h, or the runners of Tests/System/Python and Tests/System/R), and requires the
#ExamplesModels fixture of Tests/CMakeLists.txt. <origin> says where the test comes from, such as
#"basetests.csv, line 12": the configuration stops, naming both origins, when <testname> is already given.
function(registersystemtest testname origin)
	get_property(alreadygiven GLOBAL PROPERTY "FMTTESTSorigin.${testname}" SET)
	if (alreadygiven)
		get_property(registeredorigin GLOBAL PROPERTY "FMTTESTSorigin.${testname}")
		message(FATAL_ERROR "${origin}: the test name \"${testname}\" is already given to ${registeredorigin}")
	endif(alreadygiven)
	set_property(GLOBAL PROPERTY "FMTTESTSorigin.${testname}" "${origin}")
	set(command ${ARGN})
	set(interface "cpp")
	list(LENGTH command commandlength)
	if (commandlength GREATER 2)
		list(GET command 0 firstargument)
		if (firstargument STREQUAL "INTERFACE")
			list(GET command 1 interface)
			list(REMOVE_AT command 0 1)
		endif(firstargument STREQUAL "INTERFACE")
	endif(commandlength GREATER 2)
	set(labels "system;${interface}")
	list(GET command 0 executable)
	if (TARGET "${executable}")
		get_target_property(component "${executable}" FMTTESTScomponent)
		if (component)
			list(APPEND labels "${component}")
		endif(component)
	endif(TARGET "${executable}")
	add_test(
			NAME "${testname}"
			COMMAND ${command}
			WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}"
			)
	set_tests_properties("${testname}" PROPERTIES
			LABELS "${labels}"
			SKIP_RETURN_CODE 77
			FIXTURES_REQUIRED ExamplesModels
			)
endfunction(registersystemtest testname origin)

#Registers the rows of the CSV files <csvfiles>, one system test per row. A row reads
#<target>;<argument 1>;<argument 2>;<argument 3>[;<given>;<then>]: the test runs <target> with its arguments, an
#empty one left out, and is named System.<target>.<case>.
#- The case of a row that has a given and a then is Given<given>When<target>Then<then>, a name written by hand:
#  the when is always the target. The two go together, and each is in PascalCase: otherwise the configuration
#  stops, naming the file and the line (checkcsvtestnameparts).
#- The case of a row without them is built from its arguments (getcsvtestcase), and a row without argument is
#  named System.<target>. The rows of BFECtests.csv, the private rows, always keep this automatic name, and the
#  T:/ of their path in it: their given and then are ignored, with a warning.
#- The first row of a file, TEST, is its header; an empty row is skipped.
#- A row whose target does not exist is not registered, with a warning: every target a row may name must
#  exist before the call.
#- A row that names a target by its former name (getrenamedtests) is read as a row of its new name. The
#  file gets one warning, which counts these rows: tools/testMigration/Update_PrivateTests.bat renames them
#  in the private copies.
#- The rows of a target listed in <sharedoutputs> write into the same output files: they never run at the
#  same time.
#- The rows of knownbugs.csv reveal a known bug: they are registered, but not run until it is fixed.
#- Each registered target gets its output folder, tests/<target> in the build folder.
function(registercsvtests csvfiles sharedoutputs)
	getrenamedtests(formernames newnames)
	foreach(testfile IN LISTS csvfiles)
		get_filename_component(testfilename "${testfile}" NAME)
		set(renamedrows 0)
		set(ignorednamelines "")
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
				if (testname STREQUAL "TEST")#TEST is the header of the files
					continue()
				endif()
				list(GET line 1 primarylocation)
				list(GET line 2 scenario)
				list(GET line 3 dblvalue)
				set(given "")
				set(then "")
				list(LENGTH line fieldcount)
				if (fieldcount GREATER 4)
					list(GET line 4 given)
				endif()
				if (fieldcount GREATER 5)
					list(GET line 5 then)
				endif()
				if (NOT "${given}${then}" STREQUAL "")
					if (testfilename STREQUAL "BFECtests.csv")
						list(APPEND ignorednamelines ${linenumber})
						set(given "")
						set(then "")
					else()
						checkcsvtestnameparts("${testfilename}, line ${linenumber}" "${given}" "${then}")
					endif()
				endif()
				if (NOT TARGET "${testname}")
					list(FIND formernames "${testname}" renamedindex)
					if (renamedindex GREATER -1)
						list(GET newnames ${renamedindex} newname)
						if (TARGET "${newname}")
							set(testname "${newname}")
							math(EXPR renamedrows "${renamedrows} + 1")
						endif()
					endif()
				endif()
				if (TARGET "${testname}")#Got the executable!
					file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/tests/${testname}")
					if (NOT given STREQUAL "")
						set(testcase "Given${given}When${testname}Then${then}")
					else()
						getcsvtestcase("${primarylocation}" "${scenario}" testcase)
					endif()
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
				else()
					message(WARNING "${testfilename}: no target named \"${testname}\", this row is not registered in ctest")
				endif(TARGET "${testname}")
			endif(line)
		endforeach()
		if (renamedrows GREATER 0)
			message(WARNING "${testfilename}: ${renamedrows} rows name a test executable by its former name, read as its new name (Tests/Support/RenamedTests.csv): rename them in ${testfile} with tools/testMigration/Update_PrivateTests.bat")
		endif()
		if (NOT ignorednamelines STREQUAL "")
			list(LENGTH ignorednamelines ignorednamecount)
			string(REPLACE ";" ", " ignorednamelines "${ignorednamelines}")
			message(WARNING "${testfilename}: the given and then columns of ${ignorednamecount} rows are ignored (lines ${ignorednamelines}): the private rows keep their automatic name, and the T:/ of their path in it")
		endif()
	endforeach()
endfunction(registercsvtests csvfiles sharedoutputs)

#Creates the GoogleTest executable <target> from the sources that follow, the tests of the level <level>, Unit
#or Integration, on the component <component>, such as core, and registers each of its cases with ctest.
#- The executable links to FMTlib and to GoogleTest, which gives it its main, and is written to the folder of
#  the other test executables, bin/<build type>, where it finds its DLLs (createnamedexecutable).
#- ctest lists the cases when it runs, from bin/<build type>, by --gtest_list_tests (DISCOVERY_MODE PRE_TEST):
#  the build runs nothing. The list is taken again when the executable is newer than it.
#- A case is named <level>.<suite>.<case>, such as Unit.FMTOperator.GivenMultiplyAndAddWhenPrecedenceThenMultiplyComesFirst,
#  and labelled with the level in lowercase and the component: unit;core. A case whose name starts with
#  DISABLED_ (an open defect) is registered disabled, under its name without the prefix.
#- The names of the cases are only known when ctest runs, after the check of registersystemtest. They cannot
#  take the name of a system test, which starts with System., and GoogleTest refuses two cases of the same name
#  in one executable. Across executables, a script that ctest reads after the cases of <target>,
#  <target>_names.cmake in the build folder, stops ctest when one of them has the name of a case of another
#  GoogleTest executable, naming both executables.
function(registergoogletests target level component)
	createnamedexecutable(${target} ${ARGN})
	target_link_libraries(${target} PRIVATE GTest::gtest GTest::gtest_main)
	string(TOLOWER "${level}" levellabel)
	#The ; of the labels is escaped: in PRE_TEST mode, the properties are parsed twice before they reach the
	#tests, and an unescaped list keeps only its first label (CMake 3.31).
	gtest_discover_tests(${target}
			DISCOVERY_MODE PRE_TEST
			DISCOVERY_TIMEOUT 60
			TEST_PREFIX "${level}."
			WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}"
			PROPERTIES LABELS "${levellabel}\\;${component}"
			)
	#gtest_discover_tests lists the names of the cases in <target>_TESTS, once ctest has read them.
	string(CONFIGURE [=[
#Written by registergoogletests (Tests/Support/TestRegistration.cmake), read by ctest after the cases of @target@.
foreach(FMTTESTSname IN LISTS @target@_TESTS)
	if (DEFINED "FMTTESTSorigin.${FMTTESTSname}")
		message(FATAL_ERROR "@target@: the test name \"${FMTTESTSname}\" is already given to ${FMTTESTSorigin.${FMTTESTSname}}")
	endif()
	set("FMTTESTSorigin.${FMTTESTSname}" "@target@")
endforeach()
]=] namescheck @ONLY)
	file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/${target}_names.cmake" "${namescheck}")
	set_property(DIRECTORY APPEND PROPERTY TEST_INCLUDE_FILES "${CMAKE_CURRENT_BINARY_DIR}/${target}_names.cmake")
endfunction(registergoogletests target level component)
