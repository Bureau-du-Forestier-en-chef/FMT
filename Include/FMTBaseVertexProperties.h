/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTbasevertexproperties_Hm_included
#define FMTbasevertexproperties_Hm_included



#include <boost/serialization/serialization.hpp>
#include <boost/functional/hash.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/nvp.hpp>
#include <memory>
#include "FMTutility.h"


namespace Core {
	class FMTActualDevelopment;
	class FMTFuturDevelopment;
	class FMTMask;
	class FMTDevelopment;
}



namespace Graph
{
	class FMTEXPORT FMTBaseVertexProperties
	{
	public:
		// DocString: FMTBaseVertexProperties::~FMTBaseVertexProperties
		/**
		@brief Default virtual destructor for FMTBaseVertexProperties.
		*/
		virtual ~FMTBaseVertexProperties() = default;
		// DocString: FMTBaseVertexProperties::FMTBaseVertexProperties()
		/**
		@brief Default constructor for FMTBaseVertexProperties.
		*/
		FMTBaseVertexProperties() = default;
		// DocString: FMTBaseVertexProperties::FMTBaseVertexProperties(const FMTBaseVertexProperties&)
		/**
		@brief Copy constructor for FMTBaseVertexProperties.
		@param[in] rhs the FMTBaseVertexProperties to copy.
		*/
		FMTBaseVertexProperties(const FMTBaseVertexProperties& rhs);
		// DocString: FMTBaseVertexProperties::operator=
		/**
		@brief Copy assignment operator for FMTBaseVertexProperties.
		@param[in] rhs the FMTBaseVertexProperties to copy.
		@return a reference to this FMTBaseVertexProperties.
		*/
		FMTBaseVertexProperties& operator = (const FMTBaseVertexProperties& rhs);
		// DocString: FMTBaseVertexProperties::FMTBaseVertexProperties(const Core::FMTFuturDevelopment&)
		/**
		@brief Construct a FMTBaseVertexProperties from a futur development.
		@param[in] p_development the futur development.
		*/
		FMTBaseVertexProperties(const Core::FMTFuturDevelopment& p_development);
		// DocString: FMTBaseVertexProperties::FMTBaseVertexProperties(const Core::FMTActualDevelopment&)
		/**
		@brief Construct a FMTBaseVertexProperties from an actual development.
		@param[in] p_development the actual development.
		*/
		FMTBaseVertexProperties(const Core::FMTActualDevelopment& p_development);
		//FMTBaseVertexProperties(const Core::FMTDevelopment& p_development);
		// DocString: FMTBaseVertexProperties::setDevlopementMask
		/**
		@brief Set the development mask of the vertex.
		@param[in] p_newMask the new mask of the development.
		*/
		void setDevlopementMask(const Core::FMTMask& p_newMask);
		// DocString: FMTBaseVertexProperties::getBaseRhs
		/**
		@brief Get the right hand side of the constraint of the vertex.
		@return the right hand side value.
		*/
		double getBaseRhs() const;
		// DocString: FMTBaseVertexProperties::getConstraintId
		/**
		@brief Get the index of the constraint of the vertex.
		@return the constraint index.
		*/
		virtual int getConstraintId() const;
		// DocString: FMTBaseVertexProperties::get
		/**
		@brief Get a const reference to the development of the vertex.
		@return the development.
		*/
		const Core::FMTDevelopment& get() const;
		// DocString: FMTBaseVertexProperties::hash
		/**
		@brief Hash the development of the vertex.
		@return the hash of the vertex.
		*/
		size_t hash() const;
		// DocString: FMTBaseVertexProperties::operator<
		/**
		@brief Less than operator for FMTBaseVertexProperties.
		@param[in] rhs the FMTBaseVertexProperties to compare to.
		@return true if this vertex properties is less than the other else false.
		*/
		bool operator < (const FMTBaseVertexProperties& rhs) const;
		// DocString: FMTBaseVertexProperties::operator==
		/**
		@brief Comparison operator for FMTBaseVertexProperties.
		@param[in] rhs the FMTBaseVertexProperties to compare to.
		@return true if both vertex properties are equal else false.
		*/
		bool operator == (const FMTBaseVertexProperties& rhs) const;
		// DocString: FMTBaseVertexProperties::operator!=
		/**
		@brief Comparison operator for FMTBaseVertexProperties.
		@param[in] rhs the FMTBaseVertexProperties to compare to.
		@return true if both vertex properties are different else false.
		*/
		bool operator != (const FMTBaseVertexProperties& rhs) const;
	private:
		// DocString: FMTBaseVertexProperties::serialize
		/**
		@brief Serialize the FMTBaseVertexProperties for multiprocessing across multiple cpus (pickle in Python).
		@tparam Archive the archive type.
		@param[in,out] ar the archive to serialize to or from.
		@param[in] version the serialization version.
		*/
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, unsigned int version)
			{
			double area = getBaseRhs();
			ar& BOOST_SERIALIZATION_NVP(*m_development);
			ar& BOOST_SERIALIZATION_NVP(area);
			if (Archive::is_saving::value)
				{
				_save(area);
				}
			}
		// DocString: FMTBaseVertexProperties::m_development
		///The development that hold the information about the strata.
		std::unique_ptr<Core::FMTDevelopment> m_development;
		// DocString: FMTBaseVertexProperties::_save
		/**
		@brief Save the area of the vertex development during serialization.
		@param[in] p_area the area to save.
		*/
		void _save(double p_area);
	};
}


 BOOST_CLASS_EXPORT_KEY(Graph::FMTBaseVertexProperties)


 namespace boost {

	template <>
	struct hash<Graph::FMTBaseVertexProperties>
	{
		std::size_t operator()(const Graph::FMTBaseVertexProperties& vertex_properties) const
		{
			return vertex_properties.hash();
		}
	};


}


#endif
