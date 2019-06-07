#ifndef FMTCOORD_H_INCLUDED
#define FMTCOORD_H_INCLUDED

#include <memory>
#include <boost/unordered_map.hpp>
#include <vector>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
using namespace boost;
using namespace std;
//using namespace Core;

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
			ar & BOOST_SERIALIZATION_NVP(x_n);
			ar & BOOST_SERIALIZATION_NVP(y_n);
			}
        unsigned int x,y;
        static vector<int>x_n;
        static vector<int>y_n;
    public:
        FMTcoordinate();
        ~FMTcoordinate()=default;
        FMTcoordinate(unsigned int lx, unsigned int ly);
        FMTcoordinate(const FMTcoordinate& rhs);
        FMTcoordinate at(unsigned int id) const;
        double distance(const FMTcoordinate& coord) const;
        bool within(unsigned int ldistance,const FMTcoordinate& coord) const;
        void upenveloppe(vector<FMTcoordinate>& enveloppe) const;
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
    return (hash<unsigned int>()(x) && hash<unsigned int>()(y));
    }
  };

}

#endif // FMTCOORD_H_INCLUDED
