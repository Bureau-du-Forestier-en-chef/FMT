/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTedgeproperties_H_INCLUDED
#define FMTedgeproperties_H_INCLUDED

#include "FMTaction.h"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <vector>
#include <string>
#include "FMTbaseedgeproperties.h"

namespace Graph
	{
	class FMTedgeproperties : public FMTbaseedgeproperties
		{
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
			{
			ar & boost::serialization::make_nvp("FMTbaseedgeproperties", boost::serialization::base_object<FMTbaseedgeproperties>(*this));
			ar & BOOST_SERIALIZATION_NVP(variableID);
			ar & BOOST_SERIALIZATION_NVP(proportion);
			}
		int variableID;
		double proportion;
		public:
			~FMTedgeproperties()=default;
			FMTedgeproperties()=default;
			FMTedgeproperties(const FMTedgeproperties& rhs)=default;
			FMTedgeproperties& operator = (const FMTedgeproperties& rhs)=default;
			inline int	getvariableID() const override
				{
				return variableID;
				}
			void setvariableID(const int& newvariableID);
			inline  double getproportion() const override
				{
				return proportion;
				}
			FMTedgeproperties(const int& laction,
							const int& lvariableID,
							const double& lproportion);
			bool operator == (const FMTedgeproperties& rhs) const;
			bool operator != (const FMTedgeproperties& rhs) const;
			inline std::string variablename() const;
			inline const int* getvariableptr() const
				{
				return &variableID;
				}
			
		};
	}

#endif