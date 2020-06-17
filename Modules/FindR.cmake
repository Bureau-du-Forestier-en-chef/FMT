#[[
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
]]

FILE(GLOB_RECURSE R_POTENTIAL_INCLUDE $ENV{R_DIR}R.h)
list(GET R_POTENTIAL_INCLUDE 0 FIRSTINCLUDE)
get_filename_component(R_INCLUDE_DIR ${FIRSTINCLUDE} DIRECTORY)

FILE(GLOB_RECURSE R_POTENTIAL_DLL $ENV{R_DIR}R.dll)
foreach(dllpath ${R_POTENTIAL_DLL})
	if ("${dllpath}" MATCHES "x64")
		set(R_DLL "${dllpath}")
	endif()
endforeach()


set(R_LINKER_FLAGS "-lR;-lquadmath;-liconv")



include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(R DEFAULT_MSG
                                  R_INCLUDE_DIR R_LINKER_FLAGS R_DLL)

mark_as_advanced(R_INCLUDE_DIR R_LINKER_FLAGS R_DLL R_FOUND)

