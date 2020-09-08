/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTbaseedgeproperties_H_INCLUDED
#define FMTbaseedgeproperties_H_INCLUDED

#include "FMTaction.h"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <vector>
#include <string>

namespace Graph
{
	class FMTbaseedgeproperties
	{
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar & BOOST_SERIALIZATION_NVP(action);
		}
	protected:
		int action;
	public:
		virtual ~FMTbaseedgeproperties() = default;
		FMTbaseedgeproperties()=default;
		FMTbaseedgeproperties(const FMTbaseedgeproperties& rhs)=default;
		FMTbaseedgeproperties& operator = (const FMTbaseedgeproperties& rhs)=default;
		FMTbaseedgeproperties(const int& laction,
						const int& lvariableID,
						const double& lproportion);
		virtual inline int	getvariableID() const
			{
			return 0;
			}
		virtual inline double getproportion() const
			{
			return 100;
			}
		inline int getactionID() const
			{
			return action;
			}
		void setactionID(const int& newid);
		bool isaction(const std::vector<Core::FMTaction>& actions,
			const Core::FMTaction& rhsaction) const;
		FMTbaseedgeproperties(const int& laction);
		virtual bool operator == (const FMTbaseedgeproperties& rhs) const;
		virtual bool operator != (const FMTbaseedgeproperties& rhs) const;
		bool operator < (const FMTbaseedgeproperties& rhs) const;
		inline const int* getactionptr() const
			{
			return &action;
			}

	};
}

#endif
