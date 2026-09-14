#ifndef FMTWRAPPERCORE_OPERATINGAREA_HEADER
#define FMTWRAPPERCORE_OPERATINGAREA_HEADER

#include <string>

namespace Models
{
    class FMTModel;
}

namespace FMTWrapperCore
{
    /**
     * @brief Paramètres de la planification des aires d'opération.
     */
    struct OperatingAreaParameters
    {
        /** Fichier vectoriel des aires d'opération. */
        std::string vectorFilePath;
        /** Solveur, valeur de Models::FMTSolverInterface. */
        int solver = 0;
        /** Nombre de périodes du modèle (paramètre LENGTH). */
        int numberOfPeriods = 0;
        /** Nombre de threads alloués à la tâche. */
        int numberOfThreads = 1;
        /** Numéro du thème qui porte les aires d'opération, tel que l'attend FMTAreaParser::getOperatingArea. */
        int themeNumber = 0;
        /** Temps maximum accordé à l'heuristique. */
        int maximumTime = 0;
        /** Nombre maximum d'itérations de l'heuristique. */
        int numberOfIterations = 0;
        /** Champ du fichier vectoriel portant l'âge. */
        std::string ageField;
        /** Champ du fichier vectoriel portant la superficie. */
        std::string areaField;
        /** Champ du fichier vectoriel portant le verrou. */
        std::string lockField;
        /** Fichier de paramètres des aires d'opération. */
        std::string parametersFilePath;
        /** Dossier de résultat : la tâche y écrit son modèle final, et ses autres sorties sous l'emplacement Retour. */
        std::string resultFolder;
        /** Période de mise à jour (paramètre UPDATE du modèle). */
        int updatePeriod = 0;
        /** Nom de l'output de temps de retour ; vide pour n'en utiliser aucun. */
        std::string returnTimeOutputName;
    };

    /**
     * @brief Résultat de la planification des aires d'opération.
     */
    struct OperatingAreaResults
    {
        /** false quand le modèle ne se prête pas à la planification. */
        bool success = false;
        /** Motif du refus quand success vaut false, vide sinon. */
        std::string errorMessage;
    };

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
    class __declspec(dllexport) OperatingArea
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

        /**
         * @brief Planifie les aires d'opération d'un scénario du cache.
         * @param p_params Paramètres de la planification.
         * @param p_modelIndex Index du scénario dans FMTFormCache.
         * @return Le résultat ; success vaut false si le modèle a été refusé.
         */
        static OperatingAreaResults schedule(
            const OperatingAreaParameters& p_params,
            int p_modelIndex);
    };
}

#endif // FMTWRAPPERCORE_OPERATINGAREA_HEADER
