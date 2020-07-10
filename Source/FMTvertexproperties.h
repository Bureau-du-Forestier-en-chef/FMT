/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTvertexproperties_H_INCLUDED
#define FMTvertexproperties_H_INCLUDED

#include "FMTfuturdevelopment.h"
#include "FMTactualdevelopment.h"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/unique_ptr.hpp>
#include <memory>



namespace Graph
{
	class FMTvertexproperties
		{
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
			{
			ar & BOOST_SERIALIZATION_NVP(development);
			ar & BOOST_SERIALIZATION_NVP(constraintID);
			}
			std::unique_ptr<Core::FMTdevelopment>development;
			int constraintID;
		public:
			~FMTvertexproperties() = default;
			FMTvertexproperties();
			bool operator < (const FMTvertexproperties& rhs) const;
			FMTvertexproperties(const FMTvertexproperties& rhs);
			FMTvertexproperties& operator = (const FMTvertexproperties& rhs);
			FMTvertexproperties(const Core::FMTfuturdevelopment& ldevelopment,
				const int& lconstraintID);
			FMTvertexproperties(const Core::FMTactualdevelopment& ldevelopment,
				const int& lconstraintID);
            FMTvertexproperties(const Core::FMTdevelopment& ldevelopment,
                const int& lconstraintID);
			std::string constraintname() const;
			inline int getconstraintID() const
				{
				return constraintID;
				}
			void setconstraintID(const int& ID);
			inline const Core::FMTdevelopment& get() const
				{
				return (*development);
				}
			void setdevlopementmask(const Core::FMTmask& newmask);
			double getbaseRHS() const;
			size_t hash() const;
			bool operator == (const FMTvertexproperties& rhs) const;
			bool operator != (const FMTvertexproperties& rhs) const;
		};
}

namespace boost {

	template <>
	struct hash<Graph::FMTvertexproperties>
	{
		std::size_t operator()(const Graph::FMTvertexproperties& vertex_properties) const
		{
			return vertex_properties.hash();
		}
	};


}


#endif
