/*
Copyright (c) 2024 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTSerie.h"
#include <string>
#include <algorithm>
#include <boost/algorithm/string.hpp>


namespace Core{

	const std::string FMTSerie::m_SEPERATOR = " -> ";

	FMTSerie::FMTSerie() :
		m_serie(),
		m_length()
	{

	}


	FMTSerie::FMTSerie(const std::vector<std::string>& p_serie, int p_length):
		m_serie(p_serie), m_length(0)
		{
		

		}

	FMTSerie::FMTSerie(const std::vector<std::string>& p_serie,bool p_asap, bool p_alap):
		m_serie(p_serie), m_length(0)
		{
		if (p_alap)
			{
			m_length = static_cast<int>(m_serie.size());
		}else if (p_asap)
			{
			m_length = -1;
			}
		}


	int FMTSerie::getLength() const
	{
		return m_length;
	}

	const std::vector<std::string>& FMTSerie::getActions() const
		{
		return m_serie;
		}

	bool FMTSerie::isASAP() const
		{
		return  m_length < 0;
		}

	bool FMTSerie::isALAP() const
		{
		return  m_length == static_cast<int>(m_serie.size());
		}

	bool FMTSerie::isAllowedInSerie(const std::vector<std::string>& p_SerieMask) const
	{
		const std::string SERIE = getSerie();
		const std::string TEST_SERIE = FMTSerie(p_SerieMask,0).getSerie();
		std::vector<std::string>::const_iterator firstelement = std::find_first_of(p_SerieMask.begin(), p_SerieMask.end(),
																				m_serie.begin(), m_serie.end());

		if (firstelement != p_SerieMask.end() &&
			std::equal(firstelement, p_SerieMask.end(), m_serie.begin()) &&
			SERIE != TEST_SERIE)
		{
			return true;
		}
		return false;
	}

	bool FMTSerie::isEmpty() const
		{
		return m_serie.empty();
		}

	const std::string FMTSerie::getSerie() const
	{
		std::string returned;
		for (const auto& VALUE : m_serie)
		{
			returned += VALUE + m_SEPERATOR;
		}
		returned.erase(returned.size() - m_SEPERATOR.size(),
											m_SEPERATOR.size());
		return returned;
	}

	FMTSerie::operator std::string() const
	{
		std::string returned = getSerie();
		if (isALAP())
		{
			returned += " _ALAP";
		}
		if (isASAP())
		{
			returned += " _ASAP";

		}
		return  returned;
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
		const size_t SERIE_SIZE = m_serie.size();
		const size_t RHS_SERIE_SIZE = p_RHS.m_serie.size();
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
		const size_t SERIE_SIZE = m_serie.size();
		const size_t RHS_SERIE_SIZE = p_RHS.m_serie.size();
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

