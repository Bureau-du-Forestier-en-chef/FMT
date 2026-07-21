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
    // DocString: FMTCoordinate
    /**
    @brief Spatially referenced coordinate holding an x and y position on a raster grid.
    */
    class FMTEXPORT FMTCoordinate
        {
        public:
            // DocString: FMTCoordinate()
            /**
            @brief Default constructor for FMTCoordinate.
            */
            FMTCoordinate();
            // DocString: ~FMTCoordinate()
            /**
            @brief Default destructor for FMTCoordinate.
            */
            ~FMTCoordinate() = default;
            // DocString: FMTCoordinate(uint16_t, uint16_t)
            /**
            @brief Construct a FMTCoordinate from an x and y position.
            @param[in] p_x the x position.
            @param[in] p_y the y position.
            */
            FMTCoordinate(uint16_t p_x, uint16_t p_y);
            // DocString: FMTCoordinate(const FMTCoordinate&)
            /**
            @brief Copy constructor for FMTCoordinate.
            @param[in] rhs the FMTCoordinate to copy.
            */
            FMTCoordinate(const FMTCoordinate& rhs);
            // DocString: FMTCoordinate::at
            /**
            @brief Return the coordinate at a given index.
            @param[in] id the index.
            @return the coordinate at the index.
            */
            FMTCoordinate at(unsigned int id) const;
            // DocString: FMTCoordinate::distance
            /**
            @brief Return the distance to another coordinate.
            @param[in] coord the other coordinate.
            @return the distance between the two coordinates.
            */
            double distance(const FMTCoordinate& coord) const;
            // DocString: FMTCoordinate::within
            /**
            @brief Return true if another coordinate is within a given distance.
            @tparam T the distance type.
            @param[in] ldistance the distance threshold.
            @param[in] coord the other coordinate.
            @return true if the coordinate is within the distance else false.
            */
            template<typename T>
            bool within(const T& ldistance, const FMTCoordinate& coord) const;
            // DocString: FMTCoordinate::upEnveloppe
            /**
            @brief Update the bounding envelope with this coordinate.
            @param[in,out] enveloppe the envelope to update.
            */
            void upEnveloppe(std::array<FMTCoordinate, 4>& enveloppe) const;
            // DocString: FMTCoordinate::hash
            /**
            @brief Return the hash of the coordinate.
            @return the hash value.
            */
            size_t hash() const;
            // DocString: FMTCoordinate::operator=
            /**
            @brief Copy assignment operator for FMTCoordinate.
            @param[in] rhs the FMTCoordinate to copy.
            @return a reference to this FMTCoordinate.
            */
            FMTCoordinate& operator = (const FMTCoordinate& rhs);
            // DocString: FMTCoordinate::getSpatialCoordinate
            /**
            @brief Return the spatial coordinate using a geographic transform.
            @param[in] geoTransform the geographic transform.
            @return the spatial coordinate.
            */
            const std::vector<double> getSpatialCoordinate(std::vector<double> geoTransform) const;
            // DocString: FMTCoordinate::operator==
            /**
            @brief Equality comparison operator of FMTCoordinate.
            @param[in] rhs the FMTCoordinate to compare with.
            @return true if both coordinates are equal else false.
            */
            bool operator == (const FMTCoordinate& rhs) const;
            // DocString: FMTCoordinate::operator<
            /**
            @brief Less than comparison operator of FMTCoordinate.
            @param[in] rhs the FMTCoordinate to compare with.
            @return true if this coordinate is less than rhs else false.
            */
            bool operator < (const FMTCoordinate& rhs) const;
            // DocString: FMTCoordinate::closest
            /**
            @brief Return the closest coordinate among the given coordinates.
            @param[in] coordinates the coordinates to search.
            @param[out] approximation the approximated distance to the closest coordinate.
            @return an iterator to the closest coordinate.
            */
            std::set<FMTCoordinate>::const_iterator closest(const std::vector<std::set<FMTCoordinate>::const_iterator>& coordinates, double& approximation) const;
            // DocString: FMTCoordinate::operator std::string
            /**
            @brief Convert the coordinate to its string representation.
            @return the string representation of the coordinate.
            */
            operator std::string() const;
            // DocString: FMTCoordinate::getNeighbors
            /**
            @brief Return the neighboring coordinates within a given size.
            @param[in] nsize the neighborhood size.
            @param[in] circle if true uses a circular neighborhood.
            @return the set of neighboring coordinates.
            */
            std::set<FMTCoordinate> getNeighbors(const uint16_t& nsize, const bool& circle = false) const;
            // DocString: FMTCoordinate::getAverageCentroid
            /**
            @brief Return the average centroid of an envelope.
            @param[in] p_enveloppe the envelope.
            @return the average centroid coordinate.
            */
            static FMTCoordinate getAverageCentroid(const std::array<FMTCoordinate, 4>& p_enveloppe);
            // DocString: FMTCoordinate::getTerritory
            /**
            @brief Return the coordinates of a territory within a distance of an envelope.
            @param[in] p_enveloppe the envelope.
            @param[in] p_distance the distance.
            @return the set of coordinates of the territory.
            */
            static std::set<FMTCoordinate> getTerritory(const std::array<FMTCoordinate, 4>& p_enveloppe, const size_t& p_distance);
            // DocString: FMTCoordinate::getHeight
            /**
            @brief Return the height of an envelope.
            @param[in] p_enveloppe the envelope.
            @return the height of the envelope.
            */
            static size_t getHeight(const std::array<FMTCoordinate, 4>& p_enveloppe);
            // DocString: FMTCoordinate::getWidth
            /**
            @brief Return the width of an envelope.
            @param[in] p_enveloppe the envelope.
            @return the width of the envelope.
            */
            static size_t getWidth(const std::array<FMTCoordinate, 4>& p_enveloppe);
            // DocString: FMTCoordinate::getX
            /**
            @brief Return the x position of the coordinate.
            @return the x position.
            */
            uint16_t getX() const;
            // DocString: FMTCoordinate::getY
            /**
            @brief Return the y position of the coordinate.
            @return the y position.
            */
            uint16_t getY() const;
        private:
		    friend class boost::serialization::access;
            //unsigned int x,y;
            uint16_t m_x;
            uint16_t m_y;
		    // DocString: FMTCoordinate::serialize
		    /**
		    @brief Serialize the FMTCoordinate for multiprocessing across multiple cpus (pickle in Python).
		    @tparam Archive the archive type.
		    @param[in,out] ar the archive to serialize to or from.
		    @param[in] version the serialization version.
		    */
		    template<class Archive>
		    void serialize(Archive& ar, const unsigned int version)
			    {
			    ar & BOOST_SERIALIZATION_NVP(m_x);
			    ar & BOOST_SERIALIZATION_NVP(m_y);
			    }
		    // DocString: FMTCoordinate::_getXyGap
		    /**
		    @brief Compute the x and y gap to another coordinate.
		    @param[in] rhs the other coordinate.
		    @param[out] xgap the x gap.
		    @param[out] y_gap the y gap.
		    */
		    void _getXyGap(const FMTCoordinate& rhs, int& xgap, int& y_gap) const;
            // DocString: FMTCoordinate::_distanceApproximation
            /**
            @brief Return an approximation of the distance to another coordinate.
            @param[in] coord the other coordinate.
            @return the approximated distance.
            */
            double _distanceApproximation(const FMTCoordinate& coord) const;
 
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
