/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTvertexlookup_H_INCLUDED
#define FMTvertexlookup_H_INCLUDED

#include "FMTdevelopment.h"
#include <boost/functional/hash.hpp>

namespace Graph
{
	template<class tdescriptor>
	class FMTvertexlookup
	{
	public:
		tdescriptor descriptor;
		const Core::FMTdevelopment* development;
		FMTvertexlookup() = default;
		~FMTvertexlookup() = default;
		//For looking
		FMTvertexlookup(const Core::FMTdevelopment& dev) :
			descriptor(), development(&dev)
			{

			}
		//For keeping
		FMTvertexlookup(const tdescriptor& des, const Core::FMTdevelopment& dev) :
			descriptor(des), development(&dev)
			{

			}
		FMTvertexlookup(const FMTvertexlookup& rhs) :
			descriptor(rhs.descriptor), development(rhs.development)
			{

			}
		FMTvertexlookup& operator = (const FMTvertexlookup& rhs)
			{
			if (*this!=&rhs)
				{
				descriptor = rhs.descriptor;
				development = rhs.development;
				}
			return *this;
			}
		bool operator == (const FMTvertexlookup& rhs) const
			{
			return  (development != nullptr && rhs.development != nullptr  && (*development) == (*rhs.development));
			}

	};
}

namespace boost
{

	template<class tdescriptor>
	struct hash<Graph::FMTvertexlookup<tdescriptor>>
	{
		std::size_t operator()(const Graph::FMTvertexlookup<tdescriptor>& lookup) const
		{
			return lookup.development->hash();
		}
	};


}

#endif