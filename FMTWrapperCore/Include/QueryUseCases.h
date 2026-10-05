/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTWRAPPERCORE_QUERYUSECASES_HEADER
#define FMTWRAPPERCORE_QUERYUSECASES_HEADER

#include <set>
#include <string>
#include <vector>

namespace FMTWrapper::Backend
{
    /**
    @brief Questions asked of a scenario of the session, designated by its index.

    Internal to FMTWrapperCore. Each operation resolves the index in the cache, then calls
    ModelQuery. The interface reaches them through Controller, which holds no logic of its
    own.
    */
    class QueryUseCases
    {
    public:
        static int getMaxAge(int p_modelIndex);

        static double getYield(
            int p_modelIndex,
            const std::string& p_mask,
            const std::string& p_yield,
            int p_age);

        static std::set<std::string> getAllMasks(
            int p_modelIndex,
            int p_periods,
            const std::vector<int>& p_themesNumbers,
            const std::string& p_rasterPath);

        static bool validateMask(int p_modelIndex, const std::string& p_mask);

        static std::vector<std::string> getConstraintsAsText(int p_modelIndex);

        /**
        @brief Names of the outputs of a scenario.

        @return An empty list when no scenario is loaded: the interface fills its output list
        before loading one, unlike the other questions.
        */
        static std::vector<std::string> getOutputsNames(int p_modelIndex);

        static std::vector<std::string> getActionsNames(int p_modelIndex);

        static std::vector<std::string> getAggregates(int p_modelIndex);

        static std::vector<std::string> getYieldsNames(int p_modelIndex);

        static int getThemesCount(int p_modelIndex);

        static std::vector<std::string> getThemeAttributes(int p_modelIndex, int p_themeIndex);

        static int getPeriodsCount(const std::string& p_primaryFilePath, int p_modelIndex);
    };
}

#endif // FMTWRAPPERCORE_QUERYUSECASES_HEADER
