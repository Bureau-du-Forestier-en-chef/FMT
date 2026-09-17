#ifndef FMTWRAPPERCORE_EXPORT_HEADER
#define FMTWRAPPERCORE_EXPORT_HEADER

/**
 * @file FMTWrapperCoreExport.h
 * @brief Visibility of the FMTWrapperCore classes at the library boundary.
 *
 * FMT_WRAPPER_CORE_EXPORT is __declspec(dllexport) while FMTWrapperCore is compiled, and
 * __declspec(dllimport) in its clients (FMTWrapper, test executables). CMake defines
 * FMTWrapperCore_EXPORTS only for the SHARED target that builds the library: import is the
 * default, and no client has a flag to add.
 *
 * FMTEXPORT (FMTutility.h) does not fit: it belongs to FMTlib, and it is
 * __declspec(dllimport) inside FMTWrapperCore, since that target defines FMTLIBIMPORT.
 *
 * _WIN32 covers MSVC and MinGW. Elsewhere, the macro keeps the classes visible even if the
 * library is compiled with -fvisibility=hidden.
 *
 * For a static build, define FMT_WRAPPER_CORE_EXPORT as empty before any inclusion.
 */
#ifndef FMT_WRAPPER_CORE_EXPORT
    #if defined(_WIN32)
        #if defined(FMTWrapperCore_EXPORTS)
            #define FMT_WRAPPER_CORE_EXPORT __declspec(dllexport)
        #else
            #define FMT_WRAPPER_CORE_EXPORT __declspec(dllimport)
        #endif
    #else
        #define FMT_WRAPPER_CORE_EXPORT __attribute__((visibility("default")))
    #endif
#endif

#endif // FMTWRAPPERCORE_EXPORT_HEADER
