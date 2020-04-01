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

#ifndef FMTCOORD_H_INCLUDED
#define FMTCOORD_H_INCLUDED

#include <memory>
#include <boost/unordered_map.hpp>
#include <vector>
#include <array>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/array.hpp>


namespace Spatial
    {
	

    class FMTcoordinate
        {
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
			{
			ar & BOOST_SERIALIZATION_NVP(x);
			ar & BOOST_SERIALIZATION_NVP(y);
			//ar & BOOST_SERIALIZATION_NVP(x_n);
			//ar & BOOST_SERIALIZATION_NVP(y_n);
			}
        unsigned int x,y;
		//static constexpr std::array<int,8>x_n = { 0,1,0,-1,1,1,-1,-1 };
		//static constexpr std::array<int,8>y_n = { 1,0,-1,0,1,-1,-1,1 };
		//int x_n[8] = { 0,1,0,-1,1,1,-1,-1 };
		//int y_n[8] = { 1,0,-1,0,1,-1,-1,1 };
    public:
        FMTcoordinate();
        ~FMTcoordinate()=default;
        FMTcoordinate(unsigned int lx, unsigned int ly);
        FMTcoordinate(const FMTcoordinate& rhs);
        FMTcoordinate at(unsigned int id) const;
        double distance(const FMTcoordinate& coord) const;
        bool within(unsigned int ldistance,const FMTcoordinate& coord) const;
        void upenveloppe(std::vector<FMTcoordinate>& enveloppe) const;
        FMTcoordinate& operator = (const FMTcoordinate& rhs);
        unsigned int getx() const;
        unsigned int gety() const;
        bool operator == (const FMTcoordinate& rhs) const;
        bool operator < (const FMTcoordinate& rhs) const;
        };
    }

namespace boost {

  template <>
  struct hash<Spatial::FMTcoordinate>
  {
    std::size_t operator()(const Spatial::FMTcoordinate& coord) const
    {
    unsigned int x = coord.getx();
    unsigned int y = coord.gety();
    return (boost::hash<unsigned int>()(x) && hash<unsigned int>()(y));
    }
  };

}

#endif // FMTCOORD_H_INCLUDED
