#[[
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
]]

cmake_minimum_required(VERSION 3.6)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Inclure les dossiers d'en-têtes
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${FMTlib_INCLUDE_DIRS}
)

# Définir les sources du projet FMTWrapperCore
file(GLOB FMTWRAPPERCORE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/FMTWrapperCore/Source/*.cpp)
file(GLOB FMTWRAPPERCORE_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/FMTWrapperCore/Include/*.h*)

# Créer la bibliothèque FMTWrapperCore
if (FMTWRAPPERCORE_SOURCES)
	add_library(FMTWrapperCore SHARED ${FMTWRAPPERCORE_SOURCES} ${FMTWRAPPERCORE_HEADERS})
	target_precompile_headers(FMTWrapperCore PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/FMTWrapperCore/Include/stdafx.h")
	get_target_property(FMTliblocation FMTlib LIBRARY_OUTPUT_DIRECTORY)

	if (NOT WITHOUT_TESTS)
    		FILE(GLOB FMTWrapperCore_tests ${CMAKE_CURRENT_SOURCE_DIR}/FMTWrapperCore/tests/*.cpp)
    		foreach(pathtosource ${FMTWrapperCore_tests})
        		createexecutable("${pathtosource}" executablename)
        		target_link_libraries(${executablename} PRIVATE FMTWrapperCore)
        		target_include_directories(${executablename} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/FMTWrapperCore/Include)
    		endforeach()
endif()

else()
    message("No source files found for FMTWrapperCore in ${CMAKE_CURRENT_SOURCE_DIR}/FMTWrapperCore/Source")
endif()
# Lier FMTWrapperCore à FMTlib
target_link_libraries(FMTWrapperCore PUBLIC FMTlib)

# Définir les propriétés de la cible
set_target_properties(FMTWrapperCore PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)