#ifndef FMTWRAPPERCORE_PLANNING_HEADER
#define FMTWRAPPERCORE_PLANNING_HEADER

#include <functional>
#include <string>
#include <vector>

#include "FMTWrapperCoreExport.h"
#include "PlanningTypes.h"

namespace Models
{
    class FMTModel;
}

namespace FMTWrapperCore
{
    /**
     * @brief Planning and replanning of scenarios, through the parallel tasks of FMT.
     *
     * Both operations write their results to disk and return nothing; errors are raised as
     * exceptions.
     */
    class FMTWRAPPERCOREEXPORT Planning
    {
    public:
        /**
         * @brief Reports the error being handled, without interrupting the operation.
         *
         * Called from a catch block, while the exception is still active, with the method,
         * the line and the file where it was caught.
         */
        using ErrorReporter = std::function<void(const std::string&, int, const std::string&)>;

        /**
         * @brief Plans scenarios in a single FMTPlanningTask.
         *
         * Each scenario is optimized, or played back from its schedule if its playback flag
         * is set (forced partial build). The task writes the schedule of the optimized
         * scenarios into the project.
         *
         * Two historical behaviors are kept:
         * - a model present twice in p_models takes the playback flag of its first
         *   occurrence;
         * - a failed schedule read for playback is handed to p_reportScheduleError, then the
         *   scenario runs with an empty schedule. Without p_reportScheduleError, the error is
         *   raised.
         *
         * @param p_params Planning parameters.
         * @param p_models Models to plan.
         * @param p_playback Playback flag of each model, by position.
         * @param p_reportScheduleError Receives the failed schedule reads.
         */
        static void plan(
            const PlanningParameters& p_params,
            const std::vector<const Models::FMTModel*>& p_models,
            const std::vector<bool>& p_playback,
            const ErrorReporter& p_reportScheduleError);

        /**
         * @brief Replans from the strategic, stochastic and tactical models.
         *
         * While the tasks run, the interface logger (CallbackLogger) is set to the
         * p_params.taskLogLevel level; its default level is restored afterwards, even on
         * error. Any other logger is left untouched.
         *
         * @param p_params Replanning parameters.
         * @param p_strategic Global (strategic) model.
         * @param p_stochastic Stochastic model.
         * @param p_tactical Local (tactical) model.
         */
        static void replan(
            const ReplanningParameters& p_params,
            const Models::FMTModel& p_strategic,
            const Models::FMTModel& p_stochastic,
            const Models::FMTModel& p_tactical);
    };
}

#endif // FMTWRAPPERCORE_PLANNING_HEADER
