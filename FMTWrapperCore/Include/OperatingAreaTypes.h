#ifndef FMTWRAPPERCORE_OPERATINGAREATYPES_HEADER
#define FMTWRAPPERCORE_OPERATINGAREATYPES_HEADER

/**
 * @file OperatingAreaTypes.h
 * @brief DTO de OperatingArea : types std uniquement. C'est le contrat de traduction que le
 *        wrapper voit, par Controller.h.
 */

#include <string>

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
}

#endif // FMTWRAPPERCORE_OPERATINGAREATYPES_HEADER
