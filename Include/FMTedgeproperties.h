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
#include "FMTbaseedgeproperties.h"

namespace Graph
{
	class FMTEdgeProperties : public FMTBaseEdgeProperties
	{
		friend class boost::serialization::access;
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
		~FMTEdgeProperties() = default;
		FMTEdgeProperties() = default;
		FMTEdgeProperties(const FMTEdgeProperties& rhs) = default;
		FMTEdgeProperties& operator = (const FMTEdgeProperties& rhs) = default;
		inline int	getvariableID() const override
		{
			return variableID;
		}
		void setvariableID(const int& newvariableID);
		inline  double getProportion() const override
		{
			return static_cast<double>(proportion);
		}
		FMTEdgeProperties(const int& laction,
			const int& lvariableID,
			const double& lproportion);
		bool operator == (const FMTEdgeProperties& rhs) const;
		bool operator != (const FMTEdgeProperties& rhs) const;
		inline std::string variableName() const;
		inline const int* getVariablePtr() const
		{
			return &variableID;
		}

	};
}

BOOST_CLASS_EXPORT_KEY(Graph::FMTEdgeProperties)

#endif