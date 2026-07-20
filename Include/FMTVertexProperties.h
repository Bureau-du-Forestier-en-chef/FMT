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
	// DocString: FMTVertexProperties
	/**
	@brief Vertex properties of a FMTGraph, extending the base vertex properties with a constraint id.
	*/
	class FMTEXPORT FMTVertexProperties : public FMTBaseVertexProperties
		{
		friend class boost::serialization::access;
		// DocString: FMTVertexProperties::serialize
		/**
		@brief Serialize the FMTVertexProperties for multiprocessing across multiple cpus (pickle in Python).
		@tparam Archive the archive type.
		@param[in,out] ar the archive to serialize to or from.
		@param[in] version the serialization version.
		*/
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
			{
			ar & boost::serialization::make_nvp("FMTbasevertexproperties", boost::serialization::base_object<FMTBaseVertexProperties>(*this));
			ar & BOOST_SERIALIZATION_NVP(constraintID);
			}
			int constraintID;
		public:
			// DocString: ~FMTVertexProperties()
			/**
			@brief Default destructor for FMTVertexProperties.
			*/
			~FMTVertexProperties() = default;
			// DocString: FMTVertexProperties()
			/**
			@brief Default constructor for FMTVertexProperties.
			*/
			FMTVertexProperties() = default;
			// DocString: FMTVertexProperties(const FMTVertexProperties&)
			/**
			@brief Copy constructor for FMTVertexProperties.
			@param[in] rhs the FMTVertexProperties to copy.
			*/
			FMTVertexProperties(const FMTVertexProperties& rhs);
			// DocString: FMTVertexProperties::operator=
			/**
			@brief Copy assignment operator for FMTVertexProperties.
			@param[in] rhs the FMTVertexProperties to copy.
			@return a reference to this FMTVertexProperties.
			*/
			FMTVertexProperties& operator = (const FMTVertexProperties& rhs) = default;
			// DocString: FMTVertexProperties(const Core::FMTFuturDevelopment&,const int&)
			/**
			@brief Construct a FMTVertexProperties from a futur development and a constraint id.
			@param[in] ldevelopment the futur development.
			@param[in] lconstraintID the constraint id.
			*/
			FMTVertexProperties(const Core::FMTFuturDevelopment& ldevelopment,
				const int& lconstraintID);
			// DocString: FMTVertexProperties(const Core::FMTActualDevelopment&,const int&)
			/**
			@brief Construct a FMTVertexProperties from an actual development and a constraint id.
			@param[in] ldevelopment the actual development.
			@param[in] lconstraintID the constraint id.
			*/
			FMTVertexProperties(const Core::FMTActualDevelopment& ldevelopment,
				const int& lconstraintID);
            // DocString: FMTVertexProperties(const Core::FMTDevelopment&,const int&)
            /**
            @brief Construct a FMTVertexProperties from a development and a constraint id.
            @param[in] ldevelopment the development.
            @param[in] lconstraintID the constraint id.
            */
            FMTVertexProperties(const Core::FMTDevelopment& ldevelopment,
                const int& lconstraintID);
			// DocString: FMTVertexProperties::constraintName
			/**
			@brief Return the name of the constraint of the vertex.
			@return the constraint name.
			*/
			std::string constraintName() const;
			// DocString: FMTVertexProperties::getConstraintId
			/**
			@brief Return the constraint id of the vertex.
			@return the constraint id.
			*/
			inline int getConstraintId() const override
				{
				return constraintID;
				}
			// DocString: FMTVertexProperties::setConstraintID
			/**
			@brief Set the constraint id of the vertex.
			@param[in] ID the constraint id.
			*/
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
