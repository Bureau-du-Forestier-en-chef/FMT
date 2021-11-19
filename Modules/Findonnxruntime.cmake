#[[
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
]]
# This will define the following variables:
#   onnxruntime_FOUND        -- True if the system has the onnxruntime library
#   onnxruntime_INCLUDE_DIRS -- The include directories for onnxruntime
#   onnxruntime_LIBRARIES    -- Libraries to link againsts
#   onnxruntime_DLLs

include(FindPackageHandleStandardArgs)

FILE(GLOB_RECURSE ONNX_POTENTIALbin $ENV{ONNXR_DIR}/onnxruntime${CMAKE_STATIC_LIBRARY_SUFFIX})

foreach(includepath ${ONNX_POTENTIALbin})
	get_filename_component(potfolder ${includepath} DIRECTORY)
	string(TOLOWER "${includepath}" lowercase)
	if ("${lowercase}" MATCHES "debug")
		set(ONNXLIBDEBUG_DIR "${potfolder}")
	else()
		set(ONNXLIB_DIR "${potfolder}")
	endif("${lowercase}" MATCHES "debug")
endforeach()

#Needs to be for debug and release here!
get_filename_component(ONNXRBUILD_DIR ${ONNXLIB_DIR} DIRECTORY)

set(onnxruntime_INCLUDE_DIRS "$ENV{ONNXR_DIR}cmake/external/eigen;${ONNXRBUILD_DIR}/onnx;${ONNXRBUILD_DIR};$ENV{ONNXR_DIR}cmake/external/protobuf/src;$ENV{ONNXR_DIR}cmake/external/onnx;$ENV{ONNXR_DIR}cmake/external/SageInt;$ENV{ONNXR_DIR}/onnxruntime;$ENV{ONNXR_DIR}/include/onnxruntime/core/session;$ENV{ONNXR_DIR}/include/onnxruntime;$ENV{ONNXR_DIR}/include")



#look for libprotobuf
FILE(GLOB_RECURSE ONNX_POTENTIALprotobin ${ONNXRBUILD_DIR}/libprotobuf${CMAKE_STATIC_LIBRARY_SUFFIX})
list(GET ONNX_POTENTIALprotobin 0 FIRSTbin)
get_filename_component(PROTOLIB_DIR ${FIRSTbin} DIRECTORY)

#look for re2
FILE(GLOB_RECURSE ONNX_POTENTIALre2bin ${ONNXRBUILD_DIR}/re2${CMAKE_STATIC_LIBRARY_SUFFIX})
list(GET ONNX_POTENTIALre2bin 0 FIRSTbin)
get_filename_component(RE2LIB_DIR ${FIRSTbin} DIRECTORY)

#look for onnx and onnx_proto
FILE(GLOB_RECURSE ONNX_POTENTIALonnxproto ${ONNXRBUILD_DIR}/onnx${CMAKE_STATIC_LIBRARY_SUFFIX})
list(GET ONNX_POTENTIALonnxproto 0 FIRSTbin)
get_filename_component(onnxproto_DIR ${FIRSTbin} DIRECTORY)



find_library(onnxruntime_LIBRARY onnxruntime
    PATHS "${ONNXLIB_DIR}"
)

find_library(win_getopt_wide_LIBRARY win_getopt_wide
    PATHS "${ONNXLIB_DIR}"
)

find_library(onnxruntime_session_LIBRARY onnxruntime_session
    PATHS "${ONNXLIB_DIR}"
)

find_library(onnxruntime_optimizer_LIBRARY onnxruntime_optimizer
    PATHS "${ONNXLIB_DIR}"
)

find_library(onnxruntime_providers_LIBRARY onnxruntime_providers
    PATHS "${ONNXLIB_DIR}"
)
find_library(onnxruntime_util_LIBRARY onnxruntime_util
    PATHS "${ONNXLIB_DIR}"
)
find_library(onnxruntime_framework_LIBRARY onnxruntime_framework
    PATHS "${ONNXLIB_DIR}"
)
find_library(onnxruntime_graph_LIBRARY onnxruntime_graph
    PATHS "${ONNXLIB_DIR}"
)
find_library(onnxruntime_common_LIBRARY onnxruntime_common
    PATHS "${ONNXLIB_DIR}"
)
find_library(onnxruntime_mlas_LIBRARY onnxruntime_mlas
    PATHS "${ONNXLIB_DIR}"
)

find_library(PROTOLIB_LIBRARY libprotobuf-lite
    PATHS "${PROTOLIB_DIR}"
)



find_library(RE2_LIBRARY re2
    PATHS "${RE2LIB_DIR}"
)
find_library(ONNXP_LIBRARY onnx
    PATHS "${onnxproto_DIR}"
)
find_library(ONNXPROTO_LIBRARY onnx_proto
    PATHS "${onnxproto_DIR}"
)



set(onnxruntime_LIBRARIES "optimized;${onnxruntime_LIBRARY};optimized;${win_getopt_wide_LIBRARY};optimized;${onnxruntime_session_LIBRARY};optimized;${onnxruntime_optimizer_LIBRARY};optimized;${onnxruntime_providers_LIBRARY};optimized;${onnxruntime_util_LIBRARY};optimized;${onnxruntime_framework_LIBRARY};optimized;${onnxruntime_graph_LIBRARY};optimized;${onnxruntime_common_LIBRARY};optimized;${onnxruntime_mlas_LIBRARY};optimized;${PROTOLIB_LIBRARY};optimized;${RE2_LIBRARY};optimized;${ONNXP_LIBRARY};optimized;${ONNXPROTO_LIBRARY}")


if (DEFINED ONNXLIBDEBUG_DIR)#If got debug
	#Needs to be for debug and release here!
	get_filename_component(ONNXRDEBUGBUILD_DIR ${ONNXLIBDEBUG_DIR} DIRECTORY)
	#set (onnxdebuginclude "$ENV{ONNXR_DIR}cmake/external/eigen;${ONNXRDEBUGBUILD_DIR}/onnx;${ONNXRDEBUGBUILD_DIR};$ENV{ONNXR_DIR}cmake/external/protobuf/src;$ENV{ONNXR_DIR}cmake/external/onnx;$ENV{ONNXR_DIR}cmake/external/SageInt;$ENV{ONNXR_DIR}/onnxruntime;$ENV{ONNXR_DIR}/include/onnxruntime/core/session;$ENV{ONNXR_DIR}/include/onnxruntime;$ENV{ONNXR_DIR}/include")
	#list (APPEND onnxdebuginclude ${onnxruntime_INCLUDE_DIRS})
	FILE(GLOB_RECURSE onnxdebugruntime_DLLs ${ONNXLIBDEBUG_DIR}/*.dll)
	list (APPEND onnxruntime_DLLs ${onnxdebugruntime_DLLs})
	#look for libprotobuf
	FILE(GLOB_RECURSE ONNX_POTENTIALprotobin ${ONNXRDEBUGBUILD_DIR}/libprotobufd${CMAKE_STATIC_LIBRARY_SUFFIX})
	list(GET ONNX_POTENTIALprotobin 0 FIRSTbin)
	get_filename_component(PROTODEBUGLIB_DIR ${FIRSTbin} DIRECTORY)
	#look for re2
	FILE(GLOB_RECURSE ONNX_POTENTIALre2bin ${ONNXRDEBUGBUILD_DIR}/re2${CMAKE_STATIC_LIBRARY_SUFFIX})
	list(GET ONNX_POTENTIALre2bin 0 FIRSTbin)
	get_filename_component(RE2DEBUGLIB_DIR ${FIRSTbin} DIRECTORY)
	#look for onnx and onnx_proto
	FILE(GLOB_RECURSE ONNX_POTENTIALonnxproto ${ONNXRDEBUGBUILD_DIR}/onnx${CMAKE_STATIC_LIBRARY_SUFFIX})
	list(GET ONNX_POTENTIALonnxproto 0 FIRSTbin)
	get_filename_component(onnxprotoDEBUG_DIR ${FIRSTbin} DIRECTORY)
	find_library(onnxruntimedebug_LIBRARY onnxruntime
   	 PATHS "${ONNXLIBDEBUG_DIR}"
	)
	find_library(win_getopt_widedebug_LIBRARY win_getopt_wide
		PATHS "${ONNXLIBDEBUG_DIR}"
	)

	find_library(onnxruntime_sessiondebug_LIBRARY onnxruntime_session
		PATHS "${ONNXLIBDEBUG_DIR}"
	)

	find_library(onnxruntime_optimizerdebug_LIBRARY onnxruntime_optimizer
		PATHS "${ONNXLIBDEBUG_DIR}"
	)

	find_library(onnxruntime_providersdebug_LIBRARY onnxruntime_providers
		PATHS "${ONNXLIBDEBUG_DIR}"
	)
	find_library(onnxruntime_utildebug_LIBRARY onnxruntime_util
		PATHS "${ONNXLIBDEBUG_DIR}"
	)
	find_library(onnxruntime_frameworkdebug_LIBRARY onnxruntime_framework
		PATHS "${ONNXLIBDEBUG_DIR}"
	)
	find_library(onnxruntime_graphdebug_LIBRARY onnxruntime_graph
		PATHS "${ONNXLIBDEBUG_DIR}"
	)
	find_library(onnxruntime_commondebug_LIBRARY onnxruntime_common
		PATHS "${ONNXLIBDEBUG_DIR}"
	)
	find_library(onnxruntime_mlasdebug_LIBRARY onnxruntime_mlas
		PATHS "${ONNXLIBDEBUG_DIR}"
	)

	find_library(PROTOLIBdebug_LIBRARY libprotobuf-lited
		PATHS "${PROTODEBUGLIB_DIR}"
	)



	find_library(RE2debug_LIBRARY re2
		PATHS "${RE2DEBUGLIB_DIR}"
	)
	find_library(ONNXPdebug_LIBRARY onnx
		PATHS "${onnxprotoDEBUG_DIR}"
	)
	find_library(ONNXPROTOdebug_LIBRARY onnx_proto
		PATHS "${onnxprotoDEBUG_DIR}"
	)
	set(onnxruntime_DEBUGLIBRARIES "debug;${onnxruntimedebug_LIBRARY};debug;${win_getopt_widedebug_LIBRARY};debug;${onnxruntime_sessiondebug_LIBRARY};debug;${onnxruntime_optimizerdebug_LIBRARY};debug;${onnxruntime_providersdebug_LIBRARY};debug;${onnxruntime_utildebug_LIBRARY};debug;${onnxruntime_frameworkdebug_LIBRARY};debug;${onnxruntime_graphdebug_LIBRARY};debug;${onnxruntime_commondebug_LIBRARY};debug;${onnxruntime_mlasdebug_LIBRARY};debug;${PROTOLIBdebug_LIBRARY};debug;${RE2debug_LIBRARY};debug;${ONNXPdebug_LIBRARY};debug;${ONNXPROTOdebug_LIBRARY}")
	list (APPEND onnxruntime_LIBRARIES ${onnxruntime_DEBUGLIBRARIES})
	FILE(GLOB_RECURSE onnxruntime_DLLs ${ONNXLIBDEBUG_DIR}/*.dll)
else()
	FILE(GLOB_RECURSE onnxruntime_DLLs ${ONNXLIB_DIR}/*.dll)
endif(DEFINED ONNXLIBDEBUG_DIR)



find_package_handle_standard_args(onnxruntime DEFAULT_MSG onnxruntime_LIBRARIES onnxruntime_INCLUDE_DIRS onnxruntime_DLLs)

mark_as_advanced(onnxruntime_FOUND onnxruntime_LIBRARIES onnxruntime_INCLUDE_DIRS onnxruntime_CXX_FLAGS onnxruntime_DLLs)