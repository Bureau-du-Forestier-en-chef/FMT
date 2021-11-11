/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTbaseedgeproperties_H_INCLUDED
#define FMTbaseedgeproperties_H_INCLUDED


#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>
#include <vector>
#include <string>

namespace Core
{
	class FMTaction;
}

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
		int8_t action;
	public:
		virtual ~FMTbaseedgeproperties() = default;
		FMTbaseedgeproperties()=default;
		FMTbaseedgeproperties(const FMTbaseedgeproperties& rhs)=default;
		FMTbaseedgeproperties& operator = (const FMTbaseedgeproperties& rhs)=default;
		FMTbaseedgeproperties(const int& laction,
						const int& lvariableID,
						const double& lproportion);
		constexpr FMTbaseedgeproperties(const int& laction) : action(static_cast<int8_t>(laction)) {}
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
			return static_cast<int>(action);
			}
		void setactionID(const int& newid);
		bool isaction(const std::vector<Core::FMTaction>& actions,
			const Core::FMTaction& rhsaction) const;
		virtual bool operator == (const FMTbaseedgeproperties& rhs) const;
		virtual bool operator != (const FMTbaseedgeproperties& rhs) const;
		bool operator < (const FMTbaseedgeproperties& rhs) const;
		/*inline const short int* getactionptr() const
			{
			return &action;
			}*/

	};
}

BOOST_CLASS_EXPORT_KEY(Graph::FMTbaseedgeproperties)

#endif
