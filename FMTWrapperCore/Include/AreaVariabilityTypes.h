#ifndef FMTWRAPPERCORE_AREAVARIABILITYTYPES_HEADER
#define FMTWRAPPERCORE_AREAVARIABILITYTYPES_HEADER

/**
 * @file AreaVariabilityTypes.h
 * @brief AreaVariability DTOs: std types only. This is the translation contract the
 *        wrapper sees, through Controller.h.
 */

#include <string>
#include <vector>

namespace FMTWrapper::Backend
{
    /**
     * @brief Parameters of the initial area variability.
     */
    struct AreaVariabilityParameters
    {
        /** Solver, a Models::FMTSolverInterface value. */
        int solver = 0;
        /** Selected constraints, by their text representation. */
        std::vector<std::string> constraintNames;
        /** Number of periods to build. */
        int numberOfPeriods = 0;
        /** Names of the outputs to write; the operation is refused if the list is empty. */
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
        /**
         * Proportions table, as read from the parameters file: the first row is a header,
         * the last column of each row is the proportion and the previous columns form the
         * mask.
         */
        std::vector<std::vector<std::string>> proportionsTable;
        /** false if the interface sent no table. */
        bool proportionsTableProvided = false;
    };

    /**
     * @brief Result of the initial area variability.
     */
    struct AreaVariabilityResults
    {
        /** false when the operation was refused. */
        bool success = false;
        /** Reason for the refusal when success is false, empty otherwise. */
        std::string errorMessage;
    };
}

#endif // FMTWRAPPERCORE_AREAVARIABILITYTYPES_HEADER
