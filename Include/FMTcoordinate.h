/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTCOORD_Hm_included
#define FMTCOORD_Hm_included

#include <memory>
#include <boost/unordered_map.hpp>
#include <vector>
#include <array>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/array.hpp>
#include <string>
#include <set>
#include "FMTexception.h"

namespace Spatial
    {
    class FMTEXPORT FMTcoordinate
        {
        public:
            FMTcoordinate();
            ~FMTcoordinate() = default;
            FMTcoordinate(uint16_t p_x, uint16_t p_y);
            FMTcoordinate(const FMTcoordinate& rhs);
            FMTcoordinate at(unsigned int id) const;
            double distance(const FMTcoordinate& coord) const;
            template<typename T>
            bool within(const T& ldistance, const FMTcoordinate& coord) const;
            void upenveloppe(std::array<FMTcoordinate, 4>& enveloppe) const;
            size_t hash() const;
            FMTcoordinate& operator = (const FMTcoordinate& rhs);
            const std::vector<double> getSpatialCoordinate(std::vector<double> geoTransform) const;
            bool operator == (const FMTcoordinate& rhs) const;
            bool operator < (const FMTcoordinate& rhs) const;
            std::set<FMTcoordinate>::const_iterator closest(const std::vector<std::set<FMTcoordinate>::const_iterator>& coordinates, double& approximation) const;
            operator std::string() const;
            std::set<FMTcoordinate> getneighbors(const uint16_t& nsize, const bool& circle = false) const;
            static FMTcoordinate getAverageCentroid(const std::array<FMTcoordinate, 4>& p_enveloppe);
            static std::set<FMTcoordinate> getTerritory(const std::array<FMTcoordinate, 4>& p_enveloppe, const size_t& p_distance);
            static size_t getHeight(const std::array<FMTcoordinate, 4>& p_enveloppe);
            static size_t getWidth(const std::array<FMTcoordinate, 4>& p_enveloppe);
            uint16_t getx() const;
            uint16_t gety() const;
        private:
		    friend class boost::serialization::access;
		    template<class Archive>
		    void serialize(Archive& ar, const unsigned int version)
			    {
			    ar & BOOST_SERIALIZATION_NVP(m_x);
			    ar & BOOST_SERIALIZATION_NVP(m_y);
			    }
            //unsigned int x,y;
            uint16_t m_x;
            uint16_t m_y;
		    void getxygap(const FMTcoordinate& rhs, int& xgap, int& y_gap) const;
            double distanceapproximation(const FMTcoordinate& coord) const;
 
        };
    }

namespace boost {

  template <>
  struct hash<Spatial::FMTcoordinate>
  {
    std::size_t operator()(const Spatial::FMTcoordinate& p_coord) const
        {
        //unsigned int x = coord.getx();
        //unsigned int y = coord.gety();
        //return (boost::hash<unsigned int>()(x) && hash<unsigned int>()(y));
        return p_coord.hash();
        }
  };

}

#endif // FMTCOORD_Hm_included
