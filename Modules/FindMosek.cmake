
#[[
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
]]

include(FetchContent)

# Télécharge le dépôt OSI via Git (utilise le git système, pas libcurl de CMake)
# CONFIGURE_DEPENDS assure que le contenu est disponible dès la configuration
if(NOT DEFINED PORT)
    include(FetchContent)

    FetchContent_Declare(
        OsiMskSources
        GIT_REPOSITORY https://github.com/coin-or/Osi.git
        GIT_TAG        master
        GIT_SHALLOW    TRUE  
        GIT_SUBMODULES ""        
    )

    FetchContent_MakeAvailable(OsiMskSources)
    FetchContent_GetProperties(OsiMskSources SOURCE_DIR OSI_MSK_SOURCE_DIR)
endif()


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
		if (NOT "${OSI_POTENTIAL_INCLUDE}" STREQUAL "" AND NOT DEFINED PORT)
			set(OSI_MSK_DIR "${OSI_MSK_SOURCE_DIR}/src/OsiMsk")

			foreach(header_file OsiMskSolverInterface.hpp OsiMskConfig.h)
				if(EXISTS "${OSI_MSK_DIR}/${header_file}")
					file(COPY "${OSI_MSK_DIR}/${header_file}"
						DESTINATION "${CMAKE_SOURCE_DIR}/external/include/coin/")
					message(STATUS "[FindMosek] Copié: ${header_file}")
				else()
					message(WARNING "[FindMosek] Fichier introuvable après fetch: ${header_file}")
				endif()
			endforeach()

			if(EXISTS "${OSI_MSK_DIR}/OsiMskSolverInterface.cpp")
				file(COPY "${OSI_MSK_DIR}/OsiMskSolverInterface.cpp"
					DESTINATION "${CMAKE_SOURCE_DIR}/external/source/coin/")
				message(STATUS "[FindMosek] Copié: OsiMskSolverInterface.cpp")
			else()
				message(WARNING "[FindMosek] Fichier introuvable après fetch: OsiMskSolverInterface.cpp")
			endif()

			set(OSIMSK_INCLUDE "${CMAKE_SOURCE_DIR}/external/include/coin/")
			set(OSIMSK_DEFINITION "${CMAKE_SOURCE_DIR}/external/source/coin/OsiMskSolverInterface.cpp")
		else()
			message("Cannot find Osisolverinterface header...cannot compile with Mosek")
		endif(NOT "${OSI_POTENTIAL_INCLUDE}" STREQUAL "" AND NOT DEFINED PORT)
	endif(NOT "${OSI_POTENTIAL_INCLUDE}" STREQUAL "")
endif(NOT (VCPKG_PLATFORM_TOOLSET OR CMAKE_GENERATOR MATCHES "Visual Studio"))

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
	mark_as_advanced(Mosek_INCLUDE_DIR Mosek_LIB Mosek_FOUND Mosek_DLL Mosek_MAJOR_VERSION Mosek_MINOR_VERSION Mosek_LIB_LOCATION Mosek_WIN_LIBS) #OSIMSK_INCLUDE OSIMSK_DEFINITION
else()
	find_package_handle_standard_args(Mosek  Mosek_INCLUDE_DIR Mosek_LIB Mosek_DLL Mosek_MAJOR_VERSION Mosek_MINOR_VERSION OSIMSK_INCLUDE OSIMSK_DEFINITION) #
	mark_as_advanced(Mosek_INCLUDE_DIR Mosek_LIB Mosek_FOUND Mosek_DLL Mosek_MAJOR_VERSION Mosek_MINOR_VERSION Mosek_LINKER_FLAGS OSIMSK_INCLUDE OSIMSK_DEFINITION) #
endif(VCPKG_PLATFORM_TOOLSET OR CMAKE_GENERATOR MATCHES "Visual Studio")

