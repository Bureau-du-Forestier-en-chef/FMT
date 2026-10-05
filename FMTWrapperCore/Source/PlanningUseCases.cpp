/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "PlanningUseCases.h"

#include "AreaVariability.h"
#include "ModelCache.h"
#include "Planning.h"
#include "SessionUseCases.h"

#include <string>
#include <vector>

namespace FMTWrapper::Backend
{
    void PlanningUseCases::plan(
        const PlanningParameters& p_params,
        const std::vector<int>& p_modelIndexes,
        const std::vector<bool>& p_playback)
    {
        std::vector<const Models::FMTModel*> models;
        models.reserve(p_modelIndexes.size());

        for (const int MODEL_INDEX : p_modelIndexes)
        {
            models.push_back(&ModelCache::GetInstance()->getModel(MODEL_INDEX));
        }

        // A failed schedule read for playback is reported the way the interface reports any
        // error, without interrupting the planning.
        Planning::plan(p_params, models, p_playback,
            [](const std::string& p_method, int p_line, const std::string& p_file)
            {
                SessionUseCases::openErrorLocation(
                    SessionUseCases::logCurrentException("", p_method, p_line, p_file));
            });
    }

    void PlanningUseCases::replan(
        const ReplanningParameters& p_params,
        int p_strategicModelIndex,
        int p_stochasticModelIndex,
        int p_tacticalModelIndex)
    {
        ModelCache* cache = ModelCache::GetInstance();

        // Resolved in order: an invalid index always raises the same error.
        const Models::FMTModel& STRATEGIC = cache->getModel(p_strategicModelIndex);
        const Models::FMTModel& STOCHASTIC = cache->getModel(p_stochasticModelIndex);
        const Models::FMTModel& TACTICAL = cache->getModel(p_tacticalModelIndex);

        Planning::replan(p_params, STRATEGIC, STOCHASTIC, TACTICAL);
    }

    AreaVariabilityResults PlanningUseCases::runAreaVariability(
        const AreaVariabilityParameters& p_params,
        int p_modelIndex)
    {
        return AreaVariability::run(p_params, ModelCache::GetInstance()->getModel(p_modelIndex));
    }
}
