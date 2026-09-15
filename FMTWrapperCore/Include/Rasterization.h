#ifndef FMTWRAPPERCORE_RASTERIZATION_HEADER
#define FMTWRAPPERCORE_RASTERIZATION_HEADER

#include "RasterizationTypes.h"
#include "FMTWrapperCoreExport.h"

namespace Models
{
    class FMTModel;
}

namespace FMTWrapperCore
{
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
    class FMTWRAPPERCOREEXPORT Rasterization
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
    };
}

#endif // FMTWRAPPERCORE_RASTERIZATION_HEADER
