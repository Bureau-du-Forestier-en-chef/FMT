#ifndef FMTWRAPPERCORE_EXPORT_HEADER
#define FMTWRAPPERCORE_EXPORT_HEADER

/**
 * @file FMTWrapperCoreExport.h
 * @brief Visibilité des classes de FMTWrapperCore à la frontière de la bibliothèque.
 *
 * FMTWRAPPERCOREEXPORT vaut __declspec(dllexport) pendant la compilation de
 * FMTWrapperCore, et __declspec(dllimport) chez ses clients (FMTWrapper, exécutables de
 * test). CMake définit FMTWrapperCore_EXPORTS pour la seule cible SHARED qui construit la
 * bibliothèque : l'import est le cas par défaut, et aucun client n'a de drapeau à ajouter.
 *
 * FMTEXPORT (FMTutility.h) ne convient pas : il appartient à FMTlib, et vaut
 * __declspec(dllimport) dans FMTWrapperCore, puisque cette cible définit FMTLIBIMPORT.
 *
 * _WIN32 couvre MSVC et MinGW. Ailleurs, la macro garde les classes visibles même si la
 * bibliothèque est compilée avec -fvisibility=hidden.
 *
 * Pour une compilation statique, définir FMTWRAPPERCOREEXPORT vide avant toute inclusion.
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
