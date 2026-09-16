#ifndef FMTWRAPPERCORE_EXPORT_HEADER
#define FMTWRAPPERCORE_EXPORT_HEADER

/**
 * @file FMTWrapperCoreExport.h
 * @brief Visibility of the FMTWrapperCore classes at the library boundary.
 *
 * FMTWRAPPERCOREEXPORT is __declspec(dllexport) while FMTWrapperCore is compiled, and
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
 * For a static build, define FMTWRAPPERCOREEXPORT as empty before any inclusion.
 */
#ifndef FMTWRAPPERCOREEXPORT
    #if defined(_WIN32)
        #if defined(FMTWrapperCore_EXPORTS)
            #define FMTWRAPPERCOREEXPORT __declspec(dllexport)
        #else
            #define FMTWRAPPERCOREEXPORT __declspec(dllimport)
        #endif
    #else
        #define FMTWRAPPERCOREEXPORT __attribute__((visibility("default")))
    #endif
#endif

#endif // FMTWRAPPERCORE_EXPORT_HEADER
