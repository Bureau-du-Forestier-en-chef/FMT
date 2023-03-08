#[[
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
]]

cmake_minimum_required(VERSION 3.6)
set(WrapperINCLUDEDIR "${CMAKE_CURRENT_SOURCE_DIR}/UI/Include/")
set(WrapperSOURCEDIR "${CMAKE_CURRENT_SOURCE_DIR}/UI/Source/")
FILE(GLOB WrapperSOURCES ${WrapperSOURCEDIR}*.cpp)
if (MSVC AND WrapperSOURCES)
	add_library(FMTWrapper SHARED ${WrapperSOURCES})
	#get_target_property(FMTliblocation FMTlib LIBRARY_OUTPUT_DIRECTORY)
	#target_link_libraries(FMTWrapper PRIVATE "${FMTliblocation}/${CMAKE_SHARED_LIBRARY_PREFIX}${BUILD_TYPE}/FMTlib${CMAKE_STATIC_LIBRARY_SUFFIX}")
	target_link_libraries(FMTWrapper PRIVATE FMTlib)
	target_include_directories(FMTWrapper PRIVATE ${WrapperINCLUDEDIR})
	target_include_directories(FMTWrapper PRIVATE ${Boost_INCLUDE_DIRS})
	#target_include_directories(FMTWrapper PRIVATE ${Boost_INCLUDE_DIRS})
	target_precompile_headers(FMTWrapper PRIVATE "${WrapperINCLUDEDIR}/stdafx.h")
	target_link_libraries(FMTWrapper PRIVATE ${Boost_LIBRARIES})
	target_compile_definitions(FMTWrapper PRIVATE "-DFMTLIBIMPORT")
	set_property(TARGET FMTWrapper PROPERTY VS_DOTNET_REFERENCES
		"System"
    		"System.Data"
    		"System.Xml"
		)
	set_target_properties(FMTWrapper PROPERTIES COMMON_LANGUAGE_RUNTIME "")
	message("Generating build for FMTWrapper")
else()
	message("No files found in ${CMAKE_CURRENT_SOURCE_DIR}/UI/Source/ the wrapper wont be compiled headers need to be in UI/Include and sources in UI/Source")
endif(MSVC AND WrapperSOURCES)









