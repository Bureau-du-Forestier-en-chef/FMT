/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTeventcontainer.h"
#include <limits>
#include <queue>

namespace Spatial
{
    FMTeventcontainer::FMTeventcontainer():events(){}

    FMTeventcontainer::FMTeventcontainer(const FMTeventcontainer& rhs):
                                            events(rhs.events){}

    FMTeventcontainer& FMTeventcontainer::operator = (const FMTeventcontainer& rhs)
    {
        if (this!=&rhs)
            {
                events = rhs.events;
            }
        return *this;
    }

    bool FMTeventcontainer::operator==(const FMTeventcontainer& rhs) const
    {
        return events==rhs.events;
    }

    void FMTeventcontainer::swap(FMTeventcontainer& rhs)
    {
        events.swap(rhs.events);
    }

	FMTeventcontainer::const_iterator FMTeventcontainer::find(const int & period, const FMTcoordinate & coord) const
	{
		std::vector<FMTeventcontainer::const_iterator> eventsfound = getevents(period, coord);
		if (eventsfound.size() > 1)
		{
			return eventsfound.at(0);
		}
		else
		{
			return events.cend();
		}
	}

	void FMTeventcontainer::merge(const FMTeventcontainer & levents)
	{
		events.insert(levents.begin(), levents.end());
	}

	void FMTeventcontainer::erase(const FMTevent& event)
    {
        events.erase(event);
    }

    FMTeventcontainer::const_iterator FMTeventcontainer::erase(const FMTeventcontainer::const_iterator event)
    {
        return events.erase(event);
    }

    int FMTeventcontainer::lastperiod()const
    {
        return events.rbegin()->getperiod();
    }

    int FMTeventcontainer::firstperiod()const
    {
        return events.begin()->getperiod();
    }

	void FMTeventcontainer::erase(const FMTcoordinate& coord,const std::vector<FMTeventcontainer::iterator>& iterators)
	{
		std::queue<FMTevent>emodif;
		std::vector<FMTeventcontainer::iterator>::const_iterator lowerb = iterators.begin();
		std::vector<FMTeventcontainer::iterator>::const_iterator upperb = iterators.end();
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
			FMTevent event = emodif.front();
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
				std::vector<FMTevent> splittedevents;
				if (event.splitevent(1, splittedevents))
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

	void FMTeventcontainer::erasecoordinate(const FMTcoordinate& coord, const int& periodstart, const std::vector<std::vector<bool>>& actionstarget)
		{
		const int lastperiod = static_cast<int>(actionstarget.size()) + periodstart;
		size_t periodid = 0;
		for (int period = periodstart; period < lastperiod; ++period)
			{
			if (!actionstarget.at(periodid).empty())
				{
				const std::vector<FMTeventcontainer::const_iterator> iterators = getevents(period, actionstarget.at(periodid));
				if (!iterators.empty())
					{
					erase(coord, iterators);
					}

				}
			++periodid;
			}
		}

	/*void FMTeventcontainer::erasecoordinate (const FMTcoordinate& coord, const int& periodstart)
    {
        const int lperiod=lastperiod();
        for (int period=periodstart;period<=lperiod;++period)
        {
            const FMTeventcontainer::const_iterator upper = upper_bound(period);
            FMTeventcontainer::const_iterator lower = lower_bound(period);
            std::queue<FMTevent> emodif;
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
                FMTevent event = emodif.front();
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
                    std::vector<FMTevent> splittedevents;
                    if (event.splitevent(1,splittedevents))
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

    std::pair<FMTeventcontainer::const_iterator,FMTeventcontainer::const_iterator> FMTeventcontainer::getbounds(const int& period) const
    {
        return std::pair<FMTeventcontainer::const_iterator,FMTeventcontainer::const_iterator>(lower_bound(period),upper_bound(period));
    }



	void FMTeventcontainer::pushaction(const std::vector<FMTeventcontainer::const_iterator>& iterators,
		const FMTcoordinate& coord, const int& period, const int& actionid)
	{
		std::vector<FMTeventcontainer::const_iterator>aroundevents;
		aroundevents.reserve(8);
		/*std::vector<FMTeventcontainer::const_iterator>sorted(iterators);
		nthelements(sorted, coord, 8);
		std::vector<FMTeventcontainer::const_iterator>::const_iterator eventit = sorted.begin();
		size_t checked = 0;
		while (eventit!= sorted.end()&&checked<8)
			{
			if ((*eventit)->within(1, coord))
				{
				aroundevents.push_back(*eventit);
				}
			++eventit;
			++checked;
			}*/
		for (FMTeventcontainer::const_iterator eventit : iterators)
		{
			if (eventit->within(1, coord))
			{
				aroundevents.push_back(eventit);
			}
		}
		if (aroundevents.empty())
		{
			FMTevent newevent(coord, actionid, period);
			//newevent.insert(coord);
			insert(newevent);
		}
		else
		{
			FMTevent combinedevents(coord, actionid, period);
			//combinedevents.insert(coord);
			for (FMTeventcontainer::const_iterator e : aroundevents)
			{
				combinedevents.merge(*e);
				erase(*e);
			}
			insert(combinedevents);
		}
	}

	FMTeventcontainer::FMTeventiteratorsorter::FMTeventiteratorsorter(const FMTcoordinate& coordinate) : basecoordinate(coordinate)
		{

		}
	bool FMTeventcontainer::FMTeventiteratorsorter::operator() (const FMTeventcontainer::const_iterator& eventit1, const FMTeventcontainer::const_iterator& eventit2) const
		{
		double approx1 = 0;
		basecoordinate.closest(eventit1->getborders(), approx1);
		double approx2 = 0;
		basecoordinate.closest(eventit2->getborders(), approx1);
		return approx1 < approx2;
		}

	void FMTeventcontainer::nthelements(std::vector<FMTeventcontainer::const_iterator>& iterators, const FMTcoordinate& coord, const size_t& nelement) const
		{
		std::nth_element(iterators.begin(),iterators.begin()+ nelement, iterators.end(), FMTeventiteratorsorter(coord));
		//std::sort(iterators.begin(), iterators.end(), FMTeventiteratorsorter(coord));
		}


	void FMTeventcontainer::addaction(const FMTcoordinate& coord, const int& period, const int& actionid, const size_t& maxsize)
		{
		const size_t minx = coord.getx() > maxsize ? coord.getx() - maxsize : 0;
		const size_t miny = coord.gety() > maxsize ? coord.gety() - maxsize : 0;
		const FMTcoordinate lower(minx, miny);
		const FMTcoordinate upper(coord.getx()+maxsize, coord.gety() + maxsize);
		const std::vector<FMTeventcontainer::const_iterator>eventits = getevents(period, actionid, lower, upper);
		pushaction(eventits, coord, period, actionid);
		}


	void FMTeventcontainer::addaction (const FMTcoordinate& coord, const int& period,const int& actionid)
		{
		pushaction(getevents(period, actionid), coord, period, actionid);
		}

    std::vector<FMTeventcontainer::const_iterator> FMTeventcontainer::getevents(const int& period,
                                                                                    const int& action_id) const
    {
        FMTeventcontainer::const_iterator upper = upper_bound(period);
        FMTeventcontainer::const_iterator lower = lower_bound(period);
        std::vector<FMTeventcontainer::const_iterator> selectedevents;
        for  (FMTeventcontainer::const_iterator it = lower ; it != upper && it!=events.end() ; ++it)
        {
            if (it->getactionid()==action_id /*&& it->getperiod()==period*/)
            {
                selectedevents.push_back(it);
            }
        }
        return selectedevents;
    }

	std::vector<FMTeventcontainer::const_iterator> FMTeventcontainer::getevents(const int& period, const std::vector<bool>& actions_used,
		const FMTcoordinate& minimalcoordinate, const FMTcoordinate& maximalcoordinate) const
	{
		FMTeventcontainer::const_iterator lower = lower_bound(period, minimalcoordinate);
		FMTeventcontainer::const_iterator upper = upper_bound(period, maximalcoordinate);
		std::vector<FMTeventcontainer::const_iterator> selectedevents;
		for (FMTeventcontainer::const_iterator it = lower; it != upper; ++it)
		{
			if (actions_used.at(it->getactionid()) /*&& it->getperiod() == period*/)
			{
				selectedevents.push_back(it);
			}
		}
		return selectedevents;

	}

	std::vector<FMTeventcontainer::const_iterator> FMTeventcontainer::getevents(const int& period, const int& action_id,
		const FMTcoordinate& minimalcoordinate, const FMTcoordinate& maximalcoordinate) const
	{
		FMTeventcontainer::const_iterator lower = lower_bound(period, minimalcoordinate);
		FMTeventcontainer::const_iterator upper = upper_bound(period, maximalcoordinate);
		std::vector<FMTeventcontainer::const_iterator> selectedevents;
		for (FMTeventcontainer::const_iterator it = lower; it != upper; ++it)
		{
			if (it->getactionid() == action_id /*&& it->getperiod() == period*/)
			{
				selectedevents.push_back(it);
			}
		}
		return selectedevents;
	}


    std::vector<FMTeventcontainer::const_iterator> FMTeventcontainer::getevents(const int& period,
                                                                                    const std::vector<int>& actions_id) const
    {
        FMTeventcontainer::const_iterator upper = upper_bound(period);
        FMTeventcontainer::const_iterator lower = lower_bound(period);
        std::vector<FMTeventcontainer::const_iterator> selectedevents;
        for  (FMTeventcontainer::const_iterator it = lower ; it != upper ; ++it)
        {
            if (std::find(actions_id.begin(),actions_id.end(),it->getactionid())!=actions_id.end() && it->getperiod()==period)
            {
                selectedevents.push_back(it);
            }
        }
        return selectedevents;
    }

	std::vector<FMTeventcontainer::const_iterator> FMTeventcontainer::getevents(const int& period, const std::vector<bool>& actions_used) const
	{
		FMTeventcontainer::const_iterator upper = upper_bound(period);
		FMTeventcontainer::const_iterator lower = lower_bound(period);
		std::vector<FMTeventcontainer::const_iterator> selectedevents;
		for (FMTeventcontainer::const_iterator it = lower; it != upper; ++it)
		{
			if (actions_used.at(it->getactionid()) /*&& it->getperiod() == period*/)
			{
				selectedevents.push_back(it);
			}
		}
		return selectedevents;
	}

    std::vector<FMTeventcontainer::const_iterator> FMTeventcontainer::getevents(const int& period,
                                                                                    const FMTcoordinate& coord) const
    {
        FMTeventcontainer::const_iterator upper = upper_bound(period);
        FMTeventcontainer::const_iterator lower = lower_bound(period);
        std::vector<FMTeventcontainer::const_iterator> selectedevents;
        for  (FMTeventcontainer::const_iterator it = lower ; it != upper ; ++it)
        {
            if (it->contain(coord))
            {
                selectedevents.push_back(it);
            }
        }
        return selectedevents;

    }

    std::vector<FMTeventcontainer::const_iterator> FMTeventcontainer::getevents(const int& period,
                                                                                    const std::set<FMTcoordinate>& territory) const
    {
        std::vector<FMTeventcontainer::const_iterator> selectedevents;
        if (!territory.empty())
        {
            const FMTcoordinate minxy = *territory.begin();
            const FMTcoordinate maxxy = *(--territory.end());
            FMTeventcontainer::const_iterator lower = lower_bound(period,minxy);
            FMTeventcontainer::const_iterator upper = upper_bound(period,maxxy);
            ///Debug
            //std::cout<<"Upper "<<upper->averagecentroid().getx()<<upper->averagecentroid().gety()<<" Action : "<<upper->getactionid()<<" Period : "<<upper->getperiod()<<std::endl;
            //std::cout<<"Lower "<<lower->averagecentroid().getx()<<lower->averagecentroid().gety()<<" Action : "<<lower->getactionid()<<" Period : "<<lower->getperiod()<<std::endl;
            do {
                    if (lower!=events.end())
                    {
                        if(lower == upper)
                        {
                            if(
                               (upper->averagecentroid() == maxxy || upper->averagecentroid() < maxxy)&&
                               (upper->getperiod() == period)
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
            while(lower!=upper);
        }
        return selectedevents;
    }



    FMTeventcontainer::const_iterator FMTeventcontainer::upper_bound(const int& period) const
    {
        FMTeventcontainer::const_iterator upper = events.upper_bound(FMTevent(FMTcoordinate(UINT_MAX,UINT_MAX),
                                                             INT_MAX,
                                                             period));
        return upper;
    }

    FMTeventcontainer::const_iterator FMTeventcontainer::upper_bound(const int& period, const FMTcoordinate& maxxycoord) const
    {
        FMTeventcontainer::const_iterator upper = events.upper_bound(FMTevent(maxxycoord,
                                                                                  INT_MAX,
                                                                                  period));
        return upper;
    }

    FMTeventcontainer::const_iterator FMTeventcontainer::lower_bound(const int& period) const
    {
        FMTeventcontainer::const_iterator lower = events.lower_bound(FMTevent(FMTcoordinate(0,0),
                                                             0,
                                                             period));
        return lower;
    }

    FMTeventcontainer::const_iterator FMTeventcontainer::lower_bound(const int& period,const FMTcoordinate& minxycoord) const
    {
        FMTeventcontainer::const_iterator lower = events.lower_bound(FMTevent(minxycoord,
                                                             0,
                                                             period));
        return lower;
    }

    double FMTeventcontainer::minimaldistance(const FMTevent& event, const unsigned int& distancel,
                                                const int& period, const std::vector<int>& actionsid) const
    {
    double distancevalue = static_cast<double>(distancel) + 1.0;
    std::vector<FMTeventcontainer::const_iterator> potentialneighbors = getevents(period,actionsid);
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

	double FMTeventcontainer::evaluatedistance(const FMTevent& eventof, 
											const double& lowerdistancetoevent,
											const double& upperdistancetoevent,
											const int& period, const std::vector<bool>& actionsused,
											std::unordered_set<size_t>& relations) const
	{
	double distancevalue = 0;
	const size_t lowerdistance = static_cast<size_t>(lowerdistancetoevent);
	const size_t upperdistance = static_cast<size_t>(upperdistancetoevent);
	const bool testlower = (lowerdistancetoevent == -std::numeric_limits<double>::infinity()) ? false : true;
	const bool testupper = (upperdistancetoevent == std::numeric_limits<double>::infinity()) ? false : true;
	for (FMTeventcontainer::const_iterator eventit : getevents(period, actionsused))
		{
			if (&(*eventit) != &eventof)//They will have the same address if it's the same event!
			{
			const size_t straightrelation = eventof.getrelation(*eventit);
			const size_t reverserelation = eventit->getrelation(eventof);
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
}
