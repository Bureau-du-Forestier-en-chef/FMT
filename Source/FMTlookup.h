/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTlookup_H_INCLUDED
#define FMTlookup_H_INCLUDED

//#include "FMTdevelopment.h"
#include <boost/functional/hash.hpp>

namespace Core
{
	template<class inmemory,class pointer>
	class FMTlookup
	{
	public:
		inmemory memoryobject;
		const pointer* pointerobject;
		FMTlookup() = default;
		~FMTlookup() = default;
		//For looking
		FMTlookup(const pointer& ptr) :
			memoryobject(), pointerobject(&ptr)
		{

		}
		//For keeping
		FMTlookup(const inmemory& des, const pointer& dev) :
			memoryobject(des), pointerobject(&dev)
		{

		}
		FMTlookup(const FMTlookup& rhs) :
			memoryobject(rhs.memoryobject), pointerobject(rhs.pointerobject)
		{

		}
		FMTlookup& operator = (const FMTlookup& rhs)
		{
			if (this != &rhs)
			{
				memoryobject = rhs.memoryobject;
				pointerobject = rhs.pointerobject;
			}
			return *this;
		}
		bool operator == (const FMTlookup& rhs) const
		{
			return  (pointerobject != nullptr && rhs.pointerobject != nullptr && (*pointerobject) == (*rhs.pointerobject));
		}

	};
}

namespace boost
{

	template<class inmemory, class pointer>
	struct hash<Core::FMTlookup<inmemory,pointer>>
	{
		std::size_t operator()(const Core::FMTlookup<inmemory, pointer>& lookup) const
		{
			return lookup.pointerobject->hash();
		}
	};

	template<> inline std::size_t hash<Core::FMTlookup<std::vector<size_t>, std::string>>::operator()(const Core::FMTlookup<std::vector<size_t>, std::string>& lookup) const
		{
		return boost::hash<std::string>()(*lookup.pointerobject);
		}


}

/*
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


}*/

#endif