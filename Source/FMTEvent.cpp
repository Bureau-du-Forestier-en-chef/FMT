/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTEvent.h"
#include "FMTEventRelation.h"
#include <boost/unordered_set.hpp>
#include <queue>

namespace Spatial
{

	FMTEvent::FMTEvent(const FMTCoordinate& p_location,
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


    std::set<FMTCoordinate>::const_iterator FMTEvent::midPosition() const
    {
        std::set<FMTCoordinate>::const_iterator it = m_elements.begin();
        if (m_elements.size() > 1)
        {
            const size_t midlocation = (m_elements.size() / 2);
            std::advance(it, midlocation);
        }
        return it;
    }

    bool FMTEvent::empty() const { return m_elements.empty(); }


    size_t FMTEvent::size() const { return m_elements.size(); }


    const int& FMTEvent::getActionId() const { return m_actionId; }

    const int& FMTEvent::getPeriod() const { return m_period; }

    const size_t& FMTEvent::getGraphFamily() const { return m_centroidGraphFamily; }

    const std::set<FMTCoordinate>& FMTEvent::getElements() const
    {
        return m_elements;
    }


    void FMTEvent::setActionId(const int& laction_id) { m_actionId = laction_id; }




	bool FMTEvent::operator == (const FMTEvent& rhs) const
    {
        FMTCoordinate centroid = averageCentroid();
        FMTCoordinate rhscentroid = rhs.averageCentroid();
        if (m_period == rhs.m_period && 
            m_actionId == rhs.m_actionId && 
            centroid == rhscentroid)
            {
                //return true;
            if (size() == rhs.size() && getEnveloppe() == rhs.getEnveloppe())
                {
                for (std::set<FMTCoordinate>::const_iterator it = m_elements.begin(); it!= m_elements.end();it++)
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

	bool FMTEvent::operator != (const FMTEvent& rhs) const
    {
    return (!(*this==rhs));
    }

    bool FMTEvent::operator<(const FMTEvent& rhs) const
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
		 const std::set<FMTCoordinate>::const_iterator centroidit = midPosition();
		 const std::set<FMTCoordinate>::const_iterator rhscentroidit = rhs.midPosition();
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
	size_t FMTEvent::perimeter() const //gives perimeter
    {
    size_t total = 0;
    for (std::set<FMTCoordinate>::const_iterator it = m_elements.begin(); it != m_elements.end(); it++)
        {
        for (int id = 0; id < 4; ++id)
            {
            const FMTCoordinate neighbor= it->at(id);
            if (m_elements.find(neighbor)== m_elements.end())
                {
                ++total;
                }
            }

        }
    return total;
    }

	size_t FMTEvent::hash() const
		{
		size_t hashs = boost::hash<Spatial::FMTCoordinate>()(*midPosition());
		boost::hash_combine(hashs, m_actionId);
		boost::hash_combine(hashs, m_period);
		return hashs;
		}

	FMTEventRelation FMTEvent::getRelation(const FMTEvent& rhs) const
	{
		return FMTEventRelation(*this, rhs);
	}

	size_t FMTEvent::height() const
    {
    return FMTCoordinate::getHeight(getEnveloppe());
    }

	size_t FMTEvent::width() const
    {
     return FMTCoordinate::getWidth(getEnveloppe());
    }

	FMTCoordinate FMTEvent::averageCentroid() const
    {
    return FMTCoordinate::getAverageCentroid(getEnveloppe());
    }

    std::string FMTEvent::getStatsHeader()
    {
        return "Size Perimeter Height Width";
    }


	std::string FMTEvent::getStats() const
    {
    return (std::to_string(size()) +" "+
        std::to_string(perimeter()) +" "+
        std::to_string(height()) +" "+
        std::to_string(width()));
    }

	void FMTEvent::erase(const FMTCoordinate& newlocation)
    {
    if (m_elements.find(newlocation)!= m_elements.end())
        {
        m_elements.erase(newlocation);
        }
    }

     void FMTEvent::merge(const FMTEvent& event)
     {
         m_elements.insert(event.m_elements.begin(),event.m_elements.end());
     }

	void FMTEvent::insert(const FMTCoordinate& newlocation)
        {
        m_elements.insert(Spatial::FMTCoordinate(newlocation));
        }

	std::vector<std::set<FMTCoordinate>::const_iterator> FMTEvent::ignit(const size_t& eventmaximalsize,
		const std::set<FMTCoordinate>::const_iterator& ignit,
        int p_actionId, int p_period, size_t p_centroidGraphFamily)
        {
        //add set period and set action id
		std::vector<std::set<FMTCoordinate>::const_iterator>actives;
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

	bool FMTEvent::spread(const size_t& eventminimalsize, const size_t& eventmaximalsize,
		const size_t& neighboringsize, const std::set<FMTCoordinate>& territory, std::vector<std::set<FMTCoordinate>::const_iterator> active)
        {
            while((m_elements.size() < eventmaximalsize) && (!active.empty()))
                {
                std::vector<std::set<FMTCoordinate>::const_iterator>::iterator coordit;
                for(size_t id = 0; id < neighboringsize; ++id)
                    {
					coordit = active.begin();
                    const FMTCoordinate spread_coord = (*coordit)->at(static_cast<int>(id));
					const std::set<FMTCoordinate>::const_iterator spreadit = territory.find(spread_coord);
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


    std::vector<std::pair<FMTCoordinate, FMTCoordinate>>FMTEvent::getOutsideBordersPair() const
    {
        std::vector<std::pair<FMTCoordinate, FMTCoordinate>>Borders;
        std::set<FMTCoordinate>OutsideCoordinates;
        for (const auto& IN_COORD : m_elements)
            {
            for (int i = 0; i < 4; ++i)
                {
                const FMTCoordinate LOOK_UP = IN_COORD.at(i);
                if (m_elements.find(LOOK_UP)==m_elements.end()&&
                    OutsideCoordinates.insert(LOOK_UP).second)
                    {
                    Borders.push_back(
                        std::pair<FMTCoordinate, FMTCoordinate>(IN_COORD, LOOK_UP));

                    }
                }
            }
        return Borders;
       }

	std::vector<std::set<FMTCoordinate>::const_iterator>FMTEvent::getBorders() const
	{
		std::vector<std::set<FMTCoordinate>::const_iterator>borders;
		for (std::set<FMTCoordinate>::const_iterator elemit = m_elements.begin(); elemit != m_elements.end(); elemit++)
		{
			for (int id = 0; id < 4; ++id)
			{
				const FMTCoordinate neighbor = elemit->at(id);
				if (m_elements.find(neighbor) == m_elements.end())//Ok its borderelement
				{
					borders.push_back(elemit);
					break;
				}
			}
		}
		return borders;
	}

    std::array<FMTCoordinate, 4>FMTEvent::getEnveloppe() const
		{
            
        std::array<FMTCoordinate, 4>enveloppe{*m_elements.begin(),*m_elements.begin(),
                                            *m_elements.begin(),*m_elements.begin()};
            
		for (std::set<FMTCoordinate>::const_iterator border : getBorders())
			{
			border->upEnveloppe(enveloppe);
			}
		return enveloppe;
		}


	void FMTEvent::getClosesCoordinates(const FMTEvent& rhs,
		std::set<FMTCoordinate>::const_iterator& thiscoordinate,
		std::set<FMTCoordinate>::const_iterator& rhscoordinate) const
	{
		const std::vector<std::set<FMTCoordinate>::const_iterator>rhsborders = rhs.getBorders();
		double bestapproximation = std::numeric_limits<double>::infinity();
		for (const std::set<FMTCoordinate>::const_iterator& coord : getBorders())
		{
			double approx = std::numeric_limits<double>::infinity();
			std::set<FMTCoordinate>::const_iterator rhscoord = coord->closest(rhsborders, approx);
			if (approx < bestapproximation)
			{
				bestapproximation = approx;
				thiscoordinate = coord;
				rhscoordinate = rhscoord;
			}
		}
	}


    double FMTEvent::distance(const FMTEvent& rhs) const
		{
		std::set<FMTCoordinate>::const_iterator thiscoordinate;
		std::set<FMTCoordinate>::const_iterator rhscoordinate;
		getClosesCoordinates(rhs, thiscoordinate, rhscoordinate);
		return thiscoordinate->distance(*rhscoordinate);
		}
    
    template<typename T>
	bool FMTEvent::within(const T& dist, const FMTEvent& rhs) const
	{
        if(dist<0)
        {
            //raise
        }
		const std::set<FMTCoordinate>::const_iterator center = midPosition();
		const std::set<FMTCoordinate>::const_iterator rhscenter = rhs.midPosition();
		if (center->within<T>(dist, *rhscenter))
		{
			return true;
		}else
			{
			for (std::set<FMTCoordinate>::const_iterator coord = m_elements.begin();coord!= m_elements.end();++coord)
			{
				for (std::set<FMTCoordinate>::const_iterator rhscoord = rhs.m_elements.begin(); rhscoord != rhs.m_elements.end(); ++rhscoord)
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
    template bool FMTEvent::within<size_t>(const size_t& dist, const FMTEvent& rhs) const;
    template bool FMTEvent::within<unsigned int>(const unsigned int& dist, const FMTEvent& rhs) const;
    template bool FMTEvent::within<double>(const double& dist, const FMTEvent& rhs) const;
    template bool FMTEvent::within<uint16_t>(const uint16_t& dist, const FMTEvent& rhs) const;

    template<typename T>
    bool FMTEvent::within(const T& dist, const FMTCoordinate& location) const
    {
        if(dist<0)
        {
            //raise
        }
        const std::set<FMTCoordinate>::const_iterator center = midPosition();
        if(center->within(dist,location))
            {
            return true;
        }else{
            for (std::set<FMTCoordinate>::const_iterator elemit = m_elements.begin(); elemit != m_elements.end(); elemit++)
                {
                if (elemit->within(dist, location))
                    {
                    return true;
                    }
                }
            }
        return false;
    }
    template bool FMTEvent::within<unsigned int>(const unsigned int& dist, const FMTCoordinate& location) const;
    template bool FMTEvent::within<double>(const double& dist, const FMTCoordinate& location) const;
    template bool FMTEvent::within<uint16_t>(const uint16_t& dist, const FMTCoordinate& location) const;

    bool FMTEvent::contain(const FMTCoordinate& coord)const
    {
        return (m_elements.find(coord) != m_elements.end());
    }


    bool FMTEvent::willSplitEvent(const FMTCoordinate& p_coordinate) const
        {
        bool returned = false;
        if (size()>2)
            {
            FMTEvent subEvent(*this);
            subEvent.erase(p_coordinate);
            std::queue<FMTCoordinate>activeCoordinates;
            activeCoordinates.push(*subEvent.m_elements.begin());
            subEvent.erase(*subEvent.m_elements.begin());
            while (!subEvent.m_elements.empty() &&
                !activeCoordinates.empty())
                {
                for (int i = 0; i < 8; ++i)
                    {
                    FMTCoordinate iCoord = activeCoordinates.front().at(i);
                    if (subEvent.contain(iCoord))
                        {
                        activeCoordinates.push(iCoord);
                        subEvent.erase(iCoord);
                        }
                    }
                    activeCoordinates.pop();
                }
            returned = !subEvent.m_elements.empty();
            }
        return returned;
        }

	std::set<FMTCoordinate>FMTEvent::getTerritory(const size_t& distance) const
	{
        return FMTCoordinate::getTerritory(getEnveloppe(), distance);
	}

    bool FMTEvent::splitEvent(std::vector<FMTEvent>& splittedevents) const
    {
        std::queue<FMTCoordinate> active;
        std::set<FMTCoordinate> coordinates = m_elements;
        active.push(*coordinates.begin());
        boost::unordered_set<FMTCoordinate>ActivesSeen;
        while(!coordinates.empty())
        {
            FMTEvent newevent(active.front(), m_actionId,
                m_period,m_centroidGraphFamily);
            while(!active.empty())
            {
                //Kind of a spread to create new event
                const FMTCoordinate& COORDINATE = active.front();
                coordinates.erase(COORDINATE);
                newevent.insert(COORDINATE);
                if (!coordinates.empty())
                {
                    for (unsigned int i = 0; i < 8u; ++i)
                    {
                        const FMTCoordinate NEIGHBOR = COORDINATE.at(i);
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
            for (std::set<FMTCoordinate>::const_iterator ait=coordinates.begin();ait!=coordinates.end();++ait)
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

