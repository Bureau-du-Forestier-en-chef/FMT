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

	// Without a minimum duration, the calibration stops at one call per sample.
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
}
