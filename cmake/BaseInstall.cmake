#[[
Copyright (c) 2019 Gouvernement du Quebec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
]]

function(fmtExecuteRequired DESCRIPTION WORKING_DIRECTORY)
    set(COMMAND_ARGUMENTS ${ARGN})
    message(STATUS "${DESCRIPTION}")
    execute_process(
        COMMAND ${COMMAND_ARGUMENTS}
        WORKING_DIRECTORY "${WORKING_DIRECTORY}"
        RESULT_VARIABLE PROCESS_RESULT
        COMMAND_ECHO STDOUT
    )
    if(NOT PROCESS_RESULT EQUAL 0)
        message(FATAL_ERROR "${DESCRIPTION} failed with exit code ${PROCESS_RESULT}")
    endif()
endfunction()

include("${CMAKE_CURRENT_LIST_DIR}/BaseInstallRuntime.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/BaseInstallPython.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/BaseInstallR.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/BaseInstallTests.cmake")
