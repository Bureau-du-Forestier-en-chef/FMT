/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTEventContainer.h"
#include "FMTEventRelation.h"
#include "FMTExceptionHandler.h"
#include "FMTBindingSpatialAction.h"
#include "FMTSpatialGraphs.h"

#include <limits>
#include <queue>

namespace Spatial
{
   


    bool FMTEventContainer::operator==(const FMTEventContainer& rhs) const
    {
        return m_events==rhs.m_events;
    }

    void FMTEventContainer::swap(FMTEventContainer& rhs)
    {
		m_events.swap(rhs.m_events);
    }

	FMTEventContainer::const_iterator FMTEventContainer::find(const int & period, const FMTCoordinate & coord) const
	{
		std::vector<FMTEventContainer::const_iterator> eventsfound = getEvents(period, coord);
		if (eventsfound.size() > 1)
		{
			return eventsfound.at(0);
		}
		else
		{
			return m_events.cend();
		}
	}

	void FMTEventContainer::merge(const FMTEventContainer & levents)
	{
		m_events.insert(levents.begin(), levents.end());
	}

	void FMTEventContainer::erase(const FMTEvent& event)
    {
		m_events.erase(event);
    }

	void FMTEventContainer::clear()
	{
		m_events.clear();
	}

    FMTEventContainer::const_iterator FMTEventContainer::erase(const FMTEventContainer::const_iterator event)
    {
        return m_events.erase(event);
    }

    int FMTEventContainer::lastPeriod()const
    {
        return m_events.rbegin()->getPeriod();
    }

	void FMTEventContainer::insert(const FMTEvent& event)
	{
		m_events.insert(event);
	}

	FMTEventContainer::const_iterator FMTEventContainer::begin() const { return m_events.cbegin(); }

	FMTEventContainer::const_iterator FMTEventContainer::end() const { return m_events.cend(); }

	bool FMTEventContainer::empty() const { return m_events.empty(); }

	size_t FMTEventContainer::size() const { return m_events.size(); }

	FMTEventContainer::const_iterator FMTEventContainer::find(const FMTEvent& event) const { return m_events.find(event); }

    int FMTEventContainer::firstPeriod()const
    {
        return m_events.begin()->getPeriod();
    }

	void FMTEventContainer::erase(const FMTCoordinate& coord,const std::vector<FMTEventContainer::iterator>& iterators)
	{
		std::queue<FMTEvent>emodif;
		std::vector<FMTEventContainer::iterator>::const_iterator lowerb = iterators.begin();
		std::vector<FMTEventContainer::iterator>::const_iterator upperb = iterators.end();
		while (lowerb != upperb)
		{
			if ((*lowerb)->contain(coord))
			{
				if ((*lowerb)->size() > 1)
				{
					//add event to modify to queue
					emodif.push(**lowerb);
				}
				//remove event from container
				//lowerb = erase(**lowerb);
				erase(**lowerb);
			}
			//else { ++lowerb; }
			++lowerb;
		}
		while (!emodif.empty())
		{
			FMTEvent event = emodif.front();
			emodif.pop();
			if (!event.willSplitEvent(coord))
			{
				event.erase(coord);
				if (!event.empty())
				{
					insert(event);
				}
			}
			else
			{
				event.erase(coord);
				std::vector<FMTEvent> splittedevents;
				if (event.splitEvent(splittedevents))
				{
					for (auto e : splittedevents)
					{
						if (!e.empty())
						{
							insert(e);
						}

					}
				}
				else
				{
					if (!event.empty())
					{
						insert(event);
					}
				}
			}
		}
	}

	void FMTEventContainer::eraseCoordinate(const FMTCoordinate& coord, const int& periodStart, const std::vector<std::vector<bool>>& actionstarget)
		{
		const int lastPeriod = static_cast<int>(actionstarget.size()) + periodStart;
		size_t periodid = 0;
		for (int period = periodStart; period < lastPeriod; ++period)
			{
			if (!actionstarget.at(periodid).empty())
				{
				const std::vector<FMTEventContainer::const_iterator> iterators = getEvents(period, actionstarget.at(periodid));
				if (!iterators.empty())
					{
					erase(coord, iterators);
					}

				}
			++periodid;
			}
		}

	/*void FMTEventContainer::eraseCoordinate (const FMTCoordinate& coord, const int& periodStart)
    {
        const int lperiod=lastPeriod();
        for (int period=periodStart;period<=lperiod;++period)
        {
            const FMTEventContainer::const_iterator upper = upperBound(period);
            FMTEventContainer::const_iterator lower = lowerBound(period);
            std::queue<FMTEvent> emodif;
            while (lower != upper)
            {
                if (lower->contain(coord))
                {
                    if(lower->size()>1)
                    {
                        //add event to modify to queue
                        emodif.push(*lower);
                    }
                    //remove event from container
                    lower = erase(lower);
                }else{++lower;}
            }
            while (!emodif.empty())
            {
                FMTEvent event = emodif.front();
                emodif.pop();
                if (!event.potentialysplitevent(coord))
                {
                    event.erase(coord);
                    if (!event.empty())
                    {
                        insert(event);
                    }
                }
                else
                {
                    event.erase(coord);
                    std::vector<FMTEvent> splittedevents;
                    if (event.splitEvent(1,splittedevents))
                    {
                        for (auto e : splittedevents)
                        {
                            if (!e.empty())
                            {
                                insert(e);
                            }

                        }
                    }else
                    {
                        if(!event.empty())
                        {
                            insert(event);
                        }
                    }
                }
            }
        }
    }*/

    std::pair<FMTEventContainer::const_iterator,FMTEventContainer::const_iterator> FMTEventContainer::getBounds(const int& period) const
    {
        return std::pair<FMTEventContainer::const_iterator,FMTEventContainer::const_iterator>(lowerBound(period),upperBound(period));
    }



	void FMTEventContainer::pushAction(
		const std::vector<FMTEventContainer::const_iterator>& iterators,
		const FMTCoordinate& coord, const int& period, 
		const int& actionid,size_t neighborsize, size_t p_GraphFamily)
	{
		try
		{
			std::vector<FMTEventContainer::const_iterator> aroundevents;
			aroundevents.reserve(8);
			if(neighborsize>0 && !iterators.empty())
			{
				aroundevents = getAroundEvents(iterators,coord,neighborsize);
			}
			if (aroundevents.empty())
			{
				FMTEvent newevent(coord, actionid, period, p_GraphFamily);
				//newevent.insert(coord);
				insert(newevent);
			}
			else
			{
				FMTEvent combinedevents(coord, actionid, period, p_GraphFamily);
				//combinedevents.insert(coord);
				for (FMTEventContainer::const_iterator e : aroundevents)
				{
					combinedevents.merge(*e);
					erase(*e);
				}
				insert(combinedevents);
			}
		}catch(...){
			_exhandler->raiseFromCatch("","FMTEventContainer::pushAction", __LINE__, __FILE__);
		}
	}

	std::vector<FMTEventContainer::const_iterator> FMTEventContainer::getAroundEvents(const std::vector<FMTEventContainer::const_iterator>& iterators,
		const FMTCoordinate& coord, const size_t& neighborsize) const
	{
		std::vector<FMTEventContainer::const_iterator>aroundevents;
		aroundevents.reserve(8);
		try{
				const FMTCoordinate ndistcalculator = FMTCoordinate(neighborsize+1,neighborsize+1);
				const double distneighbors = ndistcalculator.at(neighborsize).distance(ndistcalculator);
				for (FMTEventContainer::const_iterator eventit : iterators)
				{
					if (eventit->within(distneighbors, coord))
					{
						aroundevents.push_back(eventit);
					}
				}
		}catch(...){
			_exhandler->raiseFromCatch("","FMTEventContainer::getAroundEvents", __LINE__, __FILE__);
		}
		return aroundevents;
	}

	FMTEventContainer::FMTEventIteratorSorter::FMTEventIteratorSorter(const FMTCoordinate& coordinate) : basecoordinate(coordinate)
		{

		}
	bool FMTEventContainer::FMTEventIteratorSorter::operator() (const FMTEventContainer::const_iterator& eventit1, const FMTEventContainer::const_iterator& eventit2) const
		{
		double approx1 = 0;
		basecoordinate.closest(eventit1->getBorders(), approx1);
		double approx2 = 0;
		basecoordinate.closest(eventit2->getBorders(), approx1);
		return approx1 < approx2;
		}

	void FMTEventContainer::nthElements(std::vector<FMTEventContainer::const_iterator>& iterators, const FMTCoordinate& coord, const size_t& nelement) const
		{
		std::nth_element(iterators.begin(),iterators.begin()+ nelement, iterators.end(), FMTEventIteratorSorter(coord));
		//std::sort(iterators.begin(), iterators.end(), FMTEventIteratorSorter(coord));
		}


	void FMTEventContainer::addAction (const FMTCoordinate& coord, const int& period,
		const int& actionid, const FMTBindingSpatialAction& binding,size_t p_GraphFamily)
		{
		// If maxsize is numeric limits, it means that there is no evaluation for maxsize and neighborsize
		//So we push the action as an event for each cell.
		uint16_t maxsizeof = 0;
		if(binding.getMaximalSize()<std::numeric_limits<size_t>::max())
		{
			maxsizeof = static_cast<uint16_t>(binding.getMaximalSize());
		}
		const uint16_t minx = coord.getX() > maxsizeof ? coord.getX() - maxsizeof : 0;
		const uint16_t miny = coord.getY() > maxsizeof ? coord.getY() - maxsizeof : 0;
		const FMTCoordinate lower(minx, miny);
		const FMTCoordinate upper(coord.getX()+ maxsizeof, coord.getY() + maxsizeof);
		pushAction(getEvents(period, actionid, lower, upper),
			coord, period, actionid, binding.getMinimalNeighborSize(), p_GraphFamily);
		}

	void FMTEventContainer::addActions(const FMTCoordinate& coord, const int& period,
		const std::vector<int>& actionids, const std::vector<FMTBindingSpatialAction>& bindings,
		size_t p_GraphFamily)
		{
		uint16_t maxsizeof = 0;
		for (const int& id : actionids)
		{
			const FMTBindingSpatialAction& binding = bindings.at(id);
			if(binding.getMaximalSize() < std::numeric_limits<size_t>::max())
			{
				const uint16_t msize = static_cast<uint16_t>(binding.getMaximalSize());
				maxsizeof = msize > maxsizeof ? msize : maxsizeof;
			}
		}
		const uint16_t minx = coord.getX() > maxsizeof ? coord.getX() - maxsizeof : 0;
		const uint16_t miny = coord.getY() > maxsizeof ? coord.getY() - maxsizeof : 0;
		const FMTCoordinate lower(minx, miny);
		const FMTCoordinate upper(coord.getX() + maxsizeof, coord.getY() + maxsizeof);
		size_t id = 0;
		for (const std::vector<FMTEventContainer::const_iterator>& eventits : getMultipleEvents(period, actionids, lower, upper))
			{
			const int& actid = actionids.at(id);
			pushAction(eventits, coord, period, actid, bindings.at(actid).getMinimalNeighborSize(), p_GraphFamily);
			++id;
			}
		}

    std::vector<FMTEventContainer::const_iterator> FMTEventContainer::getEvents(const int& period,
                                                                                    const int& action_id) const
    {
        FMTEventContainer::const_iterator upper = upperBound(period);
        FMTEventContainer::const_iterator lower = lowerBound(period);
        std::vector<FMTEventContainer::const_iterator> selectedevents;
        for  (FMTEventContainer::const_iterator it = lower ; it != upper && it!= m_events.end() ; ++it)
        {
            if (it->getActionId()==action_id /*&& it->getPeriod()==period*/)
            {
                selectedevents.push_back(it);
            }
        }
        return selectedevents;
    }

	FMTEventContainer FMTEventContainer::getEventsToAdd(const FMTCoordinate& coord, const int& period, const int& actionid,
		const FMTBindingSpatialAction& binding, FMTEventContainer& newevents, size_t p_GraphFamily) const
	{
		uint16_t maxsizeof = 0;
		if(binding.getMaximalSize()<std::numeric_limits<size_t>::max())
		{
			maxsizeof = static_cast<uint16_t>(binding.getMaximalSize());
		}
		const uint16_t minx = coord.getX() > maxsizeof ? coord.getX() - maxsizeof : 0;
		const uint16_t miny = coord.getY() > maxsizeof ? coord.getY() - maxsizeof : 0;
		const FMTCoordinate lower(minx, miny);
		const FMTCoordinate upper(coord.getX()+ maxsizeof, coord.getY() + maxsizeof);
		const std::vector<FMTEventContainer::const_iterator> eventits = getEvents(period, actionid, lower, upper);
		std::vector<FMTEventContainer::const_iterator> aroundevents;
		const size_t neighborsize = binding.getMaximalNeighborSize();
		if(neighborsize>0 && !eventits.empty())
		{
			aroundevents = getAroundEvents(eventits,coord,neighborsize);
		}
		//std::vector<FMTEventContainer::const_iterator>tocalculate;
		FMTEventContainer tocalculate;
		if (aroundevents.empty())
		{
			FMTEvent newevent(coord, actionid, period, p_GraphFamily);
			//newevent.insert(coord);
			newevents.insert(newevent);
		}
		else
		{
			FMTEvent combinedevents(coord, actionid, period, p_GraphFamily);
			//combinedevents.insert(coord);
			for (FMTEventContainer::const_iterator e : aroundevents)
			{
				combinedevents.merge(*e);
				//tocalculate.push_back(e);
				tocalculate.insert(*e);
				//erase(*e);
			}
			newevents.insert(combinedevents);
		}
	return tocalculate;
	}


	FMTEventContainer FMTEventContainer::addUpdate(const FMTEventContainer& newevents, const FMTEventContainer& eventstoremove)
	{
		for (FMTEventContainer::const_iterator it = eventstoremove.m_events.begin(); it != eventstoremove.m_events.end(); ++it)
			{
			m_events.erase(*it);
			}
		FMTEventContainer newlyadded;
		for (FMTEventContainer::const_iterator it = newevents.m_events.begin(); it != newevents.m_events.end(); ++it)
			{
			newlyadded.insert(*m_events.insert(*it).first);
			}
		return newlyadded;
	}	

	FMTEventContainer FMTEventContainer::getContainer(std::vector<FMTCoordinate> coordinates,const int& minperiod,const int& maxperiod, const size_t& buffer) const
	{
		FMTEventContainer newcontainer;
		if (!coordinates.empty())
			{
			const uint16_t bufferof = static_cast<uint16_t>(buffer);
			std::sort(coordinates.begin(), coordinates.end());
			const uint16_t minx = coordinates.begin()->getX() > bufferof ? coordinates.begin()->getX() - bufferof : 0;
			const uint16_t miny = coordinates.begin()->getY() > bufferof ? coordinates.begin()->getY() - bufferof : 0;
			const FMTCoordinate minimalcoord(minx, miny);
			const FMTCoordinate maximalcoord(coordinates.back().getX() + bufferof, coordinates.back().getY() + bufferof);
			for (int period = minperiod; period<=maxperiod ; ++period)
				{
				FMTEventContainer::const_iterator lower = lowerBound(period, minimalcoord);
				FMTEventContainer::const_iterator upper = upperBound(period, maximalcoord);
				for (FMTEventContainer::const_iterator it = lower; it != upper; ++it)
					{
					newcontainer.insert(*it);
					}

				}
			}
		return newcontainer;
	}

	FMTEventContainer FMTEventContainer::getEventsToErase(const int& fromperiod, const std::vector<std::vector<bool>>& actionstarget,
																				const FMTCoordinate& coord, const size_t& buffer, FMTEventContainer& newevents) const
	{
		const uint16_t bufferof = static_cast<uint16_t>(buffer);
		const uint16_t minx = coord.getX() > bufferof ? coord.getX() - bufferof : 0;
		const uint16_t miny = coord.getY() > bufferof ? coord.getY() - bufferof : 0;
		const FMTCoordinate minimalcoord(minx, miny);
		const FMTCoordinate maximalcoord(coord.getX()+ bufferof,coord.getY()+ bufferof);
		const int lastPeriod = static_cast<int>(actionstarget.size()) + fromperiod;
		size_t periodid = 0;
		FMTEventContainer erased;
		for (int period = fromperiod; period < lastPeriod ; ++period)
			{
			FMTEventContainer::const_iterator lower = lowerBound(period, minimalcoord);
			FMTEventContainer::const_iterator upper = upperBound(period, maximalcoord);
			if (!actionstarget.at(periodid).empty())
			{
			for (FMTEventContainer::const_iterator it = lower; it != upper; ++it)
				{
				if (actionstarget.at(periodid).at(it->getActionId()) && it->contain(coord))
					{
					std::queue<FMTEvent>emodif;
					if (it->size() > 1)
						{
						emodif.push(*it);
						}
					while (!emodif.empty())
					{
						FMTEvent event = emodif.front();
						emodif.pop();
						if (!event.willSplitEvent(coord))
						{
							event.erase(coord);
							if (!event.empty())
							{
								newevents.insert(event);
							}
						}
						else
						{
							event.erase(coord);
							std::vector<FMTEvent> splittedevents;
							if (event.splitEvent(splittedevents))
							{
								for (auto e : splittedevents)
								{
									if (!e.empty())
									{
										newevents.insert(e);
									}

								}
							}
							else
							{
								if (!event.empty())
								{
									newevents.insert(event);
								}
							}
						}
					}
					erased.insert(*it);
					}
				}
			}
			++periodid;
			}
		return erased;
	}



	std::vector<FMTEventContainer::const_iterator> FMTEventContainer::getEvents(const int& period, const std::vector<bool>& actions_used,
		const FMTCoordinate& minimalcoordinate, const FMTCoordinate& maximalcoordinate) const
	{
		FMTEventContainer::const_iterator lower = lowerBound(period, minimalcoordinate);
		FMTEventContainer::const_iterator upper = upperBound(period, maximalcoordinate);
		std::vector<FMTEventContainer::const_iterator> selectedevents;
		for (FMTEventContainer::const_iterator it = lower; it != upper; ++it)
		{
			if (actions_used.at(it->getActionId()) /*&& it->getPeriod() == period*/)
			{
				selectedevents.push_back(it);
			}
		}
		return selectedevents;

	}

	std::vector<FMTEventContainer::const_iterator> FMTEventContainer::getEvents(const int& period, const int& action_id,
		const FMTCoordinate& minimalcoordinate, const FMTCoordinate& maximalcoordinate) const
	{
		FMTEventContainer::const_iterator lower = lowerBound(period, minimalcoordinate);
		FMTEventContainer::const_iterator upper = upperBound(period, maximalcoordinate);
		std::vector<FMTEventContainer::const_iterator> selectedevents;
		for (FMTEventContainer::const_iterator it = lower; it != upper; ++it)
		{
			if (it->getActionId() == action_id /*&& it->getPeriod() == period*/)
			{
				selectedevents.push_back(it);
			}
		}
		return selectedevents;
	}

	std::vector<std::vector<FMTEventContainer::const_iterator>> FMTEventContainer::getMultipleEvents(const int& period, const std::vector<int>& action_ids,
		const FMTCoordinate& minimalcoordinate, const FMTCoordinate& maximalcoordinate) const
		{
		FMTEventContainer::const_iterator lower = lowerBound(period, minimalcoordinate);
		FMTEventContainer::const_iterator upper = upperBound(period, maximalcoordinate);
		std::vector<std::vector<FMTEventContainer::const_iterator>> selectedevents(action_ids.size(), std::vector<FMTEventContainer::const_iterator>());
		for (FMTEventContainer::const_iterator it = lower; it != upper; ++it)
			{
			for (size_t id = 0 ; id < action_ids.size();++id)
				{
				if (it->getActionId() == action_ids.at(id))
					{
					selectedevents.at(id).push_back(it);
					}
				}
			}
		return selectedevents;
		}


    std::vector<FMTEventContainer::const_iterator> FMTEventContainer::getEvents(const int& period,
                                                                                    const std::vector<int>& actions_id) const
    {
        FMTEventContainer::const_iterator upper = upperBound(period);
        FMTEventContainer::const_iterator lower = lowerBound(period);
        std::vector<FMTEventContainer::const_iterator> selectedevents;
        for  (FMTEventContainer::const_iterator it = lower ; it != upper ; ++it)
        {
            if (std::find(actions_id.begin(),actions_id.end(),it->getActionId())!=actions_id.end() && it->getPeriod()==period)
            {
                selectedevents.push_back(it);
            }
        }
        return selectedevents;
    }

	std::vector<FMTEventContainer::const_iterator> FMTEventContainer::getEvents(const int& period, const std::vector<bool>& actions_used) const
	{
		/*FMTEventContainer::const_iterator upper = upperBound(period);
		FMTEventContainer::const_iterator lower = lowerBound(period);
		std::vector<FMTEventContainer::const_iterator> selectedevents;
		for (FMTEventContainer::const_iterator it = lower; it != upper; ++it)
		{
			if (actions_used.at(it->getActionId()))
			{
				selectedevents.push_back(it);
			}
		}*/
		std::vector<FMTEventContainer::const_iterator> selectedevents;
		for (FMTEventContainer::const_iterator it = lowerBound(period); it != upperBound(period); ++it)
		{
			if (actions_used.at(it->getActionId()) && it->getPeriod()==period)
			{
				selectedevents.push_back(it);
			}
		}
		return selectedevents;
	}

    std::vector<FMTEventContainer::const_iterator> FMTEventContainer::getEvents(const int& period,
                                                                                    const FMTCoordinate& coord) const
    {
        FMTEventContainer::const_iterator upper = upperBound(period);
        FMTEventContainer::const_iterator lower = lowerBound(period);
        std::vector<FMTEventContainer::const_iterator> selectedevents;
        for  (FMTEventContainer::const_iterator it = lower ; it != upper ; ++it)
        {
            if (it->contain(coord))
            {
                selectedevents.push_back(it);
            }
        }
        return selectedevents;

    }

    std::vector<FMTEventContainer::const_iterator> FMTEventContainer::getEvents(const int& period,
                                                                                    const std::set<FMTCoordinate>& territory) const
    {
        std::vector<FMTEventContainer::const_iterator> selectedevents;
        if (!territory.empty())
        {
            const FMTCoordinate minxy = *territory.begin();
            const FMTCoordinate maxxy = *(territory.rbegin());
			
            FMTEventContainer::const_iterator lower = lowerBound(period,minxy);
            FMTEventContainer::const_iterator upper = upperBound(period,maxxy);
            ///Debug
            //std::cout<<"Upper "<<upper->averageCentroid().getX()<<upper->averageCentroid().getY()<<" Action : "<<upper->getActionId()<<" Period : "<<upper->getPeriod()<<std::endl;
            //std::cout<<"Lower "<<lower->averageCentroid().getX()<<lower->averageCentroid().getY()<<" Action : "<<lower->getActionId()<<" Period : "<<lower->getPeriod()<<std::endl;
            /*do {
                    if (lower!=events.end())
                    {
                        if(lower == upper)
                        {
                            if(
                               (upper->averageCentroid() == maxxy || upper->averageCentroid() < maxxy)&&
                               (upper->getPeriod() == period)
                               )
                            {
                                selectedevents.push_back(lower);
                            }
                        }
                        else{selectedevents.push_back(lower);}
                    }
                    if (lower!=upper)
                    {
                        lower++;
                    }
                }
            while(lower!=upper);*/
			for (FMTEventContainer::const_iterator it = lower; it != upper; ++it)
				{
				selectedevents.push_back(it);
				}
        }
        return selectedevents;
    }



    FMTEventContainer::const_iterator FMTEventContainer::upperBound(const int& period) const
    {
		constexpr uint16_t MAX_BOUND = std::numeric_limits<uint16_t>::max();
        FMTEventContainer::const_iterator upper = m_events.upper_bound(FMTEvent(FMTCoordinate(MAX_BOUND, MAX_BOUND),
                                                             INT_MAX,
                                                             period,0));
        return upper;
    }

    FMTEventContainer::const_iterator FMTEventContainer::upperBound(const int& period, const FMTCoordinate& maxxycoord) const
    {
        FMTEventContainer::const_iterator upper = m_events.upper_bound(FMTEvent(maxxycoord,
                                                                                  INT_MAX,
                                                                                  period,0));
        return upper;
    }

    FMTEventContainer::const_iterator FMTEventContainer::lowerBound(const int& period) const
    {
        FMTEventContainer::const_iterator lower = m_events.lower_bound(FMTEvent(FMTCoordinate(0,0),
                                                             0,
                                                             period,0));
        return lower;
    }

    FMTEventContainer::const_iterator FMTEventContainer::lowerBound(const int& period,const FMTCoordinate& minxycoord) const
    {
        FMTEventContainer::const_iterator lower = m_events.lower_bound(FMTEvent(minxycoord,
                                                             0,
                                                             period,0));
        return lower;
    }

    double FMTEventContainer::minimalDistance(const FMTEvent& event, const unsigned int& distancel,
                                                const int& period, const std::vector<int>& actionsid) const
    {
    double distancevalue = static_cast<double>(distancel) + 1.0;
    std::vector<FMTEventContainer::const_iterator> potentialneighbors = getEvents(period,actionsid);
    for (const auto eventit : potentialneighbors)
    {
        if (*eventit!=event)
        {
            if (eventit -> within(distancel,event))
            {
            const double value = eventit->distance(event);
            if (value < distancevalue)
                {
                distancevalue = value;
                }
            }

        }
    }
    return distancevalue;
    }

	double FMTEventContainer::evaluateDistance(const FMTEvent& eventof, 
											const double& lowerdistancetoevent,
											const double& upperdistancetoevent,
											const int& period, const std::vector<bool>& actionsused,
											boost::unordered_set<FMTEventRelation>& relations) const
	{
	double distancevalue = 0;
	const unsigned int lowerdistance = static_cast<unsigned int>(lowerdistancetoevent);
	const unsigned int upperdistance = static_cast<unsigned int>(upperdistancetoevent);
	const bool testlower = (lowerdistancetoevent == -std::numeric_limits<double>::infinity()) ? false : true;
	const bool testupper = (upperdistancetoevent == std::numeric_limits<double>::infinity()) ? false : true;
	for (FMTEventContainer::const_iterator eventit : getEvents(period, actionsused))
		{
			if (&(*eventit) != &eventof)//They will have the same address if it's the same event!
			{
			const FMTEventRelation straightrelation = eventof.getRelation(*eventit);
			const FMTEventRelation reverserelation = eventit->getRelation(eventof);
			if (relations.find(straightrelation)==relations.end() && 
				relations.find(reverserelation) == relations.end())
				{
				if (testlower && eventit->within(lowerdistance, eventof)) //too close
					{
					distancevalue += (lowerdistancetoevent - eventit->distance(eventof));
					}
				if (testupper && !eventit->within(upperdistance, eventof)) //too far
					{
					distancevalue += (eventit->distance(eventof) - upperdistancetoevent);
					}
				relations.insert(straightrelation);
				relations.insert(reverserelation);
				}

			}
		}
	return distancevalue;
	}


	FMTEventContainer::BoundingBox::BoundingBox():
			m_bottomLeft(std::numeric_limits< uint16_t>::max(),0),
			m_Width(0),
			m_Height(0),
			m_Top(),
			m_Bottom(),
			m_Left(),
			m_Right()

	{

	}

	double FMTEventContainer::evaluateSize(const std::vector<bool>& p_actions,
		int p_period, size_t  p_lowerBound, size_t p_upperBound, bool p_testLower) const
	{
		double value = 0.0;
		for (const FMTEventContainer::const_iterator& eventIt : getEvents(p_period, p_actions))
		{
			const size_t EVENT_SIZE = eventIt->size();
			size_t EventValue = 0;
			if (p_testLower && 
				EVENT_SIZE < p_lowerBound)
			{
				EventValue = p_lowerBound - EVENT_SIZE;
			}
			else if (!p_testLower && 
				EVENT_SIZE > p_upperBound)
			{
				EventValue = EVENT_SIZE - p_upperBound;
			}
			value += static_cast<double>(EventValue);
		}
		return value;
	}

	double FMTEventContainer::getDispersion(
		const std::vector<bool>& p_actions, 
		const FMTSpatialGraphs& p_Graphs,
		int p_themeId,
		int p_FirstPeriod, int p_LastPeriod, int p_bound) const
	{
		const std::vector<BoundingBox> BOXES = _getBoundingBoxes(p_actions, p_Graphs, 
									p_themeId, p_FirstPeriod, p_LastPeriod);
		double total = 0;
		for (const BoundingBox& BOX : BOXES)
			{
			total += BOX.evaluateUpperBound(p_bound);
			}
		return total;
	}

	double FMTEventContainer::getDispersion(int p_RuleId,
		const FMTSpatialGraphs& p_Graphs,
		int p_themeId, int p_FirstPeriod,
		int p_LastPeriod, int p_bound) const
	{
		const std::vector<BoundingBox> BOXES = _getBoundingBoxes(p_RuleId, p_Graphs,
			p_themeId, p_FirstPeriod, p_LastPeriod);
		double total = 0;
		for (const BoundingBox& BOX : BOXES)
		{
			total += BOX.evaluateUpperBound(p_bound);
		}
		return total;
	}



	std::vector<FMTEventContainer::const_iterator> FMTEventContainer::getDispersionConflicts(
		const std::vector<bool>& p_actions,
		const FMTSpatialGraphs& p_Graphs,
		int p_themeId,
		int p_FirstPeriod, int p_LastPeriod, int p_bound) const
	{
		const std::vector<BoundingBox> BOXES = _getBoundingBoxes(p_actions, p_Graphs, 
			p_themeId, p_FirstPeriod,p_LastPeriod);
		std::vector<FMTEventContainer::const_iterator> conflicts;
		for (const BoundingBox& BOX : BOXES)
			{
			const std::vector<FMTEventContainer::const_iterator> BOX_CONFLICTS = BOX.getEvents();
			if (BOX.evaluateUpperBound(p_bound)>FMT_DBL_TOLERANCE)
				{
				conflicts.insert(conflicts.end(),
					BOX_CONFLICTS.begin(), BOX_CONFLICTS.end());
				}
			}
		return  conflicts;
	}

	std::vector<FMTEventContainer::const_iterator> FMTEventContainer::getDispersionConflicts(int p_RuleId,
		const FMTSpatialGraphs& p_Graphs,
		int p_themeId, int p_FirstPeriod,
		int p_LastPeriod, int p_bound) const
	{
		const std::vector<BoundingBox> BOXES = _getBoundingBoxes(p_RuleId, p_Graphs,
			p_themeId, p_FirstPeriod, p_LastPeriod);
		std::vector<FMTEventContainer::const_iterator> conflicts;
		for (const BoundingBox& BOX : BOXES)
		{
			const std::vector<FMTEventContainer::const_iterator> BOX_CONFLICTS = BOX.getEvents();
			if (BOX.evaluateUpperBound(p_bound) > FMT_DBL_TOLERANCE)
			{
				conflicts.insert(conflicts.end(),
					BOX_CONFLICTS.begin(), BOX_CONFLICTS.end());
			}
		}
		return  conflicts;
	}


	std::vector<FMTEventContainer::const_iterator>FMTEventContainer::getEventsOf(int p_RuleId,
		int p_MinimalPeriod, int p_MaximalPeriod) const
		{
		std::vector<FMTEventContainer::const_iterator> RuleEvents;
		for (FMTEventContainer::const_iterator it = lowerBound(p_MinimalPeriod);
			it != upperBound(p_MaximalPeriod); ++it)
			{
			if (it->getActionId() == p_RuleId &&
				(p_MinimalPeriod <= it->getPeriod() <= p_MaximalPeriod))
				{
					RuleEvents.push_back(it);
				}
			}
		return RuleEvents;
		}



	std::vector<FMTEventContainer::BoundingBox> FMTEventContainer::_getBoundingBoxes(
		const std::vector<bool>& p_actions,
		const FMTSpatialGraphs& p_Graphs,
		int p_themeId,
		int p_FirstPeriod, int p_LastPeriod) const
	{
		size_t numberOfCategory = size_t(1);
		if (p_themeId>=0)
			{
			numberOfCategory = p_Graphs.getNumberOfCategories(p_themeId);
			}
		std::vector<BoundingBox>Boxes(numberOfCategory,BoundingBox());
		for (int period = p_FirstPeriod; period <= p_LastPeriod; ++period)
		{
			for (const auto& EVENT : getEvents(period, p_actions))
			{
				size_t category = 0;
				if (p_themeId >= 0)
					{
					category = p_Graphs.getCategoryOf(p_themeId,
						EVENT->getGraphFamily());

					}
				Boxes.at(category).add(EVENT);
			}
		}
		return Boxes;
	}

	std::vector<FMTEventContainer::BoundingBox> FMTEventContainer::_getBoundingBoxes(int p_RuleId,
		const FMTSpatialGraphs& p_Graphs, int p_themeId,
		int p_FirstPeriod, int p_LastPeriod) const
	{
		size_t numberOfCategory = size_t(1);
		if (p_themeId >= 0)
		{
			numberOfCategory = p_Graphs.getNumberOfCategories(p_themeId);
		}
		std::vector<BoundingBox>Boxes(numberOfCategory, BoundingBox());
		for (int period = p_FirstPeriod; period <= p_LastPeriod; ++period)
		{
			for (const auto& EVENT : getEvents(period, p_RuleId))
			{
				size_t category = 0;
				if (p_themeId >= 0)
				{
					category = p_Graphs.getCategoryOf(p_themeId,
						EVENT->getGraphFamily());

				}
				Boxes.at(category).add(EVENT);
			}
		}
		return Boxes;
	}



	void FMTEventContainer::BoundingBox::add(FMTEventContainer::const_iterator p_event)
	{

			const std::array<FMTCoordinate, 4> EVENT_BOX = p_event->getEnveloppe();
			
			const uint16_t EVENT_MIN_X = std::min(EVENT_BOX[2].getX(), EVENT_BOX[0].getX());
			const uint16_t MIN_X = std::min(EVENT_MIN_X,m_bottomLeft.getX());

			const uint16_t EVENT_MAX_Y = std::max(EVENT_BOX[2].getY(), EVENT_BOX[3].getY());
			const uint16_t MAX_Y = std::max(EVENT_MAX_Y, m_bottomLeft.getY());

			const uint16_t EVENT_MAX_X = std::max(EVENT_BOX[1].getX(), EVENT_BOX[3].getX());
			const uint16_t MAX_X = !_isNull() ? std::max(EVENT_MAX_X,
								uint16_t(m_bottomLeft.getX() + m_Width)) : EVENT_MAX_X;

			const uint16_t EVENT_MIN_Y = std::min(EVENT_BOX[0].getY(), EVENT_BOX[1].getY());
			const uint16_t MIN_Y = !_isNull() ? std::min(EVENT_MIN_Y,
								uint16_t(m_bottomLeft.getY() - m_Height)) : EVENT_MIN_Y;
			if (EVENT_MIN_X == MIN_X)
				{
				m_Left = p_event;
				}
			if (EVENT_MAX_X == MAX_X)
				{
				m_Right = p_event;
				}
			if (EVENT_MAX_Y == MAX_Y)
				{
				m_Bottom = p_event;
				}
			if (EVENT_MIN_Y == MIN_Y)
				{
				m_Top = p_event;
				}
			m_bottomLeft = FMTCoordinate(MIN_X, MAX_Y);
			m_Width = MAX_X - MIN_X;
			m_Height = MAX_Y - MIN_Y;
	}

	bool FMTEventContainer::BoundingBox::_isNull() const
		{
		return (m_bottomLeft.getX() == std::numeric_limits< uint16_t>::max());
		}

	std::vector<FMTEventContainer::const_iterator> FMTEventContainer::BoundingBox::getEvents() const
		{
		std::vector<FMTEventContainer::const_iterator>returned;
		if (!_isNull())
			{
			returned = std::vector<FMTEventContainer::const_iterator>{m_Top, m_Right, m_Bottom, m_Left};
			}
		return returned;
		}

	double FMTEventContainer::BoundingBox::evaluateUpperBound(int p_UpperBound) const
	{
		double value = 0.0;
		const double BOUND = static_cast<double>(p_UpperBound);
		if (!_isNull() && 
			_getSize() > BOUND)
		{
			value = _getSize() - BOUND;
		}
		return value;
	}

	double FMTEventContainer::BoundingBox::_getSize() const
		{
		return static_cast<double>(std::max(m_Width, m_Height));
		}

}
