/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTeventrelation.h"
#include "FMTevent.h"

namespace Spatial
	{
	FMTEventRelation::FMTEventRelation(const Spatial::FMTEvent& fromevent, const Spatial::FMTEvent& toevent) :
		fromaction(fromevent.getActionId()), toaction(toevent.getActionId()),
		fromperiod(fromevent.getPeriod()), toperiod(toevent.getPeriod()),
		fromcoord(*fromevent.midPosition()),tocoord(*toevent.midPosition())
		{

		}
	bool FMTEventRelation::operator == (const FMTEventRelation& rhs) const
		{
		return (fromaction == rhs.fromaction && toaction == rhs.toaction&&
			fromperiod == rhs.fromperiod && toperiod == rhs.toperiod &&
			fromcoord == rhs.tocoord);
		}
	size_t FMTEventRelation::hash() const
		{
		size_t hash = 0;
		boost::hash_combine(hash,fromaction);
		boost::hash_combine(hash,toaction);
		boost::hash_combine(hash,fromperiod);
		boost::hash_combine(hash, fromcoord.hash());
		boost::hash_combine(hash, tocoord.hash());
		return hash;
		}
	}