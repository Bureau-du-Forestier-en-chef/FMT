#[[
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
]]


cmake_minimum_required(VERSION 3.6)
set(ReleaseTestingSOURCEFILES "${CMAKE_CURRENT_SOURCE_DIR}/Examples/C++/ReleaseTesting/ReleaseTesting.cpp")
foreach(sourcefile ${FMTSOURCEFILES})
	if (NOT "${sourcefile}" MATCHES "main.cpp")
		list(APPEND ReleaseTestingSOURCEFILES "${sourcefile}")
	endif(NOT "${sourcefile}" MATCHES "main.cpp")
endforeach()

if (OSI_FOUND AND GDAL_FOUND)
	project(FMTReleaseTesting VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}" LANGUAGES CXX)
	add_executable(FMTReleaseTesting ${ReleaseTestingSOURCEFILES})
	if (CYGWIN)
		set_target_properties( FMTReleaseTesting
			PROPERTIES
			ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}"
			LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}"
			RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}"
		)
	endif(CYGWIN)
	set_property(TARGET FMTReleaseTesting PROPERTY CXX_STANDARD 11)
	set_property(TARGET FMTReleaseTesting PROPERTY CXX_STANDARD_REQUIRED ON)
	if (MOSEK_FOUND AND MSVC)
		target_sources(FMTReleaseTesting PRIVATE ${OSIMSK_DEFINITION})
	endif(MOSEK_FOUND AND MSVC)
endif(OSI_FOUND AND GDAL_FOUND)