#ifndef FMTvertexproperties_H_INCLUDED
#define FMTvertexproperties_H_INCLUDED

#include "FMTfuturdevelopment.h"
#include "FMTactualdevelopment.h"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/unique_ptr.hpp>

#include <memory>

using namespace Core;
using namespace std;

namespace Graph
{
	class FMTvertexproperties
		{
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
			{
			ar & BOOST_SERIALIZATION_NVP(development);
			ar & BOOST_SERIALIZATION_NVP(constraintID);
			}
			unique_ptr<FMTdevelopment>development;
			int constraintID;
		public:
			~FMTvertexproperties() = default;
			FMTvertexproperties();
			bool operator < (const FMTvertexproperties& rhs) const;
			FMTvertexproperties(const FMTvertexproperties& rhs);
			FMTvertexproperties& operator = (const FMTvertexproperties& rhs);
			FMTvertexproperties(const FMTfuturdevelopment& ldevelopment,
				int& lconstraintID);
			FMTvertexproperties(const FMTactualdevelopment& ldevelopment,
				int& lconstraintID);
            FMTvertexproperties(const FMTdevelopment& ldevelopment,
                int& lconstraintID);//Used for simulated annealing
			string constraintname() const;
			int getconstraintID() const;
			void setconstraintID(const int& ID);
			const FMTdevelopment& get() const;
			double getbaseRHS() const;
			size_t hash() const;
			bool operator == (const FMTvertexproperties& rhs) const;
			bool operator != (const FMTvertexproperties& rhs) const;
			/*boost::dynamic_bitset<>hash(const int& maxperiod, const int& maxage,
				const boost::dynamic_bitset<>& actions,bool periodstart = false) const;*/
		};
}

namespace boost {

	template <>
	struct hash<Graph::FMTvertexproperties>
	{
		std::size_t operator()(const Graph::FMTvertexproperties& vertex_properties) const
		{
			return vertex_properties.hash();
		}
	};


}


#endif
