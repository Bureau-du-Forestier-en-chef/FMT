/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTbasevertexproperties_H_INCLUDED
#define FMTbasevertexproperties_H_INCLUDED


#include "FMTactualdevelopment.h"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/unique_ptr.hpp>
#include <boost/serialization/split_member.hpp>
#include <memory>



namespace Graph
{
	class FMTEXPORT FMTbasevertexproperties
	{
	public:
		// DocString: FMTbasevertexproperties::~FMTbasevertexproperties
		/**
		@brief destructor of FMTbasevertexpropertie
		*/
		virtual ~FMTbasevertexproperties() = default;
		// DocString: FMTbasevertexproperties::FMTbasevertexproperties()
		/**
		@brief Default constructor
		*/
		FMTbasevertexproperties()=default;
		// DocString: FMTbasevertexproperties::FMTbasevertexproperties(const FMTbasevertexproperties&)
		/**
		@brief copy constructor
		@param[in] a vertexproperties to copy.
		*/
		FMTbasevertexproperties(const FMTbasevertexproperties& rhs)=default;
		// DocString: FMTbasevertexproperties::FMTbasevertexproperties(const FMTbasevertexproperties&)
		/**
		@brief copy constructor of vertexproperties
		@param[in] a vertexproperties to copy.
		@return a reference to the vertexproperties.
		*/
		FMTbasevertexproperties& operator = (const FMTbasevertexproperties& rhs) = default;
		// DocString: FMTbasevertexproperties::FMTbasevertexproperties(const Core::FMTfuturdevelopment&)
		/**
		@brief constructor based on futur development.
		@param[in] futur developement.
		*/
		FMTbasevertexproperties(const Core::FMTfuturdevelopment& p_development);
		// DocString: FMTbasevertexproperties::FMTbasevertexproperties(const Core::FMTactualdevelopment&)
		/**
		@brief constructor based on actual development.
		@param[in] actual developement.
		*/
		FMTbasevertexproperties(const Core::FMTactualdevelopment& p_development);
		//FMTbasevertexproperties(const Core::FMTdevelopment& p_development);
		// DocString: FMTbasevertexproperties::setDevlopementMask
		/**
		@brief set the development mask of the vertex.
		@param[in] the mask of the FMTdevlopement.
		*/
		void setDevlopementMask(const Core::FMTmask& p_newMask);
		// DocString: FMTbasevertexproperties::getBaseRhs
		/**
		@brief get the rhs of the constraint of the vertex.
		@return the double value of the rhs.
		*/
		double getBaseRhs() const;
		// DocString: FMTbasevertexproperties::getConstraintId
		/**
		@brief get the index of the constraint of the vertex
		@return the index of the vertex
		*/
		virtual int getConstraintId() const;
		// DocString: FMTbasevertexproperties::get
		/**
		@brief get a const reference to the development
		@return a reference to the development.
		*/
		const Core::FMTdevelopment& get() const;
		// DocString: FMTbasevertexproperties::hash
		/**
		@brief hash the development of the vertex.
		@return hash of the vertex.
		*/
		size_t hash() const;
		// DocString: FMTbasevertexproperties::operator<
		/**
		@brief less than operator
		@return true if less than.
		*/
		bool operator < (const FMTbasevertexproperties& rhs) const;
		// DocString: FMTbasevertexproperties::operator==
		/**
		@brief equality test operator
		@return true if equal
		*/
		bool operator == (const FMTbasevertexproperties& rhs) const;
		// DocString: FMTbasevertexproperties::operator!=
		/**
		@brief non equality test operator
		@return true if non equal
		*/
		bool operator != (const FMTbasevertexproperties& rhs) const;
	private:
		// DocString: FMTbasevertexproperties::serialize
		/**
		@brief Serialize the vertex propertie
		@param[out] the archive to serialize in
		@param[in] the version of the archive
		*/
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
			{
			ar& BOOST_SERIALIZATION_NVP(m_development);
			}
		// DocString: FMTbasevertexproperties::m_development
		///The developement that hold the information about the strata.
		Core::FMTactualdevelopment m_development;
	};
}


BOOST_CLASS_EXPORT_KEY(Graph::FMTbasevertexproperties)


namespace boost {

	template <>
	struct hash<Graph::FMTbasevertexproperties>
	{
		std::size_t operator()(const Graph::FMTbasevertexproperties& vertex_properties) const
		{
			return vertex_properties.hash();
		}
	};


}


#endif
