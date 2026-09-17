#ifndef FMTWRAPPERCORE_PLANNINGTYPES_HEADER
#define FMTWRAPPERCORE_PLANNINGTYPES_HEADER

/**
 * @file PlanningTypes.h
 * @brief Planning DTOs: std types only. This is the translation contract the wrapper
 *        sees, through Controller.h.
 */

#include <string>
#include <vector>

namespace FMTWrapper::Backend
{
    /**
     * @brief Parameters of the planning of several scenarios.
     */
    struct PlanningParameters
    {
        /** Project .pri file: schedules are read back from it, and those of the optimized scenarios are written to it. */
        std::string primaryFilePath;
        /** Solver, a Models::FMTSolverInterface value. */
        int solver = 0;
        /** Number of periods to plan (LENGTH parameter). */
        int numberOfPeriods = 0;
        /** Number of threads, shared among the scenarios. */
        int numberOfThreads = 1;
        /** Names of the outputs to write. */
        std::vector<std::string> outputNames;
        /** Output level, a Core::FMToutputlevel value. */
        int outputLevel = 0;
        /** First written period. */
        int outputMinPeriod = 0;
        /** Last written period. */
        int outputMaxPeriod = 0;
        /** Output location. */
        std::string outputPath;
        /** GDAL driver of the outputs. */
        std::string gdalProvider;
    };

    /**
     * @brief Parameters of the replanning.
     */
    struct ReplanningParameters
    {
        /** Solver of the global and local models, a Models::FMTSolverInterface value. */
        int solver = 0;
        /** Number of periods of the global model (LENGTH parameter). */
        int numberOfPeriods = 0;
        /** Number of replanning periods. */
        int replanningPeriods = 0;
        /** Minimal drift ("variabilité" in the interface). */
        double minimalDrift = 0.0;
        /** Number of threads allocated to the task. */
        int numberOfThreads = 1;
        /** Minimum number of replicates. */
        int minimumReplicates = 0;
        /** Maximum number of replicates. */
        int maximumReplicates = 0;
        /** Names of the outputs to write, taken from the outputs of the global model. */
        std::vector<std::string> outputNames;
        /** Output level, a Core::FMToutputlevel value. */
        int outputLevel = 0;
        /** Output location. */
        std::string outputPath;
        /** GDAL driver of the outputs. */
        std::string gdalProvider;
        /** Level of the interface logger while the tasks run. */
        int taskLogLevel = 0;
        /** true to write one schedule per replicate ("produire la solution" in the interface). */
        bool writeSchedules = false;
    };
}

#endif // FMTWRAPPERCORE_PLANNINGTYPES_HEADER
