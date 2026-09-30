/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "PerfYields.h"

#include "FMTMask.h"
#include "FMTModel.h"
#include "FMTModelParser.h"
#include "FMTTheme.h"

#include <filesystem>
#include <utility>

namespace
{
	const std::string SCENARIO = "perfyields";
	// A development of TWD_land where volumetotal is 120: every expected value of performance.csv
	// is computed by hand from the yield table of the scenario.
	const std::string MASK = "UNITE1 PEUPLEMENT1 UTR1";
	constexpr int AGE = 7;
}

namespace Performance
{
	const std::string PerfYields::AGE_YIELD = "VOLUMETOTAL";

	PerfYields::PerfYields(std::string p_primaryFile) :
		m_primaryFile(std::move(p_primaryFile))
	{
	}

	void PerfYields::prepare()
	{
		if (m_prepared)
		{
			return;
		}
		Parser::FMTModelParser parser;
		parser.setDefaultExceptionHandler();
		const std::vector<Models::FMTModel> MODELS = parser.readproject(m_primaryFile, std::vector<std::string>(1, SCENARIO));
		m_yields = MODELS.at(0).getYields();
		const std::vector<Core::FMTTheme> THEMES = MODELS.at(0).getThemes();
		m_developments.reserve(REQUESTS);
		m_requests.reserve(REQUESTS);
		for (std::size_t index = 0; index < REQUESTS; ++index)
		{
			const int PERIOD = static_cast<int>(index) + 1;
			m_developments.emplace_back(Core::FMTMask(MASK, THEMES), AGE, 0, PERIOD);
			m_requests.push_back(m_developments.back().getYieldRequest());
			// A new request locates the yield data of its development at its first use: done here,
			// outside the measured calls.
			m_yields.get(m_requests.back(), AGE_YIELD);
		}
		m_prepared = true;
	}

	const Core::FMTYields& PerfYields::getYields() const
	{
		return m_yields;
	}

	const Core::FMTDevelopment& PerfYields::getDevelopment(std::size_t p_index) const
	{
		return m_developments.at(p_index);
	}

	const Core::FMTYieldRequest& PerfYields::getRequest(std::size_t p_index) const
	{
		return m_requests.at(p_index);
	}

	std::string PerfYields::getDataset() const
	{
		return std::filesystem::path(m_primaryFile).stem().string() + "/" + SCENARIO;
	}
}
