#include "Controller.h"

#include <fstream>

#include "AreaVariability.h"
#include "Environment.h"
#include "FMTException.h"
#include "WarningExceptionHandler.h"
#include "ModelCache.h"
#include "CallbackLogger.h"
#include "FMTFreeExceptionHandler.h"
#include "FMTModel.h"
#include "FMTModelParser.h"
#include "ModelQuery.h"
#include "OperatingArea.h"
#include "Planning.h"
#include "Rasterization.h"
#include "SES.h"
#include "TransformationCore.h"

namespace FMTWrapperCore
{
    namespace
    {
        // Number of warnings tolerated before silence, when the interface does not specify one.
        constexpr int DEFAULT_MAX_WARNINGS = 10;

        // Resolves a cached scenario. An empty cache is reported here; an out-of-range
        // index is reported by ModelCache::getModel.
        const Models::FMTModel& getCachedModel(int p_modelIndex)
        {
            ModelCache* cache = ModelCache::GetInstance();

            if (cache->empty())
            {
                Exception::FMTFreeExceptionHandler().raise(
                    Exception::FMTexc::FMTrangeerror,
                    "no scenario in cache for index " + std::to_string(p_modelIndex),
                    "Controller::getCachedModel",
                    __LINE__,
                    __FILE__);
            }

            return cache->getModel(p_modelIndex);
        }
    }

    void Controller::initializeLogger(const std::string& p_logFilePath, void* p_callback)
    {
        ModelCache::GetInstance()->InitializeLogger(p_logFilePath, p_callback);
    }

    void Controller::recoverLoggerAndHandler(void* p_callback)
    {
        ModelCache::GetInstance()->RecoverLoggerAndHandler(p_callback);
    }

    void Controller::closeLogger()
    {
        ModelCache::GetInstance()->CloseLogger();
    }

    void Controller::setErrorsToWarnings(
        const std::vector<int>& p_exceptionIds,
        int p_maxWarnings)
    {
        std::vector<Exception::FMTexc> warnings;

        for (const int EXCEPTION_ID : p_exceptionIds)
        {
            warnings.push_back(static_cast<Exception::FMTexc>(EXCEPTION_ID));
        }

        ModelCache::GetInstance()->InitializeExceptionHandler(
            p_maxWarnings > 0 ? p_maxWarnings : DEFAULT_MAX_WARNINGS,
            warnings);
    }

    std::string Controller::logCurrentException(
        const std::string& p_text,
        const std::string& p_method,
        int p_line,
        const std::string& p_file)
    {
        ModelCache* cache = ModelCache::GetInstance();

        const std::string FORMATTED_STACK =
            cache->GetWarningHandler()->geterrorstack(p_text, p_method, p_line, p_file);

        CallbackLogger* logger = cache->GetCallbackLogger();

        if (logger)
        {
            logger->logWithLevel(
                "*************************************************************\n",
                0);

            logger->logWithLevel(
                "FMT - ERROR " + FORMATTED_STACK + "\n",
                0);
        }
        else
        {
            // Without an interface logger, the stack is appended directly to the log file.
            try
            {
                const std::string& LOGGER_FILENAME = cache->GetLoggerFilename();

                if (!LOGGER_FILENAME.empty())
                {
                    std::ofstream out(LOGGER_FILENAME, std::ios_base::app);

                    if (out.is_open())
                    {
                        out << "*************************************************************\n";
                        out << "FMT - ERROR " << FORMATTED_STACK << "\n";
                    }
                }
            }
            catch (...)
            {
            }
        }

        return FORMATTED_STACK;
    }

    void Controller::openErrorLocation(const std::string& p_errorStack)
    {
        ModelCache::GetInstance()->GetWarningHandler()->tryfileopener(p_errorStack);
    }

    void Controller::addScenarios(
        const std::string& p_primaryFilePath,
        const std::vector<std::string>& p_scenarioNames)
    {
        Parser::FMTModelParser modelParser;

        const std::vector<Models::FMTModel> MODELS =
            modelParser.readproject(p_primaryFilePath, p_scenarioNames);

        for (const Models::FMTModel& MODEL : MODELS)
        {
            ModelCache::GetInstance()->push_back(MODEL);
        }
    }

    void Controller::removeScenario(int p_modelIndex)
    {
        ModelCache::GetInstance()->erase(p_modelIndex);
    }

    void Controller::clearScenarios()
    {
        ModelCache::GetInstance()->clear();
    }

    void Controller::writeScenariosToProject(const std::string& p_destinationDirectory)
    {
        ModelCache* cache = ModelCache::GetInstance();

        if (cache->empty())
        {
            Exception::FMTFreeExceptionHandler().raise(
                Exception::FMTexc::FMTrangeerror,
                "empty cache",
                "Controller::writeScenariosToProject",
                __LINE__,
                __FILE__);
        }

        std::vector<Models::FMTModel> models;
        models.reserve(cache->size());

        for (size_t index = 0; index < cache->size(); ++index)
        {
            models.push_back(cache->getModel(static_cast<int>(index)));
        }

        ModelQuery::writeToProject(models, p_destinationDirectory);
    }

    std::vector<int> Controller::getAvailableSolvers()
    {
        return Environment::getAvailableSolvers();
    }

    std::string Controller::getSolverName(int p_solver)
    {
        return Environment::getSolverName(p_solver);
    }

    std::vector<std::string> Controller::getVectorDriverExtensions()
    {
        return Environment::getVectorDriverExtensions();
    }

    std::string Controller::getChangeLog()
    {
        return Environment::getChangeLog();
    }

    std::string Controller::getExceptionDescription(int p_exceptionId)
    {
        return Environment::getExceptionDescription(p_exceptionId);
    }

    std::vector<int> Controller::getErrorsToIgnore()
    {
        return Environment::getErrorsToIgnore();
    }

    int Controller::getMaxAge(int p_modelIndex)
    {
        return ModelQuery::getMaxAge(getCachedModel(p_modelIndex));
    }

    double Controller::getYield(
        int p_modelIndex,
        const std::string& p_mask,
        const std::string& p_yield,
        int p_age)
    {
        return ModelQuery::getYield(getCachedModel(p_modelIndex), p_mask, p_yield, p_age);
    }

    std::set<std::string> Controller::getAllMasks(
        int p_modelIndex,
        int p_periods,
        const std::vector<int>& p_themesNumbers,
        const std::string& p_rasterPath)
    {
        return ModelQuery::getAllMasks(
            getCachedModel(p_modelIndex), p_periods, p_themesNumbers, p_rasterPath);
    }

    bool Controller::validateMask(int p_modelIndex, const std::string& p_mask)
    {
        return ModelQuery::validateMask(getCachedModel(p_modelIndex), p_mask);
    }

    std::vector<std::string> Controller::getConstraintsAsText(int p_modelIndex)
    {
        return ModelQuery::getConstraintsAsText(getCachedModel(p_modelIndex));
    }

    std::vector<std::string> Controller::getOutputsNames(int p_modelIndex)
    {
        // The interface fills its output list before any scenario is loaded: an empty cache
        // is not an error here, unlike the other queries.
        if (ModelCache::GetInstance()->empty())
        {
            return std::vector<std::string>();
        }

        return ModelQuery::getOutputsNames(getCachedModel(p_modelIndex));
    }

    std::vector<std::string> Controller::getActionsNames(int p_modelIndex)
    {
        return ModelQuery::getActionsNames(getCachedModel(p_modelIndex));
    }

    std::vector<std::string> Controller::getAggregates(int p_modelIndex)
    {
        return ModelQuery::getAggregates(getCachedModel(p_modelIndex));
    }

    std::vector<std::string> Controller::getYieldsNames(int p_modelIndex)
    {
        return ModelQuery::getYieldsNames(getCachedModel(p_modelIndex));
    }

    int Controller::getThemesCount(int p_modelIndex)
    {
        return ModelQuery::getThemesCount(getCachedModel(p_modelIndex));
    }

    std::vector<std::string> Controller::getThemeAttributes(int p_modelIndex, int p_themeIndex)
    {
        return ModelQuery::getThemeAttributes(getCachedModel(p_modelIndex), p_themeIndex);
    }

    int Controller::getPeriodsCount(const std::string& p_primaryFilePath, int p_modelIndex)
    {
        return ModelQuery::getPeriodsCount(p_primaryFilePath, getCachedModel(p_modelIndex));
    }

    void Controller::aggregateAllActions(
        int p_modelIndex,
        const std::vector<std::string>& p_aggregates,
        const std::vector<std::string>& p_order,
        const std::string& p_primaryFilePath,
        const std::string& p_scenarioName)
    {
        const Models::FMTModel AGGREGATED = Transformation::aggregateAllActions(
            getCachedModel(p_modelIndex), p_aggregates, p_order, p_primaryFilePath, p_scenarioName);
        ModelCache::GetInstance()->push_back(AGGREGATED);
    }

    void Controller::splitActions(
        int p_modelIndex,
        const std::string& p_primaryFilePath,
        const std::vector<std::string>& p_splitted,
        const std::vector<std::string>& p_splittedMasks,
        const std::string& p_scenarioName)
    {
        const Models::FMTModel SPLITTED = Transformation::splitActions(
            getCachedModel(p_modelIndex), p_primaryFilePath, p_splitted, p_splittedMasks, p_scenarioName);
        ModelCache::GetInstance()->push_back(SPLITTED);
    }

    void Controller::buildAction(
        int p_modelIndex,
        const std::string& p_actionName,
        const std::string& p_targetYield,
        const std::string& p_primaryFilePath,
        const std::string& p_scenarioName)
    {
        const Models::FMTModel BUILT = Transformation::buildAction(
            getCachedModel(p_modelIndex), p_actionName, p_targetYield, p_primaryFilePath, p_scenarioName);
        ModelCache::GetInstance()->push_back(BUILT);
    }

    SESResults Controller::runSpatialSimulation(const SESParameters& p_params, int p_modelIndex)
    {
        return SES::RunSES(p_params, getCachedModel(p_modelIndex));
    }

    SAResults Controller::runSpatialOptimization(const SAParameters& p_params, int p_modelIndex)
    {
        return SES::RunOptimization(p_params, getCachedModel(p_modelIndex));
    }

    void Controller::rasterize(const RasterizationParameters& p_params, int p_modelIndex)
    {
        Rasterization::rasterize(p_params, getCachedModel(p_modelIndex));
    }

    OperatingAreaResults Controller::scheduleOperatingAreas(
        const OperatingAreaParameters& p_params,
        int p_modelIndex)
    {
        return OperatingArea::schedule(p_params, getCachedModel(p_modelIndex));
    }

    AreaVariabilityResults Controller::runAreaVariability(
        const AreaVariabilityParameters& p_params,
        int p_modelIndex)
    {
        return AreaVariability::run(p_params, getCachedModel(p_modelIndex));
    }

    void Controller::plan(
        const PlanningParameters& p_params,
        const std::vector<int>& p_modelIndexes,
        const std::vector<bool>& p_playback)
    {
        std::vector<const Models::FMTModel*> models;
        models.reserve(p_modelIndexes.size());

        for (const int MODEL_INDEX : p_modelIndexes)
        {
            models.push_back(&getCachedModel(MODEL_INDEX));
        }

        // A failed schedule read for playback is reported the way the interface reports any
        // error, without interrupting the planning.
        Planning::plan(p_params, models, p_playback,
            [](const std::string& p_method, int p_line, const std::string& p_file)
            {
                Controller::openErrorLocation(
                    Controller::logCurrentException("", p_method, p_line, p_file));
            });
    }

    void Controller::replan(
        const ReplanningParameters& p_params,
        int p_strategicModelIndex,
        int p_stochasticModelIndex,
        int p_tacticalModelIndex)
    {
        // Resolved in order: an invalid index always raises the same error.
        const Models::FMTModel& STRATEGIC = getCachedModel(p_strategicModelIndex);
        const Models::FMTModel& STOCHASTIC = getCachedModel(p_stochasticModelIndex);
        const Models::FMTModel& TACTICAL = getCachedModel(p_tacticalModelIndex);

        Planning::replan(p_params, STRATEGIC, STOCHASTIC, TACTICAL);
    }
}
