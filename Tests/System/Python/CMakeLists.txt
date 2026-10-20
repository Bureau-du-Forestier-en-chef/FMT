#[[
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
]]

#[[
The pytest suite, one system test per file: test<Domain>.py becomes System.Python.<Domain>, labelled system
and python. Each test runs runTestFile.py, which imports the FMT module of this build, FMT.pyd in the folder
of the executables, rather than the wheel last installed.

The tests are registered where cmake/BaseInstallPython.cmake builds the wheel: a build with the FMT target
and a Python interpreter, without R_DIR or WITHOUT_PYTHONLIB. FMT.pyd needs the DLLs that cmake --install
copies next to it: until then, the tests are skipped. A new test file is registered at the next
configuration.
]]

#A function, so that its variables stay out of the variables exported to the install scripts.
function(registerpythonsystemtests)
	set(binarydirectory "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}")
	file(GLOB testfiles LIST_DIRECTORIES false "${CMAKE_CURRENT_LIST_DIR}/test*.py")
	list(SORT testfiles)
	foreach(testfile IN LISTS testfiles)
		get_filename_component(domain "${testfile}" NAME_WE)
		string(REGEX REPLACE "^test" "" domain "${domain}")
		set(testname "System.Python.${domain}")
		registersystemtest("${testname}" "Tests/System/Python/CMakeLists.txt" INTERFACE python
			"${Python_EXECUTABLE}" "${CMAKE_CURRENT_LIST_DIR}/runTestFile.py" "${binarydirectory}" "${testfile}")
		set_tests_properties("${testname}" PROPERTIES
			ENVIRONMENT "FMT_SOURCE_DIR=${PROJECT_SOURCE_DIR};PYTHONPATH=${binarydirectory};PYTHONDONTWRITEBYTECODE=1"
			)
	endforeach()
endfunction(registerpythonsystemtests)

if (TARGET FMT AND EXISTS "${Python_EXECUTABLE}" AND NOT EXISTS "${R_DIR}" AND NOT DEFINED WITHOUT_PYTHONLIB)
	registerpythonsystemtests()
endif()
