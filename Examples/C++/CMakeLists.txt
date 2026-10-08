#[[
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
]]


cmake_minimum_required(VERSION 3.20)

FILE(GLOB FMTexecutables ${CMAKE_CURRENT_SOURCE_DIR}/Examples/C++/*.cpp)
foreach(pathtosource ${FMTexecutables})
	createexecutable("${pathtosource}" executablename)
endforeach()

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


#Get the tests 
#Grab everything in the test folder
#Create test folders
if (NOT WITHOUT_TESTING)
	#file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}/tests")
	file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/tests")
	enable_testing()
	#No test may modify Examples/Models: its files are hashed before the tests and compared after them
	set(FMTTESTSsnapshot "${CMAKE_BINARY_DIR}/tests/ExamplesModels.snapshot")
	add_test(
			NAME ExamplesModelsSnapshot
			COMMAND "${CMAKE_COMMAND}" -DMODE=save "-DDATA_DIR=${CMAKE_CURRENT_SOURCE_DIR}/Examples/Models" "-DSNAPSHOT_FILE=${FMTTESTSsnapshot}" -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/TestsDataSnapshot.cmake"
			)
	set_tests_properties(ExamplesModelsSnapshot PROPERTIES FIXTURES_SETUP ExamplesModels)
	add_test(
			NAME ExamplesModelsUnchanged
			COMMAND "${CMAKE_COMMAND}" -DMODE=compare "-DDATA_DIR=${CMAKE_CURRENT_SOURCE_DIR}/Examples/Models" "-DSNAPSHOT_FILE=${FMTTESTSsnapshot}" -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/TestsDataSnapshot.cmake"
			)
	set_tests_properties(ExamplesModelsUnchanged PROPERTIES FIXTURES_CLEANUP ExamplesModels)
	#Executables whose rows write into the same output files: their rows never run at the same time
	set(FMTTESTSsharedoutputs planningtest testActionsAggregations testOAschedulerBFEC testreadwriteproject)
	#Names given so far, and the CSV row of each: a name given twice stops the configuration
	set(FMTTESTSnames "")
	set(FMTTESTSorigins "")
	FILE(GLOB FMTTESTSfiles ${CMAKE_CURRENT_SOURCE_DIR}/Examples/C++/tests/*.csv)
	foreach(testfile ${FMTTESTSfiles})
		get_filename_component(testfilename "${testfile}" NAME)
		file(READ ${testfile} data)
		string(REPLACE "\r" "" data "${data}")
		string(REPLACE ";" "," data "${data}")
		string(REPLACE "\n" ";" data "${data}")
		set(linenumber 0)
		foreach(line IN LISTS data)
			math(EXPR linenumber "${linenumber} + 1")
			if (line)
				string(REPLACE "," ";" line "${line}")
				list(GET line 0 testname)
				list(LENGTH line listsize)
				list(GET line 1 primarylocation)
				list(GET line 2 scenario)
				list(GET line 3 dblvalue)
				if (TARGET "${testname}")#Got the executable!
						#file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}/tests/${testname}")
						file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/tests/${testname}")
						getcsvtestcase("${primarylocation}" "${scenario}" testcase)
						set(fullname "System.${testname}")
						if (NOT testcase STREQUAL "")
							string(APPEND fullname ".${testcase}")
						endif()
						list(FIND FMTTESTSnames "${fullname}" registeredat)
						if (NOT registeredat EQUAL -1)
							list(GET FMTTESTSorigins ${registeredat} registeredorigin)
							message(FATAL_ERROR "${testfilename}, line ${linenumber}: the test name \"${fullname}\" is already given to ${registeredorigin}")
						endif()
						list(APPEND FMTTESTSnames "${fullname}")
						list(APPEND FMTTESTSorigins "${testfilename}, line ${linenumber}")
						add_test(
								NAME "${fullname}"
								COMMAND ${testname} ${primarylocation} ${scenario} ${dblvalue}
								WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}"
								)
						#77: a feature needed by the test is not compiled (Testing::skip in tests/TestTools.h)
						set_tests_properties("${fullname}" PROPERTIES
								LABELS "system;cpp"
								SKIP_RETURN_CODE 77
								FIXTURES_REQUIRED ExamplesModels
								)
						if (testname IN_LIST FMTTESTSsharedoutputs)
							set_tests_properties("${fullname}" PROPERTIES RESOURCE_LOCK "${testname}")
						endif()
						#knownbugs.csv: tests revealing a known bug, registered but not run until it is fixed
						if (testfilename STREQUAL "knownbugs.csv")
							set_tests_properties("${fullname}" PROPERTIES DISABLED TRUE)
						endif()
				elseif (NOT testname STREQUAL "TEST")#TEST is the header of the files
						message(WARNING "${testfilename}: no target named \"${testname}\", this row is not registered in ctest")
				endif(TARGET "${testname}")
			endif(line)
		endforeach()
	endforeach()
endif (NOT WITHOUT_TESTING)