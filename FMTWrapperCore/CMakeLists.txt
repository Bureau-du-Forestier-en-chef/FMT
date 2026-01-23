#[[
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
]]

cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(WrapperCoreINCLUDEDIR "${CMAKE_CURRENT_SOURCE_DIR}/FMTWrapperCore/Include/")
set(FMTlib_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/Include/")
set(WrapperCoreSOURCEDIR "${CMAKE_CURRENT_SOURCE_DIR}/FMTWrapperCore/Source/")
# Définir les sources du projet FMTWrapperCore
file(GLOB FMTWRAPPERCORE_SOURCES ${WrapperCoreSOURCEDIR}*.cpp)
file(GLOB FMTWRAPPERCORE_HEADERS ${WrapperCoreINCLUDEDIR}*.h*)
# Inclure les dossiers d'en-têtes
include_directories(
    ${WrapperCoreINCLUDEDIR}
)

# Créer la bibliothèque FMTWrapperCore
if (FMTWRAPPERCORE_SOURCES)
	add_library(FMTWrapperCore SHARED ${FMTWRAPPERCORE_SOURCES} ${FMTWRAPPERCORE_HEADERS})
    target_include_directories(FMTWrapperCore PRIVATE "${FMTINCLUDEDIR}")
	get_target_property(FMTliblocation FMTlib LIBRARY_OUTPUT_DIRECTORY)

    target_include_directories(FMTWrapperCore PRIVATE ${Boost_INCLUDE_DIRS})
    target_link_libraries(FMTWrapperCore PRIVATE ${Boost_LIBRARIES})
    #target_compile_definitions(FMTWrapper PRIVATE "-DFMTLIBIMPORT")
    target_include_directories(FMTWrapperCore PRIVATE ${WrapperCoreINCLUDEDIR})
    #target_link_libraries(FMTWrapperCore PRIVATE "${FMTliblocation}/${CMAKE_SHARED_LIBRARY_PREFIX}${BUILD_TYPE}/FMTlib${CMAKE_STATIC_LIBRARY_SUFFIX}")
    #target_include_directories(FMTWrapper PRIVATE "${FMTINCLUDEDIR}")
	# Lier FMTWrapperCore à FMTlib
	target_link_libraries(FMTWrapperCore PRIVATE FMTlib)

	if (NOT WITHOUT_TESTS)
    		FILE(GLOB FMTWrapperCore_tests ${CMAKE_CURRENT_SOURCE_DIR}/FMTWrapperCore/tests/*.cpp)
    		foreach(pathtosource ${FMTWrapperCore_tests})
        		createexecutable("${pathtosource}" executablename)
        		target_link_libraries(${executablename} PRIVATE FMTWrapperCore)
        		target_include_directories(${executablename} PRIVATE ${WrapperCoreINCLUDEDIR})
    		endforeach()
endif()

else()
    message("No source files found for FMTWrapperCore in ${CMAKE_CURRENT_SOURCE_DIR}/FMTWrapperCore/Source")
endif()

