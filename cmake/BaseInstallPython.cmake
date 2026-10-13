#[[
Build and install the FMT Python wheel, and make pytest available to the tests. The pytest suite,
Tests/System/Python, runs in the ctest pass of BaseInstallTests.cmake.
]]

set(FMT_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}")

if(EXISTS "${FMT_OUTPUT_DIRECTORY}/setup.py" AND NOT EXISTS "${R_DIR}")
    set(PYTHON "${Python_EXECUTABLE}")
    if(NOT EXISTS "${PYTHON}")
        message(FATAL_ERROR "No Python interpreter found; cannot install FMT")
    endif()

    fmtExecuteRequired("Ensure pip is available" "${FMT_OUTPUT_DIRECTORY}"
        "${PYTHON}" -m ensurepip)

    set(PYTHON_BUILD_DEPENDENCIES pybind11 pybind11-stubgen wheel)
    if(NOT WITHOUT_TESTS)
        list(APPEND PYTHON_BUILD_DEPENDENCIES pytest)
    endif()
    fmtExecuteRequired("Install Python build dependencies" "${FMT_OUTPUT_DIRECTORY}"
        "${PYTHON}" -m pip install -q ${PYTHON_BUILD_DEPENDENCIES})

    file(GLOB STALE_WHEELS "${FMT_OUTPUT_DIRECTORY}/FMT*.whl")
    if(STALE_WHEELS)
        file(REMOVE ${STALE_WHEELS})
    endif()

    fmtExecuteRequired("Build the FMT Python wheel" "${FMT_OUTPUT_DIRECTORY}"
        "${PYTHON}" -m pip wheel -q .)

    file(GLOB wheelfiles "${FMT_OUTPUT_DIRECTORY}/FMT*.whl")
    list(LENGTH wheelfiles WHEEL_COUNT)
    if(NOT WHEEL_COUNT EQUAL 1)
        message(FATAL_ERROR "Expected exactly one FMT wheel, found ${WHEEL_COUNT}: ${wheelfiles}")
    endif()
    list(GET wheelfiles 0 wheelloc)
    get_filename_component(wheelname "${wheelloc}" NAME)

    fmtExecuteRequired("Install the FMT Python wheel" "${FMT_OUTPUT_DIRECTORY}"
        "${PYTHON}" -m pip install -q "${wheelloc}" --force-reinstall)
endif()
