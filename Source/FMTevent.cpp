/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTevent.h"
#include <queue>

namespace Spatial
{
    FMTevent::FMTevent():/*ignition(),active(),*/action_id(),period(),elements(){}
	FMTevent::FMTevent(const FMTcoordinate& location,const int& laction_id,const int& lperiod): /*ignition(location),
                                                                                                active(),*/
                                                                                                action_id(laction_id),
                                                                                                period(lperiod),
                                                                                                elements()
	
	{
		elements.insert(location);
	}

    FMTevent::FMTevent(const FMTevent& rhs):/*ignition(rhs.ignition),
                                            active(rhs.active),*/
                                            action_id(rhs.action_id),
                                            period(rhs.period),
                                            elements(rhs.elements){}

    FMTevent& FMTevent::operator = (const FMTevent& rhs)
    {
    if(this!=&rhs)
        {
        /*ignition = rhs.ignition;
        active = rhs.active;*/
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
            if (size() == rhs.size() && getenveloppe() == rhs.getenveloppe())
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
        //FMTcoordinate centroid = averagecentroid();
        //FMTcoordinate rhscentroid = rhs.averagecentroid();
        //strict ordering
		 if (period < rhs.period)
         {
            return true;
         }

		 if (rhs.period < period)
        {
            return false;
        }
		 const std::set<FMTcoordinate>::const_iterator centroidit = midposition();
		 const std::set<FMTcoordinate>::const_iterator rhscentroidit = rhs.midposition();
         if (*centroidit < *rhscentroidit)
         {
            return true;
         }

         if (*rhscentroidit < *centroidit)
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

         /*if (size() < rhs.size())
         {
             return true;
         }

		 if (rhs.size() < size())
         {
             return false;
         }*/
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

	size_t FMTevent::hash() const// {return boost::hash<Spatial::FMTcoordinate>()(ignition); }
		{
		//size_t hashs = boost::hash<Spatial::FMTcoordinate>()(ignition);
		size_t hashs = boost::hash<Spatial::FMTcoordinate>()(*midposition());
		boost::hash_combine(hashs, action_id);
		boost::hash_combine(hashs,period);
		return hashs;
		}

	FMTeventrelation FMTevent::getrelation(const FMTevent& rhs) const
	{
		return FMTeventrelation(*this, rhs);
	}

	size_t FMTevent::height() const
    {
	const std::vector<FMTcoordinate>enveloppe = getenveloppe();
    return ((static_cast<size_t>(enveloppe.at(2).gety()) - static_cast<size_t>(enveloppe.at(0).gety()))+1);
    }

	size_t FMTevent::width() const
    {
	const std::vector<FMTcoordinate>enveloppe = getenveloppe();
    return ((static_cast<size_t>(enveloppe.at(1).getx()) - static_cast<size_t>(enveloppe.at(0).getx()))+1);
    }

	FMTcoordinate FMTevent::averagecentroid() const
    {
	const std::vector<FMTcoordinate>enveloppe = getenveloppe();
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
    }

     void FMTevent::merge(const FMTevent& event)
     {
        elements.insert(event.elements.begin(),event.elements.end());
     }

	void FMTevent::insert(const FMTcoordinate& newlocation)
    {
    elements.insert(Spatial::FMTcoordinate(newlocation));
    }

	std::vector<std::set<FMTcoordinate>::const_iterator> FMTevent::ignit(const size_t& eventmaximalsize,
		const std::set<FMTcoordinate>::const_iterator& ignit, const int& laction_id, const int& lperiod)
        {
        //add set period and set action id
		std::vector<std::set<FMTcoordinate>::const_iterator>actives;
        if ((1 <= eventmaximalsize))
            {
            //ignition = ignit;
            actives.push_back(ignit);
            action_id = laction_id;
            period = lperiod;
            //return true;
            }
        //return false;
		return actives;
        }

	bool FMTevent::spread(const size_t& eventminimalsize, const size_t& eventmaximalsize,
		const size_t& eventeventsize, const std::set<FMTcoordinate>& territory, std::vector<std::set<FMTcoordinate>::const_iterator> active)
        {
            while((elements.size() < eventmaximalsize) && (!active.empty()))
                {
                std::vector<std::set<FMTcoordinate>::const_iterator>::iterator coordit;
                for(size_t id = 0; id < eventeventsize; ++id)
                    {
					coordit = active.begin();
                    const FMTcoordinate spread_coord = (*coordit)->at(static_cast<int>(id));
					const std::set<FMTcoordinate>::const_iterator spreadit = territory.find(spread_coord);
                    if(spreadit != territory.end() && elements.find(spread_coord) == elements.end())
                        {
                        if(std::find(active.begin(),active.end(), spreadit)==active.end())
                            {
                            active.push_back(spreadit);
                            }
                        }
                    }
				coordit = active.begin();
				insert(**coordit);
                //insert(*coord);
                active.erase(active.begin());
                }
            if (elements.size()>= eventminimalsize)
                {
                return true;
                }
            active.clear();
            //active.push_back(this->ignition);
            elements.clear();
            return false;
        }

	std::vector<std::set<FMTcoordinate>::const_iterator>FMTevent::getborders() const
	{
		std::vector<std::set<FMTcoordinate>::const_iterator>borders;
		for (std::set<FMTcoordinate>::const_iterator elemit = elements.begin(); elemit != elements.end(); elemit++)
		{
			for (int id = 0; id < 4; ++id)
			{
				const FMTcoordinate neighbor = elemit->at(id);
				if (elements.find(neighbor) == elements.end())//Ok its borderelement
				{
					borders.push_back(elemit);
					break;
				}
			}
		}
		return borders;
	}

	std::vector<FMTcoordinate>FMTevent::getenveloppe() const
		{
		std::vector<FMTcoordinate>enveloppe(4,*elements.begin());
		for (std::set<FMTcoordinate>::const_iterator border : getborders())
			{
			border->upenveloppe(enveloppe);
			}
		return enveloppe;
		}


	void FMTevent::getclosescoordinates(const FMTevent& rhs,
		std::set<FMTcoordinate>::const_iterator& thiscoordinate,
		std::set<FMTcoordinate>::const_iterator& rhscoordinate) const
	{
		const std::vector<std::set<FMTcoordinate>::const_iterator>rhsborders = rhs.getborders();
		double bestapproximation = std::numeric_limits<double>::infinity();
		for (const std::set<FMTcoordinate>::const_iterator& coord : getborders())
		{
			double approx = std::numeric_limits<double>::infinity();
			std::set<FMTcoordinate>::const_iterator rhscoord = coord->closest(rhsborders, approx);
			if (approx < bestapproximation)
			{
				bestapproximation = approx;
				thiscoordinate = coord;
				rhscoordinate = rhscoord;
			}
		}
	}


    double FMTevent::distance(const FMTevent& rhs) const
		{
		std::set<FMTcoordinate>::const_iterator thiscoordinate;
		std::set<FMTcoordinate>::const_iterator rhscoordinate;
		getclosescoordinates(rhs, thiscoordinate, rhscoordinate);
		return thiscoordinate->distance(*rhscoordinate);
		}

	bool FMTevent::within(unsigned int dist, const FMTevent& rhs) const
	{
		const std::set<FMTcoordinate>::const_iterator center = midposition();
		const std::set<FMTcoordinate>::const_iterator rhscenter = rhs.midposition();
		if (center->within(dist, *rhscenter))
		{
			return true;
		}else
			{
			for (std::set<FMTcoordinate>::const_iterator coord = elements.begin();coord!=elements.end();++coord)
			{
				for (std::set<FMTcoordinate>::const_iterator rhscoord = rhs.elements.begin(); rhscoord != rhs.elements.end(); ++rhscoord)
				{
					if (coord->within(dist, *rhscoord))
					{
						return true;
					}
				}
			}
			}
			return false;
		}


    bool FMTevent::within(unsigned int dist, const FMTcoordinate& location) const
    {
	const std::set<FMTcoordinate>::const_iterator center = midposition();
    if(center->within(dist,location))
        {
        return true;
	}else{
		for (std::set<FMTcoordinate>::const_iterator elemit = elements.begin(); elemit != elements.end(); elemit++)
			{
			if (elemit->within(dist, location))
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

	std::set<FMTcoordinate>FMTevent::getterritory(const size_t& distance) const
	{
		std::set<FMTcoordinate>territory;
		const std::vector<FMTcoordinate>enveloppe = getenveloppe();
		territory.insert(FMTcoordinate(std::max(enveloppe.at(0).getx() - distance, size_t(0)), std::max(enveloppe.at(0).gety() - distance, size_t(0))));
		territory.insert(FMTcoordinate(enveloppe.at(1).getx() + distance, std::max(enveloppe.at(1).gety() - distance, size_t(0))));
		territory.insert(FMTcoordinate(std::max(enveloppe.at(2).getx() - distance, size_t(0)), enveloppe.at(2).gety() + distance));
		territory.insert(FMTcoordinate(enveloppe.at(3).getx() + distance, enveloppe.at(3).gety() + distance));
		return territory;
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

