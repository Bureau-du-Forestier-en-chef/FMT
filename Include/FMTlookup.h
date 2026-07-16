/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTlookup_Hm_included
#define FMTlookup_Hm_included

//#include "FMTDevelopment.h"
#include <boost/functional/hash.hpp>

namespace Core
{
	template<class inmemory,class pointer>
	class FMTLookup
	{
	public:
		inmemory memoryobject;
		const pointer* pointerobject;
		FMTLookup() = default;
		~FMTLookup() = default;
		//For looking
		FMTLookup(const pointer& ptr) :
			memoryobject(), pointerobject(&ptr)
		{

		}
		//For keeping
		FMTLookup(const inmemory& des, const pointer& dev) :
			memoryobject(des), pointerobject(&dev)
		{

		}
		FMTLookup(const FMTLookup& rhs) :
			memoryobject(rhs.memoryobject), pointerobject(rhs.pointerobject)
		{

		}
		FMTLookup& operator = (const FMTLookup& rhs)
		{
			if (this != &rhs)
			{
				memoryobject = rhs.memoryobject;
				pointerobject = rhs.pointerobject;
			}
			return *this;
		}
		bool operator < (const FMTLookup& rhs) const
		{
			//strict ordering
			if (pointerobject == nullptr)
				return true;
			if (rhs.pointerobject == nullptr)
				return false;
			if ((*pointerobject) < (*rhs.pointerobject))
				return true;
			if ((*rhs.pointerobject) < (*pointerobject))
				return false;
			return false;
		}
		
		bool operator == (const FMTLookup& rhs) const
		{
			return  (pointerobject != nullptr && rhs.pointerobject != nullptr && (*pointerobject) == (*rhs.pointerobject));
		}

	};
}

namespace boost
{

	template<class inmemory, class pointer>
	struct hash<Core::FMTLookup<inmemory,pointer>>
	{
		std::size_t operator()(const Core::FMTLookup<inmemory, pointer>& lookup) const
		{
			return lookup.pointerobject->hash();
		}
	};

	template<> inline std::size_t hash<Core::FMTLookup<std::vector<size_t>, std::string>>::operator()(const Core::FMTLookup<std::vector<size_t>, std::string>& lookup) const
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
		const Core::FMTDevelopment* development;
		FMTvertexlookup() = default;
		~FMTvertexlookup() = default;
		//For looking
		FMTvertexlookup(const Core::FMTDevelopment& dev) :
			descriptor(), development(&dev)
			{

			}
		//For keeping
		FMTvertexlookup(const tdescriptor& des, const Core::FMTDevelopment& dev) :
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