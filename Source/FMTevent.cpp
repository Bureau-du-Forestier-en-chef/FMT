/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTevent.h"
#include "FMTeventrelation.h"
#include <boost/unordered_set.hpp>
#include <queue>

namespace Spatial
{

	FMTevent::FMTevent(const FMTcoordinate& p_location,
                    int p_actionId,
                    int p_period,
                    size_t p_centroidGraphFamily):
            m_actionId(p_actionId),
            m_period(p_period),
            m_centroidGraphFamily(p_centroidGraphFamily),
            m_elements()
	
	{
		m_elements.insert(p_location);
	}


    std::set<FMTcoordinate>::const_iterator FMTevent::midposition() const
    {
        std::set<FMTcoordinate>::const_iterator it = m_elements.begin();
        if (m_elements.size() > 1)
        {
            const size_t midlocation = (m_elements.size() / 2);
            std::advance(it, midlocation);
        }
        return it;
    }

    bool FMTevent::empty() const { return m_elements.empty(); }


    size_t FMTevent::size() const { return m_elements.size(); }


    const int& FMTevent::getactionid() const { return m_actionId; }

    const int& FMTevent::getperiod() const { return m_period; }

    const size_t& FMTevent::getGraphFamily() const { return m_centroidGraphFamily; }

    const std::set<FMTcoordinate>& FMTevent::getElements() const
    {
        return m_elements;
    }


    void FMTevent::setactionid(const int& laction_id) { m_actionId = laction_id; }




	bool FMTevent::operator == (const FMTevent& rhs) const
    {
        FMTcoordinate centroid = averagecentroid();
        FMTcoordinate rhscentroid = rhs.averagecentroid();
        if (m_period == rhs.m_period && 
            m_actionId == rhs.m_actionId && 
            centroid == rhscentroid)
            {
                //return true;
            if (size() == rhs.size() && getEnveloppe() == rhs.getEnveloppe())
                {
                for (std::set<FMTcoordinate>::const_iterator it = m_elements.begin(); it!= m_elements.end();it++)
                    {
                    if (rhs.m_elements.find(*it)==rhs.m_elements.end())
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
        //strict ordering
		 if (m_period < rhs.m_period)
         {
            return true;
         }

		 if (rhs.m_period < m_period)
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

		 if (m_actionId < rhs.m_actionId)
         {
			 return true;
         }

		 if (rhs.m_actionId < m_actionId)
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
    for (std::set<FMTcoordinate>::const_iterator it = m_elements.begin(); it != m_elements.end(); it++)
        {
        for (int id = 0; id < 4; ++id)
            {
            const FMTcoordinate neighbor= it->at(id);
            if (m_elements.find(neighbor)== m_elements.end())
                {
                ++total;
                }
            }

        }
    return total;
    }

	size_t FMTevent::hash() const
		{
		size_t hashs = boost::hash<Spatial::FMTcoordinate>()(*midposition());
		boost::hash_combine(hashs, m_actionId);
		boost::hash_combine(hashs, m_period);
		return hashs;
		}

	FMTeventrelation FMTevent::getrelation(const FMTevent& rhs) const
	{
		return FMTeventrelation(*this, rhs);
	}

	size_t FMTevent::height() const
    {
    return FMTcoordinate::getHeight(getEnveloppe());
    }

	size_t FMTevent::width() const
    {
     return FMTcoordinate::getWidth(getEnveloppe());
    }

	FMTcoordinate FMTevent::averagecentroid() const
    {
    return FMTcoordinate::getAverageCentroid(getEnveloppe());
    }

    std::string FMTevent::getstatsheader()
    {
        return "Size Perimeter Height Width";
    }


	std::string FMTevent::getstats() const
    {
    return (std::to_string(size()) +" "+
        std::to_string(perimeter()) +" "+
        std::to_string(height()) +" "+
        std::to_string(width()));
    }

	void FMTevent::erase(const FMTcoordinate& newlocation)
    {
    if (m_elements.find(newlocation)!= m_elements.end())
        {
        m_elements.erase(newlocation);
        }
    }

     void FMTevent::merge(const FMTevent& event)
     {
         m_elements.insert(event.m_elements.begin(),event.m_elements.end());
     }

	void FMTevent::insert(const FMTcoordinate& newlocation)
        {
        m_elements.insert(Spatial::FMTcoordinate(newlocation));
        }

	std::vector<std::set<FMTcoordinate>::const_iterator> FMTevent::ignit(const size_t& eventmaximalsize,
		const std::set<FMTcoordinate>::const_iterator& ignit,
        int p_actionId, int p_period, size_t p_centroidGraphFamily)
        {
        //add set period and set action id
		std::vector<std::set<FMTcoordinate>::const_iterator>actives;
        if ((1 <= eventmaximalsize))
            {
            //ignition = ignit;
            actives.push_back(ignit);
            m_actionId = p_actionId;
            m_period = p_period;
            m_centroidGraphFamily = p_centroidGraphFamily;
            //return true;
            }
        //return false;
		return actives;
        }

	bool FMTevent::spread(const size_t& eventminimalsize, const size_t& eventmaximalsize,
		const size_t& neighboringsize, const std::set<FMTcoordinate>& territory, std::vector<std::set<FMTcoordinate>::const_iterator> active)
        {
            while((m_elements.size() < eventmaximalsize) && (!active.empty()))
                {
                std::vector<std::set<FMTcoordinate>::const_iterator>::iterator coordit;
                for(size_t id = 0; id < neighboringsize; ++id)
                    {
					coordit = active.begin();
                    const FMTcoordinate spread_coord = (*coordit)->at(static_cast<int>(id));
					const std::set<FMTcoordinate>::const_iterator spreadit = territory.find(spread_coord);
                    if(spreadit != territory.end() && m_elements.find(spread_coord) == m_elements.end())
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
            if (m_elements.size()>= eventminimalsize)
                {
                return true;
                }
            active.clear();
            //active.push_back(this->ignition);
            m_elements.clear();
            return false;
        }

	std::vector<std::set<FMTcoordinate>::const_iterator>FMTevent::getborders() const
	{
		std::vector<std::set<FMTcoordinate>::const_iterator>borders;
		for (std::set<FMTcoordinate>::const_iterator elemit = m_elements.begin(); elemit != m_elements.end(); elemit++)
		{
			for (int id = 0; id < 4; ++id)
			{
				const FMTcoordinate neighbor = elemit->at(id);
				if (m_elements.find(neighbor) == m_elements.end())//Ok its borderelement
				{
					borders.push_back(elemit);
					break;
				}
			}
		}
		return borders;
	}

    std::array<FMTcoordinate, 4>FMTevent::getEnveloppe() const
		{
            
        std::array<FMTcoordinate, 4>enveloppe{*m_elements.begin(),*m_elements.begin(),
                                            *m_elements.begin(),*m_elements.begin()};
            
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
    
    template<typename T>
	bool FMTevent::within(const T& dist, const FMTevent& rhs) const
	{
        if(dist<0)
        {
            //raise
        }
		const std::set<FMTcoordinate>::const_iterator center = midposition();
		const std::set<FMTcoordinate>::const_iterator rhscenter = rhs.midposition();
		if (center->within<T>(dist, *rhscenter))
		{
			return true;
		}else
			{
			for (std::set<FMTcoordinate>::const_iterator coord = m_elements.begin();coord!= m_elements.end();++coord)
			{
				for (std::set<FMTcoordinate>::const_iterator rhscoord = rhs.m_elements.begin(); rhscoord != rhs.m_elements.end(); ++rhscoord)
				{
					if (coord->within<T>(dist, *rhscoord))
					{
						return true;
					}
				}
			}
			}
			return false;
		}
    template bool FMTevent::within<size_t>(const size_t& dist, const FMTevent& rhs) const;
    template bool FMTevent::within<unsigned int>(const unsigned int& dist, const FMTevent& rhs) const;
    template bool FMTevent::within<double>(const double& dist, const FMTevent& rhs) const;
    template bool FMTevent::within<uint16_t>(const uint16_t& dist, const FMTevent& rhs) const;

    template<typename T>
    bool FMTevent::within(const T& dist, const FMTcoordinate& location) const
    {
        if(dist<0)
        {
            //raise
        }
        const std::set<FMTcoordinate>::const_iterator center = midposition();
        if(center->within(dist,location))
            {
            return true;
        }else{
            for (std::set<FMTcoordinate>::const_iterator elemit = m_elements.begin(); elemit != m_elements.end(); elemit++)
                {
                if (elemit->within(dist, location))
                    {
                    return true;
                    }
                }
            }
        return false;
    }
    template bool FMTevent::within<unsigned int>(const unsigned int& dist, const FMTcoordinate& location) const;
    template bool FMTevent::within<double>(const double& dist, const FMTcoordinate& location) const;
    template bool FMTevent::within<uint16_t>(const uint16_t& dist, const FMTcoordinate& location) const;

    bool FMTevent::contain(const FMTcoordinate& coord)const
    {
        return (m_elements.find(coord) != m_elements.end());
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
            if (m_elements.find(icoord)!= m_elements.end())
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
        const int casetype = static_cast<int>(std::distance(firstit,upit));
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
        return FMTcoordinate::getTerritory(getEnveloppe(), distance);
	}

    bool FMTevent::splitevent(std::vector<FMTevent>& splittedevents) const
    {
        std::queue<FMTcoordinate> active;
        std::set<FMTcoordinate> coordinates = m_elements;
        active.push(*coordinates.begin());
        boost::unordered_set<FMTcoordinate>ActivesSeen;
        while(!coordinates.empty())
        {
            FMTevent newevent(active.front(), m_actionId,
                m_period,m_centroidGraphFamily);
            while(!active.empty())
            {
                //Kind of a spread to create new event
                const FMTcoordinate& COORDINATE = active.front();
                coordinates.erase(COORDINATE);
                newevent.insert(COORDINATE);
                if (!coordinates.empty())
                {
                    for (unsigned int i = 0; i < 8u; ++i)
                    {
                        const FMTcoordinate NEIGHBOR = COORDINATE.at(i);
                        if (ActivesSeen.find(NEIGHBOR) == ActivesSeen.end() &&
                            coordinates.find(NEIGHBOR) != coordinates.end())
                        {
                            ActivesSeen.insert(NEIGHBOR);
                            active.push(NEIGHBOR);
                        }

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

