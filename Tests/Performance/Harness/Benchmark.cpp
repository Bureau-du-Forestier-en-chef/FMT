/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "Benchmark.h"

#include <algorithm>

namespace Performance
{
	std::string Benchmark::getDatasetFingerprint() const
	{
		return std::string();
	}

	std::string Benchmark::getUnavailableReason() const
	{
		return std::string();
	}

	RunSettings Benchmark::getSettings(BenchmarkMode p_mode) const
	{
		return RunSettings::forMode(p_mode);
	}

	ThreadScope Benchmark::getThreadScope() const
	{
		return ThreadScope::Current;
	}

	const std::vector<std::string>& Benchmark::getPhaseNames() const
	{
		return m_phaseNames;
	}

	const std::vector<double>& Benchmark::getPhaseDurations() const
	{
		return m_phaseDurations;
	}

	void Benchmark::clearPhaseDurations()
	{
		std::fill(m_phaseDurations.begin(), m_phaseDurations.end(), 0.0);
	}

	std::size_t Benchmark::definePhase(const std::string& p_name)
	{
		m_phaseNames.push_back(p_name);
		m_phaseDurations.push_back(0.0);
		return m_phaseNames.size() - 1;
	}

	void Benchmark::beginPhases()
	{
		m_phaseMark = std::chrono::steady_clock::now();
	}

	void Benchmark::endPhase(std::size_t p_phase)
	{
		const std::chrono::steady_clock::time_point NOW = std::chrono::steady_clock::now();
		m_phaseDurations.at(p_phase) += std::chrono::duration<double, std::nano>(NOW - m_phaseMark).count();
		m_phaseMark = NOW;
	}
}
