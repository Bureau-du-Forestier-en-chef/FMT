#ifndef FMTWRAPPERCORE_CONTROLLER_HEADER
#define FMTWRAPPERCORE_CONTROLLER_HEADER

#include <set>
#include <string>
#include <vector>

#include "AreaVariabilityTypes.h"
#include "FMTWrapperCoreExport.h"
#include "OperatingAreaTypes.h"
#include "PlanningTypes.h"
#include "RasterizationTypes.h"
#include "SESTypes.h"

namespace FMTWrapper::Backend
{
    /**
     * @brief Facade controller (GRASP): the wrapper's single entry point into the Core.
     *
     * Each method matches a system operation of FMTForm. It receives std types, DTOs and
     * scenario indexes, resolves the scenario in ModelCache, then delegates to the pure
     * entry point of a service: Environment, ModelQuery, Transformation, SES,
     * Rasterization, OperatingArea, AreaVariability or Planning. The controller
     * coordinates without computing: the logic stays in the services, which the tests call
     * directly, without going through the cache.
     *
     * No FMTlib type appears in this interface: the wrapper includes only this header, and
     * therefore cannot handle any FMT object.
     *
     * The controller is stateless. The session state (loaded scenarios, interface logger
     * and exception handler) lives in ModelCache.
     *
     * Every operation that takes a scenario index raises an FMTrangeerror exception if the
     * cache is empty, except getOutputsNames, which then returns an empty list.
     */
    class FMT_WRAPPER_CORE_EXPORT Controller
    {
    public:
        // Session: log, errors and loaded scenarios.

        /**
         * @brief Installs the interface logger.
         * @param p_logFilePath Log file.
         * @param p_callback Native function pointer of the managed delegate that receives
         *        each message.
         */
        static void initializeLogger(const std::string& p_logFilePath, void* p_callback);

        /**
         * @brief Rebuilds the logger and the exception handler after a crash, with the
         *        configuration kept by ModelCache.
         * @param p_callback New function pointer of the managed delegate.
         */
        static void recoverLoggerAndHandler(void* p_callback);

        /**
         * @brief Closes the log file.
         */
        static void closeLogger();

        /**
         * @brief Installs the interface exception handler.
         * @param p_exceptionIds Exceptions to treat as warnings, Exception::FMTexc values.
         * @param p_maxWarnings Number of warnings before they are silenced; 10 if the value
         *        is not positive.
         */
        static void setErrorsToWarnings(
            const std::vector<int>& p_exceptionIds,
            int p_maxWarnings);

        /**
         * @brief Logs the exception being handled.
         *
         * To be called from a catch block: the error stack is rebuilt from the current
         * exception. It is written to the log, or directly to its file if the interface
         * logger is not installed.
         *
         * @param p_text Message to add to the stack.
         * @param p_method Method where the exception was caught.
         * @param p_line Line where the exception was caught.
         * @param p_file File where the exception was caught.
         * @return The formatted error stack, to send back to the interface.
         */
        static std::string logCurrentException(
            const std::string& p_text,
            const std::string& p_method,
            int p_line,
            const std::string& p_file);

        /**
         * @brief Opens in Notepad++ the file and line quoted by an error stack, if any.
         * @param p_errorStack Stack returned by logCurrentException.
         */
        static void openErrorLocation(const std::string& p_errorStack);

        /**
         * @brief Reads scenarios from a project and adds them to the cache.
         * @param p_primaryFilePath Project .pri file.
         * @param p_scenarioNames Names of the scenarios to read.
         */
        static void addScenarios(
            const std::string& p_primaryFilePath,
            const std::vector<std::string>& p_scenarioNames);

        /**
         * @brief Removes a scenario from the cache.
         * @param p_modelIndex Scenario index.
         */
        static void removeScenario(int p_modelIndex);

        /**
         * @brief Removes every scenario from the cache.
         */
        static void clearScenarios();

        /**
         * @brief Writes every scenario of the cache to a project; the first one is the base.
         * @param p_destinationDirectory Destination directory.
         */
        static void writeScenariosToProject(const std::string& p_destinationDirectory);

        // Environment, without any scenario: see Environment.

        /** @brief See Environment::getAvailableSolvers. */
        static std::vector<int> getAvailableSolvers();

        /** @brief See Environment::getSolverName. */
        static std::string getSolverName(int p_solver);

        /** @brief See Environment::getVectorDriverExtensions. */
        static std::vector<std::string> getVectorDriverExtensions();

        /** @brief See Environment::getChangeLog. */
        static std::string getChangeLog();

        /** @brief See Environment::getExceptionDescription. */
        static std::string getExceptionDescription(int p_exceptionId);

        /** @brief See Environment::getErrorsToIgnore. */
        static std::vector<int> getErrorsToIgnore();

        // Queries on a cached scenario, designated by its index: see ModelQuery.

        /** @brief See ModelQuery::getMaxAge. */
        static int getMaxAge(int p_modelIndex);

        /** @brief See ModelQuery::getYield. */
        static double getYield(
            int p_modelIndex,
            const std::string& p_mask,
            const std::string& p_yield,
            int p_age);

        /** @brief See ModelQuery::getAllMasks. */
        static std::set<std::string> getAllMasks(
            int p_modelIndex,
            int p_periods,
            const std::vector<int>& p_themesNumbers,
            const std::string& p_rasterPath);

        /** @brief See ModelQuery::validateMask. */
        static bool validateMask(int p_modelIndex, const std::string& p_mask);

        /** @brief See ModelQuery::getConstraintsAsText. */
        static std::vector<std::string> getConstraintsAsText(int p_modelIndex);

        /**
         * @brief See ModelQuery::getOutputsNames.
         *
         * Returns an empty list if the cache is empty: the interface fills its output list
         * before any scenario is loaded.
         */
        static std::vector<std::string> getOutputsNames(int p_modelIndex);

        /** @brief See ModelQuery::getActionsNames. */
        static std::vector<std::string> getActionsNames(int p_modelIndex);

        /** @brief See ModelQuery::getAggregates. */
        static std::vector<std::string> getAggregates(int p_modelIndex);

        /** @brief See ModelQuery::getYieldsNames. */
        static std::vector<std::string> getYieldsNames(int p_modelIndex);

        /** @brief See ModelQuery::getThemesCount. */
        static int getThemesCount(int p_modelIndex);

        /** @brief See ModelQuery::getThemeAttributes. */
        static std::vector<std::string> getThemeAttributes(int p_modelIndex, int p_themeIndex);

        /** @brief See ModelQuery::getPeriodsCount. */
        static int getPeriodsCount(const std::string& p_primaryFilePath, int p_modelIndex);

        // Transformations: see Transformation. The produced scenario is added to the cache.

        /** @brief See Transformation::aggregateAllActions. */
        static void aggregateAllActions(
            int p_modelIndex,
            const std::vector<std::string>& p_aggregates,
            const std::vector<std::string>& p_order,
            const std::string& p_primaryFilePath,
            const std::string& p_scenarioName);

        /** @brief See Transformation::splitActions. */
        static void splitActions(
            int p_modelIndex,
            const std::string& p_primaryFilePath,
            const std::vector<std::string>& p_splitted,
            const std::vector<std::string>& p_splittedMasks,
            const std::string& p_scenarioName);

        /** @brief See Transformation::buildAction. */
        static void buildAction(
            int p_modelIndex,
            const std::string& p_actionName,
            const std::string& p_targetYield,
            const std::string& p_primaryFilePath,
            const std::string& p_scenarioName);

        // Processing on a cached scenario.

        /** @brief See SES::RunSES(const SESParameters&, const Models::FMTModel&). */
        static SESResults runSpatialSimulation(const SESParameters& p_params, int p_modelIndex);

        /** @brief See SES::RunOptimization. */
        static SAResults runSpatialOptimization(const SAParameters& p_params, int p_modelIndex);

        /** @brief See Rasterization::rasterize. */
        static void rasterize(const RasterizationParameters& p_params, int p_modelIndex);

        /** @brief See OperatingArea::schedule. */
        static OperatingAreaResults scheduleOperatingAreas(
            const OperatingAreaParameters& p_params,
            int p_modelIndex);

        /** @brief See AreaVariability::run. */
        static AreaVariabilityResults runAreaVariability(
            const AreaVariabilityParameters& p_params,
            int p_modelIndex);

        /**
         * @brief See Planning::plan.
         *
         * A failed schedule read for playback is logged like any interface error
         * (logCurrentException, openErrorLocation), and the planning goes on.
         *
         * @param p_params Planning parameters.
         * @param p_modelIndexes Indexes of the scenarios to plan.
         * @param p_playback Playback flag of each scenario, by position.
         */
        static void plan(
            const PlanningParameters& p_params,
            const std::vector<int>& p_modelIndexes,
            const std::vector<bool>& p_playback);

        /**
         * @brief See Planning::replan.
         * @param p_params Replanning parameters.
         * @param p_strategicModelIndex Index of the strategic (global) scenario.
         * @param p_stochasticModelIndex Index of the stochastic scenario.
         * @param p_tacticalModelIndex Index of the tactical (local) scenario.
         */
        static void replan(
            const ReplanningParameters& p_params,
            int p_strategicModelIndex,
            int p_stochasticModelIndex,
            int p_tacticalModelIndex);
    };
}

#endif // FMTWRAPPERCORE_CONTROLLER_HEADER
