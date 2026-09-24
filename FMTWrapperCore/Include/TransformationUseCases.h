/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTWRAPPERCORE_TRANSFORMATIONUSECASES_HEADER
#define FMTWRAPPERCORE_TRANSFORMATIONUSECASES_HEADER

#include <string>
#include <vector>

namespace FMTWrapper::Backend
{
    /**
    @brief Transformations of a scenario of the session: aggregation, split and action
    building.

    Internal to FMTWrapperCore. Each transformation writes its result to the project and
    adds the produced scenario to the cache. The interface reaches them through Controller,
    which holds no logic of its own.
    */
    class TransformationUseCases
    {
    public:
        static void aggregateAllActions(
            int p_modelIndex,
            const std::vector<std::string>& p_aggregates,
            const std::vector<std::string>& p_order,
            const std::string& p_primaryFilePath,
            const std::string& p_scenarioName);

        static void splitActions(
            int p_modelIndex,
            const std::string& p_primaryFilePath,
            const std::vector<std::string>& p_splitted,
            const std::vector<std::string>& p_splittedMasks,
            const std::string& p_scenarioName);

        static void buildAction(
            int p_modelIndex,
            const std::string& p_actionName,
            const std::string& p_targetYield,
            const std::string& p_primaryFilePath,
            const std::string& p_scenarioName);
    };
}

#endif // FMTWRAPPERCORE_TRANSFORMATIONUSECASES_HEADER
