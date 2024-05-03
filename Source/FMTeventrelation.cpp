/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTeventrelation.h"
#include "FMTevent.h"

namespace Spatial
	{
	FMTeventrelation::FMTeventrelation(const Spatial::FMTevent& fromevent, const Spatial::FMTevent& toevent) :
		fromaction(fromevent.getactionid()), toaction(toevent.getactionid()),
		fromperiod(fromevent.getperiod()), toperiod(toevent.getperiod()),
		fromcoord(*fromevent.midposition()),tocoord(*toevent.midposition())
		{

		}
	bool FMTeventrelation::operator == (const FMTeventrelation& rhs) const
		{
		return (fromaction == rhs.fromaction && toaction == rhs.toaction&&
			fromperiod == rhs.fromperiod && toperiod == rhs.toperiod &&
			fromcoord == rhs.tocoord);
		}
	size_t FMTeventrelation::hash() const
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