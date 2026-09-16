#[[
Build, install, and validate the FMT R package.
]]

set(FMT_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}")
set(FMT_R_PACKAGE_DIRECTORY "${FMT_OUTPUT_DIRECTORY}/FMT")
set(FMT_R_LIBRARY_DIRECTORY "${FMT_R_PACKAGE_DIRECTORY}/inst/libs/x64")

if(EXISTS "${FMT_R_PACKAGE_DIRECTORY}")
    file(MAKE_DIRECTORY "${FMT_R_LIBRARY_DIRECTORY}")

    foreach(FMT_ASSET IN ITEMS YieldPredModels LICENSES CHANGELOG.md CHANGELOG.fr.md)
        if(EXISTS "${FMT_OUTPUT_DIRECTORY}/${FMT_ASSET}")
            file(COPY "${FMT_OUTPUT_DIRECTORY}/${FMT_ASSET}"
                 DESTINATION "${FMT_R_LIBRARY_DIRECTORY}")
        endif()
    endforeach()

    if(GDAL_FOUND)
        if(EXISTS "${FMT_OUTPUT_DIRECTORY}/GDAL_DATA")
            file(COPY "${FMT_OUTPUT_DIRECTORY}/GDAL_DATA"
                 DESTINATION "${FMT_R_LIBRARY_DIRECTORY}")
        endif()
        if(EXISTS "${FMT_OUTPUT_DIRECTORY}/proj")
            file(COPY "${FMT_OUTPUT_DIRECTORY}/proj"
                 DESTINATION "${FMT_R_LIBRARY_DIRECTORY}")
        endif()
    endif()

    foreach(FMT_LIBRARY IN ITEMS
        "${FMT_OUTPUT_DIRECTORY}/FMT${CMAKE_SHARED_LIBRARY_SUFFIX}"
        "${FMT_OUTPUT_DIRECTORY}/libFMT${CMAKE_SHARED_LIBRARY_SUFFIX}${CMAKE_STATIC_LIBRARY_SUFFIX}")
        if(EXISTS "${FMT_LIBRARY}")
            file(COPY "${FMT_LIBRARY}" DESTINATION "${FMT_R_LIBRARY_DIRECTORY}")
        else()
            message(WARNING "R package library was not found: ${FMT_LIBRARY}")
        endif()
    endforeach()

    foreach(FMTdep IN LISTS DEPENDENCIES)
        if(EXISTS "${FMTdep}")
            file(COPY "${FMTdep}" DESTINATION "${FMT_R_LIBRARY_DIRECTORY}")
        endif()
    endforeach()

    find_program(RLOCATION NAMES R R.exe
        PATHS "${R_DIR}/bin/x64" "${R_DIR}/bin"
        REQUIRED)

    if(NOT WITHOUT_TESTS)
        fmtExecuteRequired("Ensure the R testthat package is available" "${FMT_OUTPUT_DIRECTORY}"
            "${RLOCATION}" --vanilla --slave -e
            "if (!requireNamespace('testthat', quietly=TRUE)) install.packages('testthat', repos='https://cloud.r-project.org')")
    endif()

    fmtExecuteRequired("Build the FMT R package" "${FMT_OUTPUT_DIRECTORY}"
        "${RLOCATION}" CMD build FMT)

    set(R_package "FMT_${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}.tar.gz")
    if(NOT EXISTS "${FMT_OUTPUT_DIRECTORY}/${R_package}")
        message(FATAL_ERROR "The expected R package was not generated: ${R_package}")
    endif()

    fmtExecuteRequired("Install the FMT R package" "${FMT_OUTPUT_DIRECTORY}"
        "${RLOCATION}" CMD INSTALL --no-multiarch "${R_package}")

    if(NOT WITHOUT_TESTS)
        set(FMT_R_TEST_DIRECTORY
            "${CMAKE_CURRENT_SOURCE_DIR}/Examples/R/tests")
        if(NOT EXISTS "${FMT_R_TEST_DIRECTORY}")
            message(FATAL_ERROR
                "R test suite is missing: ${FMT_R_TEST_DIRECTORY}")
        endif()

        file(TO_CMAKE_PATH "${FMT_R_TEST_DIRECTORY}" FMT_R_TEST_DIRECTORY_R)
        fmtExecuteRequired("Run verbose FMT R tests" "${FMT_OUTPUT_DIRECTORY}"
            "${CMAKE_COMMAND}" -E env
            "FMT_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}"
            "${RLOCATION}" --vanilla --slave -e
            "library(FMT); testthat::test_dir('${FMT_R_TEST_DIRECTORY_R}', reporter='progress', stop_on_failure=TRUE, stop_on_warning=FALSE)")
    endif()
endif()
