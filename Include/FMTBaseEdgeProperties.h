/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTbaseedgeproperties_Hm_included
#define FMTbaseedgeproperties_Hm_included


#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>
#include <vector>
#include <string>
#include <cstdint>

namespace Core
{
	class FMTAction;
}

namespace Graph
{
	// DocString: FMTBaseEdgeProperties
	/**
	@brief Base edge properties of a FMTGraph, holding the action of the edge.
	*/
	class FMTBaseEdgeProperties
	{
	public:
		// DocString: ~FMTBaseEdgeProperties()
		/**
		@brief Default virtual destructor for FMTBaseEdgeProperties.
		*/
		virtual ~FMTBaseEdgeProperties() = default;
		// DocString: FMTBaseEdgeProperties()
		/**
		@brief Default constructor for FMTBaseEdgeProperties.
		*/
		FMTBaseEdgeProperties()=default;
		// DocString: FMTBaseEdgeProperties(const FMTBaseEdgeProperties&)
		/**
		@brief Copy constructor for FMTBaseEdgeProperties.
		@param[in] rhs the FMTBaseEdgeProperties to copy.
		*/
		FMTBaseEdgeProperties(const FMTBaseEdgeProperties& rhs)=default;
		// DocString: FMTBaseEdgeProperties::operator=
		/**
		@brief Copy assignment operator for FMTBaseEdgeProperties.
		@param[in] rhs the FMTBaseEdgeProperties to copy.
		@return a reference to this FMTBaseEdgeProperties.
		*/
		FMTBaseEdgeProperties& operator = (const FMTBaseEdgeProperties& rhs)=default;
		// DocString: FMTBaseEdgeProperties(const int&,const int&,const double&)
		/**
		@brief Construct a FMTBaseEdgeProperties from an action, a variable id and a proportion.
		@param[in] laction the action.
		@param[in] lvariableID the variable id.
		@param[in] lproportion the proportion.
		*/
		FMTBaseEdgeProperties(const int& laction,
						const int& lvariableID,
						const double& lproportion);
		// DocString: FMTBaseEdgeProperties(const int&)
		/**
		@brief Construct a FMTBaseEdgeProperties from an action.
		@param[in] laction the action.
		*/
		constexpr FMTBaseEdgeProperties(const int& laction) : action(static_cast<int8_t>(laction)) {}
		// DocString: FMTBaseEdgeProperties::getVariableID
		/**
		@brief Return the variable id of the edge.
		@return the variable id.
		*/
		virtual inline int	getVariableID() const
			{
			return 0;
			}
		// DocString: FMTBaseEdgeProperties::getProportion
		/**
		@brief Return the proportion of the edge.
		@return the proportion.
		*/
		virtual inline double getProportion() const
			{
			return 100;
			}
		// DocString: FMTBaseEdgeProperties::getActionID
		/**
		@brief Return the action id of the edge.
		@return the action id.
		*/
		inline int getActionID() const
			{
			return static_cast<int>(action);
			}
		// DocString: FMTBaseEdgeProperties::getShortActionID
		/**
		@brief Return the short (int8) action id of the edge.
		@return the short action id.
		*/
		inline int8_t getShortActionID() const
			{
			return action;
			}
		// DocString: FMTBaseEdgeProperties::setActionID
		/**
		@brief Set the action id of the edge.
		@param[in] newid the new action id.
		*/
		void setActionID(const int& newid);
		// DocString: FMTBaseEdgeProperties::isAction
		/**
		@brief Return true if the edge action matches a given action.
		@param[in] actions the model actions.
		@param[in] rhsaction the action to test.
		@return true if the edge is the given action else false.
		*/
		bool isAction(const std::vector<Core::FMTAction>& actions,
			const Core::FMTAction& rhsaction) const;
		// DocString: FMTBaseEdgeProperties::operator==
		/**
		@brief Comparison operator for FMTBaseEdgeProperties.
		@param[in] rhs the FMTBaseEdgeProperties to compare to.
		@return true if both edge properties are equal else false.
		*/
		/*virtual*/ bool operator == (const FMTBaseEdgeProperties & rhs) const;
		// DocString: FMTBaseEdgeProperties::operator!=
		/**
		@brief Comparison operator for FMTBaseEdgeProperties.
		@param[in] rhs the FMTBaseEdgeProperties to compare to.
		@return true if both edge properties are different else false.
		*/
		/*virtual*/ bool operator != (const FMTBaseEdgeProperties& rhs) const;
		// DocString: FMTBaseEdgeProperties::operator<
		/**
		@brief Less than operator for FMTBaseEdgeProperties.
		@param[in] rhs the FMTBaseEdgeProperties to compare to.
		@return true if this edge properties is less than the other else false.
		*/
		bool operator < (const FMTBaseEdgeProperties& rhs) const;
		/*inline const short int* getactionptr() const
			{
			return &action;
			}*/

	protected:
		int8_t action;
	private:
		friend class boost::serialization::access;
		// DocString: FMTBaseEdgeProperties::serialize
		/**
		@brief Serialize the FMTBaseEdgeProperties for multiprocessing across multiple cpus (pickle in Python).
		@tparam Archive the archive type.
		@param[in,out] ar the archive to serialize to or from.
		@param[in] version the serialization version.
		*/
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar & BOOST_SERIALIZATION_NVP(action);
		}
	};
}

BOOST_CLASS_EXPORT_KEY(Graph::FMTBaseEdgeProperties)

#endif
