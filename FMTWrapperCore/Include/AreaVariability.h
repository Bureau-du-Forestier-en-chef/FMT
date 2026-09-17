#ifndef FMTWRAPPERCORE_AREAVARIABILITY_HEADER
#define FMTWRAPPERCORE_AREAVARIABILITY_HEADER

#include "AreaVariabilityTypes.h"
#include "FMTWrapperCoreExport.h"

namespace Models
{
    class FMTModel;
}

namespace FMTWrapperCore
{
    /**
     * @brief Initial area variability: writes the outputs of a model whose initial area
     *        is modified by per-mask proportions.
     *
     * Three situations refuse the operation, without an exception: missing table, table
     * reduced to its header, no output requested. Masks that overlap on the same
     * development raise an exception.
     */
    class FMT_WRAPPER_CORE_EXPORT AreaVariability
    {
    public:
        /**
         * @brief Applies the proportions to the model and writes its outputs.
         * @param p_params Operation parameters.
         * @param p_model Starting model.
         * @return The result; success is false if the operation was refused.
         */
        static AreaVariabilityResults run(
            const AreaVariabilityParameters& p_params,
            const Models::FMTModel& p_model);
    };
}

#endif // FMTWRAPPERCORE_AREAVARIABILITY_HEADER
