/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTWRAPPERCORE_PLANNINGUSECASES_HEADER
#define FMTWRAPPERCORE_PLANNINGUSECASES_HEADER

#include "AreaVariabilityTypes.h"
#include "PlanningTypes.h"

#include <vector>

namespace FMTWrapper::Backend
{
    /**
    @brief Planning of the scenarios of the session: planning, replanning and initial-area
    variability.

    Internal to FMTWrapperCore. Each operation resolves the scenario indexes in the cache,
    then calls the service. The interface reaches them through Controller, which holds no
    logic of its own.
    */
    class PlanningUseCases
    {
    public:
        /**
        @brief Plans the given scenarios.

        A failed schedule read is reported the way the interface reports any error, without
        interrupting the planning.

        @param[in] p_playback One flag per scenario: true replays its schedule instead of
        optimizing it.
        */
        static void plan(
            const PlanningParameters& p_params,
            const std::vector<int>& p_modelIndexes,
            const std::vector<bool>& p_playback);

        static void replan(
            const ReplanningParameters& p_params,
            int p_strategicModelIndex,
            int p_stochasticModelIndex,
            int p_tacticalModelIndex);

        static AreaVariabilityResults runAreaVariability(
            const AreaVariabilityParameters& p_params,
            int p_modelIndex);
    };
}

#endif // FMTWRAPPERCORE_PLANNINGUSECASES_HEADER
