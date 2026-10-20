#[[
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
]]

#[[
The testthat suite, one system test per file: test<Domain>.R becomes System.R.<Domain>, labelled system and
r. Each test runs runTestFile.R with the Rscript of R_DIR, against the FMT package that
cmake/BaseInstallR.cmake installs in that R: until it is installed, at the version of this build, the
tests are skipped. runTests.R still runs the whole folder by hand.

The tests are registered with R_DIR, when its Rscript is found at configuration. A new test file is
registered at the next configuration.
]]

#A function, so that its variables stay out of the variables exported to the install scripts.
function(registerrsystemtests rscript)
	file(GLOB testfiles LIST_DIRECTORIES false "${CMAKE_CURRENT_LIST_DIR}/test*.R")
	list(SORT testfiles)
	foreach(testfile IN LISTS testfiles)
		get_filename_component(domain "${testfile}" NAME_WE)
		string(REGEX REPLACE "^test" "" domain "${domain}")
		set(testname "System.R.${domain}")
		registersystemtest("${testname}" "Tests/System/R/CMakeLists.txt" INTERFACE r
			"${rscript}" --vanilla "${CMAKE_CURRENT_LIST_DIR}/runTestFile.R" "${testfile}" "${PROJECT_VERSION}")
		set_tests_properties("${testname}" PROPERTIES
			ENVIRONMENT "FMT_SOURCE_DIR=${PROJECT_SOURCE_DIR}"
			)
	endforeach()
endfunction(registerrsystemtests rscript)

if (EXISTS "${R_DIR}")
	find_program(FMTTESTSrscript NAMES Rscript Rscript.exe PATHS "${R_DIR}/bin/x64" "${R_DIR}/bin" NO_DEFAULT_PATH)
	if (FMTTESTSrscript)
		registerrsystemtests("${FMTTESTSrscript}")
	else()
		message(WARNING "No Rscript was found in R_DIR (${R_DIR}): the R tests are not registered")
	endif(FMTTESTSrscript)
endif()
