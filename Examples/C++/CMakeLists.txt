#[[
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
]]


cmake_minimum_required(VERSION 3.6)
set(FMTSimpleplanningSOURCEFILES "${CMAKE_CURRENT_SOURCE_DIR}/Examples/C++/Simpleplanning.cpp")
set(FMTSpatialyexplicitsimulationSOURCEFILES "${CMAKE_CURRENT_SOURCE_DIR}/Examples/C++/Spatialyexplicitsimulation.cpp")
foreach(sourcefile ${FMTSOURCEFILES})
	if (NOT "${sourcefile}" MATCHES "main.cpp")
		list(APPEND FMTSimpleplanningSOURCEFILES "${sourcefile}")
		list(APPEND FMTSpatialyexplicitsimulationSOURCEFILES "${sourcefile}")
	endif(NOT "${sourcefile}" MATCHES "main.cpp")
endforeach()

if (OSI_FOUND)
	project(FMTSimpleplanning VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}" LANGUAGES CXX)
	add_executable(FMTSimpleplanning ${FMTSimpleplanningSOURCEFILES})
	if (CYGWIN)
		set_target_properties( FMTSimpleplanning
			PROPERTIES
			ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}"
			LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}"
			RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}"
		)
	endif(CYGWIN)
	set_property(TARGET FMTSimpleplanning PROPERTY CXX_STANDARD 11)
	set_property(TARGET FMTSimpleplanning PROPERTY CXX_STANDARD_REQUIRED ON)
	if (MOSEK_FOUND AND MSVC)
		target_sources(FMTSimpleplanning PRIVATE ${OSIMSK_DEFINITION})
	endif(MOSEK_FOUND AND MSVC)
endif(OSI_FOUND)

if (GDAL_FOUND)
	project(FMTSpatialyexplicitsimulation VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}" LANGUAGES CXX)
	add_executable(FMTSpatialyexplicitsimulation ${FMTSpatialyexplicitsimulationSOURCEFILES})
	if (CYGWIN)
		set_target_properties( FMTSpatialyexplicitsimulation
			PROPERTIES
			ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}"
			LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}"
			RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}"
		)
	endif(CYGWIN)
	set_property(TARGET FMTSpatialyexplicitsimulation PROPERTY CXX_STANDARD 11)
	set_property(TARGET FMTSpatialyexplicitsimulation PROPERTY CXX_STANDARD_REQUIRED ON)
	if (MOSEK_FOUND AND MSVC)
		target_sources(FMTSpatialyexplicitsimulation PRIVATE ${OSIMSK_DEFINITION})
	endif(MOSEK_FOUND AND MSVC)
endif(GDAL_FOUND)