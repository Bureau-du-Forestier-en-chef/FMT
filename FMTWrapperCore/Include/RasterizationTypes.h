#ifndef FMTWRAPPERCORE_RASTERIZATIONTYPES_HEADER
#define FMTWRAPPERCORE_RASTERIZATIONTYPES_HEADER

/**
 * @file RasterizationTypes.h
 * @brief DTO de Rasterization : types std uniquement. C'est le contrat de traduction que le
 *        wrapper voit, par Controller.h.
 */

#include <string>

namespace FMTWrapperCore
{
    /**
     * @brief Paramètres de la rastérisation d'un fichier vectoriel.
     */
    struct RasterizationParameters
    {
        /** Fichier vectoriel à rastériser. */
        std::string vectorFilePath;
        /** Dossier où écrire les rasters de la forêt. */
        std::string outputFolder;
        /** Résolution des rasters produits (taille d'une cellule). */
        int resolution = 0;
        /** Champ du fichier vectoriel portant l'âge. */
        std::string ageField;
        /** Champ du fichier vectoriel portant la superficie. */
        std::string areaField;
        /** Champ du fichier vectoriel portant le verrou ; vide s'il n'y en a pas. */
        std::string lockField;
    };
}

#endif // FMTWRAPPERCORE_RASTERIZATIONTYPES_HEADER
