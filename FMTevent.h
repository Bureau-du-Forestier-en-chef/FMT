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

	int getorder() const
		{
		return order;
		}

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
		return (enveloppe.at(2).gety() - enveloppe.at(0).gety());
		}

	size_t width() const
		{
		return (enveloppe.at(1).getx() - enveloppe.at(0).getx());
		}

	string getstats() const
		{
		return ("Size of: " + to_string(size()) +
			"Perimeter of " + to_string(perimeter()) +
			"Height of " + to_string(height()) +
			"Width of " + to_string(width()));
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
	double minimaldistance(const vector<FMTevent>& events,const unsigned int& distance) const
		{
		double distancevalue = static_cast<double>(distance) + 1.0;
		for (const FMTevent& element : events)
			{
			if (element.within(distance,element))
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
        while((elements.size() <= action.maximal_size) && (!active.empty()))
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
            /*elements.insert(pair<Spatial::FMTcoordinate,const T*>(*coord,&(territory->at(*coord))));
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
