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

#ifndef FMTEVENT_H_INCLUDED
#define FMTEVENT_H_INCLUDED

#include "FMTcoordinate.h"
#include <random>
#include <limits>
#include <iterator>
#include "FMTspatialaction.h"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
namespace Spatial
{
template<typename T>
class FMTevent
    {
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & BOOST_SERIALIZATION_NVP(ignition);
		ar & BOOST_SERIALIZATION_NVP(active);
		ar & BOOST_SERIALIZATION_NVP(territory);
		ar & BOOST_SERIALIZATION_NVP(enveloppe);
		ar & BOOST_SERIALIZATION_NVP(order);
	}
	FMTcoordinate midposition() const
		{
		//get mid point of elements!
		size_t midlocation = (elements.size() / 2);
		return ((elements.begin() + midlocation)->first);
		}
    protected:
        FMTcoordinate ignition;
        vector<FMTcoordinate>active;
        const map<FMTcoordinate,T>* territory;
        vector<FMTcoordinate>enveloppe;
		int order;
    public:
    map<FMTcoordinate,const T*>elements;
    virtual ~FMTevent() = default;
    FMTevent():
        ignition(),
        active(),
        territory(),
        enveloppe(),
		order(),
        elements()
        {

        }
	FMTevent(const FMTcoordinate& location) :
		ignition(location),
		active(),
		territory(),
		enveloppe(vector<FMTcoordinate>(4, location)),
		order(),
		elements()
	{

	}
    FMTevent(const map<FMTcoordinate,T>& lterritory, const int& pass):
        ignition(),
        active(),
        territory(&lterritory),
        enveloppe(),
		order(pass),
        elements()
        {

        }
    FMTevent(const FMTevent& rhs):
        ignition(rhs.ignition),
        active(rhs.active),
        territory(rhs.territory),
        enveloppe(rhs.enveloppe),
		order(rhs.order),
        elements(rhs.elements)
        {

        }

	bool empty() const
		{
		return elements.empty();
		}

	size_t size() const
		{
		return elements.size();
		}
	bool operator == (const FMTevent<T>& rhs) const
		{
		if (size() == rhs.size() && enveloppe == rhs.enveloppe)
			{
			for (typename map<FMTcoordinate,const T*>::const_iterator it = elements.begin(); it!=elements.end();it++)
				{
				if (rhs.elements.find(it->first)==rhs.elements.end())
					{
					return false;
					}
				}
			return true;
			}
		return false;
		}

	bool operator != (const FMTevent& rhs) const
		{
		return (!(*this==rhs));
		}

	bool operator < (const FMTevent& rhs) const
		{
		return (this->midposition() < rhs.midposition);
		}

	/*set<FMTevent>geteventswithin(const set<FMTevent>& allevents, const unsigned int& distance) const;
		{
		//
		set<FMTevent>nearevents;
		set<FMTevent>::const_iterator itlower = allevents.lower_bound(*this);
		size_t location = distance(allevents.begin(), itlower);
		while (location >= 0 && (itlower - location)->within(distance,*this))
			{
			earevents.insert(*itlower);
			--location;
			}
		set<FMTevent>::const_iterator itupper = allevents.upper_bound(*this);
		location = 0;
		while (itupper != allevents.end())
			{

			++location;
			}


		}*/

	int getorder() const
		{
		return order;
		}
	//https://www.umass.edu/landeco/research/fragstats/documents/fragstats.help.4.2.pdf
	//metrics
	size_t perimeter() const //gives perimeter
		{
		size_t total = 0;
		for (typename map<FMTcoordinate, const T*>::const_iterator it = elements.begin(); it != elements.end(); it++)
			{
			for (int id = 0; id < 4; ++id)
				{
				const FMTcoordinate neighbor= it->first.at(id);
				if (elements.find(neighbor)==elements.end())
					{
					++total;
					}
				}

			}
		return total;
		}

	size_t height() const
		{
		return ((enveloppe.at(2).gety() - enveloppe.at(0).gety())+1);
		}

	size_t width() const
		{
		return ((enveloppe.at(1).getx() - enveloppe.at(0).getx())+1);
		}

	FMTcoordinate averagecentroid() const
		{
		size_t startx = enveloppe.at(0).getx();
		size_t starty = enveloppe.at(0).gety();
		size_t plusx = enveloppe.at(1).getx() - startx;
		size_t plusy = enveloppe.at(2).gety() - starty;
		return FMTcoordinate(startx + plusx, starty + plusy);
		}

	string getstats() const
		{
		return (" "+to_string(size()) +" "+
			to_string(perimeter()) +" "+
			to_string(height()) +" "+
			to_string(width()));
		}

	void erase(const FMTcoordinate& newlocation)
		{
		if (elements.find(newlocation)!= elements.end())
			{
			elements.erase(newlocation);
			}
		if (empty())
			{
			enveloppe.clear();
		}else {
			enveloppe = vector<FMTcoordinate>(4, elements.begin()->first);
			for (auto& element : elements)
				{
				element.first.upenveloppe(enveloppe);
				}
			}
		}

	void insert(const FMTcoordinate& newlocation, const void* element)
		{
		elements.insert(pair<Spatial::FMTcoordinate, const T*>(newlocation, static_cast<const T*>(element)));
		newlocation.upenveloppe(enveloppe);
		}

    FMTevent& operator = (const FMTevent& rhs)
        {
        if(this!=&rhs)
            {
            elements = (rhs.elements);
            ignition= rhs.ignition;
            active=rhs.active;
            territory=rhs.territory;
			order = rhs.order;
            enveloppe=rhs.enveloppe;
            }
        return *this;
        }
    FMTcoordinate getignition()
        {
        return ignition;
        }
    double distance(const FMTevent& rhs) const
        {
        double minimaldistance = numeric_limits<double>::max();
        for(const FMTcoordinate& coord : enveloppe)
            {
            for(const FMTcoordinate& rhscoord : rhs.enveloppe)
                {
                double dist = coord.distance(rhscoord);
                if (dist < minimaldistance)
                    {
                    minimaldistance = dist;
                    }
                }
            }
        return minimaldistance;
        }
	double minimaldistance(const vector<FMTevent>& events,const unsigned int& distancel) const
		{
		double distancevalue = static_cast<double>(distancel) + 1.0;
		for (const FMTevent& element : events)
			{
			if (element.within(distancel,element))
				{
				double value = distance(element);
				if (value < distancevalue)
					{
					distancevalue = value;
					}
				}
			}
		return distancevalue;
		}
    bool within(unsigned int dist, const FMTevent& rhs) const
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
    bool withinc(unsigned int dist, const FMTcoordinate& location) const
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
    bool contain(const FMTcoordinate& coord)const
        {
            if (elements.find(coord)!=elements.end())
            {
                return true;
            }
            return false;
        }
    virtual bool ignit(const FMTspatialaction& action,const FMTcoordinate& ignit)
        {
        if ((1 <= action.maximal_size))
            {
            ignition = ignit;
            active.push_back(ignition);
            enveloppe = vector<FMTcoordinate>(4,ignition);
            return true;
            }
        return false;
        }
    virtual bool spread(const FMTspatialaction& action)
        {
        while((elements.size() < action.maximal_size) && (!active.empty()))
            {
            vector<FMTcoordinate>::iterator coord;
            for(size_t id = 0; id < action.neighbors_size; ++id)
                {
                coord = active.begin();
                const FMTcoordinate spread_coord = coord->at(int(id));
                if(territory->find(spread_coord)!= territory->end() && elements.find(spread_coord) == elements.end())
                    {
                    if(std::find(active.begin(),active.end(),spread_coord)==active.end())
                        {
                        active.push_back(spread_coord);
                        }

                    }
                }
            coord = active.begin();
			insert(*coord, &(territory->at(*coord)));
			/*coord->upenveloppe(enveloppe);
            elements.insert(pair<Spatial::FMTcoordinate,const T*>(*coord,&(territory->at(*coord))));
            coord->upenveloppe(enveloppe);*/
            active.erase(active.begin());
            }
        if (elements.size()>=action.minimal_size)
            {
            return true;
            }
        active.clear();
        enveloppe.clear();
        active.push_back(ignition);
        elements.clear();
        return false;
        }

    bool whithinelements(unsigned int dist, const FMTcoordinate& location) const
    {
        for (typename map<FMTcoordinate, const T*>::const_iterator elemit = elements.begin(); elemit != elements.end(); elemit++)
        {
             if (elemit->first.within(dist,location))
             {
                return true;
             }
        }
        return false;
    }

    bool splittedevent(const unsigned int& distancel, vector<FMTevent>& splittedevents) const
        //Check if events are split and fill vector of splitted events
        {
            vector<typename map<FMTcoordinate,const T*>::const_iterator> it_vect;
            while(it_vect.size()<elements.size())
            {
                size_t iteration = 0;
                size_t alloc_count = 0;
                for (typename map<FMTcoordinate, const T*>::const_iterator elemit = elements.begin(); elemit != elements.end(); elemit++)
                {
                    if (iteration == 0 && elemit==elements.begin())
                    {
                        splittedevents.clear();
                        FMTevent newevent(elemit->first);
                        newevent.insert(elemit->first,nullptr);
                        splittedevents.push_back(newevent);
                        it_vect.push_back(elemit);
                    }
                    if (find(it_vect.begin(),it_vect.end(),elemit)==it_vect.end())//If not already allocated
                    {
                        FMTevent& lastevent = splittedevents.back();
                        if (lastevent.whithinelements(distancel,elemit->first))//If in distance of 1
                        {
                            lastevent.insert(elemit->first,nullptr);
                            it_vect.push_back(elemit);
                            ++alloc_count;
                        }
                    }
                }
                if(alloc_count==0)
                {
                    for (typename map<FMTcoordinate, const T*>::const_iterator elemit = elements.begin(); elemit != elements.end(); elemit++)
                    {
                        if (find(it_vect.begin(),it_vect.end(),elemit)==it_vect.end())
                        {
                            FMTevent newevent(elemit->first);
                            newevent.insert(elemit->first,nullptr);
                            splittedevents.push_back(newevent);
                            it_vect.push_back(elemit);
                            break;
                        }
                    }

                }
                ++iteration;
            }
            if (splittedevents.size()>1)
            {
                return true;
            }
            return false;
        }
    };
}

namespace boost {

  template <typename T>
  struct hash<Spatial::FMTevent<T>>
  {
    std::size_t operator()(const Spatial::FMTevent<T>& event) const
    {

      return (hash<Spatial::FMTcoordinate>()(event.getignition()));
    }
  };

}
#endif // FMTEVENT_H_INCLUDED
