/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTWRAPPERCORE_SPATIALUSECASES_HEADER
#define FMTWRAPPERCORE_SPATIALUSECASES_HEADER

#include "OperatingAreaTypes.h"
#include "RasterizationTypes.h"
#include "SESTypes.h"

namespace FMTWrapper::Backend
{
    /**
    @brief Spatial treatments of a scenario of the session: simulation, simulated annealing,
    rasterization and operating-area scheduling.

    Internal to FMTWrapperCore. Each operation resolves the scenario index in the cache, then
    calls the service. The interface reaches them through Controller, which holds no logic of
    its own.
    */
    class SpatialUseCases
    {
    public:
        static SESResults runSpatialSimulation(
            const SESParameters& p_params,
            int p_modelIndex);

        static SAResults runSpatialOptimization(
            const SAParameters& p_params,
            int p_modelIndex);

        static void rasterize(
            const RasterizationParameters& p_params,
            int p_modelIndex);

        static OperatingAreaResults scheduleOperatingAreas(
            const OperatingAreaParameters& p_params,
            int p_modelIndex);
    };
}

#endif // FMTWRAPPERCORE_SPATIALUSECASES_HEADER
