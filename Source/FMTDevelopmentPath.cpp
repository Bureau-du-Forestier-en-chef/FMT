/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTDevelopmentPath.h"
#include "FMTDevelopment.h"

namespace Core

{ 
	FMTDevelopmentPath::FMTDevelopmentPath():
		m_development(),
		m_proportion()
	{

	}



	const FMTDevelopment& FMTDevelopmentPath::getDevelopment() const
	{
		return m_development;
	}

	FMTDevelopment& FMTDevelopmentPath::getDevelopmentReference()
	{
		return m_development;
	}
	
	double FMTDevelopmentPath::getProportion() const
	{
		return m_proportion;
	}

	void FMTDevelopmentPath::setProportion(double p_proportion)
	{
		m_proportion = p_proportion;
	}


	FMTDevelopmentPath::FMTDevelopmentPath(const FMTMask& p_mask,
		int p_age, int p_lock, int p_period, double p_proportion) :
		m_development(p_mask, p_age, p_lock, p_period),
		m_proportion(p_proportion)
	{

	}


}


