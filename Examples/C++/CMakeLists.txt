#[[
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
]]


cmake_minimum_required(VERSION 3.6)
set(FMTSimpleplanningSOURCEFILES "${CMAKE_CURRENT_SOURCE_DIR}/Examples/C++/Simpleplanning.cpp")
set(FMTSpatialyexplicitsimulationSOURCEFILES "${CMAKE_CURRENT_SOURCE_DIR}/Examples/C++/Spatialyexplicitsimulation.cpp")
set(maptoFMTforestSOURCEFILES "${CMAKE_CURRENT_SOURCE_DIR}/Examples/C++/maptoFMTforest.cpp")
set(GetCarbonpredictorsSOURCEFILES "${CMAKE_CURRENT_SOURCE_DIR}/Examples/C++/GetCarbonpredictors.cpp")
set(testyieldmodelSOURCEFILES "${CMAKE_CURRENT_SOURCE_DIR}/Examples/C++/testyieldmodel.cpp")
list(APPEND FMTSimpleplanningSOURCEFILES "${FMTHEADERFILES}")
list(APPEND FMTSpatialyexplicitsimulationSOURCEFILES "${FMTHEADERFILES}")
list(APPEND maptoFMTforestSOURCEFILES "${FMTHEADERFILES}")
list(APPEND GetCarbonpredictorsSOURCEFILES "${FMTHEADERFILES}")
list(APPEND testyieldmodelSOURCEFILES "${FMTHEADERFILES}")
foreach(sourcefile ${FMTSOURCEFILES})
		list(APPEND FMTSimpleplanningSOURCEFILES "${sourcefile}")
		list(APPEND FMTSpatialyexplicitsimulationSOURCEFILES "${sourcefile}")
		list(APPEND maptoFMTforestSOURCEFILES "${sourcefile}")
		list(APPEND GetCarbonpredictorsSOURCEFILES "${sourcefile}")
		list(APPEND testyieldmodelSOURCEFILES "${sourcefile}")
endforeach()

if (OSI_FOUND)
	#project(FMTSimpleplanning VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}" LANGUAGES CXX)
	add_executable(FMTSimpleplanning ${FMTSimpleplanningSOURCEFILES})
	if (NOT MSVC)
		set_target_properties( FMTSimpleplanning
			PROPERTIES
			ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}"
			LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}"
			RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}"
		)
	endif(NOT MSVC)
	set_property(TARGET FMTSimpleplanning PROPERTY CXX_STANDARD 11)
	set_property(TARGET FMTSimpleplanning PROPERTY CXX_STANDARD_REQUIRED ON)
	if (MOSEK_FOUND AND MSVC)
		target_sources(FMTSimpleplanning PRIVATE ${OSIMSK_DEFINITION})
	endif(MOSEK_FOUND AND MSVC)
endif(OSI_FOUND)

if (GDAL_FOUND)
	#project(FMTSpatialyexplicitsimulation VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}" LANGUAGES CXX)
	#project(GetCarbonpredictors VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}" LANGUAGES CXX)
	add_executable(FMTSpatialyexplicitsimulation ${FMTSpatialyexplicitsimulationSOURCEFILES})
	add_executable(GetCarbonpredictors ${GetCarbonpredictorsSOURCEFILES})
	add_executable(testyieldmodel ${testyieldmodelSOURCEFILES})
	if (NOT MSVC)
		set_target_properties( FMTSpatialyexplicitsimulation
			PROPERTIES
			ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}"
			LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}"
			RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}"
		)
		set_target_properties( GetCarbonpredictors
			PROPERTIES
			ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}"
			LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}"
			RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}"
		)
		set_target_properties( testyieldmodel
			PROPERTIES
			ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}"
			LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}"
			RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}"
		)
	endif(NOT MSVC)
	set_property(TARGET FMTSpatialyexplicitsimulation PROPERTY CXX_STANDARD 11)
	set_property(TARGET FMTSpatialyexplicitsimulation PROPERTY CXX_STANDARD_REQUIRED ON)
	set_property(TARGET GetCarbonpredictors PROPERTY CXX_STANDARD 11)
	set_property(TARGET GetCarbonpredictors PROPERTY CXX_STANDARD_REQUIRED ON)
	set_property(TARGET testyieldmodel PROPERTY CXX_STANDARD 11)
	set_property(TARGET testyieldmodel PROPERTY CXX_STANDARD_REQUIRED ON)
	if (MOSEK_FOUND AND MSVC)
		target_sources(FMTSpatialyexplicitsimulation PRIVATE ${OSIMSK_DEFINITION})
		target_sources(GetCarbonpredictors PRIVATE ${OSIMSK_DEFINITION})
		target_sources(testyieldmodel PRIVATE ${OSIMSK_DEFINITION})
	endif(MOSEK_FOUND AND MSVC)
endif(GDAL_FOUND)

#project(FMTlib VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}" LANGUAGES CXX)
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

if (GDAL_FOUND)
	#project(maptoFMTforest VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}" LANGUAGES CXX)
	add_executable(maptoFMTforest ${maptoFMTforestSOURCEFILES})
	if (NOT MSVC)
		set_target_properties( maptoFMTforest
			PROPERTIES
			ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}"
			LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}"
			RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}"
		)
	endif(NOT MSVC)
	set_property(TARGET maptoFMTforest PROPERTY CXX_STANDARD 11)
	set_property(TARGET maptoFMTforest PROPERTY CXX_STANDARD_REQUIRED ON)
	if (MOSEK_FOUND AND MSVC)
		target_sources(maptoFMTforest PRIVATE ${OSIMSK_DEFINITION})
	endif(MOSEK_FOUND AND MSVC)
endif(GDAL_FOUND)