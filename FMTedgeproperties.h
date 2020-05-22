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

namespace Graph
	{
	class FMTedgeproperties
		{
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
			{
			ar & BOOST_SERIALIZATION_NVP(action);
			ar & BOOST_SERIALIZATION_NVP(variableID);
			ar & BOOST_SERIALIZATION_NVP(proportion);
			}
		int action;
		int variableID;
		double proportion;
		public:
			~FMTedgeproperties()=default;
			FMTedgeproperties();
			FMTedgeproperties(const FMTedgeproperties& rhs);
			FMTedgeproperties& operator = (const FMTedgeproperties& rhs);
			bool isaction(const std::vector<Core::FMTaction>& actions,
				const Core::FMTaction& rhsaction) const;
			inline int	getvariableID() const
				{
				return variableID;
				}
			void setvariableID(const int& newvariableID);
			void setactionID(const int& newid);
			inline int getactionID() const
				{
				return action;
				}
			inline  double getproportion() const
				{
				return proportion;
				}
			FMTedgeproperties(int laction,
				int lvariableID,
				double lproportion);
			bool operator == (const FMTedgeproperties& rhs) const;
			bool operator != (const FMTedgeproperties& rhs) const;
			bool operator < (const FMTedgeproperties& rhs) const;
			inline std::string variablename() const;
			inline const int* getvariableptr() const
				{
				return &variableID;
				}
			inline const int* getactionptr() const
				{
				return &action;
				}
			
		};
	}

#endif