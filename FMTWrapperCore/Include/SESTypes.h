#ifndef FMTWRAPPERCORE_SESTYPES_HEADER
#define FMTWRAPPERCORE_SESTYPES_HEADER

/**
 * @file SESTypes.h
 * @brief SES DTOs: std types only. This is the translation contract the wrapper sees,
 *        through Controller.h.
 */

#include <map>
#include <string>
#include <vector>

namespace FMTWrapper::Backend
{
    /**
     * @brief Structure for the results of the spatial carbon report
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
     * @brief Structure for the events information
     */
    struct EventsData
    {
        std::string statistics; // Holds every formatted statistic
    };

    /**
     * @brief Structure for the outputs results
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
     * @brief Structure for the predictors
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
     * @brief Parameters of the spatially explicit simulation
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
     * @brief Complete results of the simulation
     */
    struct SESResults
    {
        bool success;
        std::string errorMessage;

        // Reports
        std::vector<std::string> infeasibilityMessages;
        CarbonReportData carbonReport;  

        // Created files
        std::vector<std::string> disturbanceFiles;
        EventsData eventsData;
        std::string eventsFilePath;

        // Outputs
        OutputsData outputsData;
        std::string scheduleFilePath;
        std::vector<std::string> spatialOutputFiles;

        // Predictors
        PredictorsData predictorsData;

        SESResults() : success(false) {}
    };

    /**
     * @brief Parameters of the spatial optimization (Simulated Annealing)
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
     * @brief Results of the spatial optimization
     */
    struct SAResults
    {
        bool success;
        std::string errorMessage;

        // Reports
        std::vector<std::string> infeasibilityMessages;

        // Created files
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
