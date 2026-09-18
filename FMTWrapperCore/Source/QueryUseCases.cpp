/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "QueryUseCases.h"

#include "ModelCache.h"
#include "ModelQuery.h"

#include <set>
#include <string>
#include <vector>

namespace FMTWrapper::Backend
{
    int QueryUseCases::getMaxAge(int p_modelIndex)
    {
        return ModelQuery::getMaxAge(ModelCache::GetInstance()->getModel(p_modelIndex));
    }

    double QueryUseCases::getYield(
        int p_modelIndex,
        const std::string& p_mask,
        const std::string& p_yield,
        int p_age)
    {
        return ModelQuery::getYield(
            ModelCache::GetInstance()->getModel(p_modelIndex), p_mask, p_yield, p_age);
    }

    std::set<std::string> QueryUseCases::getAllMasks(
        int p_modelIndex,
        int p_periods,
        const std::vector<int>& p_themesNumbers,
        const std::string& p_rasterPath)
    {
        return ModelQuery::getAllMasks(
            ModelCache::GetInstance()->getModel(p_modelIndex),
            p_periods,
            p_themesNumbers,
            p_rasterPath);
    }

    bool QueryUseCases::validateMask(int p_modelIndex, const std::string& p_mask)
    {
        return ModelQuery::validateMask(
            ModelCache::GetInstance()->getModel(p_modelIndex), p_mask);
    }

    std::vector<std::string> QueryUseCases::getConstraintsAsText(int p_modelIndex)
    {
        return ModelQuery::getConstraintsAsText(
            ModelCache::GetInstance()->getModel(p_modelIndex));
    }

    std::vector<std::string> QueryUseCases::getOutputsNames(int p_modelIndex)
    {
        // The interface fills its output list before any scenario is loaded: an empty cache
        // is not an error here, unlike the other queries.
        if (ModelCache::GetInstance()->empty())
        {
            return std::vector<std::string>();
        }

        return ModelQuery::getOutputsNames(ModelCache::GetInstance()->getModel(p_modelIndex));
    }

    std::vector<std::string> QueryUseCases::getActionsNames(int p_modelIndex)
    {
        return ModelQuery::getActionsNames(ModelCache::GetInstance()->getModel(p_modelIndex));
    }

    std::vector<std::string> QueryUseCases::getAggregates(int p_modelIndex)
    {
        return ModelQuery::getAggregates(ModelCache::GetInstance()->getModel(p_modelIndex));
    }

    std::vector<std::string> QueryUseCases::getYieldsNames(int p_modelIndex)
    {
        return ModelQuery::getYieldsNames(ModelCache::GetInstance()->getModel(p_modelIndex));
    }

    int QueryUseCases::getThemesCount(int p_modelIndex)
    {
        return ModelQuery::getThemesCount(ModelCache::GetInstance()->getModel(p_modelIndex));
    }

    std::vector<std::string> QueryUseCases::getThemeAttributes(int p_modelIndex, int p_themeIndex)
    {
        return ModelQuery::getThemeAttributes(
            ModelCache::GetInstance()->getModel(p_modelIndex), p_themeIndex);
    }

    int QueryUseCases::getPeriodsCount(const std::string& p_primaryFilePath, int p_modelIndex)
    {
        return ModelQuery::getPeriodsCount(
            p_primaryFilePath, ModelCache::GetInstance()->getModel(p_modelIndex));
    }
}
