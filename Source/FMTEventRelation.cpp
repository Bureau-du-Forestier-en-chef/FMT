/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTEventRelation.h"
#include "FMTEvent.h"

namespace Spatial
	{
	FMTEventRelation::FMTEventRelation(const Spatial::FMTEvent& fromevent, const Spatial::FMTEvent& toevent) :
		m_fromaction(fromevent.getActionId()), m_toaction(toevent.getActionId()),
		m_fromperiod(fromevent.getPeriod()), m_toperiod(toevent.getPeriod()),
		m_fromcoord(*fromevent.midPosition()),m_tocoord(*toevent.midPosition())
		{

		}
	bool FMTEventRelation::operator == (const FMTEventRelation& rhs) const
		{
		return (m_fromaction == rhs.m_fromaction && m_toaction == rhs.m_toaction&&
			m_fromperiod == rhs.m_fromperiod && m_toperiod == rhs.m_toperiod &&
			m_fromcoord == rhs.m_tocoord);
		}
	size_t FMTEventRelation::hash() const
		{
		size_t hash = 0;
		boost::hash_combine(hash,m_fromaction);
		boost::hash_combine(hash,m_toaction);
		boost::hash_combine(hash,m_fromperiod);
		boost::hash_combine(hash, m_fromcoord.hash());
		boost::hash_combine(hash, m_tocoord.hash());
		return hash;
		}
	}