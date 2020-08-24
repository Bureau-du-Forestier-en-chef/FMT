/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTevent.h"
#include <queue>

namespace Spatial
{
    FMTevent::FMTevent():ignition(),enveloppe(),active(),action_id(),period(),elements(){}
	FMTevent::FMTevent(const FMTcoordinate& location,const int& laction_id,const int& lperiod): ignition(location),
                                                                                                enveloppe(std::vector<FMTcoordinate>(4, location)),
                                                                                                active(),
                                                                                                action_id(laction_id),
                                                                                                period(lperiod),
                                                                                                elements(){}

    FMTevent::FMTevent(const FMTevent& rhs):ignition(rhs.ignition),
                                            enveloppe(rhs.enveloppe),
                                            active(rhs.active),
                                            action_id(rhs.action_id),
                                            period(rhs.period),
                                            elements(rhs.elements){}

    FMTevent& FMTevent::operator = (const FMTevent& rhs)
    {
    if(this!=&rhs)
        {
        ignition = rhs.ignition;
        enveloppe = rhs.enveloppe;
        active = rhs.active;
        action_id = rhs.action_id;
        period = rhs.period;
        elements = rhs.elements;
        }
    return *this;
    }

	bool FMTevent::operator == (const FMTevent& rhs) const
    {
        FMTcoordinate centroid = averagecentroid();
        FMTcoordinate rhscentroid = rhs.averagecentroid();
        if (period == rhs.period && action_id == rhs.action_id && centroid == rhscentroid)
            {
                //return true;
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
            }
        return false;
    }

	bool FMTevent::operator != (const FMTevent& rhs) const
    {
    return (!(*this==rhs));
    }

    bool FMTevent::operator<(const FMTevent& rhs) const
    {
        FMTcoordinate centroid = averagecentroid();
        FMTcoordinate rhscentroid = rhs.averagecentroid();
        //strict ordering
		 if (period < rhs.period)
         {
            return true;
         }

		 if (rhs.period < period)
        {
            return false;
        }

         if (centroid < rhscentroid)
         {
            return true;
         }

         if (rhscentroid < centroid)
         {
            return false;
         }

		 if (action_id < rhs.action_id)
         {
			 return true;
         }

		 if (rhs.action_id < action_id)
         {
            return false;
         }

         if (size() < rhs.size())
         {
             return true;
         }

		 if (rhs.size() < size())
         {
             return false;
         }
		 return false;
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

    bool FMTevent::ignit(const FMTspatialaction& action,const FMTcoordinate& ignit, const int& laction_id, const int& lperiod)
        {
        //add set period and set action id
        if ((1 <= action.maximal_size))
            {
            ignition = ignit;
            active.push_back(ignition);
            enveloppe = std::vector<FMTcoordinate>(4,ignition);
            action_id = laction_id;
            period = lperiod;
            return true;
            }
        return false;
        }

    bool FMTevent::spread(const FMTspatialaction& action, const std::set<FMTcoordinate>& territory)
        {
            while((elements.size() < action.maximal_size) && (!active.empty()))
                {
                std::vector<FMTcoordinate>::iterator coord;
                for(size_t id = 0; id < action.neighbors_size; ++id)
                    {
                    coord = active.begin();
                    const FMTcoordinate spread_coord = coord->at(static_cast<int>(id));
                    if(territory.find(spread_coord)!= territory.end() && elements.find(spread_coord) == elements.end())
                        {
                        if(std::find(active.begin(),active.end(),spread_coord)==active.end())
                            {
                            active.push_back(spread_coord);
                            }
                        }
                    }
                coord = active.begin();
                insert(*coord);
                active.erase(active.begin());
                }
            if (elements.size()>=action.minimal_size)
                {
                return true;
                }
            active.clear();
            enveloppe.clear();
            active.push_back(this->ignition);
            elements.clear();
            return false;
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

	bool FMTevent::withinelements(unsigned int dist, const FMTevent & rhs) const
	{
		if (ignition.within(dist, rhs.ignition))
		{
			return true;
		}
		else {
			for (const FMTcoordinate& coord : enveloppe)
			{
				for (const FMTcoordinate& rhscoord : rhs.enveloppe)
				{
					if (coord.within(dist, rhscoord))
					{
						return true;
					}
				}
			}
			for (std::set<FMTcoordinate>::const_iterator elemit = rhs.elements.begin(); elemit != rhs.elements.end(); elemit++)
			{
				if (withinelementsc(dist,*elemit))
				{
					return true;
				}
			}
		}
		return false;
	}

    bool FMTevent::withinelementsc(unsigned int dist, const FMTcoordinate& location) const
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

    bool FMTevent::potentialysplitevent(const FMTcoordinate& coord) const
    {
        std::set<int> coordat;
        for (int i=0; i<8 ; ++i)
        {
            //8//1//5//
            //4// //2//
            //7//3//6//
            FMTcoordinate icoord = coord.at(i);
            if (elements.find(icoord)!= elements.end())
            {
                coordat.insert(i+1);
            }
        }
        if (coordat.size()==8)
        {
            return false;
        }
        std::set<int>::const_iterator upit = coordat.upper_bound(4);
        std::set<int>::const_iterator firstit = coordat.begin();
        const int casetype = std::distance(firstit,upit);
        switch (casetype)
        {
            case 1:
            {
                const int first = *firstit;
                const int second = *++firstit;
                if (first==1 && (second==5||second==8))
                {
                    return false;
                }
                if (first==2 && (second==5||second==6))
                {
                    return false;
                }
                if (first==3 && (second==7||second==6))
                {
                    return false;
                }
                if (first==4 && (second==7||second==8))
                {
                    return false;
                }
                else{return true;}
            }
            case 2:
            {
                if (upit==coordat.end())
                {
                    return true;
                }
                const int first = *firstit;
                const int second = *++firstit;
                int dist = second-first;
                if (dist == 3)
                {
                    if (coordat.find(8)!=coordat.end())
                    {
                        return false;
                    }else{
                        return true;
                    }
                }
                else if (dist == 2)
                {
                    return true;
                }
                else if (dist == 1)
                {
                    if (first == 1)
                    {
                        if (coordat.find(5)!=coordat.end())
                        {
                            return false;
                        }else{return true;}
                    }
                    else if (first == 2)
                    {
                        if (coordat.find(6)!=coordat.end())
                        {
                            return false;
                        }else{return true;}
                    }
                    else if (first == 3)
                    {
                        if (coordat.find(7)!=coordat.end())
                        {
                            return false;
                        }else{return true;}
                    }else {return true;}
                }else{return true;}

            }
            case 3:
            {
                if (upit==coordat.end())
                {
                    return true;
                }
                const int first = *firstit;
                const int second = *++firstit;
                const int third = *++firstit;
                if (first == 1)
                {
                    if (second == 2)
                    {
                        if (third == 3)
                        {
                            if (coordat.find(5)!=coordat.end() && coordat.find(6)!=coordat.end())
                            {
                                return false;
                            }else{return true;}
                        }
                        else if(third == 4)
                        {
                            if (coordat.find(5)!=coordat.end() && coordat.find(8)!=coordat.end())
                            {
                                return false;
                            }else{return true;}

                        }else{return true;}
                    }
                    else if (second==3)
                    {
                        if (coordat.find(7)!=coordat.end() && coordat.find(8)!=coordat.end())
                        {
                            return false;
                        }else{return true;}
                    }else{return true;}
                }
                else if (first == 2)
                {
                    if (coordat.find(6)!=coordat.end() && coordat.find(7)!=coordat.end())
                    {
                        return false;
                    }else{return true;}
                }else{return true;}
            }
            case 4:
            {
                if (coordat.size()>6)
                {
                    return false;
                }
                else{return true;}
            }
            default:
            {
                return true;
            }
        }

    }

    bool FMTevent::splitevent(const unsigned int& ldistance, std::vector<FMTevent>& splittedevents) const
    {
        std::queue<FMTcoordinate> active;
        std::set<FMTcoordinate> coordinates = elements;
        active.push(*coordinates.begin());
        while(!coordinates.empty())
        {
            FMTevent newevent(active.front(),action_id,period);
            while(!active.empty())
            {
                //Kind of a spread to create new event
                FMTcoordinate coord = active.front();
                coordinates.erase(coord);
                newevent.insert(coord);
                std::set<FMTcoordinate> spreadcoord = coord.getneighbors((ldistance*2)+1,true);
                for (std::set<FMTcoordinate>::const_iterator nit=spreadcoord.begin();nit!=spreadcoord.end();++nit)
                {
                    if (coordinates.find(*nit)!=coordinates.end())
                    {
                        active.push(*nit);
                    }
                }
                active.pop();
            }
            //If there is no more actives put the event in the vector and find the next coord not in an event
            splittedevents.push_back(newevent);
            for (std::set<FMTcoordinate>::const_iterator ait=coordinates.begin();ait!=coordinates.end();++ait)
            {
                if (coordinates.find(*ait)!=coordinates.end())
                {
                    active.push(*ait);
                    break;
                }
            }
        }
        if (splittedevents.size()>1)
        {
            return true;
        }
        return false;
    }
}

