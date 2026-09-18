/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "Controller.h"

#include "Environment.h"
#include "PlanningUseCases.h"
#include "QueryUseCases.h"
#include "ScenarioUseCases.h"
#include "SessionUseCases.h"
#include "SpatialUseCases.h"
#include "TransformationUseCases.h"

// The controller receives the system operations of the interface and delegates each one.
// It knows the data transfer objects and the use cases, and nothing of FMT: no forest model,
// no parser, no exception type appears here.

namespace FMTWrapper::Backend
{
    void Controller::initializeLogger(const std::string& p_logFilePath, void* p_callback)
    {
        SessionUseCases::initializeLogger(p_logFilePath, p_callback);
    }

    void Controller::recoverLoggerAndHandler(void* p_callback)
    {
        SessionUseCases::recoverLoggerAndHandler(p_callback);
    }

    void Controller::closeLogger()
    {
        SessionUseCases::closeLogger();
    }

    void Controller::setErrorsToWarnings(
        const std::vector<int>& p_exceptionIds,
        int p_maxWarnings)
    {
        SessionUseCases::setErrorsToWarnings(p_exceptionIds, p_maxWarnings);
    }

    std::string Controller::logCurrentException(
        const std::string& p_text,
        const std::string& p_method,
        int p_line,
        const std::string& p_file)
    {
        return SessionUseCases::logCurrentException(p_text, p_method, p_line, p_file);
    }

    void Controller::openErrorLocation(const std::string& p_errorStack)
    {
        SessionUseCases::openErrorLocation(p_errorStack);
    }

    void Controller::addScenarios(
        const std::string& p_primaryFilePath,
        const std::vector<std::string>& p_scenarioNames)
    {
        ScenarioUseCases::addScenarios(p_primaryFilePath, p_scenarioNames);
    }

    void Controller::removeScenario(int p_modelIndex)
    {
        ScenarioUseCases::removeScenario(p_modelIndex);
    }

    void Controller::clearScenarios()
    {
        ScenarioUseCases::clearScenarios();
    }

    void Controller::writeScenariosToProject(const std::string& p_destinationDirectory)
    {
        ScenarioUseCases::writeScenariosToProject(p_destinationDirectory);
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
        return QueryUseCases::getMaxAge(p_modelIndex);
    }

    double Controller::getYield(
        int p_modelIndex,
        const std::string& p_mask,
        const std::string& p_yield,
        int p_age)
    {
        return QueryUseCases::getYield(p_modelIndex, p_mask, p_yield, p_age);
    }

    std::set<std::string> Controller::getAllMasks(
        int p_modelIndex,
        int p_periods,
        const std::vector<int>& p_themesNumbers,
        const std::string& p_rasterPath)
    {
        return QueryUseCases::getAllMasks(p_modelIndex, p_periods, p_themesNumbers, p_rasterPath);
    }

    bool Controller::validateMask(int p_modelIndex, const std::string& p_mask)
    {
        return QueryUseCases::validateMask(p_modelIndex, p_mask);
    }

    std::vector<std::string> Controller::getConstraintsAsText(int p_modelIndex)
    {
        return QueryUseCases::getConstraintsAsText(p_modelIndex);
    }

    std::vector<std::string> Controller::getOutputsNames(int p_modelIndex)
    {
        return QueryUseCases::getOutputsNames(p_modelIndex);
    }

    std::vector<std::string> Controller::getActionsNames(int p_modelIndex)
    {
        return QueryUseCases::getActionsNames(p_modelIndex);
    }

    std::vector<std::string> Controller::getAggregates(int p_modelIndex)
    {
        return QueryUseCases::getAggregates(p_modelIndex);
    }

    std::vector<std::string> Controller::getYieldsNames(int p_modelIndex)
    {
        return QueryUseCases::getYieldsNames(p_modelIndex);
    }

    int Controller::getThemesCount(int p_modelIndex)
    {
        return QueryUseCases::getThemesCount(p_modelIndex);
    }

    std::vector<std::string> Controller::getThemeAttributes(int p_modelIndex, int p_themeIndex)
    {
        return QueryUseCases::getThemeAttributes(p_modelIndex, p_themeIndex);
    }

    int Controller::getPeriodsCount(const std::string& p_primaryFilePath, int p_modelIndex)
    {
        return QueryUseCases::getPeriodsCount(p_primaryFilePath, p_modelIndex);
    }

    void Controller::aggregateAllActions(
        int p_modelIndex,
        const std::vector<std::string>& p_aggregates,
        const std::vector<std::string>& p_order,
        const std::string& p_primaryFilePath,
        const std::string& p_scenarioName)
    {
        TransformationUseCases::aggregateAllActions(
            p_modelIndex, p_aggregates, p_order, p_primaryFilePath, p_scenarioName);
    }

    void Controller::splitActions(
        int p_modelIndex,
        const std::string& p_primaryFilePath,
        const std::vector<std::string>& p_splitted,
        const std::vector<std::string>& p_splittedMasks,
        const std::string& p_scenarioName)
    {
        TransformationUseCases::splitActions(
            p_modelIndex, p_primaryFilePath, p_splitted, p_splittedMasks, p_scenarioName);
    }

    void Controller::buildAction(
        int p_modelIndex,
        const std::string& p_actionName,
        const std::string& p_targetYield,
        const std::string& p_primaryFilePath,
        const std::string& p_scenarioName)
    {
        TransformationUseCases::buildAction(
            p_modelIndex, p_actionName, p_targetYield, p_primaryFilePath, p_scenarioName);
    }

    SESResults Controller::runSpatialSimulation(const SESParameters& p_params, int p_modelIndex)
    {
        return SpatialUseCases::runSpatialSimulation(p_params, p_modelIndex);
    }

    SAResults Controller::runSpatialOptimization(const SAParameters& p_params, int p_modelIndex)
    {
        return SpatialUseCases::runSpatialOptimization(p_params, p_modelIndex);
    }

    void Controller::rasterize(const RasterizationParameters& p_params, int p_modelIndex)
    {
        SpatialUseCases::rasterize(p_params, p_modelIndex);
    }

    OperatingAreaResults Controller::scheduleOperatingAreas(
        const OperatingAreaParameters& p_params,
        int p_modelIndex)
    {
        return SpatialUseCases::scheduleOperatingAreas(p_params, p_modelIndex);
    }

    AreaVariabilityResults Controller::runAreaVariability(
        const AreaVariabilityParameters& p_params,
        int p_modelIndex)
    {
        return PlanningUseCases::runAreaVariability(p_params, p_modelIndex);
    }

    void Controller::plan(
        const PlanningParameters& p_params,
        const std::vector<int>& p_modelIndexes,
        const std::vector<bool>& p_playback)
    {
        PlanningUseCases::plan(p_params, p_modelIndexes, p_playback);
    }

    void Controller::replan(
        const ReplanningParameters& p_params,
        int p_strategicModelIndex,
        int p_stochasticModelIndex,
        int p_tacticalModelIndex)
    {
        PlanningUseCases::replan(
            p_params, p_strategicModelIndex, p_stochasticModelIndex, p_tacticalModelIndex);
    }
}
