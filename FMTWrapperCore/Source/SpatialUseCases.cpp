/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "SpatialUseCases.h"

#include "EventPublisher.h"
#include "Events.h"
#include "ModelCache.h"
#include "OperatingArea.h"
#include "Rasterization.h"
#include "SES.h"

namespace FMTWrapper::Backend
{
    SESResults SpatialUseCases::runSpatialSimulation(
        const SESParameters& p_params,
        int p_modelIndex)
    {
        ModelCache* cache = ModelCache::GetInstance();
        const SESResults RESULTS = SES::RunSES(p_params, cache->getModel(p_modelIndex));

        // The simulation refuses without raising and without logging: published here, its
        // refusal reaches an interface the way any other error does.
        if (!RESULTS.success && !RESULTS.errorMessage.empty())
        {
            cache->GetEventPublisher().publish(ErrorEvent{ RESULTS.errorMessage });
        }

        return RESULTS;
    }

    SAResults SpatialUseCases::runSpatialOptimization(
        const SAParameters& p_params,
        int p_modelIndex)
    {
        return SES::RunOptimization(p_params, ModelCache::GetInstance()->getModel(p_modelIndex));
    }

    void SpatialUseCases::rasterize(
        const RasterizationParameters& p_params,
        int p_modelIndex)
    {
        Rasterization::rasterize(p_params, ModelCache::GetInstance()->getModel(p_modelIndex));
    }

    OperatingAreaResults SpatialUseCases::scheduleOperatingAreas(
        const OperatingAreaParameters& p_params,
        int p_modelIndex)
    {
        return OperatingArea::schedule(p_params, ModelCache::GetInstance()->getModel(p_modelIndex));
    }
}
