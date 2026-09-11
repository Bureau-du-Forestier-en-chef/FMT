#ifndef FMTWRAPPERCORE_RASTERIZATION_HEADER
#define FMTWRAPPERCORE_RASTERIZATION_HEADER

#include <string>

namespace Models
{
    class FMTModel;
}

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

    /**
     * @brief Rastérisation d'un fichier vectoriel selon les thèmes d'un modèle.
     *
     * Écrit dans le dossier de sortie les rasters de la forêt (AGE.tif, THEME<i>.tif,
     * et STANLOCK.tif le cas échéant) : ce sont ceux que relisent SES et
     * ModelQuery::getAllMasks.
     *
     * L'opération ne produit pas de données en mémoire : elle ne retourne rien et
     * les erreurs remontent par exception.
     */
    class __declspec(dllexport) Rasterization
    {
    public:
        /**
         * @brief Rastérise le fichier vectoriel selon les thèmes du modèle.
         * @param p_params Paramètres de la rastérisation.
         * @param p_model Modèle fournissant les thèmes et l'aire initiale.
         */
        static void rasterize(
            const RasterizationParameters& p_params,
            const Models::FMTModel& p_model);

        /**
         * @brief Rastérise le fichier vectoriel selon un scénario du cache.
         * @param p_params Paramètres de la rastérisation.
         * @param p_modelIndex Index du scénario dans FMTFormCache.
         */
        static void rasterize(
            const RasterizationParameters& p_params,
            int p_modelIndex);
    };
}

#endif // FMTWRAPPERCORE_RASTERIZATION_HEADER
