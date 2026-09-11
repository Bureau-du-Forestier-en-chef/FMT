#include "Rasterization.h"

#include <vector>

#include "FMTAreaParser.h"
#include "FMTForest.h"
#include "FMTFormCache.h"
#include "FMTModel.h"
#include "FMTTheme.h"

namespace FMTWrapperCore
{
    namespace
    {
        // Facteurs appliqués aux champs du fichier vectoriel. L'ancienne
        // implémentation du wrapper les passait en dur ; ils sont repris tels quels.
        constexpr double AGE_FACTOR = 1.0;
        // 0.0001 convertit une superficie exprimée en m² en hectares.
        constexpr double AREA_FACTOR = 0.0001;
        // Aucune entité n'est écartée pour cause de superficie trop petite.
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

    void Rasterization::rasterize(
        const RasterizationParameters& p_params,
        int p_modelIndex)
    {
        rasterize(p_params, FMTFormCache::GetInstance()->getModel(p_modelIndex));
    }
}
