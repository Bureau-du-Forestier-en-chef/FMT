#ifndef FMTWRAPPERCORE_RASTERIZATION_HEADER
#define FMTWRAPPERCORE_RASTERIZATION_HEADER

#include "RasterizationTypes.h"
#include "FMTWrapperCoreExport.h"

namespace Models
{
    class FMTModel;
}

namespace FMTWrapper::Backend
{
    /**
     * @brief Rasterization of a vector file according to the themes of a model.
     *
     * Writes the forest rasters (AGE.tif, THEME<i>.tif, and STANLOCK.tif if applicable) to
     * the output folder: they are the ones SES and ModelQuery::getAllMasks read back.
     *
     * The operation produces no in-memory data: it returns nothing and errors are raised
     * as exceptions.
     */
    class FMT_WRAPPER_CORE_EXPORT Rasterization
    {
    public:
        /**
         * @brief Rasterizes the vector file according to the model themes.
         * @param p_params Rasterization parameters.
         * @param p_model Model providing the themes and the initial area.
         */
        static void rasterize(
            const RasterizationParameters& p_params,
            const Models::FMTModel& p_model);
    };
}

#endif // FMTWRAPPERCORE_RASTERIZATION_HEADER
