#[[
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
]]


cmake_minimum_required(VERSION 3.20)

FILE(GLOB FMTexecutables ${CMAKE_CURRENT_SOURCE_DIR}/Examples/C++/*.cpp)
foreach(pathtosource ${FMTexecutables})
	createexecutable("${pathtosource}" executablename)
endforeach()

#The rows of tests/*.csv register the tests of these executables and of FMTWrapperCore/tests
#(registercsvtests in Tests/Support/TestRegistration.cmake).
#Executables whose rows write into the same output files: their rows never run at the same time
set(FMTTESTSsharedoutputs planningtest testActionsAggregations testOAschedulerBFEC testreadwriteproject)
FILE(GLOB FMTTESTSfiles ${CMAKE_CURRENT_SOURCE_DIR}/Examples/C++/tests/*.csv)
registercsvtests("${FMTTESTSfiles}" "${FMTTESTSsharedoutputs}")