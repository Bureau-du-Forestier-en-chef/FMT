#ifndef FMTWRAPPERCORE_PLANNING_HEADER
#define FMTWRAPPERCORE_PLANNING_HEADER

#include <functional>
#include <string>
#include <vector>

#include "FMTWrapperCoreExport.h"
#include "PlanningTypes.h"

namespace Models
{
    class FMTModel;
}

namespace FMTWrapperCore
{
    /**
     * @brief Planification et replanification de scénarios, par les tâches parallèles de FMT.
     *
     * Les deux opérations écrivent leurs résultats sur disque et ne retournent rien ; les
     * erreurs remontent par exception.
     */
    class FMTWRAPPERCOREEXPORT Planning
    {
    public:
        /**
         * @brief Signale l'erreur en cours de traitement, sans interrompre l'opération.
         *
         * Appelée depuis un bloc catch, l'exception étant encore active, avec la méthode,
         * la ligne et le fichier où elle a été attrapée.
         */
        using ErrorReporter = std::function<void(const std::string&, int, const std::string&)>;

        /**
         * @brief Planifie des scénarios dans une même FMTPlanningTask.
         *
         * Chaque scénario est optimisé, ou rejoué à partir de sa cédule si son drapeau de
         * relecture est levé (construction partielle forcée). La tâche écrit dans le projet
         * la cédule des scénarios optimisés.
         *
         * Deux comportements historiques sont conservés :
         * - un même modèle présent deux fois dans p_models reprend le drapeau de relecture de
         *   sa première occurrence ;
         * - une relecture de cédule en échec est confiée à p_reportScheduleError, puis le
         *   scénario est lancé avec une cédule vide. Sans p_reportScheduleError, l'erreur
         *   remonte.
         *
         * @param p_params Paramètres de la planification.
         * @param p_models Modèles à planifier.
         * @param p_playback Drapeau de relecture de chaque modèle, par position.
         * @param p_reportScheduleError Reçoit les relectures de cédule en échec.
         */
        static void plan(
            const PlanningParameters& p_params,
            const std::vector<const Models::FMTModel*>& p_models,
            const std::vector<bool>& p_playback,
            const ErrorReporter& p_reportScheduleError);

        /**
         * @brief Replanifie à partir des modèles stratégique, stochastique et tactique.
         *
         * Pendant l'exécution des tâches, le logger de l'interface (FMTFormLogger) passe au
         * niveau p_params.taskLogLevel ; son niveau par défaut est rétabli ensuite, même en
         * cas d'erreur. Un autre logger n'est pas touché.
         *
         * @param p_params Paramètres de la replanification.
         * @param p_strategic Modèle global (stratégique).
         * @param p_stochastic Modèle stochastique.
         * @param p_tactical Modèle local (tactique).
         */
        static void replan(
            const ReplanningParameters& p_params,
            const Models::FMTModel& p_strategic,
            const Models::FMTModel& p_stochastic,
            const Models::FMTModel& p_tactical);
    };
}

#endif // FMTWRAPPERCORE_PLANNING_HEADER
