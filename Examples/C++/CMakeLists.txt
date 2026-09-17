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
	FILE(GLOB FMTTESTSfiles ${CMAKE_CURRENT_SOURCE_DIR}/Examples/C++/tests/*.csv)
	foreach(testfile ${FMTTESTSfiles})
		get_filename_component(testfilename "${testfile}" NAME)
		file(READ ${testfile} data)
		string(REPLACE ";" "," data "${data}")
		string(REPLACE "\n" ";" data "${data}")
		foreach(line ${data})
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
						set(fullname "${testname}${primarylocation}${scenario}")
						string(REPLACE " " "_" fullname "${fullname}")
						add_test(
								NAME "${fullname}"
								COMMAND ${testname} ${primarylocation} ${scenario} ${dblvalue}
								WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}"
								)
						#77: a feature needed by the test is not compiled (Testing::skip in tests/TestTools.h)
						set_tests_properties("${fullname}" PROPERTIES
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