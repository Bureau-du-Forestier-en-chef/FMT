#[[
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
]]

FILE(GLOB_RECURSE OSI_POTENTIAL_INCLUDE $ENV{OSI_DIR}OsiConfig.h)
list(GET OSI_POTENTIAL_INCLUDE 0 FIRSTINCLUDE)
get_filename_component(POTOSI_INCLUDE_DIR ${FIRSTINCLUDE} DIRECTORY)
find_path(OSI_INCLUDE_DIR
            NAMES OsiConfig.h
            PATHS ${POTOSI_INCLUDE_DIR}
            )



FILE(GLOB_RECURSE OSI_POTENTIAL_LIB $ENV{OSI_DIR}/lib*${CMAKE_STATIC_LIBRARY_SUFFIX})

if (MSVC)
	foreach(includepath ${OSI_POTENTIAL_LIB})
		get_filename_component(potfolder ${includepath} DIRECTORY)
		if(("${potfolder}" MATCHES "Release") AND ("${potfolder}" MATCHES "/Cbc/") AND ("${CMAKE_BUILD_TYPE}" STREQUAL "Release"))
			set(POTOSI_LIB_DIR ${potfolder})
		elseif(("${potfolder}" MATCHES "Debug") AND ("${potfolder}" MATCHES "/Cbc/") AND ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug"))
			set(POTOSI_LIB_DIR ${potfolder})
		endif()
	endforeach()
else()
	foreach(full_path ${OSI_POTENTIAL_LIB})
		get_filename_component(LIB_DIR ${full_path} DIRECTORY)
		list(APPEND POTOSI_LIB_DIR ${LIB_DIR})

	endforeach()
	#list(GET OSI_POTENTIAL_LIB 0 FIRSTLIB)
	#get_filename_component(POTOSI_LIB_DIR ${FIRSTLIB} DIRECTORY)
endif(MSVC)


#get CLP include
FILE(GLOB_RECURSE CLP_POTENTIAL_INCLUDE $ENV{OSI_DIR}ClpSimplex.hpp)
list(GET CLP_POTENTIAL_INCLUDE 0 FIRSTINCLUDE)
get_filename_component(POTCLP_INCLUDE_DIR ${FIRSTINCLUDE} DIRECTORY)
find_path(CLP_INCLUDE_DIR
          NAMES ClpSimplex.hpp
          PATHS ${POTCLP_INCLUDE_DIR}
          )

#get coin utils headers
FILE(GLOB_RECURSE CU_POTENTIAL_INCLUDE $ENV{OSI_DIR}CoinUtility.hpp)

list(GET CU_POTENTIAL_INCLUDE 0 FIRSTINCLUDE)
get_filename_component(POTCU_INCLUDE_DIR ${FIRSTINCLUDE} DIRECTORY)
find_path(CU_INCLUDE_DIR
          NAMES CoinUtility.hpp
          PATHS ${POTCU_INCLUDE_DIR}
          )



find_library(COINUTILS_LIBRARY
			NAMES libCoinUtils libCoinUtils.a
			PATHS ${POTOSI_LIB_DIR}
             )


find_library( OSI_LIBRARY
              NAMES Osi libOsi
              PATHS ${POTOSI_LIB_DIR}
              )

find_library( OSI_CLP_LIBRARY
              NAMES OsiClp libOsiClp
              PATHS  ${POTOSI_LIB_DIR}
              )

find_library(CLP_LIBRARY
              NAMES libClp libclp.a
              PATHS ${POTOSI_LIB_DIR}
              )



if(NOT MSVC AND DEFINED ENV{MOSEK_DIR})
	find_library(OSI_MSK_LIBRARY
				  NAMES libOsiMsk libOsiMsk.a
				  PATHS ${POTOSI_LIB_DIR}
				  )
    if(NOT OSI_MSK_LIBRARY AND DEFINED ENV{MOSEK_DIR})
        message(WARNING "Mosek path provided but Osi is not compiled with Mosek")
    endif(NOT OSI_MSK_LIBRARY AND DEFINED ENV{MOSEK_DIR})
endif(NOT MSVC AND DEFINED ENV{MOSEK_DIR})
if(NOT MSVC)
	find_library(CLP_SOLVER_LIBRARY
				NAMES libClpSolver libClpSolver.a
				PATHS ${POTOSI_LIB_DIR}
				)
	list(APPEND OSI_LINKER_FLAGS "-llapack")
	list(APPEND OSI_LINKER_FLAGS "-lBlas")
	list(APPEND OSI_LINKER_FLAGS "-lgfortran")
	list(APPEND OSI_LINKER_FLAGS "-lquadmath")
	list(APPEND OSI_LINKER_FLAGS "-lbz2")
	list(APPEND OSI_LINKER_FLAGS "-lz")
endif(NOT MSVC)





#Get Osiclpheader
FILE(GLOB_RECURSE OSI_POTENTIAL_INCLUDE $ENV{OSI_DIR}OsiClpSolverInterface.hpp)
list(GET OSI_POTENTIAL_INCLUDE 0 FIRSTINCLUDE)
get_filename_component(OSICLP_INCLUDE_DIR ${FIRSTINCLUDE} DIRECTORY)



set(OSI_INCLUDE_DIRS "${OSI_INCLUDE_DIR};${CLP_INCLUDE_DIR};${CU_INCLUDE_DIR};${OSICLP_INCLUDE_DIR}" )
if(NOT MSVC AND DEFINED ENV{MOSEK_DIR} AND OSI_MSK_LIBRARY)
	set(OSI_LIBRARIES "${OSI_CLP_LIBRARY};${CLP_SOLVER_LIBRARY};${CLP_LIBRARY};${OSI_MSK_LIBRARY};${OSI_LIBRARY};${COINUTILS_LIBRARY}")
else()
	set(OSI_LIBRARIES "${OSI_CLP_LIBRARY};${CLP_LIBRARY};${OSI_LIBRARY};${COINUTILS_LIBRARY}")
endif(NOT MSVC AND DEFINED ENV{MOSEK_DIR} AND OSI_MSK_LIBRARY)

include(FindPackageHandleStandardArgs)

if (NOT MSVC)
find_package_handle_standard_args(OSI  DEFAULT_MSG
                                  OSI_LIBRARY OSI_CLP_LIBRARY COINUTILS_LIBRARY CLP_LIBRARY OSI_INCLUDE_DIRS CLP_INCLUDE_DIR CU_INCLUDE_DIR OSI_LINKER_FLAGS OSICLP_INCLUDE_DIR)

mark_as_advanced(OSI_INCLUDE_DIRS CLP_INCLUDE_DIR
					OSI_LIBRARY OSI_CLP_LIBRARY COINUTILS_LIBRARY CLP_LIBRARY CU_INCLUDE_DIR OSICLP_INCLUDE_DIR OSI_LINKER_FLAGS OSI_FOUND)
else()
find_package_handle_standard_args(OSI  DEFAULT_MSG
                                  OSI_LIBRARY OSI_CLP_LIBRARY COINUTILS_LIBRARY CLP_LIBRARY OSI_INCLUDE_DIRS CLP_INCLUDE_DIR CU_INCLUDE_DIR OSICLP_INCLUDE_DIR)

mark_as_advanced(OSI_INCLUDE_DIRS CLP_INCLUDE_DIR
					OSI_LIBRARY OSI_CLP_LIBRARY COINUTILS_LIBRARY CLP_LIBRARY CU_INCLUDE_DIR OSICLP_INCLUDE_DIR OSI_FOUND)
endif(NOT MSVC)
