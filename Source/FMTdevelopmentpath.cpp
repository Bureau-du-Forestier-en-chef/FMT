/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTdevelopmentpath.h"
#include "FMTdevelopment.h"

namespace Core

{ 
	FMTdevelopmentpath::FMTdevelopmentpath():
		m_development(),
		m_proportion()
	{

	}



	const FMTdevelopment& FMTdevelopmentpath::getDevelopment() const
	{
		return m_development;
	}

	FMTdevelopment& FMTdevelopmentpath::getDevelopmentReference()
	{
		return m_development;
	}
	
	double FMTdevelopmentpath::getProportion() const
	{
		return m_proportion;
	}

	void FMTdevelopmentpath::setProportion(double p_proportion)
	{
		m_proportion = p_proportion;
	}


	FMTdevelopmentpath::FMTdevelopmentpath(const FMTmask& p_mask,
		int p_age, int p_lock, int p_period, double p_proportion) :
		m_development(p_mask, p_age, p_lock, p_period),
		m_proportion(p_proportion)
	{

	}


}


