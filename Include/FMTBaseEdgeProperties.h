/*
Copyright (c) 2019 Gouvernement du Québec

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
	class FMTBaseEdgeProperties
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
		virtual ~FMTBaseEdgeProperties() = default;
		FMTBaseEdgeProperties()=default;
		FMTBaseEdgeProperties(const FMTBaseEdgeProperties& rhs)=default;
		FMTBaseEdgeProperties& operator = (const FMTBaseEdgeProperties& rhs)=default;
		FMTBaseEdgeProperties(const int& laction,
						const int& lvariableID,
						const double& lproportion);
		constexpr FMTBaseEdgeProperties(const int& laction) : action(static_cast<int8_t>(laction)) {}
		virtual inline int	getvariableID() const
			{
			return 0;
			}
		virtual inline double getProportion() const
			{
			return 100;
			}
		inline int getactionID() const
			{
			return static_cast<int>(action);
			}
		inline int8_t getShortActionID() const
			{
			return action;
			}
		void setactionID(const int& newid);
		bool isAction(const std::vector<Core::FMTAction>& actions,
			const Core::FMTAction& rhsaction) const;
		/*virtual*/ bool operator == (const FMTBaseEdgeProperties & rhs) const;
		/*virtual*/ bool operator != (const FMTBaseEdgeProperties& rhs) const;
		bool operator < (const FMTBaseEdgeProperties& rhs) const;
		/*inline const short int* getactionptr() const
			{
			return &action;
			}*/

	};
}

BOOST_CLASS_EXPORT_KEY(Graph::FMTBaseEdgeProperties)

#endif
