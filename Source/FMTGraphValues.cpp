/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/



#include "FMTGraphValues.h"
#include <boost/functional/hash.hpp>



namespace Spatial
	{

	FMTGraphValues::FMTGraphValues(const std::vector<double>& p_values):
		m_Values(p_values)
	{

	}
	bool FMTGraphValues::operator == (const FMTGraphValues& p_Other) const
	{
		return m_Values == p_Other.m_Values;
	}
	size_t FMTGraphValues::hash() const
	{
		std::size_t hash = 0;
		boost::hash_range(hash, m_Values.begin(), m_Values.end());
		return hash;
	}

	const std::vector<double>& FMTGraphValues::GetValues() const
	{
		return m_Values;
	}

}


