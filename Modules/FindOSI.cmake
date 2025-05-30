#[[
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
]]

FILE(GLOB_RECURSE OSI_POTENTIAL_INCLUDE $ENV{OSI_DIR}OsiConfig.h $ENV{OSI_DIR}/OsiConfig.h)
list(GET OSI_POTENTIAL_INCLUDE 0 FIRSTINCLUDE)
get_filename_component(POTOSI_INCLUDE_DIR ${FIRSTINCLUDE} DIRECTORY)
find_path(OSI_INCLUDE_DIR
            NAMES OsiConfig.h
            PATHS ${POTOSI_INCLUDE_DIR}
            )



FILE(GLOB_RECURSE OSI_POTENTIAL_LIB $ENV{OSI_DIR}/*${CMAKE_STATIC_LIBRARY_SUFFIX})


if (MSVC)
	foreach(includepath ${OSI_POTENTIAL_LIB})
		get_filename_component(potfolder ${includepath} DIRECTORY)
		string(TOLOWER "${includepath}" lowercase)
		if ("${lowercase}" MATCHES "debug" AND "${lowercase}" MATCHES "x64")
			list(APPEND POTOSI_LIB_DEBUG_DIR ${potfolder})
		else()
			list(APPEND POTOSI_LIB_DIR ${potfolder})
		endif("${lowercase}" MATCHES "debug" AND "${lowercase}" MATCHES "x64")
        
	endforeach()
else()
	foreach(full_path ${OSI_POTENTIAL_LIB})
		get_filename_component(LIB_DIR ${full_path} DIRECTORY)
		string(TOLOWER "${full_path}" lowercase)
		if ("${lowercase}" MATCHES "debug")
			list(APPEND POTOSI_LIB_DEBUG_DIR ${LIB_DIR})
		else()
			list(APPEND POTOSI_LIB_DIR ${LIB_DIR})
		endif("${lowercase}" MATCHES "debug")
	endforeach()
	#list(GET OSI_POTENTIAL_LIB 0 FIRSTLIB)
	#get_filename_component(POTOSI_LIB_DIR ${FIRSTLIB} DIRECTORY)
endif(MSVC)


#get CLP include
FILE(GLOB_RECURSE CLP_POTENTIAL_INCLUDE $ENV{OSI_DIR}ClpSimplex.hpp $ENV{OSI_DIR}/ClpSimplex.hpp)
list(GET CLP_POTENTIAL_INCLUDE 0 FIRSTINCLUDE)
get_filename_component(POTCLP_INCLUDE_DIR ${FIRSTINCLUDE} DIRECTORY)
find_path(CLP_INCLUDE_DIR
          NAMES ClpSimplex.hpp
          PATHS ${POTCLP_INCLUDE_DIR}
          )

#get coin utils headers
FILE(GLOB_RECURSE CU_POTENTIAL_INCLUDE $ENV{OSI_DIR}CoinUtility.hpp $ENV{OSI_DIR}/CoinUtility.hpp)

list(GET CU_POTENTIAL_INCLUDE 0 FIRSTINCLUDE)
get_filename_component(POTCU_INCLUDE_DIR ${FIRSTINCLUDE} DIRECTORY)
find_path(CU_INCLUDE_DIR
          NAMES CoinUtility.hpp
          PATHS ${POTCU_INCLUDE_DIR}
          )




find_library(COINUTILS_LIBRARY_RELEASE
			NAMES CoinUtils libCoinUtils libCoinUtils.a
			PATHS ${POTOSI_LIB_DIR}
             )


find_library( OSI_LIBRARY_RELEASE
              NAMES Osi libOsi
              PATHS ${POTOSI_LIB_DIR}
              )

#if (NOT "${POTOSI_LIB_DIR}" MATCHES "vcpkg")
	find_library( OSI_CLP_LIBRARY_RELEASE
              	NAMES OsiClp libOsiClp
              	PATHS  ${POTOSI_LIB_DIR}
              	)
#else()
#	set(OSI_CLP_LIBRARY_RELEASE "${OSI_LIBRARY_RELEASE}")
#endif(NOT "${POTOSI_LIB_DIR}" MATCHES "vcpkg")





find_library(CLP_LIBRARY_RELEASE
              NAMES Clp libClp libclp
              PATHS ${POTOSI_LIB_DIR}
              )


	find_library(COINUTILS_LIBRARY_DEBUG
			NAMES CoinUtils libCoinUtils
			PATHS ${POTOSI_LIB_DEBUG_DIR}
			NO_DEFAULT_PATH
             )


	find_library( OSI_LIBRARY_DEBUG
              NAMES Osi libOsi
              PATHS ${POTOSI_LIB_DEBUG_DIR}
	      NO_DEFAULT_PATH
              )


	#if (NOT "${POTOSI_LIB_DEBUG_DIR}" MATCHES "vcpkg")
		find_library( OSI_CLP_LIBRARY_DEBUG
              	NAMES OsiClp libOsiClp
              	PATHS  ${POTOSI_LIB_DEBUG_DIR}
		NO_DEFAULT_PATH
              	)
	#else()
	#	set(OSI_CLP_LIBRARY_DEBUG "${OSI_LIBRARY_DEBUG}")
	#endif(NOT "${POTOSI_LIB_DEBUG_DIR}" MATCHES "vcpkg")


	find_library(CLP_LIBRARY_DEBUG
              NAMES Clp libClp libclp
              PATHS ${POTOSI_LIB_DEBUG_DIR}
	      NO_DEFAULT_PATH
              )






if(DEFINED ENV{MOSEK_DIR})
	find_library(OSI_MSK_LIBRARY_RELEASE
				  NAMES libOsiMsk OsiMsk
				  PATHS ${POTOSI_LIB_DIR}
				  )

	find_library(OSI_MSK_LIBRARY_DEBUG
				  NAMES libOsiMsk OsiMsk
				  PATHS ${POTOSI_LIB_DEBUG_DIR}
				  NO_DEFAULT_PATH
				  )
    if(NOT OSI_MSK_LIBRARY_RELEASE AND DEFINED ENV{MOSEK_DIR})
        message(WARNING "Mosek path provided but Osi is not compiled with Mosek")
    endif(NOT OSI_MSK_LIBRARY_RELEASE AND DEFINED ENV{MOSEK_DIR})
endif(DEFINED ENV{MOSEK_DIR})


if(NOT MSVC)
	find_library(CLP_SOLVER_LIBRARY_RELEASE
				NAMES libClpSolver
				PATHS ${POTOSI_LIB_DIR}
				)

	find_library(CLP_SOLVER_LIBRARY_DEBUG
				NAMES libClpSolver
				PATHS ${POTOSI_LIB_DEBUG_DIR}
				NO_DEFAULT_PATH
				)


    find_library(OSI_LAPACK
                NAMES liblapack.a
                PATHS $ENV{HOME})


    if (OSI_LAPACK)
        list(APPEND OSI_LINKER_FLAGS "-llapack")
        	if(NOT CYGWIN)
                list(APPEND OSI_LINKER_FLAGS "-lgfortran")
                list(APPEND OSI_LINKER_FLAGS "-lquadmath")
            endif(NOT CYGWIN)
        list(APPEND OSI_LINKER_FLAGS "-lBlas")
    endif(OSI_LAPACK)
    find_library(OSI_LIBZ
                NAMES libz.a libbz2.a
                PATHS $ENV{HOME})
    if (OSI_LIBZ)
        list(APPEND OSI_LINKER_FLAGS "-lz")
    endif(OSI_LIBZ)
    find_library(OSI_BZ2
                NAMES libbz2.a
                PATHS $ENV{HOME})
    if (OSI_BZ2)
        list(APPEND OSI_LINKER_FLAGS "-lbz2")
    endif(OSI_BZ2)
endif(NOT MSVC)



#Get Osiclpheader
FILE(GLOB_RECURSE OSI_POTENTIAL_INCLUDE $ENV{OSI_DIR}OsiClpSolverInterface.hpp $ENV{OSI_DIR}/OsiClpSolverInterface.hpp)
list(GET OSI_POTENTIAL_INCLUDE 0 FIRSTINCLUDE)
get_filename_component(OSICLP_INCLUDE_DIR ${FIRSTINCLUDE} DIRECTORY)

if (DEFINED ENV_PREFIX)
	set(OSI_CLP_LIBRARY_DEBUG "${OSI_CLP_LIBRARY_RELEASE}")
	set(CLP_LIBRARY_DEBUG "${CLP_LIBRARY_RELEASE}")
	set(OSI_LIBRARY_DEBUG "${OSI_LIBRARY_RELEASE}")
	set(COINUTILS_LIBRARY_DEBUG "${COINUTILS_LIBRARY_RELEASE}")
endif(DEFINED ENV_PREFIX)



set(OSI_INCLUDE_DIRS "${OSI_INCLUDE_DIR};${CLP_INCLUDE_DIR};${CU_INCLUDE_DIR};${OSICLP_INCLUDE_DIR}" )

#Libraries
set(OSI_LIBRARIES "optimized;${OSI_CLP_LIBRARY_RELEASE};debug;${OSI_CLP_LIBRARY_DEBUG}")
if(CLP_SOLVER_LIBRARY_RELEASE)
	set(OSI_LIBRARIES "${OSI_LIBRARIES};optimized;${CLP_SOLVER_LIBRARY_RELEASE};debug;${CLP_SOLVER_LIBRARY_DEBUG}")
endif(CLP_SOLVER_LIBRARY_RELEASE)
set(OSI_LIBRARIES "${OSI_LIBRARIES};optimized;${CLP_LIBRARY_RELEASE};debug;${CLP_LIBRARY_DEBUG}")
if(OSI_MSK_LIBRARY_RELEASE)
	set(OSI_LIBRARIES "${OSI_LIBRARIES};optimized;${OSI_MSK_LIBRARY_RELEASE};debug;${OSI_MSK_LIBRARY_DEBUG}")
endif(OSI_MSK_LIBRARY_RELEASE)
set(OSI_LIBRARIES "${OSI_LIBRARIES};optimized;${OSI_LIBRARY_RELEASE};debug;${OSI_LIBRARY_DEBUG};optimized;${COINUTILS_LIBRARY_RELEASE};debug;${COINUTILS_LIBRARY_DEBUG}")









include(FindPackageHandleStandardArgs)

if (NOT MSVC)
find_package_handle_standard_args(OSI  DEFAULT_MSG
                                  OSI_LIBRARY_RELEASE OSI_CLP_LIBRARY_RELEASE COINUTILS_LIBRARY_RELEASE CLP_LIBRARY_RELEASE OSI_INCLUDE_DIRS CLP_INCLUDE_DIR CU_INCLUDE_DIR OSI_LINKER_FLAGS OSICLP_INCLUDE_DIR)

mark_as_advanced(OSI_INCLUDE_DIRS CLP_INCLUDE_DIR
					OSI_LIBRARY_RELEASE OSI_CLP_LIBRARY_RELEASE COINUTILS_LIBRARY_RELEASE CLP_LIBRARY_RELEASE CU_INCLUDE_DIR OSICLP_INCLUDE_DIR OSI_LINKER_FLAGS OSI_FOUND)
else()
find_package_handle_standard_args(OSI  DEFAULT_MSG
                                  OSI_LIBRARY_RELEASE OSI_CLP_LIBRARY_RELEASE COINUTILS_LIBRARY_RELEASE CLP_LIBRARY_RELEASE OSI_INCLUDE_DIRS CLP_INCLUDE_DIR CU_INCLUDE_DIR OSICLP_INCLUDE_DIR)

mark_as_advanced(OSI_INCLUDE_DIRS CLP_INCLUDE_DIR
					OSI_LIBRARY_RELEASE OSI_CLP_LIBRARY_RELEASE COINUTILS_LIBRARY_RELEASE CLP_LIBRARY_RELEASE CU_INCLUDE_DIR OSICLP_INCLUDE_DIR OSI_FOUND)
endif(NOT MSVC)
