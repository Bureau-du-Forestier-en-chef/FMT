	set (PATHS_DEPS "${DLLlookuppaths}")
	set(GDAL_FOUND "${GDAL_FOUND}")
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/LICENSES" DESTINATION "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}")
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/Templates/YieldPredModels" DESTINATION "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}")
    if (GDAL_FOUND)
        #Copy gdal data
		if (NOT EXISTS "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}/GDAL_DATA")
			file(COPY "${GDAL_DATA}" DESTINATION "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}")
			file(RENAME "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}/${GDAL_DATA_NAME}" "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}/GDAL_DATA")
		endif(NOT EXISTS "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}/GDAL_DATA")
		if (NOT EXISTS "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}/proj" AND EXISTS "${PROJDIR}")
			file(COPY "${PROJDIR}" DESTINATION "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}")
			if (NOT EXISTS "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}/proj")
				file(RENAME "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}/${PROJDIRNAME}" "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}/proj")
			endif(NOT EXISTS "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}/proj")
		endif(NOT EXISTS "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}/proj" AND EXISTS "${PROJDIR}")
    endif(GDAL_FOUND)
	set(DEPENDENCIES "${FMTDEPENDENCIES}")
	if(${CMAKE_VERSION} VERSION_LESS "3.16.0") 
    	message("Please consider to switch to CMake 3.16.0")
	else()
		FILE(GLOB_RECURSE FMTlibLocations "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}/*FMTlib${CMAKE_SHARED_LIBRARY_SUFFIX}")
		list(GET FMTlibLocations 0 FMTloc)
		file(	GET_RUNTIME_DEPENDENCIES
				RESOLVED_DEPENDENCIES_VAR DEPENDENCIESfound
				UNRESOLVED_DEPENDENCIES_VAR deps_unresolved
				CONFLICTING_DEPENDENCIES_PREFIX deps_conflicts
				PRE_EXCLUDE_REGEXES 
                "api-ms-.*" "ext-ms-.*" "Azure.*" "HvsiFileTrust.*"
            	POST_EXCLUDE_REGEXES
				".*system32/.*\\.dll"
                ".*[Ww][Ii][Nn][Dd][Oo][Ww][Ss]/[Ss][Yy][Ss][Tt][Ee][Mm]32/.*\\.dll"
				".*[Ww][Ii][Nn][Dd][Oo][Ww][Ss]/[Ss][Yy][Ss][Ww][Oo][Ww]64/.*\\.dll"
				LIBRARIES "${FMTloc}"
				DIRECTORIES ${PATHS_DEPS}
				)
		if (deps_unresolved)
			message(FATAL_ERROR "Unresolved dependencies on ${deps_unresolved}")
		endif(deps_unresolved)
		list (APPEND DEPENDENCIES ${DEPENDENCIESfound})
	endif(${CMAKE_VERSION} VERSION_LESS "3.16.0")
    if (DEPENDENCIES)
        #Copy dependencies
        foreach(FMTdep ${DEPENDENCIES})
            get_filename_component(depname "${FMTdep}" NAME)
            file(COPY "${FMTdep}" DESTINATION "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}")
        endforeach()
    endif(DEPENDENCIES)
	if (EXISTS "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}/setup.py" AND NOT EXISTS "${R_DIR}")
	set (PYTHON "${Python_EXECUTABLE}")
	if (NOT EXISTS "${PYTHON}")
		#MESSAGE ERREUR
		message(FATAL_ERROR "No Python found cannot install FMT")
	else()
		#Ensure pip 
		execute_process(
          	COMMAND "${PYTHON}" -m ensurepip
         	 WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}"
       		 )
		#Call pip from python and install it....
		execute_process(
          	COMMAND "${PYTHON}" -m pip install wheel
         	 WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}"
       		 )
	endif(NOT EXISTS "${PYTHON}")
        execute_process(
          COMMAND "${PYTHON}" -m pip wheel . 
          WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}"
        )
		FILE(GLOB_RECURSE wheelfiles "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}/FMT*.whl")
		list(GET wheelfiles 0 wheelloc)
		get_filename_component(wheelname "${wheelloc}" NAME)
		execute_process(
          COMMAND "${PYTHON}" -m pip install "${wheelname}" --force-reinstall
          WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}"
        )
    endif (EXISTS "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}/setup.py" AND NOT EXISTS "${R_DIR}")
	if (EXISTS "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}/FMT")
		file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/Templates/YieldPredModels" DESTINATION "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}/FMT/inst/libs/x64/")
		if (GDAL_FOUND)
			file(COPY "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}/GDAL_DATA" DESTINATION "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}/FMT/inst/libs/x64/")
			if (EXISTS "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}/proj")
				file(COPY "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}/proj" DESTINATION "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}/FMT/inst/libs/x64/")
			endif(EXISTS "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}/proj")
		endif(GDAL_FOUND)
		file(COPY "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}/FMT${CMAKE_SHARED_LIBRARY_SUFFIX}" DESTINATION "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}/FMT/inst/libs/x64/")
		file(COPY "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}/libFMT${CMAKE_SHARED_LIBRARY_SUFFIX}${CMAKE_STATIC_LIBRARY_SUFFIX}" DESTINATION "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}/FMT/inst/libs/x64/")
        if (DEPENDENCIES)
			#Copy dependencies
			foreach(FMTdep ${DEPENDENCIES})
				file(COPY "${FMTdep}" DESTINATION "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}/FMT/inst/libs/x64/")
			endforeach()
		 endif(DEPENDENCIES)
		find_program(RLOCATION "R" PATHS "${R_DIR}/bin/x64")
        execute_process(
          COMMAND "${RLOCATION}" CMD build FMT
          WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}"
        )
		set(R_package "FMT_${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}.tar.gz")
		execute_process(
          COMMAND "${RLOCATION}" CMD INSTALL --no-multiarch "${R_package}"
          WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}"
        )
endif (EXISTS "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}/FMT")
	set (WITHOUT_TESTS "${WITHOUT_TESTS}")
	if (NOT WITHOUT_TESTS)
		execute_process(
					  COMMAND ctest -C "${BUILD_TYPE}" -j "${PARALLEL_TESTS}"
					  WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
					)
		set (DO_COVERAGE "${COVERAGE}")
		set(MSVC "${MSVC}")
		#Replace in readme the coverage
		set (readme "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
		set (frreadme "${CMAKE_CURRENT_SOURCE_DIR}/README.fr.md")
		if (DO_COVERAGE AND NOT MSVC)
			 execute_process(
					  COMMAND ctest -C "${BUILD_TYPE}" -j "${PARALLEL_TESTS}" -T Coverage
					  WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
					)
			FILE(GLOB_RECURSE config_files "${CMAKE_BINARY_DIR}/Testing/*Coverage.xml")
			list(GET config_files 0 config_file)
			file(STRINGS "${config_file}" configm_lines)
			while(configm_lines)
				list(POP_FRONT configm_lines LINE)
				string(REGEX MATCH "(<LOCTested>)([0-9]*)(<\\/LOCTested>)" _ "${LINE}")
				if (CMAKE_MATCH_2)
					set(SUCCESS "${CMAKE_MATCH_2}")
				else()
					string(REGEX MATCH "(<LOC>)([0-9]*)(<\\/LOC>)" _ "${LINE}")
					if (CMAKE_MATCH_2)
						set(TESTED "${CMAKE_MATCH_2}")
					endif(CMAKE_MATCH_2)
				endif(CMAKE_MATCH_2)
			endwhile()
			math(EXPR total_coverage "((${SUCCESS} * 100) / ${TESTED})")
			set (coverage_badge "${CMAKE_CXX_COMPILER_ID} ![](https://img.shields.io/badge/coverage-${total_coverage}%25-red)")
			if (total_coverage GREATER_EQUAL 80)
				set (coverage_badge "${CMAKE_CXX_COMPILER_ID} ![](https://img.shields.io/badge/coverage-${total_coverage}%25-brightgreen)")
			elseif(total_coverage GREATER_EQUAL 50)
				set (coverage_badge "${CMAKE_CXX_COMPILER_ID} ![](https://img.shields.io/badge/coverage-${total_coverage}%25-orange)")
			endif(total_coverage GREATER_EQUAL 80)
		set (coverage_regex "${CMAKE_CXX_COMPILER_ID}[^\\n]*coverage[^\\n]*")
		#English
		file(READ "${readme}" filedata)
		string(REGEX REPLACE "${coverage_regex}" "${coverage_badge}"  filedata "${filedata}")
		file(WRITE "${readme}" "${filedata}")
		#French
		file(READ "${frreadme}" filedata)
		string(REGEX REPLACE "${coverage_regex}" "${coverage_badge}"  filedata "${filedata}")
		file(WRITE "${frreadme}" "${filedata}")
		endif(DO_COVERAGE AND NOT MSVC)
	if (EXISTS "${CMAKE_BINARY_DIR}/Testing/Temporary/LastTestsFailed.log")
		file(STRINGS "${CMAKE_BINARY_DIR}/Testing/Temporary/LastTestsFailed.log" TESTS_FILE)
	endif(EXISTS "${CMAKE_BINARY_DIR}/Testing/Temporary/LastTestsFailed.log")
	list(LENGTH TESTS_FILE FAILED_TESTS)
	set (build_badge "${CMAKE_CXX_COMPILER_ID} ![](https://img.shields.io/badge/build-broken-red)")
	if (FAILED_TESTS LESS 10)#Not Broken
		set (build_badge "${CMAKE_CXX_COMPILER_ID} ![](https://img.shields.io/badge/build-pass-brightgreen)")
	endif(FAILED_TESTS LESS 10)
	set (build_regex "${CMAKE_CXX_COMPILER_ID}[^\\n^c]*build[^\\n]*")
	#English
	file(READ "${readme}" filedata)
	string(REGEX REPLACE "${build_regex}" "${build_badge}"  filedata "${filedata}")
	file(WRITE "${readme}" "${filedata}")
	#French
	file(READ "${frreadme}" filedata)
	string(REGEX REPLACE "${build_regex}" "${build_badge}"  filedata "${filedata}")
	file(WRITE "${frreadme}" "${filedata}")
	endif(NOT WITHOUT_TESTS)