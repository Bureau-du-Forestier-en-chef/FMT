#[[
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
]]


cmake_minimum_required(VERSION 3.20)

#[[
The C++ system tests, included by Tests/System/CMakeLists.txt. Every .cpp file of this folder becomes a
test executable, and so does every one of FMTWrapperCore/, linked to the FMTWrapperCore library, and of
Excel/, linked to the FMTExcelWrapper library. The executables of these two folders carry the label of
their component, wrappercore or excel, in their FMTTESTScomponent property, which registersystemtest adds
to their tests. The rows of the CSV files of this folder then register the tests of all these executables
(registercsvtests in Tests/Support/TestRegistration.cmake).
]]

#A function, so that its variables stay out of the variables exported to the install scripts.
function(createcppsystemtests)
	FILE(GLOB FMTexecutables ${CMAKE_CURRENT_LIST_DIR}/*.cpp)
	foreach(pathtosource ${FMTexecutables})
		createexecutable("${pathtosource}" executablename)
	endforeach()

	#Created with the FMTWrapperCore library, when FMTWrapperCore/Source has sources
	if (TARGET FMTWrapperCore)
		FILE(GLOB FMTexecutables ${CMAKE_CURRENT_LIST_DIR}/FMTWrapperCore/*.cpp)
		foreach(pathtosource ${FMTexecutables})
			createexecutable("${pathtosource}" executablename)
			target_link_libraries(${executablename} PRIVATE FMTWrapperCore)
			target_include_directories(${executablename} PRIVATE ${WrapperCoreINCLUDEDIR})
			set_property(TARGET ${executablename} PROPERTY FMTTESTScomponent wrappercore)
		endforeach()
	endif(TARGET FMTWrapperCore)

	#Created with the FMTExcelWrapper library: with MSVC, when Excel/Source has sources
	if (TARGET FMTExcelWrapper)
		FILE(GLOB FMTexecutables ${CMAKE_CURRENT_LIST_DIR}/Excel/*.cpp)
		foreach(pathtosource ${FMTexecutables})
			createexecutable("${pathtosource}" executablename)
			if ("${executablename}" STREQUAL "UnitTestFMTexcelcache" OR "${executablename}" STREQUAL "UnitTestFMToutputCache")
				set_property(TARGET ${executablename} PROPERTY VS_DOTNET_REFERENCES
					"System"
						"System.Data"
						"System.Xml"
					)
				set_target_properties(${executablename} PROPERTIES COMMON_LANGUAGE_RUNTIME "")
			endif("${executablename}" STREQUAL "UnitTestFMTexcelcache" OR "${executablename}" STREQUAL "UnitTestFMToutputCache")
			target_link_libraries(${executablename} PRIVATE FMTExcelWrapper)
			target_include_directories(${executablename} PRIVATE ${ExcelWrapperINCLUDEDIR})
			set_property(TARGET ${executablename} PROPERTY FMTTESTScomponent excel)
			if ("${executablename}" STREQUAL "UnitTestFMTexcelcache")
				#Registered like the rows of the CSV files
				registersystemtest("System.${executablename}" "Tests/System/Cpp/CMakeLists.txt" ${executablename})
			endif("${executablename}" STREQUAL "UnitTestFMTexcelcache")
		endforeach()
	endif(TARGET FMTExcelWrapper)

	#Executables whose rows write into the same output files: their rows never run at the same time
	set(FMTTESTSsharedoutputs planningtest testActionsAggregations testOAschedulerBFEC testreadwriteproject)
	FILE(GLOB FMTTESTSfiles ${CMAKE_CURRENT_LIST_DIR}/*.csv)
	#BFECtests.csv, the private rows, is local and ignored by git. A copy left in Examples/C++/tests, its
	#folder until phase 6 of the test reorganization (#350), is read in its place, first, as the glob lists it.
	set(FMTTESTSformerprivate "${PROJECT_SOURCE_DIR}/Examples/C++/tests/BFECtests.csv")
	if (NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/BFECtests.csv" AND EXISTS "${FMTTESTSformerprivate}")
		message(WARNING "${FMTTESTSformerprivate} is read from its former folder: move it to ${CMAKE_CURRENT_LIST_DIR}/BFECtests.csv")
		list(PREPEND FMTTESTSfiles "${FMTTESTSformerprivate}")
	endif()
	registercsvtests("${FMTTESTSfiles}" "${FMTTESTSsharedoutputs}")
endfunction(createcppsystemtests)

createcppsystemtests()
