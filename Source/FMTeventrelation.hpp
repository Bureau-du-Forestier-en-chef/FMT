/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTeventrelation_H_INCLUDED
#define FMTeventrelation_H_INCLUDED

#include "FMTcoordinate.hpp"
#include <boost/functional/hash.hpp>

namespace Spatial
{
	class FMTevent;
}

namespace Spatial
{
	class FMTeventrelation
	{
		int fromaction;
		int toaction;
		int fromperiod;
		int toperiod;
		FMTcoordinate fromcoord;
		FMTcoordinate tocoord;
	public:
		FMTeventrelation(const Spatial::FMTevent& fromevent, const Spatial::FMTevent& toevent);
		bool operator == (const FMTeventrelation& rhs) const;
		size_t hash() const;
		FMTeventrelation(const FMTeventrelation& rhs) = default;
		FMTeventrelation& operator = (const FMTeventrelation& rhs) = default;
		~FMTeventrelation() = default;
	};

}

namespace boost 
	{
	template <>
	struct hash<Spatial::FMTeventrelation>
		{
			std::size_t operator()(const Spatial::FMTeventrelation& relation) const
			{
				return relation.hash();
			}
		};
	}


#endif
