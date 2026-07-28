/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTeventrelation_Hm_included
#define FMTeventrelation_Hm_included

#include "FMTCoordinate.h"
#include <boost/functional/hash.hpp>

namespace Spatial
{
	class FMTEvent;
}

namespace Spatial
{
	// DocString: FMTEventRelation
	/**
	@brief Relation between two spatial events, holding the from and to action, period and coordinate of the closest points.
	*/
	class FMTEventRelation
	{
	public:
		// DocString: FMTEventRelation(const Spatial::FMTEvent&,const Spatial::FMTEvent&)
		/**
		@brief Construct a FMTEventRelation from two events.
		@param[in] fromevent the from event.
		@param[in] toevent the to event.
		*/
		FMTEventRelation(const Spatial::FMTEvent& fromevent, const Spatial::FMTEvent& toevent);
		// DocString: FMTEventRelation::operator==
		/**
		@brief Comparison operator for FMTEventRelation.
		@param[in] rhs the FMTEventRelation to compare to.
		@return true if both relations are equal else false.
		*/
		bool operator == (const FMTEventRelation& rhs) const;
		// DocString: FMTEventRelation::hash
		/**
		@brief Hash the event relation.
		@return the hash of the relation.
		*/
		size_t hash() const;
		// DocString: FMTEventRelation(const FMTEventRelation&)
		/**
		@brief Copy constructor for FMTEventRelation.
		@param[in] rhs the FMTEventRelation to copy.
		*/
		FMTEventRelation(const FMTEventRelation& rhs) = default;
		// DocString: FMTEventRelation::operator=
		/**
		@brief Copy assignment operator for FMTEventRelation.
		@param[in] rhs the FMTEventRelation to copy.
		@return a reference to this FMTEventRelation.
		*/
		FMTEventRelation& operator = (const FMTEventRelation& rhs) = default;
		// DocString: ~FMTEventRelation()
		/**
		@brief Default destructor for FMTEventRelation.
		*/
		~FMTEventRelation() = default;
	private:
		int m_fromaction;
		int m_toaction;
		int m_fromperiod;
		int m_toperiod;
		FMTCoordinate m_fromcoord;
		FMTCoordinate m_tocoord;
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
