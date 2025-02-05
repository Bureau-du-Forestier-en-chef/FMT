#[[
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
]]

cmake_minimum_required(VERSION 3.6)
set(ExcelWrapperINCLUDEDIR "${CMAKE_CURRENT_SOURCE_DIR}/Excel/Include/")
set(ExcelWrapperSOURCEDIR "${CMAKE_CURRENT_SOURCE_DIR}/Excel/Source/")
FILE(GLOB ExcelWrapperSOURCES ${ExcelWrapperSOURCEDIR}*.cpp)
if (MSVC AND ExcelWrapperSOURCES)
	add_library(FMTExcelWrapper SHARED ${ExcelWrapperSOURCES})
	target_include_directories(FMTExcelWrapper PRIVATE "${FMTINCLUDEDIR}")
	get_target_property(FMTliblocation FMTlib LIBRARY_OUTPUT_DIRECTORY)
	target_link_libraries(FMTExcelWrapper PRIVATE "${FMTliblocation}/${CMAKE_SHARED_LIBRARY_PREFIX}${BUILD_TYPE}/FMTlib${CMAKE_STATIC_LIBRARY_SUFFIX}")
	target_include_directories(FMTExcelWrapper PRIVATE ${ExcelWrapperINCLUDEDIR})
	target_include_directories(FMTExcelWrapper PRIVATE ${Boost_INCLUDE_DIRS})
	#target_include_directories(FMTExcelWrapper PRIVATE ${Boost_INCLUDE_DIRS})
	target_precompile_headers(FMTExcelWrapper PRIVATE "${ExcelWrapperINCLUDEDIR}/stdafx.h")
	target_link_libraries(FMTExcelWrapper PRIVATE ${Boost_LIBRARIES})
	target_compile_definitions(FMTExcelWrapper PRIVATE "-DFMTLIBIMPORT")
	set_property(TARGET FMTExcelWrapper PROPERTY VS_DOTNET_REFERENCES
		"System"
    		"System.Data"
    		"System.Xml"
		)
	set_target_properties(FMTExcelWrapper PROPERTIES COMMON_LANGUAGE_RUNTIME "")
	set_target_properties( FMTExcelWrapper
		PROPERTIES
		ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
		LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
		RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
	)
	message("Generating build for FMTExcelWrapper")
	if (NOT WITHOUT_TESTS)
		FILE(GLOB FMTexecutables ${CMAKE_CURRENT_SOURCE_DIR}/Excel/tests/*.cpp)
		foreach(pathtosource ${FMTexecutables})
			createexecutable("${pathtosource}" executablename)
			if ("${executablename}" STREQUAL "UnitTestFMTexcelcache")
				set_property(TARGET ${executablename} PROPERTY VS_DOTNET_REFERENCES
					"System"
						"System.Data"
						"System.Xml"
					)
				set_target_properties(${executablename} PROPERTIES COMMON_LANGUAGE_RUNTIME "")
			endif("${executablename}" STREQUAL "UnitTestFMTexcelcache")
			target_link_libraries(${executablename} PRIVATE FMTExcelWrapper)
			target_include_directories(${executablename} PRIVATE ${ExcelWrapperINCLUDEDIR})
			add_test(
								NAME "${executablename}"
								COMMAND ${executablename}
								WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}"
								)
		endforeach()
	endif(NOT WITHOUT_TESTS)
else()
	message("No files found in ${CMAKE_CURRENT_SOURCE_DIR}/Excel/Source/ the wrapper wont be compiled headers need to be in UI/Include and sources in UI/Source")
endif(MSVC AND ExcelWrapperSOURCES)









