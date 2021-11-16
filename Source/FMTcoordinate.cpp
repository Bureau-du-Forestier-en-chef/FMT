/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTcoordinate.hpp"
#include <cstdlib>
#include <iostream>

namespace Spatial
    {
    //Implementation for Rcpp .. Normally should be static member
	constexpr std::array<int,8> x_n = { 0,1,0,-1,1,1,-1,-1 };
	constexpr std::array<int,8> y_n = { -1,0,1,0,-1,1,1,-1 };
    FMTcoordinate::FMTcoordinate():x(),y(){}
    FMTcoordinate::FMTcoordinate(unsigned int lx, unsigned int ly):
        x(lx),y(ly){}
    FMTcoordinate::FMTcoordinate(const FMTcoordinate& rhs):x(rhs.x),y(rhs.y){}

    FMTcoordinate FMTcoordinate::at(unsigned int id) const
        {
        ///Max 12
        //7//0//4//
        //3// //1//
        //6//2//5//
        /// Factor is a floor
        const int factor = ((id / 8) + 1);
        id = (id - (factor-1) * 8);
        return FMTcoordinate(x+(x_n[id]*factor),y+(y_n[id]*factor));
        }

	void FMTcoordinate::getxygap(const FMTcoordinate& rhs, int& xgap, int& y_gap) const
		{
		xgap = (static_cast<int>(x) - static_cast<int>(rhs.x));
		y_gap = (static_cast<int>(y) - static_cast<int>(rhs.y));
		}

    double FMTcoordinate::distance(const FMTcoordinate& coord) const
        {
        return std::sqrt(distanceapproximation(coord));
        }

	double FMTcoordinate::distanceapproximation(const FMTcoordinate& coord) const
		{
		int distancex = 0;
		int distancey = 0;
		getxygap(coord, distancex, distancey);
		return static_cast<double>(distancex * distancex + distancey * distancey);
		}

	std::set<FMTcoordinate>::const_iterator FMTcoordinate::closest(const std::vector<std::set<FMTcoordinate>::const_iterator>& coordinates, double& approximation) const
		{
		approximation = std::numeric_limits<double>::infinity();
		std::set<FMTcoordinate>::const_iterator bestcoordinate;
		for (const std::set<FMTcoordinate>::const_iterator& coordinate : coordinates)
			{
			const double value = distanceapproximation(*coordinate);
			if (value < approximation)
				{
				approximation = value;
				bestcoordinate = coordinate;
				}
			}
		return bestcoordinate;
		}


    bool FMTcoordinate::within(unsigned int ldistance,const FMTcoordinate& coord) const
        {
		int distancex = 0;
		int distancey = 0;
		getxygap(coord, distancex, distancey);
		return (static_cast<unsigned int>(std::abs(distancex)) <= ldistance && static_cast<unsigned int>(std::abs(distancey)) <= ldistance &&
			std::sqrt(distancex * distancex + distancey * distancey)<= static_cast<double>(ldistance));
        }
    unsigned int FMTcoordinate::getx() const
        {
        return x;
        }
    unsigned int FMTcoordinate::gety() const
        {
        return y;
        }
    FMTcoordinate& FMTcoordinate::operator = (const FMTcoordinate& rhs)
        {
        if(this!=&rhs)
            {
            x = rhs.x;
            y = rhs.y;
            }
        return *this;
        }
    bool FMTcoordinate::operator == (const FMTcoordinate& rhs) const
        {
        return (x == rhs.x && y == rhs.y);
        }
     bool FMTcoordinate::operator < (const FMTcoordinate& rhs) const
        {
        return ((y<rhs.y)||((y==rhs.y)&&(x<rhs.x)));
        }

	 size_t FMTcoordinate::hash() const
		{
		 size_t hash = 0;
		 boost::hash_combine(hash, x);
		 boost::hash_combine(hash, y);
		 return hash;
		}

    void FMTcoordinate::upenveloppe(std::vector<FMTcoordinate>& enveloppe) const
        {
		//0//-//1//
		//-//-//-//
		//2//-//3//
		int minx = std::min(enveloppe[0].x,x);
		int maxx = std::max(enveloppe[1].x,x);
		int miny = std::min(enveloppe[0].y,y);
		int maxy = std::max(enveloppe[2].y,y);
		enveloppe[0].x = minx;
		enveloppe[0].y = miny;
		enveloppe[1].x = maxx;
		enveloppe[1].y = miny;
		enveloppe[2].x = minx;
		enveloppe[2].y = maxy;
		enveloppe[3].x = maxx;
		enveloppe[3].y = maxy;
        }
    FMTcoordinate::operator std::string() const
        {
		return "X"+std::to_string(x)+" Y"+std::to_string(y);
        }
    std::set<FMTcoordinate> FMTcoordinate::getneighbors(const unsigned int& nsize,const bool& circle) const
        {   ///nsize must be odd number
            ///https://grass.osgeo.org/grass78/manuals/r.neighbors.html
            std::set<FMTcoordinate> n;
            if (circle)
            {
                const unsigned int radius = (nsize-1)/2;
                const int ymax = y+radius;
                int ly = y-radius;
                while (ly<=ymax)
                {
					const int diffint = (ly - y);
                    const int lydistance = std::abs(diffint);
                    const int lxdistance = static_cast<unsigned int>(sqrt((radius*radius)-(lydistance*lydistance)));
                    int lx = x-lxdistance;
                    const int xmax = x+lxdistance;
                    while(lx<=xmax)
                    {
                        if ((lx>=0 && ly>=0) && !(static_cast<unsigned int>(lx)==x && static_cast<unsigned int>(lx)==y))
                        {
                            n.emplace(static_cast<unsigned int>(lx),static_cast<unsigned int>(ly));
                        }
                        lx+=1;
                    }
                    ly+=1;
                }
            }
            else
            {
                const unsigned int d = (nsize-1)/2;
                const int xmin = x-d;
                const int ymin = y-d;
                const int xmax = x+d;
                const int ymax = y+d;
                int lx = xmin;
                int ly = ymin;
                while(ly<=ymax)
                {
                    while(lx<=xmax)
                    {
                        if ((lx>=0 && ly>=0) && !(static_cast<unsigned int>(lx)==x && static_cast<unsigned int>(lx)==y))
                        {
                            n.emplace(static_cast<unsigned int>(lx),static_cast<unsigned int>(ly));
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
