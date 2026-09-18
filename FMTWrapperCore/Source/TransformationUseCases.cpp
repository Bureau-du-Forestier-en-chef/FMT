/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "TransformationUseCases.h"

#include "FMTModel.h"
#include "ModelCache.h"
#include "TransformationCore.h"

#include <string>
#include <vector>

namespace FMTWrapper::Backend
{
    void TransformationUseCases::aggregateAllActions(
        int p_modelIndex,
        const std::vector<std::string>& p_aggregates,
        const std::vector<std::string>& p_order,
        const std::string& p_primaryFilePath,
        const std::string& p_scenarioName)
    {
        const Models::FMTModel AGGREGATED = Transformation::aggregateAllActions(
            ModelCache::GetInstance()->getModel(p_modelIndex),
            p_aggregates,
            p_order,
            p_primaryFilePath,
            p_scenarioName);
        ModelCache::GetInstance()->push_back(AGGREGATED);
    }

    void TransformationUseCases::splitActions(
        int p_modelIndex,
        const std::string& p_primaryFilePath,
        const std::vector<std::string>& p_splitted,
        const std::vector<std::string>& p_splittedMasks,
        const std::string& p_scenarioName)
    {
        const Models::FMTModel SPLITTED = Transformation::splitActions(
            ModelCache::GetInstance()->getModel(p_modelIndex),
            p_primaryFilePath,
            p_splitted,
            p_splittedMasks,
            p_scenarioName);
        ModelCache::GetInstance()->push_back(SPLITTED);
    }

    void TransformationUseCases::buildAction(
        int p_modelIndex,
        const std::string& p_actionName,
        const std::string& p_targetYield,
        const std::string& p_primaryFilePath,
        const std::string& p_scenarioName)
    {
        const Models::FMTModel BUILT = Transformation::buildAction(
            ModelCache::GetInstance()->getModel(p_modelIndex),
            p_actionName,
            p_targetYield,
            p_primaryFilePath,
            p_scenarioName);
        ModelCache::GetInstance()->push_back(BUILT);
    }
}
