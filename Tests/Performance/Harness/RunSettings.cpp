/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "RunSettings.h"

namespace Performance
{
	RunSettings RunSettings::forMode(BenchmarkMode p_mode)
	{
		RunSettings settings;
		if (p_mode == BenchmarkMode::Full)
		{
			settings.warmUpCalls = 100;
			settings.samples = 30;
			settings.minimumSampleDuration = std::chrono::milliseconds(5);
			settings.countedCalls = 1000;
		}
		else
		{
			settings.warmUpCalls = 3;
			settings.samples = 3;
			settings.minimumSampleDuration = std::chrono::microseconds(100);
			settings.countedCalls = 20;
		}
		return settings;
	}

	// Without a minimum duration, a sample is one call, and nothing is calibrated.
	RunSettings RunSettings::forSlowCalls(BenchmarkMode p_mode)
	{
		RunSettings settings;
		if (p_mode == BenchmarkMode::Full)
		{
			settings.warmUpCalls = 2;
			settings.samples = 10;
			settings.countedCalls = 20;
		}
		else
		{
			settings.warmUpCalls = 1;
			settings.samples = 2;
			settings.countedCalls = 3;
		}
		return settings;
	}

	// In the short mode, the timed call comes before the counted one: the counted call is never
	// the first of the process, whose one-time initializations would read as retained memory.
	RunSettings RunSettings::forFlows(BenchmarkMode p_mode)
	{
		RunSettings settings;
		if (p_mode == BenchmarkMode::Full)
		{
			settings.warmUpCalls = 1;
			settings.samples = 3;
			settings.countedCalls = 1;
		}
		else
		{
			settings.samples = 1;
			settings.countedCalls = 1;
		}
		return settings;
	}
}
