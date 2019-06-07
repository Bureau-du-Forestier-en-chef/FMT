#include "FMTcoordinate.h"
#include <iostream>

namespace Spatial
    {
	vector<int>FMTcoordinate::x_n = { 0,1,0,-1,1,1,-1,-1 };
	vector<int>FMTcoordinate::y_n = { 1,0,-1,0,1,-1,-1,1 };


    FMTcoordinate::FMTcoordinate():x(),y()/*,x_n({0,1,0,-1,1,1,-1,-1}),y_n({1,0,-1,0,1,-1,-1,1})*/{}
    FMTcoordinate::FMTcoordinate(unsigned int lx, unsigned int ly):
        x(lx),y(ly)/*,x_n({0,1,0,-1,1,1,-1,-1}),y_n({1,0,-1,0,1,-1,-1,1})*/{}
    FMTcoordinate::FMTcoordinate(const FMTcoordinate& rhs):x(rhs.x),y(rhs.y)/*,x_n(rhs.x_n),y_n(rhs.y_n)*/{}

    FMTcoordinate FMTcoordinate::at(unsigned int id) const
        {
        int factor = ((id / 8) + 1);
        id = (id - (factor-1) * 8);
        return FMTcoordinate(x+(x_n[id]*factor),y+(y_n[id]*factor));
        }
    double FMTcoordinate::distance(const FMTcoordinate& coord) const
        {
        int distancex = (x - coord.x)^2;
        int distancey = (y - coord.y)^2;
        return sqrt(distancex - distancey);
        }
    bool FMTcoordinate::within(unsigned int ldistance,const FMTcoordinate& coord) const
        {
        int distancex = (x - coord.x);
        int distancey = (y - coord.y);
        if (static_cast<unsigned int>(abs(distancex))<=ldistance && 
			(static_cast<unsigned int>(abs(distancey))<=ldistance))
            {
            if (this->distance(coord) <= double(ldistance))
                {
                return true;
                }
            }
        return false;
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
            /*x_n = rhs.x_n;
            y_n = rhs.y_n;*/
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

    void FMTcoordinate::upenveloppe(vector<FMTcoordinate>& enveloppe) const
        {
        if (y > enveloppe[0].y && x > enveloppe[0].x)
            {
            enveloppe[0] = *this;
            }else if(y < enveloppe[1].y && x > enveloppe[1].x)
                {
                enveloppe[1] = *this;
                }else if(y < enveloppe[2].y && x < enveloppe[2].x)
                    {
                    enveloppe[2] = *this;
                    }else if(y > enveloppe[3].y && x < enveloppe[3].x)
                        {
                        enveloppe[3] = *this;
                        }
        }
    }
