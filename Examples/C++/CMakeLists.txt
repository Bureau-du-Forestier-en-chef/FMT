#[[
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
]]


cmake_minimum_required(VERSION 3.6)

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
	FILE(GLOB FMTTESTSfiles ${CMAKE_CURRENT_SOURCE_DIR}/Examples/C++/tests/*.csv)
	foreach(testfile ${FMTTESTSfiles})
		file(READ ${testfile} data)
		string(REPLACE ";" "," data "${data}")
		string(REPLACE "\n" ";" data "${data}")
		enable_testing()
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
				endif(TARGET "${testname}")
			endif(line)
		endforeach()
	endforeach()
endif (NOT WITHOUT_TESTING)