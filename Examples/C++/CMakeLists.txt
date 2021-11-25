#[[
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
]]


cmake_minimum_required(VERSION 3.6)

function(createexecutable executablecpplocation)
  set(newprojectsourcefiles "${executablecpplocation}")
  list(APPEND newprojectsourcefiles "${FMTHEADERFILES}")
  list(APPEND newprojectsourcefiles "${FMTSOURCEFILES}")
  get_filename_component(executablename "${executablecpplocation}" NAME_WE)
  message("Generating build for ${executablename}")
  add_executable(${executablename} ${newprojectsourcefiles})
  if (NOT MSVC)
		set_target_properties(${executablename}
			PROPERTIES
			ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}"
			LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}"
			RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}"
		)
		set_target_properties(${executablename}
			PROPERTIES
			ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}"
			LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}"
			RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}"
		)
	endif(NOT MSVC)
	set_property(TARGET ${executablename} PROPERTY CXX_STANDARD 11)
	set_property(TARGET ${executablename} PROPERTY CXX_STANDARD_REQUIRED ON)
	if (MOSEK_FOUND AND MSVC)
		target_sources(${executablename} PRIVATE ${OSIMSK_DEFINITION})
	endif(MOSEK_FOUND AND MSVC)
endfunction(createexecutable executablecpplocation)

FILE(GLOB FMTexecutables ${CMAKE_CURRENT_SOURCE_DIR}/Examples/C++/*.cpp)
foreach(pathtosource ${FMTexecutables})
	createexecutable("${pathtosource}")
endforeach()



add_library(FMTlib SHARED ${FMTSOURCEFILES})
if (NOT MSVC)
		set_target_properties( FMTlib
			PROPERTIES
			ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}"
			LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}"
			RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}"
		)
else()
set_target_properties( FMTlib
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
endif(NOT MSVC)
set_property(TARGET FMTlib PROPERTY CXX_STANDARD 11)
set_property(TARGET FMTlib PROPERTY CXX_STANDARD_REQUIRED ON)
target_compile_definitions(FMTlib PRIVATE "-DFMTSHAREDLIB")
if (MOSEK_FOUND)
	target_sources(FMTlib PRIVATE ${OSIMSK_DEFINITION})
endif(MOSEK_FOUND)


#Get the tests 
#Grab everything in the test folder
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
					set(fullname "${testname}${primarylocation}${scenario}")
					string(REPLACE " " "_" fullname "${fullname}")
					add_test(
							NAME "${fullname}"
							COMMAND ${testname} ${primarylocation} ${scenario} ${dblvalue}
							WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}"
							)
			endif(TARGET "${testname}")
		endif(line)
	endforeach()
endforeach()
