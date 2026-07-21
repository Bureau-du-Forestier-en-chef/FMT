/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTedgeproperties_Hm_included
#define FMTedgeproperties_Hm_included

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <vector>
#include <string>
#include "FMTBaseEdgeProperties.h"

namespace Graph
{
	// DocString: FMTEdgeProperties
	/**
	@brief Edge properties of a FMTGraph, extending the base edge properties with a variable id and a proportion.
	*/
	class FMTEdgeProperties : public FMTBaseEdgeProperties
	{
		friend class boost::serialization::access;
		// DocString: FMTEdgeProperties::serialize
		/**
		@brief Serialize the FMTEdgeProperties for multiprocessing across multiple cpus (pickle in Python).
		@tparam Archive the archive type.
		@param[in,out] ar the archive to serialize to or from.
		@param[in] version the serialization version.
		*/
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& boost::serialization::make_nvp("FMTbaseedgeproperties", boost::serialization::base_object<FMTBaseEdgeProperties>(*this));
			ar& BOOST_SERIALIZATION_NVP(variableID);
			ar& BOOST_SERIALIZATION_NVP(proportion);
		}
		int variableID;
		float proportion;
	public:
		// DocString: ~FMTEdgeProperties()
		/**
		@brief Default destructor for FMTEdgeProperties.
		*/
		~FMTEdgeProperties() = default;
		// DocString: FMTEdgeProperties()
		/**
		@brief Default constructor for FMTEdgeProperties.
		*/
		FMTEdgeProperties() = default;
		// DocString: FMTEdgeProperties(const FMTEdgeProperties&)
		/**
		@brief Copy constructor for FMTEdgeProperties.
		@param[in] rhs the FMTEdgeProperties to copy.
		*/
		FMTEdgeProperties(const FMTEdgeProperties& rhs) = default;
		// DocString: FMTEdgeProperties::operator=
		/**
		@brief Copy assignment operator for FMTEdgeProperties.
		@param[in] rhs the FMTEdgeProperties to copy.
		@return a reference to this FMTEdgeProperties.
		*/
		FMTEdgeProperties& operator = (const FMTEdgeProperties& rhs) = default;
		// DocString: FMTEdgeProperties::getvariableID
		/**
		@brief Return the variable id of the edge.
		@return the variable id.
		*/
		inline int	getvariableID() const override
		{
			return variableID;
		}
		// DocString: FMTEdgeProperties::setvariableID
		/**
		@brief Set the variable id of the edge.
		@param[in] newvariableID the new variable id.
		*/
		void setvariableID(const int& newvariableID);
		// DocString: FMTEdgeProperties::getProportion
		/**
		@brief Return the proportion of the edge.
		@return the proportion.
		*/
		inline  double getProportion() const override
		{
			return static_cast<double>(proportion);
		}
		// DocString: FMTEdgeProperties(const int&,const int&,const double&)
		/**
		@brief Construct a FMTEdgeProperties from an action, a variable id and a proportion.
		@param[in] laction the action.
		@param[in] lvariableID the variable id.
		@param[in] lproportion the proportion.
		*/
		FMTEdgeProperties(const int& laction,
			const int& lvariableID,
			const double& lproportion);
		// DocString: FMTEdgeProperties::operator==
		/**
		@brief Comparison operator for FMTEdgeProperties.
		@param[in] rhs the FMTEdgeProperties to compare to.
		@return true if both edge properties are equal else false.
		*/
		bool operator == (const FMTEdgeProperties& rhs) const;
		// DocString: FMTEdgeProperties::operator!=
		/**
		@brief Comparison operator for FMTEdgeProperties.
		@param[in] rhs the FMTEdgeProperties to compare to.
		@return true if both edge properties are different else false.
		*/
		bool operator != (const FMTEdgeProperties& rhs) const;
		// DocString: FMTEdgeProperties::variableName
		/**
		@brief Return the name of the variable of the edge.
		@return the variable name.
		*/
		inline std::string variableName() const;
		// DocString: FMTEdgeProperties::getVariablePtr
		/**
		@brief Return a pointer to the variable id of the edge.
		@return a pointer to the variable id.
		*/
		inline const int* getVariablePtr() const
		{
			return &variableID;
		}

	};
}

BOOST_CLASS_EXPORT_KEY(Graph::FMTEdgeProperties)

#endif