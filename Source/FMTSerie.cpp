/*
Copyright (c) 2024 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTSerie.h"
#include <string>
#include <algorithm>


namespace Core{


	FMTSerie::FMTSerie(const std::string& p_serie, int p_length) :
		m_serie(p_serie), m_length(p_length)
	{

	}

	int FMTSerie::getLength() const
	{
		return m_length;
	}

	const std::string& FMTSerie::getSerie() const
	{
		return m_serie;
	}

	bool FMTSerie::operator == (const FMTSerie& p_RHS) const
	{
		return (m_length == p_RHS.m_length && m_serie == p_RHS.m_serie);
	}

	bool FMTSerie::operator != (const FMTSerie& p_RHS) const
	{
		return (!FMTSerie::operator == (p_RHS));
	}

	bool FMTSerie::operator < (const FMTSerie& p_RHS) const
	{
		const size_t SERIE_SIZE = std::count(m_serie.begin(), m_serie.end(), '-');
		const size_t RHS_SERIE_SIZE = std::count(p_RHS.m_serie.begin(), p_RHS.m_serie.end(), '-');
		//strict ordering
		if (SERIE_SIZE < RHS_SERIE_SIZE)
			return true;
		if (RHS_SERIE_SIZE < SERIE_SIZE)
			return false;
		if (m_length < p_RHS.m_length)
			return true;
		if (p_RHS.m_length < m_length)
			return false;
		if (m_serie < p_RHS.m_serie)
			return true;
		if (p_RHS.m_serie < m_serie)
			return false;
		return false;
	}

	bool FMTSerie::operator > (const FMTSerie& p_RHS) const
	{
		const size_t SERIE_SIZE = std::count(m_serie.begin(), m_serie.end(), '-');
		const size_t RHS_SERIE_SIZE = std::count(p_RHS.m_serie.begin(), p_RHS.m_serie.end(), '-');
		if (SERIE_SIZE > RHS_SERIE_SIZE)
			return true;
		if (RHS_SERIE_SIZE > SERIE_SIZE)
			return false;
		if (m_length > p_RHS.m_length)
			return true;
		if (p_RHS.m_length > m_length)
			return false;
		if (m_serie > p_RHS.m_serie)
			return true;
		if (p_RHS.m_serie > m_serie)
			return false;
		return false;
	}
    
}

