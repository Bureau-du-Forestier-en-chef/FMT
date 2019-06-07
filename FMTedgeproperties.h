#ifndef FMTedgeproperties_H_INCLUDED
#define FMTedgeproperties_H_INCLUDED

#include "FMTaction.h"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>

using namespace Core;

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
			bool isaction(const vector<FMTaction>& actions,
				const FMTaction& rhsaction) const;
			int getvariable() const;
			int getactionID() const;
			double getproportion() const;
			FMTedgeproperties(int laction,
				int lvariableID,
				double lproportion);
			bool operator == (const FMTedgeproperties& rhs) const;
			bool operator != (const FMTedgeproperties& rhs) const;
			bool operator < (const FMTedgeproperties& rhs) const;
			string variablename() const;
			
		};
	}

#endif