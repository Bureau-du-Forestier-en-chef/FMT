#ifndef FMTWRAPPERCORE_OPERATINGAREA_HEADER
#define FMTWRAPPERCORE_OPERATINGAREA_HEADER

#include "OperatingAreaTypes.h"
#include "FMTWrapperCoreExport.h"

namespace Models
{
    class FMTModel;
}

namespace FMTWrapperCore
{
    /**
     * @brief Operating area scheduling: generates the COS schedule.
     *
     * Adds the reserved aggregate ~BFECOPTOUTPUTYOUVERT~ to the actions that use the
     * YOUVERT yield, reads the operating areas, then runs FMTOpAreaSchedulerTask.
     *
     * Two situations of the model refuse the scheduling, without an exception: an action
     * already carries the reserved aggregate, or no action uses YOUVERT. The reason is then
     * in OperatingAreaResults::errorMessage, and logged. Any other error is raised as an
     * exception.
     */
    class FMT_WRAPPER_CORE_EXPORT OperatingArea
    {
    public:
        /**
         * @brief Schedules the operating areas of the model.
         * @param p_params Scheduling parameters.
         * @param p_model Model to schedule.
         * @return The result; success is false if the model was refused.
         */
        static OperatingAreaResults schedule(
            const OperatingAreaParameters& p_params,
            const Models::FMTModel& p_model);
    };
}

#endif // FMTWRAPPERCORE_OPERATINGAREA_HEADER
