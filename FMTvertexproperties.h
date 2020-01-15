/*
MIT License

Copyright (c) [2019] [Bureau du forestier en chef]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

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
			inline int getconstraintID() const
				{
				return constraintID;
				}
			void setconstraintID(const int& ID);
			inline const FMTdevelopment& get() const
				{
				return (*development);
				}
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
