/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/
#ifndef FMTGRAPHVALUES_H
#define FMTGRAPHVALUES_H

#include <vector>
#include <boost/functional/hash.hpp>


namespace Spatial
	{
	class FMTGraphValues
		{
		std::vector<double> m_Values;
		public:
			FMTGraphValues() = default;
			FMTGraphValues(const std::vector<double>& p_values);
			bool operator == (const FMTGraphValues& p_Other) const;
			const std::vector<double>& GetValues() const;
			size_t hash() const;
		};
	}

#endif 
