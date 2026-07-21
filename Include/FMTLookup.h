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
	// DocString: FMTLookup
	/**
	@brief Lookup helper holding an in-memory object and a pointer to a keyed object, used for hashing and comparison.
	@tparam inmemory the in-memory object type.
	@tparam pointer the pointed object type.
	*/
	template<class inmemory,class pointer>
	class FMTLookup
	{
	public:
		inmemory memoryobject;
		const pointer* pointerobject;
		// DocString: FMTLookup()
		/**
		@brief Default constructor for FMTLookup.
		*/
		FMTLookup() = default;
		// DocString: ~FMTLookup()
		/**
		@brief Default destructor for FMTLookup.
		*/
		~FMTLookup() = default;
		//For looking
		// DocString: FMTLookup(const pointer&)
		/**
		@brief Construct a lookup for searching from a pointer.
		@param[in] ptr the pointed object.
		*/
		FMTLookup(const pointer& ptr) :
			memoryobject(), pointerobject(&ptr)
		{

		}
		//For keeping
		// DocString: FMTLookup(const inmemory&, const pointer&)
		/**
		@brief Construct a lookup for keeping from an in-memory object and a pointer.
		@param[in] des the in-memory object.
		@param[in] dev the pointed object.
		*/
		FMTLookup(const inmemory& des, const pointer& dev) :
			memoryobject(des), pointerobject(&dev)
		{

		}
		// DocString: FMTLookup(const FMTLookup&)
		/**
		@brief Copy constructor for FMTLookup.
		@param[in] rhs the FMTLookup to copy.
		*/
		FMTLookup(const FMTLookup& rhs) :
			memoryobject(rhs.memoryobject), pointerobject(rhs.pointerobject)
		{

		}
		// DocString: FMTLookup::operator=
		/**
		@brief Copy assignment operator for FMTLookup.
		@param[in] rhs the FMTLookup to copy.
		@return a reference to this FMTLookup.
		*/
		FMTLookup& operator = (const FMTLookup& rhs)
		{
			if (this != &rhs)
			{
				memoryobject = rhs.memoryobject;
				pointerobject = rhs.pointerobject;
			}
			return *this;
		}
		// DocString: FMTLookup::operator<
		/**
		@brief Less than comparison operator of FMTLookup.
		@param[in] rhs the lookup to compare with.
		@return true if this lookup is less than rhs else false.
		*/
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
		
		// DocString: FMTLookup::operator==
		/**
		@brief Equality comparison operator of FMTLookup.
		@param[in] rhs the lookup to compare with.
		@return true if both lookups are equal else false.
		*/
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
	class FMTVertexLookup
	{
	public:
		tdescriptor descriptor;
		const Core::FMTDevelopment* development;
		FMTVertexLookup() = default;
		~FMTVertexLookup() = default;
		//For looking
		FMTVertexLookup(const Core::FMTDevelopment& dev) :
			descriptor(), development(&dev)
			{

			}
		//For keeping
		FMTVertexLookup(const tdescriptor& des, const Core::FMTDevelopment& dev) :
			descriptor(des), development(&dev)
			{

			}
		FMTVertexLookup(const FMTVertexLookup& rhs) :
			descriptor(rhs.descriptor), development(rhs.development)
			{

			}
		FMTVertexLookup& operator = (const FMTVertexLookup& rhs)
			{
			if (*this!=&rhs)
				{
				descriptor = rhs.descriptor;
				development = rhs.development;
				}
			return *this;
			}
		bool operator == (const FMTVertexLookup& rhs) const
			{
			return  (development != nullptr && rhs.development != nullptr  && (*development) == (*rhs.development));
			}

	};
}

namespace boost
{

	template<class tdescriptor>
	struct hash<Graph::FMTVertexLookup<tdescriptor>>
	{
		std::size_t operator()(const Graph::FMTVertexLookup<tdescriptor>& lookup) const
		{
			return lookup.development->hash();
		}
	};


}*/

#endif