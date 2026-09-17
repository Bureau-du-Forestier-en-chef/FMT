#ifndef FMTWRAPPERCORE_SES_HEADER
#define FMTWRAPPERCORE_SES_HEADER

#include <string>
#include <vector>

#include "FMTWrapperCoreExport.h"
#include "SESTypes.h"

namespace Core {
    class FMTSchedule;
    class FMTConstraint;
    class FMTOutput;
    class FMTTheme;
    class FMTAction;
}

namespace Models {
    class FMTModel;
    class FMTSeModel;
}

namespace Spatial {
    class FMTSpatialSchedule;
}

namespace FMTWrapperCore
{
    /**
     * @brief Class for spatially explicit simulations
     */
    class FMT_WRAPPER_CORE_EXPORT SES
    {
    public:
        /**
         * @brief Runs a spatially explicit simulation the way the interface launches it
         * @param params Simulation parameters; the schedules are read from
         *        params.primaryFilePath
         * @param baseModel Base FMT model
         * @return Complete simulation results
         *
         * Reads the model schedules, passes a clone of the current logger back to the model,
         * then logs the progress around the simulation itself, done by
         * RunSES(const SESParameters&, const Models::FMTModel&, const std::vector<Core::FMTSchedule>&).
         */
        static SESResults RunSES(
            const SESParameters& params,
            const Models::FMTModel& baseModel);

        /**
         * @brief Runs a complete spatially explicit simulation
         * @param params Simulation parameters
         * @param baseModel Base FMT model
         * @param schedules Schedules to use for the simulation
         * @return Complete simulation results
         *
         * This method orchestrates the whole simulation and can be called directly from
         * pure C++ code for testing and debugging.
         */
        static SESResults RunSES(
            const SESParameters& params,
            const Models::FMTModel& baseModel,
            const std::vector<Core::FMTSchedule>& schedules);

        /**
         * @brief Runs a spatial optimization (Simulated Annealing)
         * @param params Optimization parameters
         * @param baseModel Base FMT model (already loaded)
         * @return Complete optimization results
         *
         * This method orchestrates the whole spatial optimization, logs its progress and
         * outputs, and can be called directly from pure C++ code for testing and debugging.
         */
        static SAResults RunOptimization(
            const SAParameters& params,
            const Models::FMTModel& baseModel);

    private:
        /**
         * @brief Builds the growth themes from their indices
         * @param allThemes All the model themes
         * @param themeIndices Theme indices (1-based)
         * @return Vector of the selected themes
         */
        static std::vector<Core::FMTTheme> buildGrowthThemes(
            const std::vector<Core::FMTTheme>& allThemes,
            const std::vector<int>& themeIndices);

        /**
         * @brief Converts every transition of the model into a "single" transition
         * @param model The model whose transitions are modified in place
         */
        static void applySingleTransitions(Models::FMTModel& model);

        /**
         * @brief Builds the raster paths, reads the initial forest and assigns it to the
         *        model.
         * @param model The SES model receiving the initial forest
         * @param rastersPath Base path of the rasters
         * @param useStanlock true to use the STANLOCK raster
         * @param[out] ageRasterPath Path of the built age raster
         * @param[out] themeRasterPaths Paths of the built theme rasters
         */
        static void prepareInitialForest(
            Models::FMTSeModel& model,
            const std::string& rastersPath,
            bool useStanlock,
            std::string& ageRasterPath,
            std::vector<std::string>& themeRasterPaths);

        /**
         * @brief Generates the report of the infeasible constraints
         * @param semodel The SES model to analyze
         * @return Vector of infeasibility messages (one per broken constraint, followed by
         *         the global percentage of infeasible constraints)
         */
        static std::vector<std::string> generateInfeasibilityReport(const Models::FMTSeModel& semodel);

        /**
         * @brief Generates the spatial carbon report
         * @param semodel The SES model
         * @param numberOfPeriods Number of periods
         * @param schedules Vector of the original schedules
         * @return Structure holding the report data
         */
        static CarbonReportData generateCarbonReport(
            const Models::FMTSeModel& semodel,
            const int numberOfPeriods,
            const std::vector<Core::FMTSchedule>& schedules);

        /**
         * @brief Writes the disturbances (GCBM transitions)
         * @param semodel The SES model
         * @param outputBasePath Base path of the outputs
         * @param numberOfPeriods Number of periods
         * @param growthThemeIndices Indices of the growth themes (1-based)
         * @return Vector of the paths of the created transition files
         */
        static std::vector<std::string> writeDisturbances(
            const Models::FMTSeModel& semodel,
            const std::string& outputBasePath,
            const int numberOfPeriods,
            const std::vector<int>& growthThemeIndices);

        /**
         * @brief Generates the events data
         * @param semodel The SES model
         * @return Structure holding the events statistics
         */
        static EventsData generateEventsData(const Models::FMTSeModel& semodel);

        /**
         * @brief Generates the events data and writes it to a file
         * @param semodel The SES model
         * @param eventsFilePath Full path of the events file to write
         * @return Structure holding the events statistics
         */
        static EventsData writeEventsFile(
            const Models::FMTSeModel& semodel,
            const std::string& eventsFilePath);

        /**
         * @brief Computes the outputs for every period
         * @param semodel The SES model
         * @param outputNames Names of the outputs to compute
         * @param numberOfPeriods Number of periods
         * @param[out] selectedOutputs Outputs selected by outputNames, in the model order
         * @return Structure holding the outputs results
         */
        static OutputsData calculateOutputs(
            const Models::FMTSeModel& semodel,
            const std::vector<std::string>& outputNames,
            const int numberOfPeriods,
            std::vector<Core::FMTOutput>& selectedOutputs);

        /**
         * @brief Writes the spatial outputs
         * @param semodel The SES model
         * @param outputs Vector of the outputs to write
         * @param minPeriod Minimum period
         * @param maxPeriod Maximum period
         * @param outputPath Output path
         * @return Vector of the paths of the created raster files
         */
        static std::vector<std::string> writeSpatialOutputs(
            const Models::FMTSeModel& semodel,
            const std::vector<Core::FMTOutput>& outputs,
            const int minPeriod,
            const int maxPeriod,
            const std::string& outputPath);

        /**
         * @brief Computes the predictors
         * @param semodel The SES model
         * @param rasterPath Path of the rasters
         * @param numberOfPeriods Number of periods
         * @param predictorYields Names of the yields used as predictors
         * @return Structure holding the predictors data
         */
        static PredictorsData calculatePredictors(
            const Models::FMTSeModel& semodel,
            const std::string& rasterPath,
            const int numberOfPeriods,
            const std::vector<std::string>& predictorYields);

        /**
         * @brief Writes the schedule to a file
         * @param semodel The SES model
         * @param outputPath Path of the output file
         * @return Full path of the created file
         */
        static std::string writeSchedule(
            const Models::FMTSeModel& semodel,
            const std::string& outputPath);

        /**
         * @brief Writes the updated forest (rasters)
         * @param semodel The SES model
         * @param rasterPath Base path of the rasters
         * @param themeRasterPaths Paths of the theme rasters
         * @param ageRasterPath Path of the age raster
         * @param stanlockRasterPath Path of the stanlock raster
         */
        static void writeUpdatedForest(
            const Models::FMTSeModel& semodel,
            const std::string& rasterPath,
            const std::vector<std::string>& themeRasterPaths,
            const std::string& ageRasterPath,
            const std::string& stanlockRasterPath);

        /**
         * @brief Exports the results through the model parser
         * @param semodel The SES model
         * @param outputs Vector of the outputs
         * @param minPeriod Minimum period
         * @param maxPeriod Maximum period
         * @param outputPath Output path
         * @param outputLevel Detail level of the outputs
         * @param gdalProvider GDAL provider to use
         */
        static void exportResults(
            const Models::FMTSeModel& semodel,
            const std::vector<Core::FMTOutput>& outputs,
            const int minPeriod,
            const int maxPeriod,
            const std::string& outputPath,
            const int outputLevel,
            const std::string& gdalProvider);
    };
}

#endif // FMTWRAPPERCORE_SES_HEADER