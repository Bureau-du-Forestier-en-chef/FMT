#[[
Build, install, and validate the FMT R package.
]]

set(
    FMT_OUTPUT_DIRECTORY
    "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}"
)

set(
    FMT_R_PACKAGE_DIRECTORY
    "${FMT_OUTPUT_DIRECTORY}/FMT"
)

set(
    FMT_R_LIBRARY_DIRECTORY
    "${FMT_R_PACKAGE_DIRECTORY}/inst/libs/x64"
)

if(EXISTS "${FMT_R_PACKAGE_DIRECTORY}")
    file(MAKE_DIRECTORY "${FMT_R_LIBRARY_DIRECTORY}")

    foreach(FMT_ASSET IN ITEMS
        YieldPredModels
        LICENSES
        CHANGELOG.md
        CHANGELOG.fr.md
    )
        if(EXISTS "${FMT_OUTPUT_DIRECTORY}/${FMT_ASSET}")
            file(
                COPY
                "${FMT_OUTPUT_DIRECTORY}/${FMT_ASSET}"
                DESTINATION
                "${FMT_R_LIBRARY_DIRECTORY}"
            )
        else()
            message(
                WARNING
                "R package asset was not found: "
                "${FMT_OUTPUT_DIRECTORY}/${FMT_ASSET}"
            )
        endif()
    endforeach()

    if(GDAL_FOUND)
        if(EXISTS "${FMT_OUTPUT_DIRECTORY}/GDAL_DATA")
            file(
                COPY
                "${FMT_OUTPUT_DIRECTORY}/GDAL_DATA"
                DESTINATION
                "${FMT_R_LIBRARY_DIRECTORY}"
            )
        else()
            message(
                WARNING
                "GDAL_DATA was not found in ${FMT_OUTPUT_DIRECTORY}"
            )
        endif()

        if(EXISTS "${FMT_OUTPUT_DIRECTORY}/proj")
            file(
                COPY
                "${FMT_OUTPUT_DIRECTORY}/proj"
                DESTINATION
                "${FMT_R_LIBRARY_DIRECTORY}"
            )
        endif()
    endif()

    set(
        FMT_R_LIBRARIES
        "${FMT_OUTPUT_DIRECTORY}/FMT${CMAKE_SHARED_LIBRARY_SUFFIX}"
        "${FMT_OUTPUT_DIRECTORY}/libFMT${CMAKE_SHARED_LIBRARY_SUFFIX}${CMAKE_STATIC_LIBRARY_SUFFIX}"
    )

    foreach(FMT_LIBRARY IN LISTS FMT_R_LIBRARIES)
        if(EXISTS "${FMT_LIBRARY}")
            file(
                COPY
                "${FMT_LIBRARY}"
                DESTINATION
                "${FMT_R_LIBRARY_DIRECTORY}"
            )
        else()
            message(
                WARNING
                "R package library was not found: ${FMT_LIBRARY}"
            )
        endif()
    endforeach()

    foreach(FMTdep IN LISTS DEPENDENCIES)
        if(EXISTS "${FMTdep}")
            file(
                COPY
                "${FMTdep}"
                DESTINATION
                "${FMT_R_LIBRARY_DIRECTORY}"
            )
        else()
            message(
                WARNING
                "R package dependency was not found: ${FMTdep}"
            )
        endif()
    endforeach()

    find_program(
        RLOCATION
        NAMES
            R
            R.exe
        PATHS
            "${R_DIR}/bin/x64"
            "${R_DIR}/bin"
        REQUIRED
    )

    get_filename_component(
        R_BINARY_DIRECTORY
        "${RLOCATION}"
        DIRECTORY
    )

    find_program(
        RSCRIPT_LOCATION
        NAMES
            Rscript
            Rscript.exe
        PATHS
            "${R_BINARY_DIRECTORY}"
            "${R_DIR}/bin/x64"
            "${R_DIR}/bin"
        NO_DEFAULT_PATH
    )

    if(NOT RSCRIPT_LOCATION)
        find_program(
            RSCRIPT_LOCATION
            NAMES
                Rscript
                Rscript.exe
            REQUIRED
        )
    endif()

    if(NOT WITHOUT_TESTS)
        fmtExecuteRequired(
            "Ensure the R testthat package is available"
            "${FMT_OUTPUT_DIRECTORY}"
            "${RSCRIPT_LOCATION}"
            --vanilla
            -e
            "if (!requireNamespace('testthat', quietly=TRUE)) install.packages('testthat', repos='https://cloud.r-project.org')"
        )
    endif()

    file(
        GLOB
        STALE_R_PACKAGES
        "${FMT_OUTPUT_DIRECTORY}/FMT_*.tar.gz"
    )

    if(STALE_R_PACKAGES)
        file(REMOVE ${STALE_R_PACKAGES})
    endif()

    fmtExecuteRequired(
        "Build the FMT R package"
        "${FMT_OUTPUT_DIRECTORY}"
        "${RLOCATION}"
        CMD
        build
        FMT
    )

    set(
        R_package
        "FMT_${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}.tar.gz"
    )

    set(
        R_PACKAGE_PATH
        "${FMT_OUTPUT_DIRECTORY}/${R_package}"
    )

    if(NOT EXISTS "${R_PACKAGE_PATH}")
        message(
            FATAL_ERROR
            "The expected R package was not generated: "
            "${R_PACKAGE_PATH}"
        )
    endif()

    fmtExecuteRequired(
        "Install the FMT R package"
        "${FMT_OUTPUT_DIRECTORY}"
        "${RLOCATION}"
        CMD
        INSTALL
        --no-multiarch
        "${R_PACKAGE_PATH}"
    )

    if(NOT WITHOUT_TESTS)
        set(
            FMT_R_TEST_DIRECTORY
            "${CMAKE_CURRENT_SOURCE_DIR}/Examples/R/tests"
        )

        set(
            FMT_R_TEST_RUNNER
            "${FMT_R_TEST_DIRECTORY}/runTests.R"
        )

        if(NOT EXISTS "${FMT_R_TEST_DIRECTORY}")
            message(
                FATAL_ERROR
                "R test suite is missing: "
                "${FMT_R_TEST_DIRECTORY}"
            )
        endif()

        if(NOT EXISTS "${FMT_R_TEST_RUNNER}")
            message(
                FATAL_ERROR
                "R test runner is missing: "
                "${FMT_R_TEST_RUNNER}"
            )
        endif()

        fmtExecuteRequired(
            "Run FMT R tests"
            "${FMT_OUTPUT_DIRECTORY}"
            "${CMAKE_COMMAND}"
            -E
            env
            "FMT_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}"
            "${RSCRIPT_LOCATION}"
            --vanilla
            "${FMT_R_TEST_RUNNER}"
        )
    endif()
endif()
