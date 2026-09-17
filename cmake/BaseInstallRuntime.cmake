#[[
Deploy common FMT assets and native runtime dependencies.
]]

set(PATHS_DEPS ${DLLlookuppaths})
set(FMT_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}")
file(MAKE_DIRECTORY "${FMT_OUTPUT_DIRECTORY}")

foreach(FMT_ASSET IN ITEMS LICENSES CHANGELOG.md CHANGELOG.fr.md)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${FMT_ASSET}")
        file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/${FMT_ASSET}"
             DESTINATION "${FMT_OUTPUT_DIRECTORY}")
    else()
        message(FATAL_ERROR "Required FMT asset is missing: ${CMAKE_CURRENT_SOURCE_DIR}/${FMT_ASSET}")
    endif()
endforeach()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Templates/YieldPredModels")
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/Templates/YieldPredModels"
         DESTINATION "${FMT_OUTPUT_DIRECTORY}")
else()
    message(FATAL_ERROR "YieldPredModels directory is missing")
endif()

if(GDAL_FOUND)
    if(NOT EXISTS "${FMT_OUTPUT_DIRECTORY}/GDAL_DATA")
        if(NOT EXISTS "${GDAL_DATA}")
            message(FATAL_ERROR "GDAL data directory is missing: ${GDAL_DATA}")
        endif()
        file(COPY "${GDAL_DATA}" DESTINATION "${FMT_OUTPUT_DIRECTORY}")
        if(EXISTS "${FMT_OUTPUT_DIRECTORY}/${GDAL_DATA_NAME}")
            file(RENAME "${FMT_OUTPUT_DIRECTORY}/${GDAL_DATA_NAME}"
                        "${FMT_OUTPUT_DIRECTORY}/GDAL_DATA")
        elseif(NOT EXISTS "${FMT_OUTPUT_DIRECTORY}/GDAL_DATA")
            message(FATAL_ERROR "Unable to normalize the copied GDAL_DATA directory")
        endif()
    endif()

    if(NOT EXISTS "${FMT_OUTPUT_DIRECTORY}/proj" AND EXISTS "${PROJDIR}")
        file(COPY "${PROJDIR}" DESTINATION "${FMT_OUTPUT_DIRECTORY}")
        if(NOT EXISTS "${FMT_OUTPUT_DIRECTORY}/proj" AND
           EXISTS "${FMT_OUTPUT_DIRECTORY}/${PROJDIRNAME}")
            file(RENAME "${FMT_OUTPUT_DIRECTORY}/${PROJDIRNAME}"
                        "${FMT_OUTPUT_DIRECTORY}/proj")
        endif()
    endif()
endif()

set(DEPENDENCIES ${FMTDEPENDENCIES})

file(GLOB_RECURSE FMTlibLocations
     "${FMT_OUTPUT_DIRECTORY}/*FMTlib${CMAKE_SHARED_LIBRARY_SUFFIX}")
list(LENGTH FMTlibLocations FMTLIB_COUNT)
if(FMTLIB_COUNT EQUAL 0)
    message(FATAL_ERROR "Unable to locate FMTlib under ${FMT_OUTPUT_DIRECTORY}")
endif()
list(GET FMTlibLocations 0 FMTloc)

file(GET_RUNTIME_DEPENDENCIES
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

if(deps_unresolved)
    message(FATAL_ERROR "Unresolved runtime dependencies: ${deps_unresolved}")
endif()

list(APPEND DEPENDENCIES ${DEPENDENCIESfound})
list(REMOVE_DUPLICATES DEPENDENCIES)

foreach(FMTdep IN LISTS DEPENDENCIES)
    if(EXISTS "${FMTdep}")
        file(COPY "${FMTdep}" DESTINATION "${FMT_OUTPUT_DIRECTORY}")
    else()
        message(WARNING "Skipping missing runtime dependency: ${FMTdep}")
    endif()
endforeach()
