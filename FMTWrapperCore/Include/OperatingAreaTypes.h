#ifndef FMTWRAPPERCORE_OPERATINGAREATYPES_HEADER
#define FMTWRAPPERCORE_OPERATINGAREATYPES_HEADER

/**
 * @file OperatingAreaTypes.h
 * @brief OperatingArea DTOs: std types only. This is the translation contract the
 *        wrapper sees, through Controller.h.
 */

#include <string>

namespace FMTWrapper::Backend
{
    /**
     * @brief Parameters of the operating area scheduling.
     */
    struct OperatingAreaParameters
    {
        /** Vector file of the operating areas. */
        std::string vectorFilePath;
        /** Solver, a Models::FMTSolverInterface value. */
        int solver = 0;
        /** Number of periods of the model (LENGTH parameter). */
        int numberOfPeriods = 0;
        /** Number of threads allocated to the task. */
        int numberOfThreads = 1;
        /** Number of the theme holding the operating areas, as FMTAreaParser::getOperatingArea expects it. */
        int themeNumber = 0;
        /** Maximum time given to the heuristic. */
        int maximumTime = 0;
        /** Maximum number of iterations of the heuristic. */
        int numberOfIterations = 0;
        /** Field of the vector file holding the age. */
        std::string ageField;
        /** Field of the vector file holding the area. */
        std::string areaField;
        /** Field of the vector file holding the lock. */
        std::string lockField;
        /** Parameters file of the operating areas. */
        std::string parametersFilePath;
        /** Result folder: the task writes its final model there, and its other outputs under the Retour location. */
        std::string resultFolder;
        /** Update period (UPDATE parameter of the model). */
        int updatePeriod = 0;
        /** Name of the return time output; empty to use none. */
        std::string returnTimeOutputName;
    };

    /**
     * @brief Result of the operating area scheduling.
     */
    struct OperatingAreaResults
    {
        /** false when the model does not lend itself to the scheduling. */
        bool success = false;
        /** Reason for the refusal when success is false, empty otherwise. */
        std::string errorMessage;
    };
}

#endif // FMTWRAPPERCORE_OPERATINGAREATYPES_HEADER
