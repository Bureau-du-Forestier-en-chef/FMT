#[[
Copyright (c) 2019 Gouvernement du Quebec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt

FindOSI.cmake

Behavior:
  * VCPKG_TOOLCHAIN: use the vcpkg installation and never call build-osi.sh.
  * Windows MinGW/RTools without vcpkg: use the private matched stack and run
    cmake/build-osi.sh during configuration only when that stack is incomplete.
  * Other configurations: preserve OSI_DIR-based discovery.

Legacy output variable names are preserved.
]]

include(FindPackageHandleStandardArgs)

# -----------------------------------------------------------------------------
# Helpers
# -----------------------------------------------------------------------------

function(_osi_find_header_dir output_var header_name)
    set(_result "")
    foreach(_root IN LISTS ARGN)
        if(NOT _root OR NOT IS_DIRECTORY "${_root}")
            continue()
        endif()

        file(GLOB_RECURSE _matches LIST_DIRECTORIES FALSE
            "${_root}/${header_name}")
        if(_matches)
            list(GET _matches 0 _first)
            get_filename_component(_result "${_first}" DIRECTORY)
            break()
        endif()
    endforeach()
    set(${output_var} "${_result}" PARENT_SCOPE)
endfunction()

function(_osi_debug_fallback release_var debug_var)
    if(DEFINED ${release_var}
       AND ${release_var}
       AND NOT "${${release_var}}" MATCHES "-NOTFOUND$")
        if(NOT DEFINED ${debug_var}
           OR NOT ${debug_var}
           OR "${${debug_var}}" MATCHES "-NOTFOUND$")
            set(${debug_var} "${${release_var}}" PARENT_SCOPE)
        endif()
    endif()
endfunction()

function(_osi_append_config_library list_var release_lib debug_lib)
    set(_list "${${list_var}}")
    set(_have_release FALSE)
    set(_have_debug FALSE)

    if(release_lib AND NOT "${release_lib}" MATCHES "-NOTFOUND$")
        set(_have_release TRUE)
    endif()

    if(debug_lib AND NOT "${debug_lib}" MATCHES "-NOTFOUND$")
        set(_have_debug TRUE)
    endif()

    if(_have_release AND _have_debug)
        if("${release_lib}" STREQUAL "${debug_lib}")
            # RTools uses the same static archive for all configurations.
            # Do not emit optimized/debug keywords around the same path.
            list(APPEND _list "${release_lib}")
        else()
            list(APPEND _list
                optimized "${release_lib}"
                debug "${debug_lib}")
        endif()
    elseif(_have_release)
        list(APPEND _list "${release_lib}")
    elseif(_have_debug)
        list(APPEND _list "${debug_lib}")
    endif()

    set(${list_var} "${_list}" PARENT_SCOPE)
endfunction()

# -----------------------------------------------------------------------------
# Configuration
# -----------------------------------------------------------------------------

set(OSI_LINKER_FLAGS "")

get_filename_component(OSI_PROJECT_ROOT "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

set(OSI_BUILD_SCRIPT
    "${OSI_PROJECT_ROOT}/cmake/build-coin-stack.sh"
    CACHE FILEPATH
    "Script used to build the matched OSI stack with RTools")

set(OSI_RTOOLS_ROOT
    ""
    CACHE PATH
    "Optional RTools installation root; empty enables automatic detection")

set(OSI_RTOOLS_BASH
    ""
    CACHE FILEPATH
    "Optional RTools Bash executable; empty enables automatic detection")

set(OSI_RTOOLS_TOOLCHAIN_ROOT
    ""
    CACHE PATH
    "Optional RTools target toolchain root; empty enables automatic detection")

function(_osi_detect_rtools)
    set(_detected_root "")
    set(_detected_toolchain "")
    set(_detected_bash "")

    # 1. Explicit cache override.
    if(OSI_RTOOLS_ROOT)
        get_filename_component(_detected_root "${OSI_RTOOLS_ROOT}" ABSOLUTE)
    endif()

    # 2. Standard environment variables.
    if(NOT _detected_root AND DEFINED ENV{RTOOLS45_HOME})
        file(TO_CMAKE_PATH "$ENV{RTOOLS45_HOME}" _detected_root)
    endif()
    if(NOT _detected_root AND DEFINED ENV{RTOOLS_HOME})
        file(TO_CMAKE_PATH "$ENV{RTOOLS_HOME}" _detected_root)
    endif()

    # 3. Derive the installation from the active C++ compiler.
    if(NOT _detected_root AND CMAKE_CXX_COMPILER)
        get_filename_component(_compiler_realpath "${CMAKE_CXX_COMPILER}" REALPATH)
        get_filename_component(_compiler_bin_dir "${_compiler_realpath}" DIRECTORY)
        get_filename_component(_compiler_toolchain_dir "${_compiler_bin_dir}" DIRECTORY)
        get_filename_component(_compiler_rtools_dir "${_compiler_toolchain_dir}" DIRECTORY)
        get_filename_component(_toolchain_dir_name "${_compiler_toolchain_dir}" NAME)

        if(_toolchain_dir_name MATCHES "^(x86_64|aarch64)-w64-mingw32.*"
           AND EXISTS "${_compiler_rtools_dir}/usr/bin/bash.exe")
            set(_detected_root "${_compiler_rtools_dir}")
            set(_detected_toolchain "${_compiler_toolchain_dir}")
        endif()
    endif()

    if(NOT _detected_root)
        message(FATAL_ERROR
            "FindOSI could not detect RTools. Active C++ compiler: "
            "${CMAKE_CXX_COMPILER}. Set OSI_RTOOLS_ROOT, RTOOLS45_HOME, "
            "or RTOOLS_HOME.")
    endif()

    cmake_path(NORMAL_PATH _detected_root OUTPUT_VARIABLE _detected_root)

    # Detect the target toolchain directory.
    if(OSI_RTOOLS_TOOLCHAIN_ROOT)
        get_filename_component(
            _detected_toolchain "${OSI_RTOOLS_TOOLCHAIN_ROOT}" ABSOLUTE)
    endif()

    if(NOT _detected_toolchain)
        foreach(_candidate
            "${_detected_root}/x86_64-w64-mingw32.static.posix"
            "${_detected_root}/aarch64-w64-mingw32.static.posix")
            if(EXISTS "${_candidate}/bin/g++.exe"
               OR EXISTS "${_candidate}/bin/clang++.exe")
                set(_detected_toolchain "${_candidate}")
                break()
            endif()
        endforeach()
    endif()

    if(NOT _detected_toolchain)
        message(FATAL_ERROR
            "FindOSI found RTools at ${_detected_root}, but no supported "
            "target toolchain directory was found.")
    endif()

    cmake_path(NORMAL_PATH _detected_toolchain
        OUTPUT_VARIABLE _detected_toolchain)

    # Detect Bash.
    if(OSI_RTOOLS_BASH)
        get_filename_component(_detected_bash "${OSI_RTOOLS_BASH}" ABSOLUTE)
    else()
        set(_detected_bash "${_detected_root}/usr/bin/bash.exe")
    endif()

    if(NOT EXISTS "${_detected_bash}")
        message(FATAL_ERROR
            "FindOSI could not locate RTools Bash at ${_detected_bash}. "
            "Set OSI_RTOOLS_BASH explicitly.")
    endif()

    cmake_path(NORMAL_PATH _detected_bash OUTPUT_VARIABLE _detected_bash)

    set(OSI_RTOOLS_ROOT "${_detected_root}" CACHE PATH
        "Detected RTools installation root" FORCE)
    set(OSI_RTOOLS_TOOLCHAIN_ROOT "${_detected_toolchain}" CACHE PATH
        "Detected RTools target toolchain root" FORCE)
    set(OSI_RTOOLS_BASH "${_detected_bash}" CACHE FILEPATH
        "Detected RTools Bash executable" FORCE)

    set(OSI_RTOOLS_ROOT "${_detected_root}" PARENT_SCOPE)
    set(OSI_RTOOLS_TOOLCHAIN_ROOT "${_detected_toolchain}" PARENT_SCOPE)
    set(OSI_RTOOLS_BASH "${_detected_bash}" PARENT_SCOPE)
endfunction()

set(OSI_BUILD_ROOT
    "${CMAKE_BINARY_DIR}/coin-stack"
    CACHE PATH
    "Working directory for the matched COIN-OR stack")

set(OSI_PRIVATE_PREFIX
    "${OSI_BUILD_ROOT}/install"
    CACHE PATH
    "Install prefix for the matched COIN-OR stack")

set(OSI_BUILD_JOBS
    "8"
    CACHE STRING
    "Parallel jobs used by build-coin-stack.sh")

set(OSI_WITH_MOSEK FALSE)
if(DEFINED ENV{Mosek_DIR} AND NOT "$ENV{Mosek_DIR}" STREQUAL "")
    set(OSI_WITH_MOSEK TRUE)
endif()

set(OSI_USING_VCPKG FALSE)
set(OSI_USING_RTOOLS_BUILD FALSE)

if(VCPKG_TOOLCHAIN)
    set(OSI_USING_VCPKG TRUE)
elseif(WIN32 AND MINGW)
    set(OSI_USING_RTOOLS_BUILD TRUE)
    _osi_detect_rtools()

    message(STATUS "FindOSI: detected RTools root: ${OSI_RTOOLS_ROOT}")
    message(STATUS
        "FindOSI: detected RTools toolchain: ${OSI_RTOOLS_TOOLCHAIN_ROOT}")
    message(STATUS "FindOSI: detected RTools Bash: ${OSI_RTOOLS_BASH}")
    message(STATUS "FindOSI: active C++ compiler: ${CMAKE_CXX_COMPILER}")
endif()

# -----------------------------------------------------------------------------
# Select roots and optionally build the private RTools stack
# -----------------------------------------------------------------------------

if(OSI_USING_VCPKG)
    message(STATUS "FindOSI: using vcpkg; custom OSI build is disabled")

    if(NOT VCPKG_INSTALLED_DIR OR NOT VCPKG_TARGET_TRIPLET)
        message(FATAL_ERROR
            "VCPKG_TOOLCHAIN is set, but VCPKG_INSTALLED_DIR or "
            "VCPKG_TARGET_TRIPLET is missing")
    endif()

    set(OSI_SEARCH_ROOT "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}")
    set(POTOSI_LIB_DIR "${OSI_SEARCH_ROOT}/lib")
    set(POTOSI_LIB_DEBUG_DIR "${OSI_SEARCH_ROOT}/debug/lib")
    set(OSI_EXTERNAL_LIB_DIR "${POTOSI_LIB_DIR}")
    set(OSI_EXTERNAL_LIB_DEBUG_DIR "${POTOSI_LIB_DEBUG_DIR}")
    set(OSI_INCLUDE_SEARCH_ROOTS "${OSI_SEARCH_ROOT}/include")

elseif(OSI_USING_RTOOLS_BUILD)
    message(STATUS "FindOSI: using Windows RTools/MinGW workflow")
    message(STATUS "FindOSI: private prefix is ${OSI_PRIVATE_PREFIX}")

    set(_osi_required_files
        "${OSI_PRIVATE_PREFIX}/lib/libCoinUtils.a"
        "${OSI_PRIVATE_PREFIX}/lib/libOsi.a"
        "${OSI_PRIVATE_PREFIX}/lib/libOsiClp.a"
        "${OSI_PRIVATE_PREFIX}/lib/libOsiGlpk.a"
        "${OSI_PRIVATE_PREFIX}/lib/libClp.a")
    if(OSI_WITH_MOSEK)
        list(APPEND _osi_required_files
            "${OSI_PRIVATE_PREFIX}/lib/libOsiMsk.a")
    endif()

    set(_osi_stack_complete TRUE)
    foreach(_required IN LISTS _osi_required_files)
        if(NOT EXISTS "${_required}")
            set(_osi_stack_complete FALSE)
            message(STATUS "FindOSI: missing ${_required}")
        endif()
    endforeach()

    if(NOT EXISTS "${OSI_PRIVATE_PREFIX}/include/coin-or/OsiSolverInterface.hpp"
       AND NOT EXISTS "${OSI_PRIVATE_PREFIX}/include/coin/OsiSolverInterface.hpp")
        set(_osi_stack_complete FALSE)
        message(STATUS "FindOSI: missing private OsiSolverInterface.hpp")
    endif()

    # Rebuild when the installed feature mode differs from ENV{Mosek_DIR}.
    # This prevents a stack built with OsiMsk from being silently reused when
    # MOSEK is disabled, and vice versa.
    set(_osi_expected_mosek 0)
    if(OSI_WITH_MOSEK)
        set(_osi_expected_mosek 1)
    endif()
    set(_osi_info_file "${OSI_PRIVATE_PREFIX}/.coin-stack-info")
    if(EXISTS "${_osi_info_file}")
        file(STRINGS "${_osi_info_file}" _osi_mosek_info
            REGEX "^WithMosek=[01]$")
        if(_osi_mosek_info)
            list(GET _osi_mosek_info 0 _osi_mosek_line)
            string(REPLACE "WithMosek=" "" _osi_installed_mosek
                "${_osi_mosek_line}")
            if(NOT "${_osi_installed_mosek}" STREQUAL
                   "${_osi_expected_mosek}")
                set(_osi_stack_complete FALSE)
                message(STATUS
                    "FindOSI: MOSEK feature changed; private stack will rebuild")
            endif()
        else()
            set(_osi_stack_complete FALSE)
            message(STATUS
                "FindOSI: private stack metadata lacks WithMosek; rebuilding")
        endif()
    elseif(_osi_stack_complete)
        set(_osi_stack_complete FALSE)
        message(STATUS "FindOSI: private stack metadata missing; rebuilding")
    endif()

    if(NOT _osi_stack_complete)
        if(NOT EXISTS "${OSI_BUILD_SCRIPT}")
            message(FATAL_ERROR "FindOSI: build script not found: ${OSI_BUILD_SCRIPT}")
        endif()
        if(NOT EXISTS "${OSI_RTOOLS_BASH}")
            message(FATAL_ERROR "FindOSI: RTools bash not found: ${OSI_RTOOLS_BASH}")
        endif()

        file(MAKE_DIRECTORY "${OSI_BUILD_ROOT}")

        foreach(_pair
            "ROOT;${OSI_BUILD_ROOT}"
            "PREFIX;${OSI_PRIVATE_PREFIX}"
            "SCRIPT;${OSI_BUILD_SCRIPT}")
            list(GET _pair 0 _name)
            list(GET _pair 1 _windows_path)
            execute_process(
                COMMAND "${OSI_RTOOLS_BASH}" -lc "cygpath -u \"${_windows_path}\""
                OUTPUT_VARIABLE _msys_path
                OUTPUT_STRIP_TRAILING_WHITESPACE
                RESULT_VARIABLE _cygpath_result
                ERROR_VARIABLE _cygpath_error)
            if(NOT _cygpath_result EQUAL 0)
                message(FATAL_ERROR
                    "FindOSI: cygpath failed for ${_windows_path}: ${_cygpath_error}")
            endif()
            set(OSI_${_name}_MSYS "${_msys_path}")
        endforeach()

        execute_process(
            COMMAND "${OSI_RTOOLS_BASH}" -lc
                "cygpath -u \"${OSI_RTOOLS_TOOLCHAIN_ROOT}\""
            OUTPUT_VARIABLE OSI_RTOOLS_TOOLCHAIN_ROOT_MSYS
            OUTPUT_STRIP_TRAILING_WHITESPACE
            RESULT_VARIABLE _toolchain_cygpath_result
            ERROR_VARIABLE _toolchain_cygpath_error)
        if(NOT _toolchain_cygpath_result EQUAL 0)
            message(FATAL_ERROR
                "FindOSI: cygpath failed for the RTools toolchain: "
                "${_toolchain_cygpath_error}")
        endif()

        set(_osi_env
            "ROOT=${OSI_ROOT_MSYS}"
            "PREFIX=${OSI_PREFIX_MSYS}"
            "RTOOLS_ROOT=${OSI_RTOOLS_TOOLCHAIN_ROOT_MSYS}"
            "JOBS=${OSI_BUILD_JOBS}")
        if(OSI_WITH_MOSEK)
            execute_process(
                COMMAND "${OSI_RTOOLS_BASH}" -lc
                    "cygpath -u \"$ENV{Mosek_DIR}\""
                OUTPUT_VARIABLE _osi_mosek_root_msys
                OUTPUT_STRIP_TRAILING_WHITESPACE
                RESULT_VARIABLE _osi_mosek_cygpath_result
                ERROR_VARIABLE _osi_mosek_cygpath_error)
            if(NOT _osi_mosek_cygpath_result EQUAL 0)
                message(FATAL_ERROR
                    "FindOSI: could not convert ENV{Mosek_DIR}: "
                    "${_osi_mosek_cygpath_error}")
            endif()
            list(APPEND _osi_env "MOSEK_ROOT=${_osi_mosek_root_msys}")
            message(STATUS
                "FindOSI: MOSEK enabled from ENV{Mosek_DIR}=$ENV{Mosek_DIR}")
        else()
            message(STATUS
                "FindOSI: MOSEK disabled because ENV{Mosek_DIR} is not set")
        endif()

        message(STATUS "FindOSI: building the matched private COIN-OR stack")
        execute_process(
            COMMAND "${CMAKE_COMMAND}" -E env ${_osi_env}
                    "${OSI_RTOOLS_BASH}" "${OSI_SCRIPT_MSYS}"
            WORKING_DIRECTORY "${OSI_BUILD_ROOT}"
            RESULT_VARIABLE _osi_build_result
            COMMAND_ECHO STDOUT)

        if(NOT _osi_build_result EQUAL 0)
            message(FATAL_ERROR
                "FindOSI: build-coin-stack.sh failed with exit code ${_osi_build_result}")
        endif()

        foreach(_required IN LISTS _osi_required_files)
            if(NOT EXISTS "${_required}")
                message(FATAL_ERROR
                    "FindOSI: build succeeded but required file is missing: ${_required}")
            endif()
        endforeach()
    else()
        message(STATUS "FindOSI: complete private COIN-OR stack already exists")
    endif()

    set(OSI_SEARCH_ROOT "${OSI_PRIVATE_PREFIX}")
    set(POTOSI_LIB_DIR "${OSI_PRIVATE_PREFIX}/lib")
    set(POTOSI_LIB_DEBUG_DIR "${OSI_PRIVATE_PREFIX}/lib")
    set(OSI_EXTERNAL_LIB_DIR "${OSI_RTOOLS_TOOLCHAIN_ROOT}/lib")
    set(OSI_EXTERNAL_LIB_DEBUG_DIR "${OSI_RTOOLS_TOOLCHAIN_ROOT}/lib")
    set(OSI_INCLUDE_SEARCH_ROOTS
        "${OSI_PRIVATE_PREFIX}/include"
        "${OSI_RTOOLS_TOOLCHAIN_ROOT}/include")

    # Preserve legacy behavior for callers that inspect OSI_DIR.
    set(ENV{OSI_DIR} "${OSI_PRIVATE_PREFIX}")

else()
    message(STATUS "FindOSI: using legacy OSI_DIR discovery")

    set(OSI_SEARCH_ROOT "$ENV{OSI_DIR}")
    set(POTOSI_LIB_DIR "")
    set(POTOSI_LIB_DEBUG_DIR "")

    if(OSI_SEARCH_ROOT AND IS_DIRECTORY "${OSI_SEARCH_ROOT}")
        file(GLOB_RECURSE OSI_POTENTIAL_LIB LIST_DIRECTORIES FALSE
            "${OSI_SEARCH_ROOT}/*${CMAKE_STATIC_LIBRARY_SUFFIX}")
        foreach(_full_path IN LISTS OSI_POTENTIAL_LIB)
            get_filename_component(_lib_dir "${_full_path}" DIRECTORY)
            string(TOLOWER "${_full_path}" _lower)
            if(_lower MATCHES "debug")
                list(APPEND POTOSI_LIB_DEBUG_DIR "${_lib_dir}")
            else()
                list(APPEND POTOSI_LIB_DIR "${_lib_dir}")
            endif()
        endforeach()
        list(REMOVE_DUPLICATES POTOSI_LIB_DIR)
        list(REMOVE_DUPLICATES POTOSI_LIB_DEBUG_DIR)
    endif()

    set(OSI_EXTERNAL_LIB_DIR ${POTOSI_LIB_DIR})
    set(OSI_EXTERNAL_LIB_DEBUG_DIR ${POTOSI_LIB_DEBUG_DIR})
    set(OSI_INCLUDE_SEARCH_ROOTS "${OSI_SEARCH_ROOT}")
endif()

# -----------------------------------------------------------------------------
# Headers
# -----------------------------------------------------------------------------

_osi_find_header_dir(POTOSI_INCLUDE_DIR "OsiConfig.h" ${OSI_INCLUDE_SEARCH_ROOTS})
find_path(OSI_INCLUDE_DIR NAMES OsiConfig.h
    PATHS "${POTOSI_INCLUDE_DIR}" ${OSI_INCLUDE_SEARCH_ROOTS}
    PATH_SUFFIXES coin-or coin NO_DEFAULT_PATH)

_osi_find_header_dir(POTCLP_INCLUDE_DIR "ClpSimplex.hpp" ${OSI_INCLUDE_SEARCH_ROOTS})
find_path(CLP_INCLUDE_DIR NAMES ClpSimplex.hpp
    PATHS "${POTCLP_INCLUDE_DIR}" ${OSI_INCLUDE_SEARCH_ROOTS}
    PATH_SUFFIXES coin-or coin NO_DEFAULT_PATH)

_osi_find_header_dir(POTGLPK_INCLUDE_DIR "glpk.h" ${OSI_INCLUDE_SEARCH_ROOTS})
find_path(GLPK_INCLUDE_DIR NAMES glpk.h
    PATHS "${POTGLPK_INCLUDE_DIR}" ${OSI_INCLUDE_SEARCH_ROOTS}
          "${OSI_RTOOLS_TOOLCHAIN_ROOT}/include"
    NO_DEFAULT_PATH)

_osi_find_header_dir(POTCU_INCLUDE_DIR "CoinUtility.hpp" ${OSI_INCLUDE_SEARCH_ROOTS})
find_path(CU_INCLUDE_DIR NAMES CoinUtility.hpp
    PATHS "${POTCU_INCLUDE_DIR}" ${OSI_INCLUDE_SEARCH_ROOTS}
    PATH_SUFFIXES coin-or coin NO_DEFAULT_PATH)

_osi_find_header_dir(OSICLP_INCLUDE_DIR "OsiClpSolverInterface.hpp"
    ${OSI_INCLUDE_SEARCH_ROOTS})
_osi_find_header_dir(OSIGLPK_INCLUDE_DIR "OsiGlpkSolverInterface.hpp"
    ${OSI_INCLUDE_SEARCH_ROOTS})
_osi_find_header_dir(OSIMSK_INCLUDE_DIR "OsiMskSolverInterface.hpp"
    ${OSI_INCLUDE_SEARCH_ROOTS})

# -----------------------------------------------------------------------------
# Libraries
# -----------------------------------------------------------------------------

find_library(COINUTILS_LIBRARY_RELEASE
    NAMES CoinUtils libCoinUtils libCoinUtils.a
    PATHS ${POTOSI_LIB_DIR} NO_DEFAULT_PATH)
find_library(OSI_LIBRARY_RELEASE
    NAMES Osi libOsi libOsi.a
    PATHS ${POTOSI_LIB_DIR} NO_DEFAULT_PATH)
find_library(OSI_CLP_LIBRARY_RELEASE
    NAMES OsiClp libOsiClp libOsiClp.a
    PATHS ${POTOSI_LIB_DIR} NO_DEFAULT_PATH)
find_library(OSI_GLPK_LIBRARY_RELEASE
    NAMES OsiGlpk libOsiGlpk libOsiGlpk.a
    PATHS ${POTOSI_LIB_DIR} NO_DEFAULT_PATH)
unset(OSI_MSK_LIBRARY_RELEASE CACHE)
unset(OSI_MSK_LIBRARY_RELEASE)
if(OSI_USING_VCPKG OR OSI_WITH_MOSEK OR (NOT OSI_USING_RTOOLS_BUILD AND DEFINED ENV{Mosek_DIR}))
    find_library(OSI_MSK_LIBRARY_RELEASE
        NAMES OsiMsk libOsiMsk libOsiMsk.a
        PATHS ${POTOSI_LIB_DIR} NO_DEFAULT_PATH)
endif()
find_library(CLP_LIBRARY_RELEASE
    NAMES Clp libClp libClp.a libclp
    PATHS ${POTOSI_LIB_DIR} NO_DEFAULT_PATH)
find_library(CLP_SOLVER_LIBRARY_RELEASE
    NAMES ClpSolver libClpSolver libClpSolver.a
    PATHS ${POTOSI_LIB_DIR} NO_DEFAULT_PATH)
find_library(GLPK_LIBRARY_RELEASE
    NAMES glpk libglpk libglpk.a libGlpk
    PATHS ${POTOSI_LIB_DIR} ${OSI_EXTERNAL_LIB_DIR} NO_DEFAULT_PATH)
find_library(GLPK_SOLVER_LIBRARY_RELEASE
    NAMES GlpkSolver libGlpkSolver libGlpkSolver.a
    PATHS ${POTOSI_LIB_DIR} ${OSI_EXTERNAL_LIB_DIR} NO_DEFAULT_PATH)

find_library(COINUTILS_LIBRARY_DEBUG
    NAMES CoinUtils libCoinUtils libCoinUtils.a
    PATHS ${POTOSI_LIB_DEBUG_DIR} NO_DEFAULT_PATH)
find_library(OSI_LIBRARY_DEBUG
    NAMES Osi libOsi libOsi.a
    PATHS ${POTOSI_LIB_DEBUG_DIR} NO_DEFAULT_PATH)
find_library(OSI_CLP_LIBRARY_DEBUG
    NAMES OsiClp libOsiClp libOsiClp.a
    PATHS ${POTOSI_LIB_DEBUG_DIR} NO_DEFAULT_PATH)
find_library(OSI_GLPK_LIBRARY_DEBUG
    NAMES OsiGlpk libOsiGlpk libOsiGlpk.a
    PATHS ${POTOSI_LIB_DEBUG_DIR} NO_DEFAULT_PATH)
unset(OSI_MSK_LIBRARY_DEBUG CACHE)
unset(OSI_MSK_LIBRARY_DEBUG)
if(OSI_USING_VCPKG OR OSI_WITH_MOSEK OR (NOT OSI_USING_RTOOLS_BUILD AND DEFINED ENV{Mosek_DIR}))
    find_library(OSI_MSK_LIBRARY_DEBUG
        NAMES OsiMsk libOsiMsk libOsiMsk.a
        PATHS ${POTOSI_LIB_DEBUG_DIR} NO_DEFAULT_PATH)
endif()
find_library(CLP_LIBRARY_DEBUG
    NAMES Clp libClp libClp.a libclp
    PATHS ${POTOSI_LIB_DEBUG_DIR} NO_DEFAULT_PATH)
find_library(CLP_SOLVER_LIBRARY_DEBUG
    NAMES ClpSolver libClpSolver libClpSolver.a
    PATHS ${POTOSI_LIB_DEBUG_DIR} NO_DEFAULT_PATH)
find_library(GLPK_LIBRARY_DEBUG
    NAMES glpk libglpk libglpk.a libGlpk
    PATHS ${POTOSI_LIB_DEBUG_DIR} ${OSI_EXTERNAL_LIB_DEBUG_DIR}
    NO_DEFAULT_PATH)
find_library(GLPK_SOLVER_LIBRARY_DEBUG
    NAMES GlpkSolver libGlpkSolver libGlpkSolver.a
    PATHS ${POTOSI_LIB_DEBUG_DIR} ${OSI_EXTERNAL_LIB_DEBUG_DIR}
    NO_DEFAULT_PATH)

# Static RTools builds do not produce separate Debug archives.
if(OSI_USING_RTOOLS_BUILD)
    _osi_debug_fallback(COINUTILS_LIBRARY_RELEASE COINUTILS_LIBRARY_DEBUG)
    _osi_debug_fallback(OSI_LIBRARY_RELEASE OSI_LIBRARY_DEBUG)
    _osi_debug_fallback(OSI_CLP_LIBRARY_RELEASE OSI_CLP_LIBRARY_DEBUG)
    _osi_debug_fallback(OSI_GLPK_LIBRARY_RELEASE OSI_GLPK_LIBRARY_DEBUG)
    _osi_debug_fallback(OSI_MSK_LIBRARY_RELEASE OSI_MSK_LIBRARY_DEBUG)
    _osi_debug_fallback(CLP_LIBRARY_RELEASE CLP_LIBRARY_DEBUG)
    _osi_debug_fallback(CLP_SOLVER_LIBRARY_RELEASE CLP_SOLVER_LIBRARY_DEBUG)
    _osi_debug_fallback(GLPK_LIBRARY_RELEASE GLPK_LIBRARY_DEBUG)
    _osi_debug_fallback(GLPK_SOLVER_LIBRARY_RELEASE GLPK_SOLVER_LIBRARY_DEBUG)
endif()

# -----------------------------------------------------------------------------
# Linker flags for non-MSVC static dependencies
# -----------------------------------------------------------------------------

if(NOT MSVC)
    find_library(OSI_LAPACK NAMES lapack liblapack.a
        PATHS ${OSI_EXTERNAL_LIB_DIR} ${POTOSI_LIB_DIR})
    if(OSI_LAPACK)
        list(APPEND OSI_LINKER_FLAGS "-llapack")
        if(NOT CYGWIN)
            list(APPEND OSI_LINKER_FLAGS "-lgfortran" "-lquadmath")
        endif()
        list(APPEND OSI_LINKER_FLAGS "-lblas")
    endif()

    find_library(OSI_LIBZ NAMES z libz.a
        PATHS ${OSI_EXTERNAL_LIB_DIR} ${POTOSI_LIB_DIR})
    if(OSI_LIBZ)
        list(APPEND OSI_LINKER_FLAGS "-lz")
    endif()

    find_library(OSI_BZ2 NAMES bz2 libbz2.a
        PATHS ${OSI_EXTERNAL_LIB_DIR} ${POTOSI_LIB_DIR})
    if(OSI_BZ2)
        list(APPEND OSI_LINKER_FLAGS "-lbz2")
    endif()
endif()

# -----------------------------------------------------------------------------
# Preserve legacy aggregate variables
# -----------------------------------------------------------------------------

set(OSI_INCLUDE_DIRS "")
foreach(_include_dir
    "${OSI_INCLUDE_DIR}"
    "${CLP_INCLUDE_DIR}"
    "${CU_INCLUDE_DIR}"
    "${OSICLP_INCLUDE_DIR}"
    "${OSIGLPK_INCLUDE_DIR}"
    "${OSIMSK_INCLUDE_DIR}"
    "${GLPK_INCLUDE_DIR}")
    if(_include_dir)
        list(APPEND OSI_INCLUDE_DIRS "${_include_dir}")
    endif()
endforeach()
list(REMOVE_DUPLICATES OSI_INCLUDE_DIRS)

set(OSI_LIBRARIES "")

# GNU ld scans static archives from left to right. Keep every adapter before
# the implementation libraries that satisfy its unresolved symbols.
_osi_append_config_library(OSI_LIBRARIES
    "${OSI_CLP_LIBRARY_RELEASE}" "${OSI_CLP_LIBRARY_DEBUG}")
_osi_append_config_library(OSI_LIBRARIES
    "${OSI_GLPK_LIBRARY_RELEASE}" "${OSI_GLPK_LIBRARY_DEBUG}")
_osi_append_config_library(OSI_LIBRARIES
    "${OSI_MSK_LIBRARY_RELEASE}" "${OSI_MSK_LIBRARY_DEBUG}")

# Solver implementations follow their OSI adapters.
_osi_append_config_library(OSI_LIBRARIES
    "${CLP_SOLVER_LIBRARY_RELEASE}" "${CLP_SOLVER_LIBRARY_DEBUG}")
_osi_append_config_library(OSI_LIBRARIES
    "${CLP_LIBRARY_RELEASE}" "${CLP_LIBRARY_DEBUG}")

# Base OSI and CoinUtils satisfy references from the adapters and Clp.
_osi_append_config_library(OSI_LIBRARIES
    "${OSI_LIBRARY_RELEASE}" "${OSI_LIBRARY_DEBUG}")
_osi_append_config_library(OSI_LIBRARIES
    "${COINUTILS_LIBRARY_RELEASE}" "${COINUTILS_LIBRARY_DEBUG}")

# GLPK must follow both OsiGlpk and CoinUtils, because both static archives can
# reference glp_* symbols. An optional GlpkSolver archive follows afterward.
_osi_append_config_library(OSI_LIBRARIES
    "${GLPK_LIBRARY_RELEASE}" "${GLPK_LIBRARY_DEBUG}")
_osi_append_config_library(OSI_LIBRARIES
    "${GLPK_SOLVER_LIBRARY_RELEASE}" "${GLPK_SOLVER_LIBRARY_DEBUG}")

# Never remove duplicates from OSI_LIBRARIES. It can contain CMake's
# optimized/debug keywords, and repeated archives can be significant to GNU ld.
if(OSI_LINKER_FLAGS)
    list(REMOVE_DUPLICATES OSI_LINKER_FLAGS)
endif()

# -----------------------------------------------------------------------------
# Link diagnostics
# -----------------------------------------------------------------------------

message(STATUS "FindOSI: Osi library: ${OSI_LIBRARY_RELEASE}")
message(STATUS "FindOSI: OsiClp library: ${OSI_CLP_LIBRARY_RELEASE}")
message(STATUS "FindOSI: OsiGlpk library: ${OSI_GLPK_LIBRARY_RELEASE}")
message(STATUS "FindOSI: MOSEK requested: ${OSI_WITH_MOSEK}")
message(STATUS "FindOSI: OsiMsk library: ${OSI_MSK_LIBRARY_RELEASE}")
message(STATUS "FindOSI: CoinUtils library: ${COINUTILS_LIBRARY_RELEASE}")
message(STATUS "FindOSI: Clp library: ${CLP_LIBRARY_RELEASE}")
message(STATUS "FindOSI: GLPK library: ${GLPK_LIBRARY_RELEASE}")
message(STATUS "FindOSI: ordered libraries: ${OSI_LIBRARIES}")

# -----------------------------------------------------------------------------
# Package result
# -----------------------------------------------------------------------------

find_package_handle_standard_args(OSI DEFAULT_MSG
    OSI_LIBRARY_RELEASE
    OSI_CLP_LIBRARY_RELEASE
    COINUTILS_LIBRARY_RELEASE
    CLP_LIBRARY_RELEASE
    OSI_INCLUDE_DIRS
    CLP_INCLUDE_DIR
    CU_INCLUDE_DIR)

# Preserve original optional behavior. GLPK and MOSEK adapters are included in
# OSI_LIBRARIES when found, but they are not mandatory for OSI_FOUND outside the
# private RTools stack. In RTools mode the pre-build completeness check requires
# them before discovery begins.

mark_as_advanced(
    OSI_BUILD_SCRIPT
    OSI_RTOOLS_ROOT
    OSI_RTOOLS_BASH
    OSI_RTOOLS_TOOLCHAIN_ROOT
    OSI_BUILD_ROOT
    OSI_PRIVATE_PREFIX
    OSI_BUILD_JOBS
    OSI_WITH_MOSEK
    OSI_INCLUDE_DIR
    OSI_INCLUDE_DIRS
    CLP_INCLUDE_DIR
    GLPK_INCLUDE_DIR
    CU_INCLUDE_DIR
    OSICLP_INCLUDE_DIR
    OSIGLPK_INCLUDE_DIR
    OSIMSK_INCLUDE_DIR
    OSI_LIBRARY_RELEASE
    OSI_LIBRARY_DEBUG
    OSI_CLP_LIBRARY_RELEASE
    OSI_CLP_LIBRARY_DEBUG
    OSI_GLPK_LIBRARY_RELEASE
    OSI_GLPK_LIBRARY_DEBUG
    OSI_MSK_LIBRARY_RELEASE
    OSI_MSK_LIBRARY_DEBUG
    COINUTILS_LIBRARY_RELEASE
    COINUTILS_LIBRARY_DEBUG
    CLP_LIBRARY_RELEASE
    CLP_LIBRARY_DEBUG
    CLP_SOLVER_LIBRARY_RELEASE
    CLP_SOLVER_LIBRARY_DEBUG
    GLPK_LIBRARY_RELEASE
    GLPK_LIBRARY_DEBUG
    GLPK_SOLVER_LIBRARY_RELEASE
    GLPK_SOLVER_LIBRARY_DEBUG
    OSI_LAPACK
    OSI_LIBZ
    OSI_BZ2
    OSI_LINKER_FLAGS
    OSI_FOUND)
