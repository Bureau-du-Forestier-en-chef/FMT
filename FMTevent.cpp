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

#include "FMTevent.h"

namespace Spatial
{
    FMTevent::FMTevent():ignition(),enveloppe(),elements(){}
	FMTevent::FMTevent(const FMTcoordinate& location):  ignition(location),
                                                        enveloppe(std::vector<FMTcoordinate>(4, location)),
                                                        elements(){}

    FMTevent::FMTevent(const FMTevent& rhs):ignition(rhs.ignition),enveloppe(rhs.enveloppe),elements(rhs.elements){}

    FMTevent& FMTevent::operator = (const FMTevent& rhs)
    {
    if(this!=&rhs)
        {
        ignition = rhs.ignition;
        enveloppe = rhs.enveloppe;
        elements = rhs.elements;
        }
    return *this;
    }

	bool FMTevent::operator == (const FMTevent& rhs) const
    {
    if (size() == rhs.size() && enveloppe == rhs.enveloppe)
        {
        for (std::set<FMTcoordinate>::const_iterator it = elements.begin(); it!=elements.end();it++)
            {
            if (rhs.elements.find(*it)==rhs.elements.end())
                {
                return false;
                }
            }
        return true;
        }
    return false;
    }

	bool FMTevent::operator != (const FMTevent& rhs) const
    {
    return (!(*this==rhs));
    }

	//https://www.umass.edu/landeco/research/fragstats/documents/fragstats.help.4.2.pdf
	//metrics
	size_t FMTevent::perimeter() const //gives perimeter
    {
    size_t total = 0;
    for (std::set<FMTcoordinate>::const_iterator it = elements.begin(); it != elements.end(); it++)
        {
        for (int id = 0; id < 4; ++id)
            {
            const FMTcoordinate neighbor= it->at(id);
            if (elements.find(neighbor)==elements.end())
                {
                ++total;
                }
            }

        }
    return total;
    }

	size_t FMTevent::height() const
    {
    return ((static_cast<size_t>(enveloppe.at(2).gety()) - static_cast<size_t>(enveloppe.at(0).gety()))+1);
    }

	size_t FMTevent::width() const
    {
    return ((static_cast<size_t>(enveloppe.at(1).getx()) - static_cast<size_t>(enveloppe.at(0).getx()))+1);
    }

	FMTcoordinate FMTevent::averagecentroid() const
    {
    const size_t startx = enveloppe.at(0).getx();
    const size_t starty = enveloppe.at(0).gety();
    const size_t plusx = (enveloppe.at(1).getx() - startx)/2;
    const size_t plusy = (enveloppe.at(2).gety() - starty)/2;
    return FMTcoordinate(startx + plusx, starty + plusy);
    }

	std::string FMTevent::getstats() const
    {
    return (" "+ std::to_string(size()) +" "+
        std::to_string(perimeter()) +" "+
        std::to_string(height()) +" "+
        std::to_string(width()));
    }

	void FMTevent::erase(const FMTcoordinate& newlocation)
    {
    if (elements.find(newlocation)!= elements.end())
        {
        elements.erase(newlocation);
        }
    if (empty())
        {
        enveloppe.clear();
    }else {
        enveloppe = std::vector<FMTcoordinate>(4, *elements.begin());
        for (auto& element : elements)
            {
            element.upenveloppe(enveloppe);
            }
        }
    }

     void FMTevent::merge(const FMTevent& event)
     {
        elements.insert(event.elements.begin(),event.elements.end());
        for (auto& element : elements)
            {
            element.upenveloppe(enveloppe);
            }
     }

	void FMTevent::insert(const FMTcoordinate& newlocation)
    {
    elements.insert(Spatial::FMTcoordinate(newlocation));
    newlocation.upenveloppe(enveloppe);
    }

    double FMTevent::distance(const FMTevent& rhs) const
    {
    double minimaldistance = std::numeric_limits<double>::max();
    for(const FMTcoordinate& coord : enveloppe)
        {
        for(const FMTcoordinate& rhscoord : rhs.enveloppe)
            {
            const double dist = coord.distance(rhscoord);
            if (dist < minimaldistance)
                {
                minimaldistance = dist;
                }
            }
        }
    return minimaldistance;
    }

    bool FMTevent::within(unsigned int dist, const FMTevent& rhs) const
    {
    if(ignition.within(dist,rhs.ignition))
        {
        return true;
        }else{
            for(const FMTcoordinate& coord : enveloppe)
                {
                for(const FMTcoordinate& rhscoord : rhs.enveloppe)
                    {
                    if (coord.within(dist,rhscoord))
                        {
                        return true;
                        }
                    }
                }
            }
    return false;
    }

    bool FMTevent::withinc(unsigned int dist, const FMTcoordinate& location) const
    {
    if(ignition.within(dist,location))
        {
        return true;
        }else{
            for(const FMTcoordinate& coord : enveloppe)
                {
                if (coord.within(dist,location))
                    {
                    return true;
                    }
                }
            }
    return false;
    }

    bool FMTevent::contain(const FMTcoordinate& coord)const
    {
        if (elements.find(coord)!=elements.end())
        {
            return true;
        }
        return false;
    }

    bool FMTevent::whithinelements(unsigned int dist, const FMTcoordinate& location) const
    {
        for (std::set<FMTcoordinate>::const_iterator elemit = elements.begin(); elemit != elements.end(); elemit++)
        {
             if (elemit->within(dist,location))
             {
                return true;
             }
        }
        return false;
    }
}
