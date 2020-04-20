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

#ifndef FMTSESEVENT_H
#define FMTSESEVENT_H

#include "FMTevent.h"


namespace Spatial
{
template<class T>
class FMTsesevent : public FMTevent
{
    friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & boost::serialization::make_nvp("event", boost::serialization::base_object<FMTevent>(*this));
		ar & BOOST_SERIALIZATION_NVP(territory);
		ar & BOOST_SERIALIZATION_NVP(active);
	}
    protected:
        const std::map<FMTcoordinate,T>* territory;
        std::vector<FMTcoordinate>active;
        int order;

    public:
        std::map<FMTcoordinate,const T*> mapped_elements;

        FMTsesevent():
                    FMTevent(),
                    territory(),
                    active(),
                    order(),
                    mapped_elements(){}

        FMTsesevent(const FMTevent& rhs):
                    FMTevent(rhs),
                    territory(),
                    active(),
                    order(),
                    mapped_elements(){}

        FMTsesevent(const FMTcoordinate& location):
                    FMTevent(location),
                    territory(),
                    active(),
                    order(),
                    mapped_elements(){}

        FMTsesevent(const std::map<FMTcoordinate,T>& lterritory, const int& pass):
                    FMTevent(),
                    territory(&lterritory),
                    active(),
                    order(pass),
                    mapped_elements(){}

        FMTsesevent(const FMTsesevent& rhs):
                    FMTevent(rhs),
                    territory(rhs.territory),
                    active(rhs.active),
                    order(rhs.order),
                    mapped_elements(rhs.mapped_elements){}


        FMTsesevent& operator=(const FMTsesevent& rhs)
        {
            if (this!=&rhs)
            {
                FMTevent::operator = (rhs);
                territory = rhs.territory;
                active = rhs.active;
                order = rhs.order;
                mapped_elements = rhs.mapped_elements;
            }
            return *this;
        }

        virtual ~FMTsesevent() = default;

        void erase(const FMTcoordinate& newlocation)
        {
        if (this->elements.find(newlocation)!= this->elements.end())
            {
            mapped_elements.erase(newlocation);
			this->elements.erase(newlocation);
            }
        if (this->empty())
            {
            this->enveloppe.clear();
        }else {
            this->enveloppe = std::vector<FMTcoordinate>(4, *this->elements.begin());
            for (auto& element : this->elements)
                {
                element.upenveloppe(this->enveloppe);
                }
            }
        }

		void insert(const FMTcoordinate& newlocation, const void* element)
		{
            this->elements.insert(FMTcoordinate(newlocation));
            mapped_elements.insert(std::pair<Spatial::FMTcoordinate, const T*>(newlocation, static_cast<const T*>(element)));
            newlocation.upenveloppe(this->enveloppe);
		}

		void merge(FMTsesevent& event)
		{
            this->elements.insert(event.elements.begin(),event.elements.end());
            for (auto& element : this->elements)
                {
                element.upenveloppe(this->enveloppe);
                }
            mapped_elements.insert(event.mapped_elements.begin(),event.mapped_elements.end());
		}

        inline int getorder() const
        {
            return order;
        }

		bool ignit(const FMTspatialaction& action,const FMTcoordinate& ignit)
        {
        if ((1 <= action.maximal_size))
            {
            this->ignition = ignit;
            active.push_back(this->ignition);
            this->enveloppe = std::vector<FMTcoordinate>(4,this->ignition);
            return true;
            }
        return false;
        }

        bool spread(const FMTspatialaction& action)
        {
            while((mapped_elements.size() < action.maximal_size) && (!active.empty()))
                {
                std::vector<FMTcoordinate>::iterator coord;
                for(size_t id = 0; id < action.neighbors_size; ++id)
                    {
                    coord = active.begin();
                    const FMTcoordinate spread_coord = coord->at(static_cast<int>(id));
                    if(territory->find(spread_coord)!= territory->end() && mapped_elements.find(spread_coord) == mapped_elements.end())
                        {
                        if(std::find(active.begin(),active.end(),spread_coord)==active.end())
                            {
                            active.push_back(spread_coord);
                            }

                        }
                    }
                coord = active.begin();
                insert(*coord, &(territory->at(*coord)));
                active.erase(active.begin());
                }
            if (mapped_elements.size()>=action.minimal_size)
                {
                return true;
                }
            active.clear();
            this->enveloppe.clear();
            active.push_back(this->ignition);
            this->elements.clear();
            mapped_elements.clear();
            return false;
        }


};
}

#endif // FMTSESEVENT_H
