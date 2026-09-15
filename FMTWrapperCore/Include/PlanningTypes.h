#ifndef FMTWRAPPERCORE_PLANNINGTYPES_HEADER
#define FMTWRAPPERCORE_PLANNINGTYPES_HEADER

/**
 * @file PlanningTypes.h
 * @brief DTO de Planning : types std uniquement. C'est le contrat de traduction que le
 *        wrapper voit, par Controller.h.
 */

#include <string>
#include <vector>

namespace FMTWrapperCore
{
    /**
     * @brief Paramètres de la planification de plusieurs scénarios.
     */
    struct PlanningParameters
    {
        /** Fichier .pri du projet : les cédules y sont relues, et celles des scénarios optimisés y sont écrites. */
        std::string primaryFilePath;
        /** Solveur, valeur de Models::FMTSolverInterface. */
        int solver = 0;
        /** Nombre de périodes à planifier (paramètre LENGTH). */
        int numberOfPeriods = 0;
        /** Nombre de threads, partagés entre les scénarios. */
        int numberOfThreads = 1;
        /** Noms des outputs à écrire. */
        std::vector<std::string> outputNames;
        /** Niveau des sorties, valeur de Core::FMToutputlevel. */
        int outputLevel = 0;
        /** Première période écrite. */
        int outputMinPeriod = 0;
        /** Dernière période écrite. */
        int outputMaxPeriod = 0;
        /** Emplacement des sorties. */
        std::string outputPath;
        /** Pilote GDAL des sorties. */
        std::string gdalProvider;
    };

    /**
     * @brief Paramètres de la replanification.
     */
    struct ReplanningParameters
    {
        /** Solveur des modèles global et local, valeur de Models::FMTSolverInterface. */
        int solver = 0;
        /** Nombre de périodes du modèle global (paramètre LENGTH). */
        int numberOfPeriods = 0;
        /** Nombre de périodes de replanification. */
        int replanningPeriods = 0;
        /** Dérive minimale (« variabilité » dans l'interface). */
        double minimalDrift = 0.0;
        /** Nombre de threads alloués à la tâche. */
        int numberOfThreads = 1;
        /** Nombre minimum de réplicats. */
        int minimumReplicates = 0;
        /** Nombre maximum de réplicats. */
        int maximumReplicates = 0;
        /** Noms des outputs à écrire, pris parmi ceux du modèle global. */
        std::vector<std::string> outputNames;
        /** Niveau des sorties, valeur de Core::FMToutputlevel. */
        int outputLevel = 0;
        /** Emplacement des sorties. */
        std::string outputPath;
        /** Pilote GDAL des sorties. */
        std::string gdalProvider;
        /** Niveau du logger de l'interface pendant l'exécution des tâches. */
        int taskLogLevel = 0;
        /** true pour écrire une cédule par réplicat (« produire la solution » dans l'interface). */
        bool writeSchedules = false;
    };
}

#endif // FMTWRAPPERCORE_PLANNINGTYPES_HEADER
