#ifndef FMTWRAPPERCORE_OPERATINGAREA_HEADER
#define FMTWRAPPERCORE_OPERATINGAREA_HEADER

#include "OperatingAreaTypes.h"
#include "FMTWrapperCoreExport.h"

namespace Models
{
    class FMTModel;
}

namespace FMTWrapperCore
{
    /**
     * @brief Planification des aires d'opération : génère le calendrier de COS.
     *
     * Ajoute l'agrégat réservé ~BFECOPTOUTPUTYOUVERT~ aux actions qui utilisent le
     * yield YOUVERT, lit les aires d'opération, puis lance FMTOpAreaSchedulerTask.
     *
     * Deux situations du modèle font refuser la planification, sans exception : une
     * action porte déjà l'agrégat réservé, ou aucune action n'utilise YOUVERT. Le motif
     * est alors dans OperatingAreaResults::errorMessage, et journalisé. Toute autre
     * erreur remonte par exception.
     */
    class FMTWRAPPERCOREEXPORT OperatingArea
    {
    public:
        /**
         * @brief Planifie les aires d'opération du modèle.
         * @param p_params Paramètres de la planification.
         * @param p_model Modèle à planifier.
         * @return Le résultat ; success vaut false si le modèle a été refusé.
         */
        static OperatingAreaResults schedule(
            const OperatingAreaParameters& p_params,
            const Models::FMTModel& p_model);
    };
}

#endif // FMTWRAPPERCORE_OPERATINGAREA_HEADER
