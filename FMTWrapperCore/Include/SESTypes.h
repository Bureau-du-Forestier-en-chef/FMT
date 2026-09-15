#ifndef FMTWRAPPERCORE_SESTYPES_HEADER
#define FMTWRAPPERCORE_SESTYPES_HEADER

/**
 * @file SESTypes.h
 * @brief DTO de SES : types std uniquement. C'est le contrat de traduction que le
 *        wrapper voit, par Controller.h.
 */

#include <map>
#include <string>
#include <vector>

namespace FMTWrapperCore
{
    /**
     * @brief Structure pour les résultats du rapport de carbone spatial
     */
    struct CarbonReportData
    {
        struct PeriodData
        {
            int period;
            double objectiveValue;
            double primalInfeasibility;
            double totalRatio;
            std::map<std::string, double> actionRatios; // actionName -> ratio
        };

        std::vector<PeriodData> periods;
    };

    /**
     * @brief Structure pour les informations d'événements
     */
    struct EventsData
    {
        std::string statistics; // Contient toutes les stats formatées
    };

    /**
     * @brief Structure pour les résultats des outputs
     */
    struct OutputsData
    {
        struct OutputResult
        {
            std::string outputName;
            std::map<int, double> periodValues; // period -> value
        };

        std::vector<OutputResult> results;
    };

    /**
     * @brief Structure pour les prédicteurs
     */
    struct PredictorsData
    {
        struct PredictorNode
        {
            int period;
            int nodeIndex;
            std::vector<double> values;
        };

        std::vector<std::string> predictorNames;
        std::vector<PredictorNode> nodes;
    };

    /**
     * @brief Paramètres pour la simulation spatiale explicite
     */
    struct SESParameters
    {
        std::string primaryFilePath;
        std::string rastersPath;
        std::string scenarioName;
        std::vector<std::string> constraintNames;
        int numberOfPeriods;
        int greedySearchIterations;
        std::vector<std::string> outputNames;
        bool useStanlock;
        int outputLevel;
        int outputMinPeriod;
        int outputMaxPeriod;
        std::string outputPath;
        bool generateEvents;
        bool generateSpatialOutputs;
        std::string gdalProvider;
        bool carbonMode;
        std::vector<std::string> predictorYields;
        std::vector<int> growthThemes;
    };

    /**
     * @brief Résultats complets de la simulation
     */
    struct SESResults
    {
        bool success;
        std::string errorMessage;

        // Rapports
        std::vector<std::string> infeasibilityMessages;
        CarbonReportData carbonReport;  

        // Fichiers créés
        std::vector<std::string> disturbanceFiles;
        EventsData eventsData;
        std::string eventsFilePath;

        // Outputs
        OutputsData outputsData;
        std::string scheduleFilePath;
        std::vector<std::string> spatialOutputFiles;

        // Prédicteurs
        PredictorsData predictorsData;

        SESResults() : success(false) {}
    };

    /**
     * @brief Paramètres pour l'optimisation spatiale (Simulated Annealing)
     */
    struct SAParameters
    {
        std::string rastersPath;
        std::string scenarioName;
        std::vector<std::string> constraintNames;
        int numberOfPeriods;
        int maxMoves;
        int maxAcceptedMoves;
        int maxCycleMoves;
        std::vector<std::string> outputNames;
        bool useStanlock;
        int outputLevel;
        int outputMinPeriod;
        int outputMaxPeriod;
        std::string outputPath;
        bool generateEvents;
        bool generateSpatialOutputs;
        std::string gdalProvider;
    };

    /**
     * @brief Résultats de l'optimisation spatiale
     */
    struct SAResults
    {
        bool success;
        std::string errorMessage;

        // Rapports
        std::vector<std::string> infeasibilityMessages;

        // Fichiers créés
        std::vector<std::string> disturbanceFiles;
        EventsData eventsData;
        std::string eventsFilePath;

        // Outputs
        OutputsData outputsData;
        std::string scheduleFilePath;
        std::vector<std::string> spatialOutputFiles;

        SAResults() : success(false) {}
    };
}

#endif // FMTWRAPPERCORE_SESTYPES_HEADER
