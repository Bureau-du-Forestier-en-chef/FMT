/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTCoordinate.h"
#include <cstdlib>
#include <iostream>

namespace Spatial
    {
    //Implementation for Rcpp .. Normally should be static member
	constexpr std::array<int,8> x_n = { 0,1,0,-1,1,1,-1,-1 };
	constexpr std::array<int,8> y_n = { -1,0,1,0,-1,1,1,-1 };
    FMTCoordinate::FMTCoordinate():m_x(), m_y(){}
    FMTCoordinate::FMTCoordinate(uint16_t p_x, uint16_t p_y):
        m_x(p_x),m_y(p_y){}
    FMTCoordinate::FMTCoordinate(const FMTCoordinate& rhs):m_x(rhs.m_x), m_y(rhs.m_y){}

    FMTCoordinate FMTCoordinate::at(unsigned int id) const
        {
        //7//0//4//
        //3// //1//
        //6//2//5//
        /// Factor is a floor
        const int factor = ((id / 8) + 1);
        id = (id - (factor-1) * 8);
        return FMTCoordinate(m_x+(x_n[id]*factor), m_y+(y_n[id]*factor));
        }

	void FMTCoordinate::_getXyGap(const FMTCoordinate& rhs, int& xgap, int& y_gap) const
		{
		xgap = (static_cast<int>(m_x) - static_cast<int>(rhs.m_x));
		y_gap = (static_cast<int>(m_y) - static_cast<int>(rhs.m_y));
		}

    double FMTCoordinate::distance(const FMTCoordinate& coord) const
        {
        return std::sqrt(_distanceApproximation(coord));
        }

	double FMTCoordinate::_distanceApproximation(const FMTCoordinate& coord) const
		{
		int distancex = 0;
		int distancey = 0;
		_getXyGap(coord, distancex, distancey);
		return static_cast<double>(distancex * distancex + distancey * distancey);
		}

	std::set<FMTCoordinate>::const_iterator FMTCoordinate::closest(const std::vector<std::set<FMTCoordinate>::const_iterator>& coordinates, double& approximation) const
		{
		approximation = std::numeric_limits<double>::infinity();
		std::set<FMTCoordinate>::const_iterator bestcoordinate;
		for (const std::set<FMTCoordinate>::const_iterator& coordinate : coordinates)
			{
			const double value = _distanceApproximation(*coordinate);
			if (value < approximation)
				{
				approximation = value;
				bestcoordinate = coordinate;
				}
			}
		return bestcoordinate;
		}

    template<class T>
    bool FMTCoordinate::within(const T& ldistance,const FMTCoordinate& coord) const
        {
            if(ldistance<0)
            {
                //raise
            }
            int distancex = 0;
            int distancey = 0;
            _getXyGap(coord, distancex, distancey);
            return (static_cast<decltype(ldistance)>(std::abs(distancex)) <= ldistance && static_cast<decltype(ldistance)>(std::abs(distancey)) <= ldistance &&
                std::sqrt(distancex * distancex + distancey * distancey)<= static_cast<double>(ldistance));
        }
    template bool FMTCoordinate::within<size_t>(const size_t& ldistance, const FMTCoordinate& coord) const;
    template bool FMTCoordinate::within<unsigned int>(const unsigned int& ldistance, const FMTCoordinate& coord) const;
    template bool FMTCoordinate::within<double>(const double& ldistance,const FMTCoordinate& coord) const;
    template bool FMTCoordinate::within<uint16_t>(const uint16_t& ldistance, const FMTCoordinate& coord) const;



    FMTCoordinate FMTCoordinate::getAverageCentroid(const std::array<FMTCoordinate, 4>& p_enveloppe)
    {
        const uint16_t startx = p_enveloppe.at(0).m_x;
        const uint16_t starty = p_enveloppe.at(0).m_y;
        const uint16_t plusx = ((p_enveloppe.at(1).m_x) - startx) / 2;
        const uint16_t plusy = ((p_enveloppe.at(2).m_y) - starty) / 2;
        return FMTCoordinate(startx + plusx, starty + plusy);
    }

    std::set<FMTCoordinate> FMTCoordinate::getTerritory(const std::array<FMTCoordinate, 4>& p_enveloppe, const size_t& p_distance)
    {
        std::set<FMTCoordinate>territory;
        const int distanceof = static_cast<int>(p_distance);
        const int zeroof = 0;
        territory.insert(FMTCoordinate(std::max(static_cast<int>(p_enveloppe.at(0).m_x) - distanceof, zeroof), std::max(static_cast<int>(p_enveloppe.at(0).m_y) - distanceof, zeroof)));
        territory.insert(FMTCoordinate(static_cast<int>(p_enveloppe.at(1).m_x) + distanceof, std::max(static_cast<int>(p_enveloppe.at(1).m_y) - distanceof, zeroof)));
        territory.insert(FMTCoordinate(std::max(static_cast<int>(p_enveloppe.at(2).m_x) - distanceof, zeroof), static_cast<int>(p_enveloppe.at(2).m_y) + distanceof));
        territory.insert(FMTCoordinate(static_cast<int>(p_enveloppe.at(3).m_x) + distanceof, static_cast<int>(p_enveloppe.at(3).m_y) + distanceof));
        return territory;
    }

    size_t FMTCoordinate::getHeight(const std::array<FMTCoordinate, 4>& p_enveloppe)
    {
        size_t height = 0;
        if (p_enveloppe.at(0).m_y > p_enveloppe.at(2).m_y)
        {
            height = static_cast<size_t>(p_enveloppe.at(0).m_y - p_enveloppe.at(2).m_y);
        }
        else {
            height = static_cast<size_t>(p_enveloppe.at(2).m_y - p_enveloppe.at(0).m_y);
        }
        return height + 1;

    }

    size_t FMTCoordinate::getWidth(const std::array<FMTCoordinate, 4>& p_enveloppe)
    {
        //const std::vector<FMTCoordinate>enveloppe = getenveloppe();
        //return static_cast<size_t>(enveloppe.at(1).getXDistance(enveloppe.at(0))) + 1;
        size_t width = 0;
        if (p_enveloppe.at(0).m_x > p_enveloppe.at(1).m_x)
        {
            width = static_cast<size_t>(p_enveloppe.at(0).m_x - p_enveloppe.at(1).m_x);
        }
        else {
            width = static_cast<size_t>(p_enveloppe.at(1).m_x - p_enveloppe.at(0).m_x);
        }
        return width + 1;

    }


    /*unsigned int FMTCoordinate::getX() const
        {
        return x;
        }
    unsigned int FMTCoordinate::getY() const
        {
        return y;
        }*/

    const std::vector<double> FMTCoordinate::getSpatialCoordinate(std::vector<double> geoTransform) const
    {
        double xGeo = geoTransform[0] + (m_x + 0.5) * geoTransform[1] + (m_y + 0.5) * geoTransform[2];
        double yGeo = geoTransform[3] + (m_x + 0.5) * geoTransform[4] + (m_y + 0.5) * geoTransform[5];

        return std::vector<double>({ xGeo, yGeo });
    }

    FMTCoordinate& FMTCoordinate::operator = (const FMTCoordinate& rhs)
        {
        if(this!=&rhs)
            {
            m_x = rhs.m_x;
            m_y = rhs.m_y;
            }
        return *this;
        }
    bool FMTCoordinate::operator == (const FMTCoordinate& rhs) const
        {
        return (m_x == rhs.m_x && m_y == rhs.m_y);
        }
     bool FMTCoordinate::operator < (const FMTCoordinate& rhs) const
        {
        return ((m_y<rhs.m_y)||((m_y==rhs.m_y)&&(m_x<rhs.m_x)));
        }

	 size_t FMTCoordinate::hash() const
		{
		 size_t hash = 0;
		 boost::hash_combine(hash, m_x);
		 boost::hash_combine(hash, m_y);
		 return hash;
		}

    void FMTCoordinate::upEnveloppe(std::array<FMTCoordinate, 4>& enveloppe) const
        {
		//0//-//1//
		//-//-//-//
		//2//-//3//
		const uint16_t minx = std::min(enveloppe[0].m_x, m_x);
		const uint16_t maxx = std::max(enveloppe[1].m_x, m_x);
		const uint16_t miny = std::min(enveloppe[0].m_y, m_y);
		const uint16_t maxy = std::max(enveloppe[2].m_y, m_y);
		enveloppe[0].m_x = minx;
		enveloppe[0].m_y = miny;
		enveloppe[1].m_x = maxx;
		enveloppe[1].m_y = miny;
		enveloppe[2].m_x = minx;
		enveloppe[2].m_y = maxy;
		enveloppe[3].m_x = maxx;
		enveloppe[3].m_y = maxy;
        }
    FMTCoordinate::operator std::string() const
        {
		return "X"+std::to_string(m_x)+" Y"+std::to_string(m_y);
        }

    uint16_t FMTCoordinate::getX() const
    {
        return m_x;
    }
    uint16_t FMTCoordinate::getY() const
    {
        return m_y;
    }

    std::set<FMTCoordinate> FMTCoordinate::getNeighbors(const uint16_t& nsize,const bool& circle) const
        {   ///nsize must be odd number
            ///https://grass.osgeo.org/grass78/manuals/r.neighbors.html
            std::set<FMTCoordinate> n;
            if (circle)
            {
                const int radius = (static_cast<int>(nsize)-1)/2;
                const int ymax = m_y+radius;
                int ly = static_cast<int>(m_y)- radius;
                while (ly<=ymax)
                {
					const int diffint = (ly - static_cast<int>(m_y));
                    const int lydistance = std::abs(diffint);
                    const int lxdistance = static_cast<uint16_t>(sqrt((radius*radius)-(lydistance*lydistance)));
                    int lx = static_cast<int>(m_x)-lxdistance;
                    const int xmax = static_cast<int>(m_x)+lxdistance;
                    while(lx<=xmax)
                    {
                        if ((lx>=0 && ly>=0) && !(static_cast<uint16_t>(lx)==m_x && static_cast<uint16_t>(lx)==m_y))
                        {
                            n.emplace(static_cast<uint16_t>(lx),static_cast<uint16_t>(ly));
                        }
                        lx+=1;
                    }
                    ly+=1;
                }
            }
            else
            {
                const int d = (static_cast<int>(nsize)-1)/2;
                const int xmin = static_cast<int>(m_x)-d;
                const int ymin = static_cast<int>(m_y) -d;
                const int xmax = static_cast<int>(m_x) +d;
                const int ymax = static_cast<int>(m_y) +d;
                int lx = xmin;
                int ly = ymin;
                while(ly<=ymax)
                {
                    while(lx<=xmax)
                    {
                        if ((lx>=0 && ly>=0) && !(static_cast<uint16_t>(lx)==m_x && static_cast<uint16_t>(lx)==m_y))
                        {
                            n.emplace(static_cast<uint16_t>(lx),static_cast<uint16_t>(ly));
                        }
                        lx+=1;
                    }
                    lx=xmin;
                    ly+=1;
                }
            }
            return n;
        }
    }
