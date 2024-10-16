/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTdevelopmentpath.h"
#include "FMTdevelopment.h"

namespace Core

{ 
	const FMTdevelopment& FMTdevelopmentpath::getDevelopment() const
	{
		return *m_development;
	}
	
	double FMTdevelopmentpath::getProportion() const
	{
		return m_proportion;
	}

	void FMTdevelopmentpath::setProportion(double p_proportion)
		{
		m_proportion = p_proportion;
		}

	FMTdevelopmentpath::FMTdevelopmentpath(): m_development(), m_proportion()
		{

		}
	FMTdevelopmentpath::FMTdevelopmentpath(const FMTdevelopmentpath& rhs) : m_development(), m_proportion(rhs.m_proportion)
		{
		m_development = std::unique_ptr<FMTdevelopment>(new FMTdevelopment(*rhs.m_development));
		}
	FMTdevelopmentpath::FMTdevelopmentpath(const FMTdevelopment& ldevelopment,
		const double& lproportion) : m_development(), m_proportion(lproportion)
		{
		m_development = std::unique_ptr<FMTdevelopment>(new FMTdevelopment(ldevelopment));
		}
	FMTdevelopmentpath& FMTdevelopmentpath::operator = (const FMTdevelopmentpath& rhs)
		{
		if (this!=&rhs)
			{
			m_development = std::unique_ptr<FMTdevelopment>(new FMTdevelopment(*rhs.m_development));
			m_proportion = rhs.m_proportion;
			}
		return *this;
		}

}


