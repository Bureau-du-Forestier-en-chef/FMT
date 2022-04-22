/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
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
#include <string>
#include <set>
#include "FMTexception.hpp"

namespace Spatial
    {
    class FMTEXPORT FMTcoordinate
        {
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
			{
			ar & BOOST_SERIALIZATION_NVP(x);
			ar & BOOST_SERIALIZATION_NVP(y);
			}
        unsigned int x,y;
		void getxygap(const FMTcoordinate& rhs, int& xgap, int& y_gap) const;
        double distanceapproximation(const FMTcoordinate& coord) const;
    public:
        FMTcoordinate();
        ~FMTcoordinate()=default;
        FMTcoordinate(unsigned int lx, unsigned int ly);
        FMTcoordinate(const FMTcoordinate& rhs);
        FMTcoordinate at(unsigned int id) const;
        double distance(const FMTcoordinate& coord) const;
        template<typename T>
        bool within(const T& ldistance,const FMTcoordinate& coord) const;
        void upenveloppe(std::vector<FMTcoordinate>& enveloppe) const;
		size_t hash() const;
        FMTcoordinate& operator = (const FMTcoordinate& rhs);
        unsigned int getx() const;
        unsigned int gety() const;
        const std::vector<double> getSpatialCoordinate(std::vector<double> geoTransform) const;
        bool operator == (const FMTcoordinate& rhs) const;
        bool operator < (const FMTcoordinate& rhs) const;
		std::set<FMTcoordinate>::const_iterator closest(const std::vector<std::set<FMTcoordinate>::const_iterator>& coordinates, double& approximation) const;
        operator std::string() const;
        std::set<FMTcoordinate> getneighbors(const unsigned int& nsize,const bool& circle=false) const;
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
