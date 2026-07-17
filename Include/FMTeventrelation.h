/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTeventrelation_Hm_included
#define FMTeventrelation_Hm_included

#include "FMTcoordinate.h"
#include <boost/functional/hash.hpp>

namespace Spatial
{
	class FMTEvent;
}

namespace Spatial
{
	class FMTEventRelation
	{
		int fromaction;
		int toaction;
		int fromperiod;
		int toperiod;
		FMTCoordinate fromcoord;
		FMTCoordinate tocoord;
	public:
		FMTEventRelation(const Spatial::FMTEvent& fromevent, const Spatial::FMTEvent& toevent);
		bool operator == (const FMTEventRelation& rhs) const;
		size_t hash() const;
		FMTEventRelation(const FMTEventRelation& rhs) = default;
		FMTEventRelation& operator = (const FMTEventRelation& rhs) = default;
		~FMTEventRelation() = default;
	};

}

namespace boost 
	{
	template <>
	struct hash<Spatial::FMTEventRelation>
		{
			std::size_t operator()(const Spatial::FMTEventRelation& relation) const
			{
				return relation.hash();
			}
		};
	}


#endif
