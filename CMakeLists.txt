#[[
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
]]


cmake_minimum_required(VERSION 3.6)

if (DEFINED CMAKE_BUILD_TYPE)
	set(BUILD_TYPE "${CMAKE_BUILD_TYPE}")
endif()
if (DEFINED BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "${BUILD_TYPE}")
endif()


project(FMT VERSION 0.9.1 LANGUAGES CXX)

set(FMTMAJOR "-DFMT_MAJOR=${PROJECT_VERSION_MAJOR}")
add_definitions("${FMTMAJOR}")
set(FMTMINOR "-DFMT_MINOR=${PROJECT_VERSION_MINOR}")
add_definitions("${FMTMINOR}")
set(FMTPATCH "-DFMT_PATCH=${PROJECT_VERSION_PATCH}")
add_definitions("${FMTPATCH}")
message("Generating Build file for FMT ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")


set(FMTSOURCEDIR "${CMAKE_CURRENT_SOURCE_DIR}/Source/")

if (DEFINED VCPKG_TARGET_TRIPLET)
	find_package(Clp CONFIG REQUIRED)
endif(DEFINED VCPKG_TARGET_TRIPLET)

set(DLLlookuppaths "${CMAKE_SYSTEM_LIBRARY_PATH}")
if (DEFINED ENV{PATH})
	list(APPEND DLLlookuppaths $ENV{PATH})
	string(REPLACE "\\" "/" DLLlookuppaths "${DLLlookuppaths}")
endif(DEFINED ENV{PATH})

if (DEFINED OSI_DIR)
	if (EXISTS "${OSI_DIR}")
		set(ENV{OSI_DIR} "${OSI_DIR}")
		list(APPEND DLLlookuppaths ${OSI_DIR})
	else()
		message(WARNING "${OSI_DIR} is not a valid directory")
	endif(EXISTS "${OSI_DIR}")
endif(DEFINED OSI_DIR)

if (DEFINED MOSEK_DIR)
	if (EXISTS "${MOSEK_DIR}")
		set(ENV{MOSEK_DIR} "${MOSEK_DIR}")
		list(APPEND DLLlookuppaths ${MOSEK_DIR})
	else()
		message(WARNING "${MOSEK_DIR} is not a valid directory")
	endif(EXISTS "${MOSEK_DIR}")
endif(DEFINED MOSEK_DIR)

if (DEFINED BOOST_DIR)
	if (EXISTS "${BOOST_DIR}")
		set(ENV{BOOSTROOT} "${BOOST_DIR}")
		list(APPEND DLLlookuppaths ${BOOST_DIR})
	else()
		message(WARNING "${BOOST_DIR} is not a valid directory")
	endif(EXISTS "${BOOST_DIR}")
endif(DEFINED BOOST_DIR)

if (DEFINED GDAL_DIR)
	if (EXISTS "${GDAL_DIR}")
		set(ENV{GDAL_DIR} "${GDAL_DIR}")
		list(APPEND DLLlookuppaths ${GDAL_DIR})
	else()
		message(WARNING "${GDAL_DIR} is not a valid directory")
	endif(EXISTS "${GDAL_DIR}")
endif(DEFINED GDAL_DIR)

if (DEFINED R_DIR)
	if (EXISTS "${R_DIR}")
		set(ENV{R_DIR} "${R_DIR}")
		list(APPEND DLLlookuppaths ${R_DIR})
	else()
		message(WARNING "${R_DIR} is not a valid directory")
	endif(EXISTS "${R_DIR}")
endif(DEFINED R_DIR)

if (DEFINED GEOS_DIR)
	if (EXISTS "${GEOS_DIR}")
		set(ENV{GEOS_DIR} "${GEOS_DIR}")
		list(APPEND DLLlookuppaths ${GEOS_DIR})
	else()
		message(WARNING "${GEOS_DIR} is not a valid directory")
	endif(EXISTS "${GEOS_DIR}")
endif(DEFINED GEOS_DIR)

if (DEFINED PYTHON_INCLUDE)
	set(ENV{PYTHON_DIR} "${PYTHON_INCLUDE}")
endif(DEFINED PYTHON_INCLUDE)

if (DEFINED PYTHON_DIR)
	if (EXISTS "${PYTHON_DIR}")
		set(ENV{PYTHON_DIR} "${PYTHON_DIR}")
		list(APPEND DLLlookuppaths ${PYTHON_DIR})
	else()
		message(WARNING "${PYTHON_DIR} is not a valid directory")
	endif(EXISTS "${PYTHON_DIR}")
endif(DEFINED PYTHON_DIR)


if (DEFINED ONNXR_DIR)
	if (EXISTS "${ONNXR_DIR}")
		set(ENV{ONNXR_DIR} "${ONNXR_DIR}")
		list(APPEND DLLlookuppaths ${ONNXR_DIR})
	else()
		message(WARNING "${ONNXR_DIR} is not a valid directory")
	endif(EXISTS "${ONNXR_DIR}")
endif(DEFINED ONNXR_DIR)





if (MSVC)
	add_definitions("-DFMT_EXPORTS")
	add_definitions("-D_WINDOWS")
	add_definitions("-D_USRDLL")
	add_definitions("/bigobj")
	add_definitions("-D_CRT_SECURE_NO_WARNINGS")
else()
	add_definitions("-Wa,-mbig-obj")
endif(MSVC)


add_definitions("-DBOOST_DYNAMIC_BITSET_DONT_USE_FRIENDS")
add_definitions("-DHAVE_SNPRINTF")
add_definitions("-DBOOST_FILESYSTEM_NO_DEPRECATED")

FILE(GLOB FMTSOURCEFILES ${FMTSOURCEDIR}FMT*.cpp)
FILE(GLOB FMTHEADERFILES ${FMTSOURCEDIR}FMT*.hpp)
list (APPEND FMTHEADERFILES "${FMTSOURCEDIR}/resource.hpp")
set(FMTPYNRSOURCEFILES "${FMTSOURCEFILES}")
list(APPEND FMTPYNRSOURCEFILES "${FMTSOURCEDIR}/main.cpp")
add_library(FMT SHARED ${FMTPYNRSOURCEFILES})

set_target_properties(FMT PROPERTIES PREFIX "")

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )


#Create a include directory for the lib
file(REMOVE_RECURSE "${CMAKE_BINARY_DIR}/include")
foreach(includedfile ${FMTHEADERFILES})
	file(COPY "${includedfile}" DESTINATION "${CMAKE_BINARY_DIR}/include")
endforeach()
include_directories("${CMAKE_BINARY_DIR}/include")
#Create R/PY API include need to call python to do that
if (DEFINED PYTHON_DIR OR DEFINED R_DIR)
	message("Generating comments in R and Python API")
	file(REMOVE_RECURSE "${CMAKE_BINARY_DIR}/PYNRinclude")
	if (DEFINED PYTHON_DIR)#Use python add comments to R and Python
		find_program(PYTHON python PATHS ${PYTHON_DIR})
		execute_process(
			COMMAND "${PYTHON}" commentsPythonandR.py "${CMAKE_BINARY_DIR}/PYNRinclude"
			WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Documentation"
			)
	else()
		message(WARNING "Cannot find Python R classes wont be documented ")
		FILE(GLOB FMTPYNRINCLUDE ${FMTSOURCEDIR}/PY*.hpp ${FMTSOURCEDIR}/R*.hpp)
		foreach(includedfile ${FMTPYNRINCLUDE})
			file(COPY "${includedfile}" DESTINATION "${CMAKE_BINARY_DIR}/PYNRinclude")
		endforeach()
	endif(DEFINED PYTHON_DIR)
	target_include_directories(FMT PRIVATE "${CMAKE_BINARY_DIR}/PYNRinclude")
endif(DEFINED PYTHON_DIR OR DEFINED R_DIR)

if (NOT MSVC)
set_target_properties( FMT
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}"
)
else()
set_target_properties( FMT
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
endif(NOT MSVC)


list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/modules)

if (DEFINED ONNXR_DIR)
find_package(onnxruntime)
if (onnxruntime_FOUND)
	message("Found Onnxruntime")
	target_link_libraries(FMT ${onnxruntime_LIBRARIES})
	link_libraries(${onnxruntime_LIBRARIES})
	include_directories(${onnxruntime_INCLUDE_DIRS})
	set(FMTWITHONNXR "-DFMTWITHONNXR")
	add_definitions("${FMTWITHONNXR}")
	list (APPEND FMTDEPENDENCIES ${onnxruntime_DLLs})
endif(onnxruntime_FOUND)
endif(DEFINED ONNXR_DIR)

set_property(TARGET FMT PROPERTY CXX_STANDARD 11)
set_property(TARGET FMT PROPERTY CXX_STANDARD_REQUIRED ON)


if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "4.9.0")
    message(FATAL_ERROR "Insufficient g++ version you need at least 4.9.0")
  endif()
endif()



if (NOT MSVC AND DEFINED ENV{R_DIR})
	find_package(R REQUIRED)
	if (R_FOUND)
		message("Found R include here: ${R_INCLUDE_DIR}")
		set(FMTWITHR "-DFMTWITHR")
		#add_definitions("${FMTWITHR}")
		target_compile_definitions(FMT PRIVATE "${FMTWITHR}")
		#include_directories(${R_INCLUDE_DIR})
		target_include_directories(FMT PRIVATE ${R_INCLUDE_DIR})
		set(ENV{RCPP_DIR} "$ENV{R_DIR}")
		find_package(RCPP REQUIRED)
		if (RCPP_FOUND)
			message("Found Rcpp include here: ${RCPP_INCLUDE_DIR}")
			#include_directories(${RCPP_INCLUDE_DIR})
			target_include_directories(FMT PRIVATE ${RCPP_INCLUDE_DIR})
			get_filename_component(R_DLL_DIR ${R_DLL} DIRECTORY)
			target_link_libraries(FMT "-L${R_DLL_DIR}")
			foreach(linkerflags ${R_LINKER_FLAGS})
				target_link_libraries(FMT ${linkerflags})
			endforeach()
			set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--gc-sections")
			set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--export-all-symbols")
			set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--version-script=${CMAKE_CURRENT_SOURCE_DIR}/Templates/Rpackage/FMTRcpplinkerscript.txt")
			set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--allow-multiple-definition")
			set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -Wl,--allow-multiple-definition")
			file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/Templates/Rpackage/FMT" DESTINATION "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}")
			if (DEFINED PYTHON_DIR)
			find_program(PYTHON python PATHS ${PYTHON_DIR})
				execute_process(
					COMMAND "${PYTHON}" ExposeEnum.py "${FMTSOURCEDIR}" "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}/FMT/R"
					WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Templates/Rpackage"
					)
				else()
					message(WARNING "Cannot find Python, enum and enum class exists as list.")
			endif(DEFINED PYTHON_DIR)
		endif()
	endif()
endif()


if ((NOT DEFINED ENV{R_DIR}) AND (DEFINED ENV{PYTHON_DIR}))
	#if(NOT DEFINED PYTHON_INCLUDE)
	#	FILE(GLOB_RECURSE PYTHON_INCLUDE $ENV{PYTHON_DIR}include/Python.h)
	#	if(PYTHON_INCLUDE)
    #        list(GET PYTHON_INCLUDE 0 FIRSTINCLUDE)
    #        get_filename_component(PYTHON_INCLUDE_DIR ${FIRSTINCLUDE} DIRECTORY)
    #    endif(PYTHON_INCLUDE)
	#endif(NOT DEFINED PYTHON_INCLUDE)
	find_package(PythonLibs)
	if (PythonLibs_FOUND)
		string(REGEX MATCH "^([0-9]*)(.)([0-9]*)(.)([0-9]*)" dummy ${PYTHONLIBS_VERSION_STRING})
		set(PYTHONNAMING "python${CMAKE_MATCH_1}${CMAKE_MATCH_3}")
		set(PYTHONMAJOR "${CMAKE_MATCH_1}")
		set(PYTHONMINOR "${CMAKE_MATCH_3}")
		set(FMTWITHPYTHON "-DFMTWITHPYTHON")
		#add_definitions("${FMTWITHPYTHON}")
		target_compile_definitions(FMT PRIVATE "${FMTWITHPYTHON}")
		message("Found Python include here: ${PYTHON_INCLUDE_DIRS}")
		set(PYTHON_LIBRARIES "optimized;${PYTHON_LIBRARIES}")
		if(PYTHON_DEBUG_LIBRARIES)
			LIST(APPEND PYTHON_LIBRARIES "debug;${PYTHON_DEBUG_LIBRARIES}")
		endif(PYTHON_DEBUG_LIBRARIES)
		message("Found Python library: ${PYTHON_LIBRARIES}")
		#include_directories(${PYTHON_INCLUDE_DIRS})
		target_include_directories(FMT PRIVATE ${PYTHON_INCLUDE_DIRS})
		if(WIN32 AND MSVC)
			target_link_libraries(FMT ${PYTHON_LIBRARY_RELEASE})
		else()
			target_link_libraries(FMT ${PYTHON_LIBRARIES})
		endif()
		if (NOT CYGWIN)
			set_target_properties(FMT PROPERTIES SUFFIX ".pyd")
		endif(NOT CYGWIN)
		configure_file("${CMAKE_CURRENT_SOURCE_DIR}/Templates/__init__.py.in"
                    "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}/__init__.py" @ONLY
                )
        configure_file("${CMAKE_CURRENT_SOURCE_DIR}/Templates/setup.py.in"
                    "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}/setup.py" @ONLY
                )
	else()
		message("Python not found")
	endif(PythonLibs_FOUND)
endif()





#Try to find a dynamic boost lib
if (DEFINED PYTHONNAMING)
        if("${PYTHONMAJOR}" STREQUAL "2")
            set(BPY "python")
       	else()
            set(BPY ${PYTHONNAMING})
        endif("${PYTHONMAJOR}" STREQUAL "2")
		if(MSVC)
        	find_package(Boost COMPONENTS system filesystem serialization iostreams regex thread "${BPY}" OPTIONAL_COMPONENTS zlib)
		else()
			find_package(Boost COMPONENTS system filesystem serialization iostreams regex thread "${BPY}" OPTIONAL_COMPONENTS zlib) 
		endif(MSVC)
    else()
		if(MSVC)
			find_package(Boost COMPONENTS system filesystem serialization iostreams regex thread "${BPY}" OPTIONAL_COMPONENTS zlib)
		else()
			find_package(Boost COMPONENTS system filesystem serialization iostreams regex thread "${BPY}" OPTIONAL_COMPONENTS zlib) 
		endif(MSVC)
endif(DEFINED PYTHONNAMING)

#If you have a dynamic boost lib then go take the .dll
if (Boost_FOUND)
	if (MSVC)
        #Go find the DLL
		if(BUILD_TYPE MATCHES Release)
			if (DEFINED CMAKE_TOOLCHAIN_FILE)#if vcpkg
				get_filename_component(boost_bin ${Boost_LIBRARY_DIR_RELEASE} DIRECTORY)
				set(boost_bin "${boost_bin}/bin")
			else()
				set(boost_bin "${Boost_LIBRARY_DIR_RELEASE}")
			endif(DEFINED CMAKE_TOOLCHAIN_FILE)
		else()
			if (DEFINED CMAKE_TOOLCHAIN_FILE)#if vcpkg
					get_filename_component(boost_bin ${Boost_LIBRARY_DIR_DEBUG} DIRECTORY)
					set(boost_bin "${boost_bin}/bin")
				else()
					set(boost_bin "${Boost_LIBRARY_DIR_DEBUG}")
				endif(DEFINED CMAKE_TOOLCHAIN_FILE)
		endif(BUILD_TYPE MATCHES Release)
		FILE(GLOB_RECURSE BOOST_DLL ${boost_bin}/*.dll)
		LIST(APPEND FMTDEPENDENCIES ${BOOST_DLL})
	endif(MSVC)
		set(Boost_USE_STATIC_LIBS OFF)
		add_definitions("-DBOOST_ALL_DYN_LINK")
		add_definitions("-DBOOST_THREAD_DYN_LINK")
endif(Boost_FOUND)

#If you dont have a dynamic boost lib try to find the static
if (NOT Boost_FOUND)#here try to find a dynamic boost llib
	set(Boost_USE_STATIC_LIBS ON) #By default try to find a static lib
	if (DEFINED PYTHONNAMING)
        	if("${PYTHONMAJOR}" STREQUAL "2")
            	set(BPY "python")
        	else()
            	set(BPY ${PYTHONNAMING})
        	endif("${PYTHONMAJOR}" STREQUAL "2")
        	if(MSVC)
				find_package(Boost COMPONENTS system filesystem serialization iostreams regex thread "${BPY}" OPTIONAL_COMPONENTS zlib)
			else()
				find_package(Boost COMPONENTS system filesystem serialization iostreams regex thread "${BPY}" OPTIONAL_COMPONENTS zlib) 
			endif(MSVC)
    	else()
		if(MSVC)
			find_package(Boost COMPONENTS system filesystem serialization iostreams regex thread "${BPY}" OPTIONAL_COMPONENTS zlib)
		else()
			find_package(Boost COMPONENTS system filesystem serialization iostreams regex thread "${BPY}" OPTIONAL_COMPONENTS zlib) 
		endif(MSVC)
	endif(DEFINED PYTHONNAMING)
	if (Boost_FOUND)
		add_definitions("-DBOOST_PYTHON_STATIC_LIB")
	endif(Boost_FOUND)
endif(NOT Boost_FOUND)


#the hard way
if (NOT Boost_FOUND AND NOT MSVC)
    find_package(Boost REQUIRED)
    file(GLOB_RECURSE Boost_LIBRARIES_PATHS $ENV{BOOSTROOT}*${CMAKE_STATIC_LIBRARY_SUFFIX})
    foreach(boostlibpath ${Boost_LIBRARIES_PATHS})
        get_filename_component(libname ${boostlibpath} NAME)
        if( NOT ("${libname}" MATCHES ".dll.a") AND
        ("${libname}" MATCHES "libboost" AND NOT "${libname}" MATCHES "-s") AND
        (("${libname}" MATCHES "zlib") OR
        ("${libname}" MATCHES "system") OR
        ("${libname}" MATCHES "serialization") OR
        ("${libname}" MATCHES "iostreams") OR
	("${libname}" MATCHES "regex") OR
	("${libname}" MATCHES "thread") OR
        (PythonLibs_FOUND AND "${libname}" MATCHES "${PYTHONNAMING}"))
        )
	   if ("${libname}" MATCHES "gd-x64" OR "${libname}" MATCHES "d-x64-")
		list(APPEND Boost_LIBRARIES "debug")
                list(APPEND Boost_LIBRARIES ${boostlibpath})
                set(Complete_boost "complete")
           elseif("${libname}" MATCHES "-x64")
		list(APPEND Boost_LIBRARIES "optimized")
                list(APPEND Boost_LIBRARIES ${boostlibpath})
           endif()
        endif()
    endforeach()
endif(NOT Boost_FOUND AND NOT MSVC)


if (NOT Boost_FOUND)
    message(FATAL_ERROR "Boost not found!")
endif(NOT Boost_FOUND)


message("Found Boost include here: ${Boost_INCLUDE_DIRS}")
include_directories(${Boost_INCLUDE_DIRS})
message("Found Boost libraries here: ${Boost_LIBRARIES}")
target_link_libraries(FMT ${Boost_LIBRARIES})
link_libraries(${Boost_LIBRARIES})



if (DEFINED ENV{GDAL_DIR})
    find_package(GDAL)
    if(GDAL_FOUND)
		if (NOT MSVC)
			if (NOT GDAL_CONFIG)
				FILE(GLOB_RECURSE GDAL_config_locations $ENV{GDAL_DIR}gdal-config)
				list(GET GDAL_config_locations 0 GDAL_CONFIG)
			endif(NOT GDAL_CONFIG)
			set(gdaldeparg "--dep-libs")
			set(gdalliarg "--libs")
			set(gdalverarg "--version")
			set(gdaldataarg "--datadir")
			if (NOT CYGWIN)
				set(gdaldeparg "${GDAL_CONFIG} --dep-libs")
				set(gdalliarg "${GDAL_CONFIG} --libs")
				set(gdalverarg "${GDAL_CONFIG} --version")
				set(gdaldataarg "${GDAL_CONFIG} --datadir")
				set(GDAL_CONFIG "sh")
				exec_program(${GDAL_CONFIG} ARGS "${gdaldeparg}" OUTPUT_VARIABLE GDAL_LINKER_FLAGS) #Cygwin does not like linker flags ?
				exec_program(${GDAL_CONFIG} ARGS "${gdalliarg}" OUTPUT_VARIABLE GDAL_LIBRARIES) #Cygwin does not like gdallibrary 
			endif(NOT CYGWIN)
			exec_program(${GDAL_CONFIG} ARGS "${gdalverarg}" OUTPUT_VARIABLE GDAL_FULLVERSION)
			#exec_program(${GDAL_CONFIG} ARGS "${gdaldataarg}" OUTPUT_VARIABLE GDAL_DATA)

			if ("${GDAL_FULLVERSION}" VERSION_LESS "2.3")
				 message(WARNING "You need at least GDAL 2.3 to get GDAL functionalities")
				 unset(GDAL_FOUND)
			endif()
		endif(NOT MSVC)
		if(GDAL_FOUND)
			set(potdata "/usr/share/gdal/")
			FILE(GLOB_RECURSE GDAL_POTDLL ${potdata}gt_ellips.csv)
			if (NOT GDAL_POTDLL)
				FILE(GLOB_RECURSE GDAL_POTDLL $ENV{GDAL_DIR}gt_ellips.csv)
			endif(NOT GDAL_POTDLL)
            list(GET GDAL_POTDLL 0 GDAL_elip)
            get_filename_component(GDAL_DATA "${GDAL_elip}" DIRECTORY)
            if(MSVC)
                #Go find the DLL
                FILE(GLOB_RECURSE GDAL_POTDLL $ENV{GDAL_DIR}gdal*.dll)
                list(GET GDAL_POTDLL 0 GDAL_DLL)
            endif(MSVC)
        endif(GDAL_FOUND)
    #else()
    #    find_package(GDALCUSTOM)
    endif(GDAL_FOUND)
        if (GDAL_FOUND)
            set(FMTWITHGDAL "-DFMTWITHGDAL")
            message("Found GDAL include here: ${GDAL_INCLUDE_DIR}")
            add_definitions("${FMTWITHGDAL}")
            include_directories(${GDAL_INCLUDE_DIR})
            message("Found GDAL library here: ${GDAL_LIBRARY}")
            target_link_libraries(FMT ${GDAL_LIBRARY})
			link_libraries(${GDAL_LIBRARY})
			if (GDAL_LIBRARIES)
				foreach(linkerflags ${GDAL_LIBRARIES})
					if ((MSVC AND "${linkerflags}" MATCHES "::") OR NOT MSVC)
						target_link_libraries(FMT ${linkerflags})
						link_libraries(${linkerflags})
					endif((MSVC AND "${linkerflags}" MATCHES "::") OR NOT MSVC)
                endforeach()
			endif(GDAL_LIBRARIES)
            if (NOT MSVC AND GDAL_LINKER_FLAGS)
                foreach(linkerflags ${GDAL_LINKER_FLAGS})
                    target_link_libraries(FMT ${linkerflags})
					link_libraries(${linkerflags})
                endforeach()
            endif(NOT MSVC AND GDAL_LINKER_FLAGS)
            if (WIN32 AND DEFINED GDAL_DLL)
                message("GDAL DLL found here: ${GDAL_DLL}")
                LIST(APPEND FMTDEPENDENCIES ${GDAL_DLL})
            endif()
            link_directories(${GDAL_LIBRARY_DIRS})
			FILE(GLOB_RECURSE Projdb $ENV{GDAL_DIR}proj.db)#Might be somewhere else
			FILE(GLOB_RECURSE Projdb2 $ENV{PROJ_LIB}proj.db)#Might be somewhere else
			list(APPEND Projdb Projdb2)
			if(Projdb)
				list(GET Projdb 0 projloc)
				get_filename_component(PROJDIR "${projloc}" DIRECTORY)
				get_filename_component(PROJDIRNAME "${PROJDIR}" NAME)
				message("Found proj here ${PROJDIR}")
			else()
				message(WARNING "No proj directory found")
			endif(Projdb)



            if (((NOT GDAL_CONFIG_LIBS OR (NOT "${GDAL_CONFIG_LIBS}" MATCHES "lgeos_c")) AND (GDAL_LINKER_FLAGS AND (NOT "${GDAL_LINKER_FLAGS}" MATCHES "lgeos_c"))) OR MSVC)
				find_package(GEOS)
                if (GEOS_FOUND)
                    message("Found GEOS libraries here: ${GEOS_LIBRARIES}")
                    target_link_libraries(FMT ${GEOS_LIBRARIES})
					link_libraries(${GEOS_LIBRARIES})
                    if (WIN32 AND DEFINED GEOS_DLL)
                        message("GEOS DLL found here: ${GEOS_DLL}")
                        LIST(APPEND FMTDEPENDENCIES ${GEOS_DLL})
                    endif()
                endif(GEOS_FOUND)
            endif()
        else()
            message("GDAL not found take a look at (https://github.com/OSGeo/gdal)")
            message("Compile GDAL with GEOS for more functionalities (https://github.com/libgeos/geos)")
        endif(GDAL_FOUND)
endif(DEFINED ENV{GDAL_DIR})


if (DEFINED ENV{OSI_DIR})
	find_package(OSI)
	if (OSI_FOUND)
		set(FMTWITHOSI "-DFMTWITHOSI")
		add_definitions("${FMTWITHOSI}")
		message("Found OSI include here: ${OSI_INCLUDE_DIRS}")
		include_directories(${OSI_INCLUDE_DIRS})
		message("Found OSI library here: ${OSI_LIBRARIES}")
		target_link_libraries(FMT ${OSI_LIBRARIES})
		link_libraries(${OSI_LIBRARIES})
		if (NOT MSVC)
			foreach(linkerflags ${OSI_LINKER_FLAGS})
				target_link_libraries(FMT ${linkerflags})
				link_libraries(${linkerflags})
			endforeach()
		endif(NOT MSVC)
		if (DEFINED ENV{MOSEK_DIR} AND (MSVC OR OSI_MSK_LIBRARY_RELEASE))
            find_package(MOSEK)
			if (MOSEK_FOUND)
				set(FMTWITHMOSEK "-DFMTWITHMOSEK")
				add_definitions("${FMTWITHMOSEK}")
				message("Found Mosek include here: ${MOSEK_INCLUDE_DIR}")
				include_directories(${MOSEK_INCLUDE_DIR})
				message("Found Mosek library here: ${MOSEK_LIB}")
				target_link_libraries(FMT ${MOSEK_LIB})
				link_libraries(${MOSEK_LIB})
				include_directories(${OSIMSK_INCLUDE})
				if (NOT MSVC)
					foreach(linkerflags ${MOSEK_LINKER_FLAGS})
						target_link_libraries(FMT ${linkerflags})
						link_libraries(${linkerflags})
					endforeach()
				else()
					target_sources(FMT PRIVATE ${OSIMSK_DEFINITION})
					list(APPEND sourcefile "${OSIMSK_DEFINITION}")
				endif(NOT MSVC)
				if (NOT CYGWIN AND DEFINED MOSEK_DLL)
					message("MOSEK DLL found here: ${MOSEK_DLL}")
					LIST(APPEND FMTDEPENDENCIES ${MOSEK_DLL})
				endif(NOT CYGWIN AND DEFINED MOSEK_DLL)
			endif(MOSEK_FOUND)
		endif(DEFINED ENV{MOSEK_DIR} AND (MSVC OR OSI_MSK_LIBRARY_RELEASE))
	else()
		message("Get OSI and CLP throught CBC project: (https://github.com/coin-or/Cbc)")
	endif(OSI_FOUND)
endif(DEFINED ENV{OSI_DIR})

if (NOT MSVC)
	target_link_libraries(FMT "-lbz2")
	link_libraries("-lbz2")
	target_link_libraries(FMT "-lz")
	link_libraries("-lz")
ENDIF(NOT MSVC)



if (DEFINED CMAKE_TOOLCHAIN_FILE)
	get_filename_component(PARENT_DIR ${CMAKE_TOOLCHAIN_FILE} DIRECTORY)
	get_filename_component(PARENT_DIR1 ${PARENT_DIR} DIRECTORY)
	get_filename_component(PARENT_DIR2 ${PARENT_DIR1} DIRECTORY)
	FILE(GLOB_RECURSE VCPKG_DLL ${PARENT_DIR2}/installed/${VCPKG_TARGET_TRIPLET}/bin/*.dll)
	list (APPEND FMTDEPENDENCIES ${VCPKG_DLL})
	list( REMOVE_DUPLICATES FMTDEPENDENCIES)
	list(APPEND DLLlookuppaths "${PARENT_DIR2}/installed/${VCPKG_TARGET_TRIPLET}/bin/")
endif(DEFINED CMAKE_TOOLCHAIN_FILE)


get_filename_component(GDAL_DATA_NAME "${GDAL_DATA}" NAME)



include(${CMAKE_CURRENT_SOURCE_DIR}/Examples/C++/CMakeLists.txt) #will polulate tests

if (NOT DEFINED PARALLEL_TESTS)
	set(PARALLEL_TESTS 1)
endif(NOT DEFINED PARALLEL_TESTS)

list( REMOVE_DUPLICATES DLLlookuppaths)

#install part
install (CODE
    "set (PATHS_DEPS \"${DLLlookuppaths}\")
	set(GDAL_FOUND \"${GDAL_FOUND}\")
    file(COPY \"${CMAKE_CURRENT_SOURCE_DIR}/LICENSES\" DESTINATION \"${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}\")
    file(COPY \"${CMAKE_CURRENT_SOURCE_DIR}/Templates/YieldPredModels\" DESTINATION \"${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}\")
    if (GDAL_FOUND)
        #Copy gdal data
		if (NOT EXISTS \"${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}/GDAL_DATA\")
			file(COPY \"${GDAL_DATA}\" DESTINATION \"${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}\")
			file(RENAME \"${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}/${GDAL_DATA_NAME}\" \"${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}/GDAL_DATA\")
		endif(NOT EXISTS \"${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}/GDAL_DATA\")
		if (NOT EXISTS \"${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}/proj\" AND EXISTS \"${PROJDIR}\")
			file(COPY \"${PROJDIR}\" DESTINATION \"${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}\")
			if (NOT EXISTS \"${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}/proj\")
				file(RENAME \"${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}/${PROJDIRNAME}\" \"${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}/proj\")
			endif(NOT EXISTS \"${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}/proj\")
		endif(NOT EXISTS \"${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}/proj\" AND EXISTS \"${PROJDIR}\")
    endif(GDAL_FOUND)
	set(DEPENDENCIES \"${FMTDEPENDENCIES}\")
	if(${CMAKE_VERSION} VERSION_LESS \"3.16.0\") 
    	message(\"Please consider to switch to CMake 3.16.0\")
	else()
		FILE(GLOB_RECURSE FMTlibLocations \"${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}/FMT.*\")
		list(GET FMTlibLocations 0 FMTloc)
		file(	GET_RUNTIME_DEPENDENCIES
				RESOLVED_DEPENDENCIES_VAR DEPENDENCIESfound
				UNRESOLVED_DEPENDENCIES_VAR deps_unresolved
				CONFLICTING_DEPENDENCIES_PREFIX deps_conflicts
				LIBRARIES \"\${FMTloc}\"
				DIRECTORIES \${PATHS_DEPS}
				)
		list (APPEND DEPENDENCIES \${DEPENDENCIESfound})
	endif(${CMAKE_VERSION} VERSION_LESS \"3.16.0\")
    if (DEPENDENCIES)
        #Copy dependencies
        foreach(FMTdep \${DEPENDENCIES}\)
            get_filename_component(depname \"\${FMTdep}\" NAME)
            file(COPY \"\${FMTdep}\" DESTINATION \"${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}\")
        endforeach()
    endif(DEPENDENCIES)
	if (EXISTS \"${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}/setup.py\")
        find_program(PYTHON \"python\" PATHS \"${PYTHON_DIR}\")
        execute_process(
          COMMAND \"$\{PYTHON\}\" -m pip wheel .
          WORKING_DIRECTORY \"${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}\"
        )
    endif (EXISTS \"${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}/setup.py\")
	if (EXISTS \"${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}/FMT\")
		file(COPY \"${CMAKE_CURRENT_SOURCE_DIR}/Templates/YieldPredModels\" DESTINATION \"${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}/FMT/inst/libs/x64/\")
		file(COPY \"${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}/GDAL_DATA\" DESTINATION \"${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}/FMT/inst/libs/x64/\")
		if (EXISTS \"${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}/proj\")
			file(COPY \"${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}/proj\" DESTINATION \"${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}/FMT/inst/libs/x64/\")
		endif(EXISTS \"${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}/proj\")
		file(COPY \"${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}/FMT${CMAKE_SHARED_LIBRARY_SUFFIX}\" DESTINATION \"${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}/FMT/inst/libs/x64/\")
		file(COPY \"${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}/libFMT${CMAKE_SHARED_LIBRARY_SUFFIX}${CMAKE_STATIC_LIBRARY_SUFFIX}\" DESTINATION \"${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}/FMT/inst/libs/x64/\")
        if (DEPENDENCIES)
			#Copy dependencies
			foreach(FMTdep \${DEPENDENCIES}\)
				file(COPY \"\${FMTdep}\" DESTINATION \"${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}/FMT/inst/libs/x64/\")
			endforeach()
		 endif(DEPENDENCIES)
		find_program(RLOCATION \"R\" PATHS \"${R_DIR}/bin/x64\")
        execute_process(
          COMMAND \"$\{RLOCATION\}\" CMD build FMT
          WORKING_DIRECTORY \"${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}\"
        )
		execute_process(
          COMMAND \"$\{RLOCATION\}\" CMD INSTALL --no-multiarch \"FMT_${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.tar.gz\"
          WORKING_DIRECTORY \"${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}\"
        )
endif (EXISTS \"${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}/FMT\")
file(MAKE_DIRECTORY \"${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}/tests\")
 execute_process(
          COMMAND ctest -C \"${BUILD_TYPE}\" -j \"${PARALLEL_TESTS}\"
          WORKING_DIRECTORY \"${CMAKE_BINARY_DIR}\"
        )
file(REMOVE_RECURSE \"${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}/tests\")
")








