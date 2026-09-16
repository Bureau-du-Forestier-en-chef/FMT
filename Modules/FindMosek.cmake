
#[[
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
]]

FILE(GLOB_RECURSE Mosek_POTENTIAL_INCLUDE $ENV{Mosek_DIR}mosek.h)
list(GET Mosek_POTENTIAL_INCLUDE 0 FIRSTINCLUDE)
get_filename_component(POTMosek_INCLUDE_DIR ${FIRSTINCLUDE} DIRECTORY)
find_path(Mosek_INCLUDE_DIR
          NAMES mosek.h
          PATHS ${POTMosek_INCLUDE_DIR})
# MOSEK library detection
if (VCPKG_PLATFORM_TOOLSET OR CMAKE_GENERATOR MATCHES "Visual Studio")
	FILE(GLOB_RECURSE Mosek_POTENTIAL_LIB $ENV{Mosek_DIR}mosek64_*_*.lib)
else()
	FILE(GLOB_RECURSE Mosek_POTENTIAL_LIB $ENV{Mosek_DIR}libmosek64_*_*${CMAKE_STATIC_LIBRARY_SUFFIX})
ENDIF(VCPKG_PLATFORM_TOOLSET OR CMAKE_GENERATOR MATCHES "Visual Studio")

get_filename_component(Mosek_LIB_NAME ${Mosek_POTENTIAL_LIB} NAME)
list(GET Mosek_POTENTIAL_LIB 0 FIRSTLIB)
get_filename_component(POTMosek_LIB_DIR ${FIRSTLIB} DIRECTORY)


FIND_LIBRARY(Mosek_LIB NAMES "${Mosek_LIB_NAME}" mosek64 libmosek libmosek.so libmosek64 libmosek64.so libmosek64.a  PATHS ${POTMosek_LIB_DIR})
if (VCPKG_PLATFORM_TOOLSET OR CMAKE_GENERATOR MATCHES "Visual Studio")
	get_filename_component(Mosek_LIB_LOCATION ${Mosek_LIB} DIRECTORY)
	FILE(GLOB_RECURSE Mosek_WIN_LIBS_PATHS "${Mosek_LIB_LOCATION}/*.lib")
	foreach(LIB ${Mosek_WIN_LIBS_PATHS})
			get_filename_component(THE_LIB ${LIB} NAME)
			set(Mosek_WIN_LIBS "${Mosek_WIN_LIBS} ${THE_LIB}")
	endforeach()
	string(REPLACE "Program Files" "PROGRA~1" Mosek_LIB_LOCATION ${Mosek_LIB_LOCATION})
	string(REPLACE "Program Files" "PROGRA~1" Mosek_INCLUDE_DIR ${Mosek_INCLUDE_DIR})
	string(REPLACE "\"" "" Mosek_INCLUDE_DIR ${Mosek_INCLUDE_DIR})
ENDIF(VCPKG_PLATFORM_TOOLSET OR CMAKE_GENERATOR MATCHES "Visual Studio")


#Dependencies
FILE(GLOB_RECURSE Mosek_POTENTIAL_DLL $ENV{Mosek_DIR}mosek64_*_*.dll)
list(GET Mosek_POTENTIAL_DLL 0 Mosek_DLL)
get_filename_component(libname ${Mosek_DLL} NAME)
string(REGEX MATCH "^(.*)(_)([0-9]*)(_)([0-9]*)(.*)" dummy ${libname})
set(Mosek_MAJOR_VERSION "${CMAKE_MATCH_3}")
set(Mosek_MINOR_VERSION "${CMAKE_MATCH_5}")
message("Mosek ${Mosek_MAJOR_VERSION}.${Mosek_MINOR_VERSION}")


FILE(GLOB_RECURSE Mosek_POTENTIAL_DLL $ENV{Mosek_DIR}*.dll)
if (NOT Mosek_POTENTIAL_DLL)
	foreach(dll_file ${Mosek_POTENTIAL_DLL})
		list(APPEND Mosek_DLL ${dll_file})
	endforeach()
endif(NOT Mosek_POTENTIAL_DLL)

if (Mosek_MAJOR_VERSION LESS_EQUAL 9)
	list(APPEND Mosek_LINKER_FLAGS "-lpthread")
endif(Mosek_MAJOR_VERSION LESS_EQUAL 9)


include(FindPackageHandleStandardArgs)

if (VCPKG_PLATFORM_TOOLSET OR CMAKE_GENERATOR MATCHES "Visual Studio")
	find_package_handle_standard_args(Mosek  Mosek_INCLUDE_DIR 
									  Mosek_LIB Mosek_DLL Mosek_MAJOR_VERSION Mosek_MINOR_VERSION Mosek_LIB_LOCATION Mosek_WIN_LIBS) #OSIMSK_INCLUDE OSIMSK_DEFINITION
	mark_as_advanced(
		Mosek_INCLUDE_DIR Mosek_LIB Mosek_FOUND
		Mosek_DLL Mosek_MAJOR_VERSION Mosek_MINOR_VERSION
		Mosek_LIB_LOCATION Mosek_WIN_LIBS) #OSIMSK_INCLUDE OSIMSK_DEFINITION
else()
	find_package_handle_standard_args(
		Mosek
		Mosek_INCLUDE_DIR
		Mosek_LIB
		Mosek_DLL
		Mosek_MAJOR_VERSION
		Mosek_MINOR_VERSION)
	mark_as_advanced(
		Mosek_INCLUDE_DIR Mosek_LIB Mosek_FOUND 
		Mosek_DLL Mosek_MAJOR_VERSION Mosek_MINOR_VERSION
		Mosek_LINKER_FLAGS) #
endif(VCPKG_PLATFORM_TOOLSET OR CMAKE_GENERATOR MATCHES "Visual Studio")

