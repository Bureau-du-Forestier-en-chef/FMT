/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTvertexproperties_Hm_included
#define FMTvertexproperties_Hm_included


#include <boost/serialization/serialization.hpp>
#include <boost/functional/hash.hpp>
#include "FMTBaseVertexProperties.h"
#include <boost/serialization/export.hpp>
#include "FMTutility.h"

namespace Core
{
	class FMTFuturDevelopment;
	class FMTActualDevelopment;
}



namespace Graph
{
	class FMTEXPORT FMTVertexProperties : public FMTBaseVertexProperties
		{
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
			{
			ar & boost::serialization::make_nvp("FMTbasevertexproperties", boost::serialization::base_object<FMTBaseVertexProperties>(*this));
			ar & BOOST_SERIALIZATION_NVP(constraintID);
			}
			int constraintID;
		public:
			~FMTVertexProperties() = default;
			FMTVertexProperties() = default;
			FMTVertexProperties(const FMTVertexProperties& rhs);
			FMTVertexProperties& operator = (const FMTVertexProperties& rhs) = default;
			FMTVertexProperties(const Core::FMTFuturDevelopment& ldevelopment,
				const int& lconstraintID);
			FMTVertexProperties(const Core::FMTActualDevelopment& ldevelopment,
				const int& lconstraintID);
            FMTVertexProperties(const Core::FMTDevelopment& ldevelopment,
                const int& lconstraintID);
			std::string constraintName() const;
			inline int getConstraintId() const override
				{
				return constraintID;
				}
			void setConstraintID(const int& ID);
		};
}

BOOST_CLASS_EXPORT_KEY(Graph::FMTVertexProperties)

namespace boost {

	template <>
	struct hash<Graph::FMTVertexProperties>
	{
		std::size_t operator()(const Graph::FMTVertexProperties& vertex_properties) const
		{
			return vertex_properties.hash();
		}
	};


}


#endif
