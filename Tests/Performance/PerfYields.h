/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef PERFORMANCE_PERFYIELDS_H_INCLUDED
#define PERFORMANCE_PERFYIELDS_H_INCLUDED

#include "FMTDevelopment.h"
#include "FMTYieldRequest.h"
#include "FMTYields.h"

#include <cstddef>
#include <string>
#include <vector>

namespace Performance
{
	// The perfyields scenario of TWD_land, read once, and one yield request per period for the
	// development UNITE1 PEUPLEMENT1 UTR1 at age 7, whose yields performance.csv computes by hand.
	// The requests locate their yield data when they are prepared, outside the measured calls, as
	// the graph of a model does before asking for yields.
	class PerfYields
	{
	public:
		// Periods 1 to 255: the complex-yield cache keeps the period of its keys on 8 bits, so that
		// the requests give as many distinct keys for the same values.
		static constexpr std::size_t REQUESTS = 255;
		// An age yield of every development: VOLUMETOTAL, 120 at age 7.
		static const std::string AGE_YIELD;

		explicit PerfYields(std::string p_primaryFile);
		// Reads the scenario and prepares the requests. Only the first call does the work.
		void prepare();
		const Core::FMTYields& getYields() const;
		// Development of period p_index + 1, and its prepared request.
		const Core::FMTDevelopment& getDevelopment(std::size_t p_index) const;
		const Core::FMTYieldRequest& getRequest(std::size_t p_index) const;
		std::string getDataset() const;

	private:
		std::string m_primaryFile;
		Core::FMTYields m_yields;
		// The requests point to these developments: reserved once, the vector never moves them.
		std::vector<Core::FMTDevelopment> m_developments;
		std::vector<Core::FMTYieldRequest> m_requests;
		bool m_prepared = false;
	};
}

#endif
