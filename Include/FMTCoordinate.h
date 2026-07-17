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
#include "FMTException.h"

namespace Spatial
    {
    class FMTEXPORT FMTCoordinate
        {
        public:
            FMTCoordinate();
            ~FMTCoordinate() = default;
            FMTCoordinate(uint16_t p_x, uint16_t p_y);
            FMTCoordinate(const FMTCoordinate& rhs);
            FMTCoordinate at(unsigned int id) const;
            double distance(const FMTCoordinate& coord) const;
            template<typename T>
            bool within(const T& ldistance, const FMTCoordinate& coord) const;
            void upEnveloppe(std::array<FMTCoordinate, 4>& enveloppe) const;
            size_t hash() const;
            FMTCoordinate& operator = (const FMTCoordinate& rhs);
            const std::vector<double> getSpatialCoordinate(std::vector<double> geoTransform) const;
            bool operator == (const FMTCoordinate& rhs) const;
            bool operator < (const FMTCoordinate& rhs) const;
            std::set<FMTCoordinate>::const_iterator closest(const std::vector<std::set<FMTCoordinate>::const_iterator>& coordinates, double& approximation) const;
            operator std::string() const;
            std::set<FMTCoordinate> getNeighbors(const uint16_t& nsize, const bool& circle = false) const;
            static FMTCoordinate getAverageCentroid(const std::array<FMTCoordinate, 4>& p_enveloppe);
            static std::set<FMTCoordinate> getTerritory(const std::array<FMTCoordinate, 4>& p_enveloppe, const size_t& p_distance);
            static size_t getHeight(const std::array<FMTCoordinate, 4>& p_enveloppe);
            static size_t getWidth(const std::array<FMTCoordinate, 4>& p_enveloppe);
            uint16_t getX() const;
            uint16_t getY() const;
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
		    void getXyGap(const FMTCoordinate& rhs, int& xgap, int& y_gap) const;
            double distanceApproximation(const FMTCoordinate& coord) const;
 
        };
    }

namespace boost {

  template <>
  struct hash<Spatial::FMTCoordinate>
  {
    std::size_t operator()(const Spatial::FMTCoordinate& p_coord) const
        {
        //unsigned int x = coord.getX();
        //unsigned int y = coord.getY();
        //return (boost::hash<unsigned int>()(x) && hash<unsigned int>()(y));
        return p_coord.hash();
        }
  };

}

#endif // FMTCOORD_Hm_included
