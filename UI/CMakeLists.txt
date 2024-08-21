#[[
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
]]

cmake_minimum_required(VERSION 3.6)
set(WrapperINCLUDEDIR "${CMAKE_CURRENT_SOURCE_DIR}/UI/Include/")
set(WrapperSOURCEDIR "${CMAKE_CURRENT_SOURCE_DIR}/UI/Source/")
FILE(GLOB WrapperSOURCES ${WrapperSOURCEDIR}*.cpp)
FILE(GLOB WrapperHEADER ${WrapperINCLUDEDIR}*.h*)
if (MSVC AND WrapperSOURCES)
    add_library(FMTWrapper SHARED ${WrapperSOURCES} ${WrapperHEADER})

    # Inclure les en-têtes de FMTWrapperCore
    target_include_directories(FMTWrapper PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/FMTWrapperCore/Include")
    target_include_directories(FMTWrapper PRIVATE "${FMTINCLUDEDIR}")
    target_include_directories(FMTWrapper PRIVATE ${WrapperINCLUDEDIR})
    target_include_directories(FMTWrapper PRIVATE ${Boost_INCLUDE_DIRS})

    # Lier FMTWrapper à FMTWrapperCore
    target_link_libraries(FMTWrapper PRIVATE FMTWrapperCore)

    # Précompilation des en-têtes
    target_precompile_headers(FMTWrapper PRIVATE "${WrapperINCLUDEDIR}/stdafx.h")

	get_target_property(FMTliblocation FMTlib LIBRARY_OUTPUT_DIRECTORY)
	target_link_libraries(FMTWrapper PRIVATE "${FMTliblocation}/${CMAKE_SHARED_LIBRARY_PREFIX}${BUILD_TYPE}/FMTlib${CMAKE_STATIC_LIBRARY_SUFFIX}")
	target_precompile_headers(FMTWrapper PRIVATE "${WrapperINCLUDEDIR}/stdafx.h")
	target_link_libraries(FMTWrapper PRIVATE ${Boost_LIBRARIES})
	target_compile_definitions(FMTWrapper PRIVATE "-DFMTLIBIMPORT")
	set_property(TARGET FMTWrapper PROPERTY VS_DOTNET_REFERENCES
		"System"
    		"System.Data"
    		"System.Xml"
		)    
	set_target_properties(FMTWrapper PROPERTIES COMMON_LANGUAGE_RUNTIME "")    
	message("Generating build for FMTWrapper")
	set(udflocation "${CMAKE_CURRENT_SOURCE_DIR}/UI/userDefineLang.xml")
	list (APPEND FMTDEPENDENCIES ${udflocation})
	if (NOT WITHOUT_TESTS)
		FILE(GLOB FMTexecutables ${CMAKE_CURRENT_SOURCE_DIR}/UI/tests/*.cpp)
		foreach(pathtosource ${FMTexecutables})
			createexecutable("${pathtosource}" executablename)
			target_link_libraries(${executablename} PRIVATE FMTWrapper)
			target_include_directories(${executablename} PRIVATE ${WrapperINCLUDEDIR})
		endforeach()    
	endif(NOT WITHOUT_TESTS)    
else()    
	message("No files found in ${CMAKE_CURRENT_SOURCE_DIR}/UI/Source/ the wrapper wont be compiled headers need to be in UI/Include and sources in UI/Source")
endif(MSVC AND WrapperSOURCES)    







	



