
#[[
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
]]

FILE(GLOB_RECURSE MOSEK_POTENTIAL_INCLUDE $ENV{MOSEK_DIR}mosek.h)
list(GET MOSEK_POTENTIAL_INCLUDE 0 FIRSTINCLUDE)
get_filename_component(POTMOSEK_INCLUDE_DIR ${FIRSTINCLUDE} DIRECTORY)
find_path(MOSEK_INCLUDE_DIR
          NAMES mosek.h
          PATHS ${POTMOSEK_INCLUDE_DIR})
# MOSEK library detection
if (VCPKG_PLATFORM_TOOLSET OR CMAKE_GENERATOR MATCHES "Visual Studio")
	FILE(GLOB_RECURSE MOSEK_POTENTIAL_LIB $ENV{MOSEK_DIR}mosek64_*_*.lib)
else()
	FILE(GLOB_RECURSE MOSEK_POTENTIAL_LIB $ENV{MOSEK_DIR}libmosek64_*_*${CMAKE_STATIC_LIBRARY_SUFFIX})
ENDIF(VCPKG_PLATFORM_TOOLSET OR CMAKE_GENERATOR MATCHES "Visual Studio")

get_filename_component(MOSEK_LIB_NAME ${MOSEK_POTENTIAL_LIB} NAME)
list(GET MOSEK_POTENTIAL_LIB 0 FIRSTLIB)
get_filename_component(POTMOSEK_LIB_DIR ${FIRSTLIB} DIRECTORY)


FIND_LIBRARY(MOSEK_LIB NAMES "${MOSEK_LIB_NAME}" mosek64 libmosek libmosek.so libmosek64 libmosek64.so libmosek64.a  PATHS ${POTMOSEK_LIB_DIR})
if (VCPKG_PLATFORM_TOOLSET OR CMAKE_GENERATOR MATCHES "Visual Studio")
	get_filename_component(MOSEK_LIB_LOCATION ${MOSEK_LIB} DIRECTORY)
	FILE(GLOB_RECURSE MOSEK_WIN_LIBS_PATHS "${MOSEK_LIB_LOCATION}/*.lib")
	foreach(LIB ${MOSEK_WIN_LIBS_PATHS})
			get_filename_component(THE_LIB ${LIB} NAME)
			set(MOSEK_WIN_LIBS "${MOSEK_WIN_LIBS} ${THE_LIB}")
	endforeach()
	string(REPLACE "Program Files" "PROGRA~1" MOSEK_LIB_LOCATION ${MOSEK_LIB_LOCATION})
	string(REPLACE "Program Files" "PROGRA~1" MOSEK_INCLUDE_DIR ${MOSEK_INCLUDE_DIR})
	string(REPLACE "\"" "" MOSEK_INCLUDE_DIR ${MOSEK_INCLUDE_DIR})
ENDIF(VCPKG_PLATFORM_TOOLSET OR CMAKE_GENERATOR MATCHES "Visual Studio")


#Go find the Osiabstractclass if you dont have the osimoseklib but have osi and mosek dir
if (NOT (VCPKG_PLATFORM_TOOLSET OR CMAKE_GENERATOR MATCHES "Visual Studio"))
	FILE(GLOB_RECURSE OSI_POTENTIAL_INCLUDE $ENV{OSI_DIR}OsiMskSolverInterface.hpp)
	if (NOT "${OSI_POTENTIAL_INCLUDE}" STREQUAL "")
		list(GET OSI_POTENTIAL_INCLUDE 0 FIRST_HEADER)
		get_filename_component(OSIMSK_INCLUDE ${FIRST_HEADER} DIRECTORY)
		if (MSVC)
			FILE(GLOB_RECURSE OSI_POTENTIAL_INCLUDE $ENV{OSI_DIR}OsiMskSolverInterface.cpp)
			list(GET OSI_POTENTIAL_INCLUDE 0 OSIMSK_DEFINITION)
		endif(MSVC)
	else()
		FILE(GLOB_RECURSE OSI_POTENTIAL_INCLUDE $ENV{OSI_DIR}OsiSolverInterface.hpp)
		if (NOT "${OSI_POTENTIAL_INCLUDE}" STREQUAL "")
			file(DOWNLOAD
   			 https://raw.githubusercontent.com/coin-or/Osi/master/src/OsiMsk/OsiMskSolverInterface.hpp
    			${CMAKE_SOURCE_DIR}/external/include/coin/OsiMskSolverInterface.hpp)
			file(DOWNLOAD
   			 https://raw.githubusercontent.com/coin-or/Osi/master/src/OsiMsk/OsiMskConfig.h
    			${CMAKE_SOURCE_DIR}/external/include/coin/OsiMskConfig.h)
			file(DOWNLOAD
   			 https://raw.githubusercontent.com/coin-or/Osi/master/src/OsiMsk/OsiMskSolverInterface.cpp
    			${CMAKE_SOURCE_DIR}/external/source/coin/OsiMskSolverInterface.cpp)
			set(OSIMSK_INCLUDE "${CMAKE_SOURCE_DIR}/external/include/coin/")
			set(OSIMSK_DEFINITION "${CMAKE_SOURCE_DIR}/external/source/coin/OsiMskSolverInterface.cpp")
		else()
			message("Cannot find Osisolverinterface header...cannot compile with Mosek")
		endif(NOT "${OSI_POTENTIAL_INCLUDE}" STREQUAL "")
	endif(NOT "${OSI_POTENTIAL_INCLUDE}" STREQUAL "")
endif(NOT (VCPKG_PLATFORM_TOOLSET OR CMAKE_GENERATOR MATCHES "Visual Studio"))

#Dependencies
FILE(GLOB_RECURSE MOSEK_POTENTIAL_DLL $ENV{MOSEK_DIR}mosek64_*_*.dll)
list(GET MOSEK_POTENTIAL_DLL 0 MOSEK_DLL)
get_filename_component(libname ${MOSEK_DLL} NAME)
string(REGEX MATCH "^(.*)(_)([0-9]*)(_)([0-9]*)(.*)" dummy ${libname})
set(MOSEK_MAJOR_VERSION "${CMAKE_MATCH_3}")
set(MOSEK_MINOR_VERSION "${CMAKE_MATCH_5}")
message("Mosek ${MOSEK_MAJOR_VERSION}.${MOSEK_MINOR_VERSION}")


FILE(GLOB_RECURSE MOSEK_POTENTIAL_DLL $ENV{MOSEK_DIR}*.dll)
if (NOT MOSEK_POTENTIAL_DLL)
	foreach(dll_file ${MOSEK_POTENTIAL_DLL})
		list(APPEND MOSEK_DLL ${dll_file})
	endforeach()
endif(NOT MOSEK_POTENTIAL_DLL)

if (MOSEK_MAJOR_VERSION LESS_EQUAL 9)
	list(APPEND MOSEK_LINKER_FLAGS "-lpthread")
endif(MOSEK_MAJOR_VERSION LESS_EQUAL 9)


include(FindPackageHandleStandardArgs)

if (VCPKG_PLATFORM_TOOLSET OR CMAKE_GENERATOR MATCHES "Visual Studio")
	find_package_handle_standard_args(MOSEK  MOSEK_INCLUDE_DIR 
									  MOSEK_LIB MOSEK_DLL MOSEK_MAJOR_VERSION MOSEK_MINOR_VERSION MOSEK_LIB_LOCATION MOSEK_WIN_LIBS) #OSIMSK_INCLUDE OSIMSK_DEFINITION
	mark_as_advanced(MOSEK_INCLUDE_DIR MOSEK_LIB MOSEK_FOUND MOSEK_DLL MOSEK_MAJOR_VERSION MOSEK_MINOR_VERSION MOSEK_LIB_LOCATION MOSEK_WIN_LIBS) #OSIMSK_INCLUDE OSIMSK_DEFINITION
else()
	find_package_handle_standard_args(MOSEK  MOSEK_INCLUDE_DIR MOSEK_LIB MOSEK_DLL MOSEK_MAJOR_VERSION MOSEK_MINOR_VERSION) #OSIMSK_INCLUDE OSIMSK_DEFINITION
	mark_as_advanced(MOSEK_INCLUDE_DIR MOSEK_LIB MOSEK_FOUND MOSEK_DLL MOSEK_MAJOR_VERSION MOSEK_MINOR_VERSION MOSEK_LINKER_FLAGS) #OSIMSK_INCLUDE OSIMSK_DEFINITION
endif(VCPKG_PLATFORM_TOOLSET OR CMAKE_GENERATOR MATCHES "Visual Studio")

