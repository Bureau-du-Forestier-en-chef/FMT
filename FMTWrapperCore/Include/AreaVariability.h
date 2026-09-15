#ifndef FMTWRAPPERCORE_AREAVARIABILITY_HEADER
#define FMTWRAPPERCORE_AREAVARIABILITY_HEADER

#include "AreaVariabilityTypes.h"
#include "FMTWrapperCoreExport.h"

namespace Models
{
    class FMTModel;
}

namespace FMTWrapperCore
{
    /**
     * @brief Variabilité de l'aire initiale : écrit les sorties d'un modèle dont l'aire
     *        initiale est modifiée selon des proportions par masque.
     *
     * Trois situations font refuser l'opération, sans exception : table absente, table
     * réduite à son en-tête, aucun output demandé. Des masques qui se recoupent sur un
     * même développement lèvent une exception.
     */
    class FMTWRAPPERCOREEXPORT AreaVariability
    {
    public:
        /**
         * @brief Applique les proportions au modèle et écrit ses sorties.
         * @param p_params Paramètres de l'opération.
         * @param p_model Modèle de départ.
         * @return Le résultat ; success vaut false si l'opération a été refusée.
         */
        static AreaVariabilityResults run(
            const AreaVariabilityParameters& p_params,
            const Models::FMTModel& p_model);
    };
}

#endif // FMTWRAPPERCORE_AREAVARIABILITY_HEADER
