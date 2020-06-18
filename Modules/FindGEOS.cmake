#[[
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
]]

FILE(GLOB_RECURSE GEOS_DLL1 $ENV{GEOS_DIR}geos.dll)
FILE(GLOB_RECURSE GEOS_DLL2 $ENV{GEOS_DIR}geos_c.dll)
list(APPEND POTGEOS_DLL "${GEOS_DLL1}")
list(APPEND POTGEOS_DLL "${GEOS_DLL2}")


if (WIN32)
	foreach(dllpath ${POTGEOS_DLL})
		if(("${dllpath}" MATCHES "Release") AND ("${CMAKE_BUILD_TYPE}" STREQUAL "Release"))
			list(APPEND GEOS_DLL "${dllpath}")
		elseif(("${dllpath}" MATCHES "Debug") AND ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug"))
			list(APPEND GEOS_DLL "${dllpath}")
		endif()
	endforeach()
endif()



FILE(GLOB_RECURSE GEOS_LIB1 $ENV{GEOS_DIR}geos${CMAKE_STATIC_LIBRARY_SUFFIX})
FILE(GLOB_RECURSE GEOS_LIB2 $ENV{GEOS_DIR}geos_c${CMAKE_STATIC_LIBRARY_SUFFIX})
FILE(GLOB_RECURSE GEOS_LIB3 $ENV{GEOS_DIR}libgeos${CMAKE_STATIC_LIBRARY_SUFFIX})
FILE(GLOB_RECURSE GEOS_LIB4 $ENV{GEOS_DIR}libgeos_c${CMAKE_STATIC_LIBRARY_SUFFIX})
list(APPEND POTGEOS_LIB "${GEOS_LIB1}")
list(APPEND POTGEOS_LIB "${GEOS_LIB2}")
list(APPEND POTGEOS_LIB "${GEOS_LIB3}")
list(APPEND POTGEOS_LIB "${GEOS_LIB4}")

set(GEOS_LINKER_FLAGS "-lgeos")

foreach(libpath ${POTGEOS_LIB})
		if(("${libpath}" MATCHES "Release") AND ("${CMAKE_BUILD_TYPE}" STREQUAL "Release"))
			list(APPEND GEOS_LIBRARIES "${libpath}")
		elseif(("${libpath}" MATCHES "Debug") AND ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug"))
			list(APPEND GEOS_LIBRARIES "${libpath}")
		elseif((NOT MSVC) AND ( NOT ("${libpath}" MATCHES "Debug")) AND ( NOT ("${libpath}" MATCHES "Release")))
			list(APPEND GEOS_LIBRARIES "${libpath}")
		endif()
	endforeach()

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(GEOS  DEFAULT_MSG
                                  GEOS_LIBRARIES GEOS_LINKER_FLAGS)

mark_as_advanced(GDAL  GEOS_LIBRARIES GEOS_LINKER_FLAGS GEOS_FOUND)
