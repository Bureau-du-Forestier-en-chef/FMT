#include "Planning.h"

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

#include "FMTDefaultLogger.h"
#include "CallbackLogger.h"
#include "FMTLogger.h"
#include "FMTLpModel.h"
#include "FMTModel.h"
#include "FMTNssModel.h"
#include "FMTOutput.h"
#include "FMTPlanningTask.h"
#include "FMTReplanningTask.h"
#include "FMTSchedule.h"
#include "FMTTaskHandler.h"
#include "ModelQuery.h"
#include "Selection.h"

namespace FMTWrapperCore
{
    namespace
    {
        // The GDAL CSV driver separates fields with commas; FMT outputs use
        // semicolons.
        std::vector<std::string> getLayersOptions(const std::string& p_gdalProvider)
        {
            std::vector<std::string> layersOptions;

            if (p_gdalProvider == "CSV")
            {
                layersOptions.push_back("SEPARATOR=SEMICOLON");
            }

            return layersOptions;
        }

        // Sets the interface logger to the task level for the lifetime of a scope, and
        // restores its default level on exit, exceptions included.
        class ScopedTaskLoggingLevel
        {
        public:
            ScopedTaskLoggingLevel(CallbackLogger* p_logger, int p_level)
                : m_logger(p_logger)
            {
                if (m_logger)
                {
                    m_logger->settasklogginglevel(p_level);
                }
            }

            ~ScopedTaskLoggingLevel()
            {
                if (m_logger)
                {
                    m_logger->setdefaultlogginglevel();
                }
            }

            ScopedTaskLoggingLevel(const ScopedTaskLoggingLevel&) = delete;
            ScopedTaskLoggingLevel& operator=(const ScopedTaskLoggingLevel&) = delete;

        private:
            CallbackLogger* m_logger;
        };
    }

    void Planning::plan(
        const PlanningParameters& p_params,
        const std::vector<const Models::FMTModel*>& p_models,
        const std::vector<bool>& p_playback,
        const ErrorReporter& p_reportScheduleError)
    {
        try
        {
            // FMT static logger: in the interface, it is the CallbackLogger; in a
            // test, the default logger.
            Logging::FMTLogger& logger = *Models::FMTModel::getLogger();

            logger << Logging::FMTDefaultLogger().getLogStamp() << "\n";

            Parallel::FMTPlanningTask planningTask(
                p_params.outputMinPeriod,
                p_params.outputMaxPeriod,
                p_params.outputPath,
                p_params.gdalProvider,
                getLayersOptions(p_params.gdalProvider),
                static_cast<Core::FMToutputlevel>(p_params.outputLevel),
                p_params.primaryFilePath);

            const int NUMBER_OF_SCENARIOS = static_cast<int>(p_models.size());

            for (const Models::FMTModel* MODEL : p_models)
            {
                Models::FMTLpModel optimizationModel(
                    *MODEL,
                    static_cast<Models::FMTSolverInterface>(p_params.solver));

                logger << "FMT -> Préparation pour le scénario : " + optimizationModel.getName() << "\n";

                // Historical behavior: a scenario present twice takes the playback flag of its
                // first occurrence.
                const size_t FIRST_OCCURRENCE = static_cast<size_t>(
                    std::find(p_models.begin(), p_models.end(), MODEL) - p_models.begin());
                const bool PLAYBACK = p_playback.at(FIRST_OCCURRENCE);

                std::vector<Core::FMTSchedule> schedules;

                if (PLAYBACK)
                {
                    logger << "FMT -> Lecture de cédule pour le scénario : " + optimizationModel.getName() << "\n";

                    // Historical behavior: a failed read is reported, then the scenario runs
                    // with an empty schedule.
                    try
                    {
                        schedules = ModelQuery::readSchedules(p_params.primaryFilePath, *MODEL);
                    }
                    catch (...)
                    {
                        if (!p_reportScheduleError)
                        {
                            throw;
                        }

                        p_reportScheduleError("Planning::plan", __LINE__, __FILE__);
                    }
                }

                optimizationModel.setStrictlyPositivesOutputsMatrix();
                optimizationModel.setParameter(Models::FMTintmodelparameters::LENGTH, p_params.numberOfPeriods);

                // Threads are shared among the scenarios, if there is at least one per scenario.
                int threadsPerScenario = 1;

                if (NUMBER_OF_SCENARIOS <= p_params.numberOfThreads)
                {
                    threadsPerScenario = p_params.numberOfThreads / NUMBER_OF_SCENARIOS;
                }

                optimizationModel.setParameter(Models::FMTintmodelparameters::NUMBER_OF_THREADS, threadsPerScenario);
                optimizationModel.Models::FMTModel::setParameter(Models::FMTdblmodelparameters::TOLERANCE, 0.01);

                const std::vector<Core::FMTOutput> SELECTED_OUTPUTS =
                    Selection::selectOutputs(optimizationModel.getOutputs(), p_params.outputNames);

                optimizationModel.setParameter(Models::FMTboolmodelparameters::FORCE_PARTIAL_BUILD, PLAYBACK);
                planningTask.push_back(optimizationModel, schedules, SELECTED_OUTPUTS);

                logger << "FMT -> Scénario : " + optimizationModel.getName() + " prêt a être lancer." << "\n";
            }

            Parallel::FMTTaskHandler handler(planningTask, p_params.numberOfThreads);
            handler.conccurentRun();
        }
        catch (...)
        {
            Models::FMTModel::getExceptionHandler()->raiseFromCatch(
                "", "Planning::plan", __LINE__, __FILE__);
        }
    }

    void Planning::replan(
        const ReplanningParameters& p_params,
        const Models::FMTModel& p_strategic,
        const Models::FMTModel& p_stochastic,
        const Models::FMTModel& p_tactical)
    {
        try
        {
            // FMT static logger: in the interface, it is the CallbackLogger; in a
            // test, the default logger.
            Logging::FMTLogger& logger = *Models::FMTModel::getLogger();

            logger << Logging::FMTDefaultLogger().getLogStamp() << "\n";

            const Models::FMTSolverInterface SOLVER =
                static_cast<Models::FMTSolverInterface>(p_params.solver);

            Models::FMTLpModel global(p_strategic, SOLVER);
            global.setParameter(Models::FMTintmodelparameters::LENGTH, p_params.numberOfPeriods);
            global.setParameter(Models::FMTboolmodelparameters::DEBUG_MATRIX, true);
            global.setParameter(Models::FMTintmodelparameters::NUMBER_OF_THREADS, 1);
            global.setParameter(Models::FMTboolmodelparameters::PRESOLVE_CAN_REMOVE_STATIC_THEMES, true);

            Models::FMTNssModel stochastic(p_stochastic, 0);
            stochastic.setParameter(Models::FMTintmodelparameters::LENGTH, 1);
            stochastic.setParameter(Models::FMTboolmodelparameters::DEBUG_MATRIX, true);

            Models::FMTLpModel local(p_tactical, SOLVER);
            local.setParameter(Models::FMTintmodelparameters::LENGTH, 1);
            local.setParameter(Models::FMTintmodelparameters::NUMBER_OF_THREADS, 1);
            local.setParameter(Models::FMTboolmodelparameters::DEBUG_MATRIX, true);

            const std::vector<Core::FMTOutput> SELECTED_OUTPUTS =
                Selection::selectOutputs(global.getOutputs(), p_params.outputNames);

            logger << "FMT -> Préparation de la replanification " << "\n";
            logger.logTime();

            std::unique_ptr<Parallel::FMTReplanningTask> replanningTask(new Parallel::FMTReplanningTask(
                global,
                stochastic,
                local,
                SELECTED_OUTPUTS,
                p_params.outputPath,
                p_params.gdalProvider,
                getLayersOptions(p_params.gdalProvider),
                p_params.maximumReplicates,
                p_params.replanningPeriods,
                p_params.minimalDrift,
                static_cast<Core::FMToutputlevel>(p_params.outputLevel),
                p_params.writeSchedules));
            replanningTask->setReplicates(p_params.minimumReplicates, p_params.maximumReplicates);

            const std::unique_ptr<Parallel::FMTTask> MAIN_TASK(replanningTask.release());

            logger << "FMT -> Préparation de la replanification terminée" << "\n";

            Parallel::FMTTaskHandler handler(MAIN_TASK, p_params.numberOfThreads);

            {
                const ScopedTaskLoggingLevel TASK_LOGGING_LEVEL(
                    dynamic_cast<CallbackLogger*>(&logger),
                    p_params.taskLogLevel);

                handler.onDemandRun();
                logger.logTime();
            }
        }
        catch (...)
        {
            Models::FMTModel::getExceptionHandler()->raiseFromCatch(
                "", "Planning::replan", __LINE__, __FILE__);
        }
    }
}
