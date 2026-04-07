function(SetBinaryLocation LibName)
  if (NOT MSVC)
		set_target_properties( ${LibName}
			PROPERTIES
			ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}"
			LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}"
			RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}"
		)
	elseif(NOT DEFINED WITHOUT_PYTHONLIB)
		set_target_properties( ${LibName}
			PROPERTIES
			ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
			LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
			RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
		)
	endif(NOT MSVC)
endfunction(SetBinaryLocation LibName)

function(target_lib libtotarget)
	string(STRIP "${libtotarget}" libtotarget)
		target_link_libraries(FMTlib PRIVATE ${libtotarget})
		if (NOT MSVC AND DEFINED ENV{R_DIR})
			target_link_libraries(FMT PRIVATE ${libtotarget})
		endif(NOT MSVC AND DEFINED ENV{R_DIR})
endfunction(target_lib libtotarget)

function(target_include includetotarget)
	target_include_directories(FMTlib PRIVATE ${includetotarget})
	if (NOT MSVC AND DEFINED ENV{R_DIR})
		target_include_directories(FMT PRIVATE ${includetotarget})
	endif(NOT MSVC AND DEFINED ENV{R_DIR})
endfunction(target_include  includetotarget)

function(createexecutable executablecpplocation executablename)
  set(newprojectsourcefiles "${executablecpplocation}")
  #list(APPEND newprojectsourcefiles "${FMTSOURCEFILES}")
  get_filename_component(executablename "${executablecpplocation}" NAME_WE)
  message("Generating build for ${executablename}")
  SET(executablename "${executablename}" PARENT_SCOPE)
  add_executable(${executablename} ${newprojectsourcefiles})
  target_include_directories(${executablename} PRIVATE ${Boost_INCLUDE_DIRS})
  target_link_libraries(${executablename} PRIVATE ${Boost_LIBRARIES})
  target_compile_definitions(${executablename} PRIVATE "-DFMTLIBIMPORT")
  target_link_libraries(${executablename} PRIVATE FMTlib)
  target_include_directories(${executablename} PRIVATE ${FMTINCLUDEDIR})
  SetBinaryLocation(${executablename})
	if (MSVC AND (${CMAKE_VERSION} VERSION_GREATER 3.8.0))
	set_target_properties(
   		${executablename} PROPERTIES
    		VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}")
	endif(MSVC AND (${CMAKE_VERSION} VERSION_GREATER 3.8.0))
	set_property(TARGET ${executablename} PROPERTY CXX_STANDARD 17)
	set_property(TARGET ${executablename} PROPERTY CXX_STANDARD_REQUIRED ON)
	#if (MOSEK_FOUND AND MSVC)
	#	target_sources(${executablename} PRIVATE ${OSIMSK_DEFINITION})
	#endif(MOSEK_FOUND AND MSVC)
endfunction(createexecutable executablecpplocation)

function(TargetDependency DependencyDirectory DllList)
    if (DEFINED ${DependencyDirectory})
        if (EXISTS "${${DependencyDirectory}}")
            set("ENV{${DependencyDirectory}}" "${${DependencyDirectory}}")
            list(APPEND ${DllList} "${${DependencyDirectory}}")
            set(${DllList} "${${DllList}}" PARENT_SCOPE)
        else()
            message(WARNING "${DependencyDirectory} is not a valid directory")
        endif(EXISTS "${${DependencyDirectory}}")
    endif(DEFINED ${DependencyDirectory})
endfunction(TargetDependency DependencyDirectory DllList)

function (ExportAllVariablesToInstall)
    get_cmake_property(_variableNames VARIABLES)
    list(SORT _variableNames)
    foreach(_variableName ${_variableNames})
		if(NOT "${_variableName}" MATCHES "REGEX")
			if("${${_variableName}}" MATCHES "\\\\")
				string(REPLACE "\\" "\\\\" ${_variableName} "${${_variableName}}")
				string(REGEX REPLACE "[\r\n]+" "" ${_variableName} "${${_variableName}}")
			endif()
			install(CODE "set(\"${_variableName}\" \"${${_variableName}}\")")
		endif()
    endforeach()
endfunction(ExportAllVariablesToInstall)
