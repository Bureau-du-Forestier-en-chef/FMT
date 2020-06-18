#[[
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
]]

FILE(GLOB_RECURSE GDAL_POTENTIAL_INCLUDE1 $ENV{GDAL_DIR}gdal.h)
FILE(GLOB_RECURSE GDAL_POTENTIAL_INCLUDE2 $ENV{GDAL_DIR}cpl_error.h)
list(APPEND GDAL_POTENTIAL_INCLUDE "${GDAL_POTENTIAL_INCLUDE1}")
list(APPEND GDAL_POTENTIAL_INCLUDE "${GDAL_POTENTIAL_INCLUDE2}")
list(SORT GDAL_POTENTIAL_INCLUDE)
foreach(includepath ${GDAL_POTENTIAL_INCLUDE})
	get_filename_component(potfolder ${includepath} DIRECTORY)
	if (potfolder STREQUAL folderheader)
		set(GDAL_INCLUDE_DIR "${potfolder}")
	else()
		set(folderheader "${potfolder}")
	endif()
endforeach()

find_path(GDAL_DATA NAMES ellipsoid.csv PATHS $ENV{GDAL_DIR}/data)

find_path(GDAL_INCLUDE_DIR 
          NAMES gdal.h
          PATHS ${POTGDAL_INCLUDE_DIR}
          )
if(WIN32)
	#Go find the DLL
	FILE(GLOB_RECURSE GDAL_POTDLL $ENV{GDAL_DIR}gdal*.dll)
	list(GET GDAL_POTDLL 0 GDAL_DLL)
endif(WIN32)

if (MSVC)
	FILE(GLOB_RECURSE GDAL_POTENTIAL_LIBRARY $ENV{GDAL_DIR}gdal.lib)
	if (NOT DEFINED {GDAL_POTENTIAL_LIBRARY})
		FILE(GLOB_RECURSE GDAL_POTENTIAL_LIBRARY $ENV{GDAL_DIR}gdal*.lib)
	endif()
	if (NOT DEFINED {GDAL_POTENTIAL_LIBRARY})
		FILE(GLOB_RECURSE GDAL_POTENTIAL_LIBRARY $ENV{GDAL_DIR}gdal_i.lib)
	endif()
else()
	FILE(GLOB_RECURSE GDAL_POTENTIAL_LIBRARY $ENV{GDAL_DIR}libgdal.a)
endif(MSVC)

if (WIN32 AND NOT MSVC)
	FILE(GLOB_RECURSE GDAL_config_locations $ENV{GDAL_DIR}gdal-config)
	list(GET GDAL_config_locations 0 GDAL_config_location)
	file(READ "${GDAL_config_location}" GDAL_CONFIG_FILE)
	string(REGEX MATCH "^(.*)(CONFIG_DEP_LIBS=)([^\n]*)(.+)" dummy ${GDAL_CONFIG_FILE})
	string(REGEX REPLACE " " "" outvar1 "${CMAKE_MATCH_3}")
	string(REGEX REPLACE "\"" "" outvar2 "${outvar1}")
	string(REGEX REPLACE "-l" ";-l" outvar3 "${outvar2}")
	string(LENGTH "${outvar3}" outlength)
	string(SUBSTRING "${outvar3}" 1 ${outlength} outvar4)
	foreach(gdaldep ${outvar4})
		list(APPEND GDAL_LINKER_FLAGS "${gdaldep}")
	endforeach()
endif(WIN32 AND NOT MSVC)


list(GET GDAL_POTENTIAL_LIBRARY 0 GDAL_LIBRARY)
get_filename_component(GDAL_LIBRARY_DIRS ${GDAL_LIBRARY} DIRECTORY)


include(FindPackageHandleStandardArgs)

if (NOT MSVC)
	find_package_handle_standard_args(GDAL  DEFAULT_MSG
									  GDAL_LIBRARY GDAL_INCLUDE_DIR GDAL_LIBRARY_DIRS GDAL_DATA GDAL_LINKER_FLAGS)

	mark_as_advanced(GDAL  DEFAULT_MSG GDAL_LIBRARY GDAL_INCLUDE_DIR GDAL_LIBRARY_DIRS GDAL_DATA GDAL_LINKER_FLAGS GDAL_FOUND)
else()
	find_package_handle_standard_args(GDAL  DEFAULT_MSG
									  GDAL_LIBRARY GDAL_INCLUDE_DIR GDAL_LIBRARY_DIRS GDAL_DATA)

	mark_as_advanced(GDAL  DEFAULT_MSG GDAL_LIBRARY GDAL_INCLUDE_DIR GDAL_LIBRARY_DIRS GDAL_DATA GDAL_FOUND)
endif(NOT MSVC)


