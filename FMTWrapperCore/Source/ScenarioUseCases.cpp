/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "ScenarioUseCases.h"

#include "FMTException.h"
#include "FMTFreeExceptionHandler.h"
#include "FMTModel.h"
#include "FMTModelParser.h"
#include "ModelCache.h"
#include "ModelQuery.h"

#include <string>
#include <vector>

namespace FMTWrapper::Backend
{
    void ScenarioUseCases::addScenarios(
        const std::string& p_primaryFilePath,
        const std::vector<std::string>& p_scenarioNames)
    {
        Parser::FMTModelParser modelParser;

        const std::vector<Models::FMTModel> MODELS =
            modelParser.readproject(p_primaryFilePath, p_scenarioNames);

        for (const Models::FMTModel& MODEL : MODELS)
        {
            ModelCache::GetInstance()->push_back(MODEL);
        }
    }

    void ScenarioUseCases::removeScenario(int p_modelIndex)
    {
        ModelCache::GetInstance()->erase(p_modelIndex);
    }

    void ScenarioUseCases::clearScenarios()
    {
        ModelCache::GetInstance()->clear();
    }

    void ScenarioUseCases::writeScenariosToProject(const std::string& p_destinationDirectory)
    {
        ModelCache* cache = ModelCache::GetInstance();

        if (cache->empty())
        {
            Exception::FMTFreeExceptionHandler().raise(
                Exception::FMTexc::FMTrangeerror,
                "empty cache",
                "ScenarioUseCases::writeScenariosToProject",
                __LINE__,
                __FILE__);
        }

        std::vector<Models::FMTModel> models;
        models.reserve(cache->size());

        for (size_t index = 0; index < cache->size(); ++index)
        {
            models.push_back(cache->getModel(static_cast<int>(index)));
        }

        ModelQuery::writeToProject(models, p_destinationDirectory);
    }
}
