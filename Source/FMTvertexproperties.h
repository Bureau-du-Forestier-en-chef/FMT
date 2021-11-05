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
#include "FMTbasevertexproperties.h"



namespace Graph
{
	class FMTvertexproperties : public FMTbasevertexproperties
		{
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
			{
			ar & boost::serialization::make_nvp("FMTbasevertexproperties", boost::serialization::base_object<FMTbasevertexproperties>(*this));
			ar & BOOST_SERIALIZATION_NVP(constraintID);
			}
			int constraintID;
		public:
			~FMTvertexproperties() = default;
			FMTvertexproperties() = default;
			FMTvertexproperties(const FMTvertexproperties& rhs);
			FMTvertexproperties& operator = (const FMTvertexproperties& rhs) = default;
			FMTvertexproperties(const Core::FMTfuturdevelopment& ldevelopment,
				const int& lconstraintID);
			FMTvertexproperties(const Core::FMTactualdevelopment& ldevelopment,
				const int& lconstraintID);
            FMTvertexproperties(const Core::FMTdevelopment& ldevelopment,
                const int& lconstraintID);
			std::string constraintname() const;
			inline int getconstraintID() const override
				{
				return constraintID;
				}
			void setconstraintID(const int& ID);
		};
}

BOOST_CLASS_EXPORT_KEY(Graph::FMTvertexproperties)

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
