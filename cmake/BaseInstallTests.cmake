#[[
Run CTest and print the test summary.
INSTALL_TEST_SUITE selects the tests: public (all but the private tests, whose name holds T:/), all,
or none, which runs nothing.
]]

if(NOT DEFINED INSTALL_TEST_SUITE OR INSTALL_TEST_SUITE STREQUAL "")
    set(INSTALL_TEST_SUITE "public")
endif()

if(INSTALL_TEST_SUITE STREQUAL "none")
    message(STATUS "INSTALL_TEST_SUITE is none: no test is run")
elseif(NOT WITHOUT_TESTS)
    set(LAST_TESTS_FAILED_FILE "${CMAKE_BINARY_DIR}/Testing/Temporary/LastTestsFailed.log")
    set(TEST_LIST_FILE "${CMAKE_BINARY_DIR}/Testing/Temporary/InstallTests.json")
    set(TEST_RESULTS_FILE "${CMAKE_BINARY_DIR}/Testing/Temporary/InstallTests.xml")
    file(REMOVE "${LAST_TESTS_FAILED_FILE}" "${TEST_LIST_FILE}" "${TEST_RESULTS_FILE}")
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/Testing/Temporary")

    set(TEST_FILTER "")
    if(INSTALL_TEST_SUITE STREQUAL "public")
        set(TEST_FILTER -E "T:/")
    endif()

    # Listed before the pass: every call of ctest rewrites LastTest.log.
    execute_process(
        COMMAND "${CMAKE_CTEST_COMMAND}" --test-dir "${CMAKE_BINARY_DIR}"
                -C "${BUILD_TYPE}" ${TEST_FILTER} --show-only=json-v1
        OUTPUT_FILE "${TEST_LIST_FILE}"
    )

    execute_process(
        COMMAND "${CMAKE_CTEST_COMMAND}" --test-dir "${CMAKE_BINARY_DIR}"
                -C "${BUILD_TYPE}" --parallel "${PARALLEL_TESTS}"
                ${TEST_FILTER} --output-on-failure
                --output-junit "${TEST_RESULTS_FILE}"
        RESULT_VARIABLE CTEST_RESULT
    )

    execute_process(
        COMMAND "${CMAKE_COMMAND}" "-DTEST_RESULTS=${TEST_RESULTS_FILE}"
                "-DTEST_LIST=${TEST_LIST_FILE}"
                -P "${CMAKE_CURRENT_SOURCE_DIR}/Tests/Support/TestSummary.cmake"
    )

    set(FAILED_TESTS 0)
    if(EXISTS "${LAST_TESTS_FAILED_FILE}")
        file(STRINGS "${LAST_TESTS_FAILED_FILE}" TESTS_FILE)
        list(LENGTH TESTS_FILE FAILED_TESTS)
    endif()

    if(NOT CTEST_RESULT EQUAL 0)
        message(FATAL_ERROR "CTest failed with exit code ${CTEST_RESULT}; ${FAILED_TESTS} tests were reported as failed")
    endif()
endif()
