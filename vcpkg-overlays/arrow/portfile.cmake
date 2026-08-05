vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO apache/arrow
    REF "apache-arrow-25.0.0"
    SHA512 70d0fe61dd96dd6a43711647634b6879af95d79526c5e3cecc5c0854e0a3817c6aedda5aae81fc32f95b7ce825922930f5bf53312c11a90c6e0fc63ffe35e2a7
    HEAD_REF master
    PATCHES
        0001-msvc-static-name.patch
        0003-android-musl.patch
        0004-android-datetime.patch
        0005-cmake-msvcruntime.patch
        0007-use-vcpkg-mimalloc.patch
)


vcpkg_check_features(OUT_FEATURE_OPTIONS FEATURE_OPTIONS
    FEATURES
        parquet     ARROW_PARQUET
        filesystem  ARROW_FILESYSTEM
		mimalloc    ARROW_MIMALLOC
)

if(VCPKG_TARGET_IS_WINDOWS AND NOT VCPKG_TARGET_IS_MINGW)
    list(APPEND FEATURE_OPTIONS "-DARROW_USE_NATIVE_INT128=OFF")
endif()

if(VCPKG_TARGET_IS_WINDOWS AND VCPKG_TARGET_ARCHITECTURE STREQUAL "arm64")
    list(APPEND FEATURE_OPTIONS "-DARROW_SIMD_LEVEL=NONE")
endif()

string(COMPARE EQUAL ${VCPKG_LIBRARY_LINKAGE} "dynamic" ARROW_BUILD_SHARED)
string(COMPARE EQUAL ${VCPKG_LIBRARY_LINKAGE} "static" ARROW_BUILD_STATIC)
string(COMPARE EQUAL ${VCPKG_LIBRARY_LINKAGE} "dynamic" ARROW_DEPENDENCY_USE_SHARED)

# Rétrocompatibilité : Utilisation de vcpkg_configure_cmake (ancien standard)
vcpkg_configure_cmake(
    SOURCE_PATH "${SOURCE_PATH}/cpp"
    PREFER_NINJA
    OPTIONS
        ${FEATURE_OPTIONS}
		-DARROW_BUILD_SHARED=${ARROW_BUILD_SHARED}
        -DARROW_BUILD_STATIC=${ARROW_BUILD_STATIC}
        -DARROW_BUILD_TESTS=OFF
        -DARROW_DEPENDENCY_USE_SHARED=${ARROW_DEPENDENCY_USE_SHARED}
        -DBoost_NO_BOOST_CMAKE=ON
)

# Rétrocompatibilité : Utilisation de vcpkg_install_cmake (ancien standard)
vcpkg_install_cmake()
vcpkg_copy_pdbs()

vcpkg_fixup_pkgconfig()

if(EXISTS "${CURRENT_PACKAGES_DIR}/lib/arrow_static.lib")
    message(FATAL_ERROR "Installed lib file should be named 'arrow.lib' via patching the upstream build.")
endif()

# Rétrocompatibilité : Utilisation de vcpkg_fixup_cmake_targets pour chaque sous-module
if("dataset" IN_LIST FEATURES)
    vcpkg_fixup_cmake_targets(
        CONFIG_PATH lib/cmake/ArrowDataset
        TARGET_PATH share/arrowdataset
        DO_NOT_DELETE_PARENT_CONFIG_PATH
    )
endif()

if("acero" IN_LIST FEATURES)
    vcpkg_fixup_cmake_targets(
        CONFIG_PATH lib/cmake/ArrowAcero
        TARGET_PATH share/arrowacero
        DO_NOT_DELETE_PARENT_CONFIG_PATH
    )
endif()

if("compute" IN_LIST FEATURES)
    vcpkg_fixup_cmake_targets(
        CONFIG_PATH lib/cmake/ArrowCompute
        TARGET_PATH share/arrowcompute
        DO_NOT_DELETE_PARENT_CONFIG_PATH
    )
endif()

if("flight" IN_LIST FEATURES)
    vcpkg_fixup_cmake_targets(
        CONFIG_PATH lib/cmake/ArrowFlight
        TARGET_PATH share/arrowflight
        DO_NOT_DELETE_PARENT_CONFIG_PATH
    )
endif()

if("flightsql" IN_LIST FEATURES)
    vcpkg_fixup_cmake_targets(
        CONFIG_PATH lib/cmake/ArrowFlightSql
        TARGET_PATH share/arrowflightsql
        DO_NOT_DELETE_PARENT_CONFIG_PATH
    )
endif()

if("parquet" IN_LIST FEATURES)
    vcpkg_fixup_cmake_targets(
        CONFIG_PATH lib/cmake/Parquet
        TARGET_PATH share/parquet          # <-- GDAL s'attend à trouver ParquetConfig.cmake dans share/parquet
        DO_NOT_DELETE_PARENT_CONFIG_PATH
    )
endif()

file(GLOB main_configs "${CURRENT_PACKAGES_DIR}/lib/cmake/Arrow/*onfig.cmake")
file(GLOB extra_configs "${CURRENT_PACKAGES_DIR}/lib/cmake/*/*onfig.cmake")
list(REMOVE_ITEM extra_configs ${main_configs})
if(NOT "${extra_configs}" STREQUAL "")
    message("${Z_VCPKG_BACKCOMPAT_MESSAGE_LEVEL}"
        "Unhandled CMake config: ${extra_configs}\n"
        "This might be caused by insufficient feature dependencies in ports/arrow/vcpkg.json."
    )
endif()

# Rétrocompatibilité : Correction finale du dossier de configuration racine d'Arrow
vcpkg_fixup_cmake_targets(CONFIG_PATH lib/cmake/Arrow TARGET_PATH share/arrow)

# Déploiement des fichiers d'usage (aide post-installation)
if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/usage")
    file(INSTALL "${CMAKE_CURRENT_LIST_DIR}/usage" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}")
    foreach(feature IN ITEMS parquet dataset acero compute flight flightsql)
        if(feature IN_LIST FEATURES AND EXISTS "${CMAKE_CURRENT_LIST_DIR}/usage-${feature}")
            file(READ "${CMAKE_CURRENT_LIST_DIR}/usage-${feature}" feature_usage)
            file(APPEND "${CURRENT_PACKAGES_DIR}/share/${PORT}/usage" "${feature_usage}")
        endif()
    endforeach()
endif()

if("example" IN_LIST FEATURES)
    file(INSTALL "${SOURCE_PATH}/cpp/examples/minimal_build/" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}/example")
endif()

# Nettoyage des répertoires temporaires et doublons interdits
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/share")
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/share/doc")

# Rétrocompatibilité : Remplacement de vcpkg_install_copyright par une copie de fichier classique
file(INSTALL "${SOURCE_PATH}/LICENSE.txt" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}" RENAME copyright)
