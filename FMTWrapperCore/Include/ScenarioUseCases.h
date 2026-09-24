/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTWRAPPERCORE_SCENARIOUSECASES_HEADER
#define FMTWRAPPERCORE_SCENARIOUSECASES_HEADER

#include <string>
#include <vector>

namespace FMTWrapper::Backend
{
    /**
    @brief Scenarios loaded in the session: reading, removal and writing to a project.

    Internal to FMTWrapperCore. The interface reaches these operations through Controller,
    which holds no logic of its own.
    */
    class ScenarioUseCases
    {
    public:
        /**
        @brief Reads the given scenarios of a project and adds them to the cache.
        */
        static void addScenarios(
            const std::string& p_primaryFilePath,
            const std::vector<std::string>& p_scenarioNames);

        static void removeScenario(int p_modelIndex);

        static void clearScenarios();

        /**
        @brief Writes every cached scenario to a project.

        @throws Exception::FMTexception When no scenario is loaded.
        */
        static void writeScenariosToProject(const std::string& p_destinationDirectory);
    };
}

#endif // FMTWRAPPERCORE_SCENARIOUSECASES_HEADER
