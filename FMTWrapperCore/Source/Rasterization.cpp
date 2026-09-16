#include "Rasterization.h"

#include <vector>

#include "FMTAreaParser.h"
#include "FMTForest.h"
#include "FMTModel.h"
#include "FMTTheme.h"

namespace FMTWrapperCore
{
    namespace
    {
        // Factors applied to the fields of the vector file. The former wrapper
        // implementation hardcoded them; they are kept as is.
        constexpr double AGE_FACTOR = 1.0;
        // 0.0001 converts an area in square meters to hectares.
        constexpr double AREA_FACTOR = 0.0001;
        // No feature is discarded for being too small.
        constexpr double MINIMAL_AREA = 0.0;
    }

    void Rasterization::rasterize(
        const RasterizationParameters& p_params,
        const Models::FMTModel& p_model)
    {
        try
        {
            const std::vector<Core::FMTTheme> THEMES = p_model.getThemes();
            const std::vector<Core::FMTActualDevelopment> AREA = p_model.getArea();

            Parser::FMTAreaParser areaParser;

            Spatial::FMTForest forest = areaParser.vectormaptoFMTforest(
                p_params.vectorFilePath,
                p_params.resolution,
                THEMES,
                p_params.ageField,
                p_params.areaField,
                AGE_FACTOR,
                AREA_FACTOR,
                p_params.lockField,
                MINIMAL_AREA);

            areaParser.writeForestExtended(
                forest,
                p_params.vectorFilePath,
                THEMES,
                AREA,
                p_params.outputFolder);
        }
        catch (...)
        {
            p_model.getExceptionHandler()->raiseFromCatch(
                "", "Rasterization::rasterize", __LINE__, __FILE__);
        }
    }
}
