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
		@brief destructor of FMTbasevertexpropertie
		*/
		virtual ~FMTBaseVertexProperties() = default;
		// DocString: FMTBaseVertexProperties::FMTBaseVertexProperties()
		/**
		@brief Default constructor
		*/
		FMTBaseVertexProperties() = default;
		// DocString: FMTBaseVertexProperties::FMTBaseVertexProperties(const FMTBaseVertexProperties&)
		/**
		@brief copy constructor
		@param[in] a vertexproperties to copy.
		*/
		FMTBaseVertexProperties(const FMTBaseVertexProperties& rhs);
		// DocString: FMTBaseVertexProperties::FMTBaseVertexProperties(const FMTBaseVertexProperties&)
		/**
		@brief copy constructor of vertexproperties
		@param[in] a vertexproperties to copy.
		@return a reference to the vertexproperties.
		*/
		FMTBaseVertexProperties& operator = (const FMTBaseVertexProperties& rhs);
		// DocString: FMTBaseVertexProperties::FMTBaseVertexProperties(const Core::FMTFuturDevelopment&)
		/**
		@brief constructor based on futur development.
		@param[in] futur development.
		*/
		FMTBaseVertexProperties(const Core::FMTFuturDevelopment& p_development);
		// DocString: FMTBaseVertexProperties::FMTBaseVertexProperties(const Core::FMTActualDevelopment&)
		/**
		@brief constructor based on actual development.
		@param[in] actual development.
		*/
		FMTBaseVertexProperties(const Core::FMTActualDevelopment& p_development);
		//FMTBaseVertexProperties(const Core::FMTDevelopment& p_development);
		// DocString: FMTBaseVertexProperties::setDevlopementMask
		/**
		@brief set the development mask of the vertex.
		@param[in] the mask of the FMTdevlopement.
		*/
		void setDevlopementMask(const Core::FMTMask& p_newMask);
		// DocString: FMTBaseVertexProperties::getBaseRhs
		/**
		@brief get the rhs of the constraint of the vertex.
		@return the double value of the rhs.
		*/
		double getBaseRhs() const;
		// DocString: FMTBaseVertexProperties::getConstraintId
		/**
		@brief get the index of the constraint of the vertex
		@return the index of the vertex
		*/
		virtual int getConstraintId() const;
		// DocString: FMTBaseVertexProperties::get
		/**
		@brief get a const reference to the development
		@return a reference to the development.
		*/
		const Core::FMTDevelopment& get() const;
		// DocString: FMTBaseVertexProperties::hash
		/**
		@brief hash the development of the vertex.
		@return hash of the vertex.
		*/
		size_t hash() const;
		// DocString: FMTBaseVertexProperties::operator<
		/**
		@brief less than operator
		@return true if less than.
		*/
		bool operator < (const FMTBaseVertexProperties& rhs) const;
		// DocString: FMTBaseVertexProperties::operator==
		/**
		@brief equality test operator
		@return true if equal
		*/
		bool operator == (const FMTBaseVertexProperties& rhs) const;
		// DocString: FMTBaseVertexProperties::operator!=
		/**
		@brief non equality test operator
		@return true if non equal
		*/
		bool operator != (const FMTBaseVertexProperties& rhs) const;
	private:
		// DocString: FMTBaseVertexProperties::serialize
		/**
		@brief Serialize the vertex propertie
		@param[out] the archive to serialize in
		@param[in] the version of the archive
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
