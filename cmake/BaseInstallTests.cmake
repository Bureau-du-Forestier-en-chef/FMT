#[[
Run CTest, collect optional coverage, and update README badges.
]]

if(NOT WITHOUT_TESTS)
    set(readme "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
    set(frreadme "${CMAKE_CURRENT_SOURCE_DIR}/README.fr.md")
    set(LAST_TESTS_FAILED_FILE "${CMAKE_BINARY_DIR}/Testing/Temporary/LastTestsFailed.log")
    file(REMOVE "${LAST_TESTS_FAILED_FILE}")

    execute_process(
        COMMAND "${CMAKE_CTEST_COMMAND}" --test-dir "${CMAKE_BINARY_DIR}"
                -C "${BUILD_TYPE}" --parallel "${PARALLEL_TESTS}"
                --output-on-failure
        RESULT_VARIABLE CTEST_RESULT
    )

    if(COVERAGE AND NOT MSVC)
        execute_process(
            COMMAND "${CMAKE_CTEST_COMMAND}" --test-dir "${CMAKE_BINARY_DIR}"
                    -C "${BUILD_TYPE}" --parallel "${PARALLEL_TESTS}"
                    -T Coverage
            RESULT_VARIABLE COVERAGE_RESULT
        )
        if(NOT COVERAGE_RESULT EQUAL 0)
            message(WARNING "Coverage collection failed with exit code ${COVERAGE_RESULT}")
        else()
            file(GLOB_RECURSE config_files "${CMAKE_BINARY_DIR}/Testing/*Coverage.xml")
            list(LENGTH config_files COVERAGE_FILE_COUNT)
            if(COVERAGE_FILE_COUNT GREATER 0)
                list(GET config_files 0 config_file)
                file(STRINGS "${config_file}" configm_lines)
                unset(SUCCESS)
                unset(TESTED)
                foreach(LINE IN LISTS configm_lines)
                    string(REGEX MATCH "(&lt;LOCTested&gt;)([0-9]*)(&lt;/LOCTested&gt;)" _ "${LINE}")
                    if(CMAKE_MATCH_2)
                        set(SUCCESS "${CMAKE_MATCH_2}")
                    endif()
                    string(REGEX MATCH "(&lt;LOC&gt;)([0-9]*)(&lt;/LOC&gt;)" _ "${LINE}")
                    if(CMAKE_MATCH_2)
                        set(TESTED "${CMAKE_MATCH_2}")
                    endif()
                endforeach()

                if(DEFINED SUCCESS AND DEFINED TESTED AND TESTED GREATER 0)
                    math(EXPR total_coverage "(${SUCCESS} * 100) / ${TESTED}")
                    set(coverage_color red)
                    if(total_coverage GREATER_EQUAL 80)
                        set(coverage_color brightgreen)
                    elseif(total_coverage GREATER_EQUAL 50)
                        set(coverage_color orange)
                    endif()
                    set(coverage_badge "${CMAKE_CXX_COMPILER_ID} ![](https://img.shields.io/badge/coverage-${total_coverage}%25-${coverage_color})")
                    set(coverage_regex "${CMAKE_CXX_COMPILER_ID}[^&gt;]*coverage[^&lt;]*")
                    foreach(README_FILE IN ITEMS "${readme}" "${frreadme}")
                        if(EXISTS "${README_FILE}")
                            file(READ "${README_FILE}" filedata)
                            string(REGEX REPLACE "${coverage_regex}" "${coverage_badge}" filedata "${filedata}")
                            file(WRITE "${README_FILE}" "${filedata}")
                        endif()
                    endforeach()
                else()
                    message(WARNING "Unable to extract coverage totals from ${config_file}")
                endif()
            else()
                message(WARNING "CTest did not generate a Coverage.xml file")
            endif()
        endif()
    endif()

    set(FAILED_TESTS 0)
    if(EXISTS "${LAST_TESTS_FAILED_FILE}")
        file(STRINGS "${LAST_TESTS_FAILED_FILE}" TESTS_FILE)
        list(LENGTH TESTS_FILE FAILED_TESTS)
    endif()

    if(CTEST_RESULT EQUAL 0 AND FAILED_TESTS EQUAL 0)
        set(build_badge "${CMAKE_CXX_COMPILER_ID} ![](https://img.shields.io/badge/build-pass-brightgreen)")
    else()
        set(build_badge "${CMAKE_CXX_COMPILER_ID} ![](https://img.shields.io/badge/build-broken-red)")
    endif()
    set(build_regex "${CMAKE_CXX_COMPILER_ID}[^&gt;]*build[^&lt;]*")
    foreach(README_FILE IN ITEMS "${readme}" "${frreadme}")
        if(EXISTS "${README_FILE}")
            file(READ "${README_FILE}" filedata)
            string(REGEX REPLACE "${build_regex}" "${build_badge}" filedata "${filedata}")
            file(WRITE "${README_FILE}" "${filedata}")
        endif()
    endforeach()

    if(NOT CTEST_RESULT EQUAL 0)
        message(FATAL_ERROR "CTest failed with exit code ${CTEST_RESULT}; ${FAILED_TESTS} tests were reported as failed")
    endif()
endif()
