/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTeventcontainer.hpp"
#include "FMTeventrelation.hpp"
#include "FMTexceptionhandler.hpp"
#include "FMTbindingspatialaction.hpp"

#include <limits>
#include <queue>

namespace Spatial
{
    FMTeventcontainer::FMTeventcontainer():FMTobject(),events(){}

    FMTeventcontainer::FMTeventcontainer(const FMTeventcontainer& rhs):
											FMTobject(rhs),
                                            events(rhs.events){}

    FMTeventcontainer& FMTeventcontainer::operator = (const FMTeventcontainer& rhs)
    {
        if (this!=&rhs)
            {
				FMTobject::operator=(rhs);
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
		const FMTcoordinate& coord, const int& period, const int& actionid,size_t neighborsize)
	{
		try
		{
			std::vector<FMTeventcontainer::const_iterator> aroundevents;
			aroundevents.reserve(8);
			if(neighborsize>0 && !iterators.empty())
			{
				aroundevents = getaroundevents(iterators,coord,neighborsize);
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
		}catch(...){
			_exhandler->raisefromcatch("","FMTeventcontainer::pushaction", __LINE__, __FILE__);
		}
	}

	std::vector<FMTeventcontainer::const_iterator> FMTeventcontainer::getaroundevents(const std::vector<FMTeventcontainer::const_iterator>& iterators,
		const FMTcoordinate& coord, const size_t& neighborsize) const
	{
		std::vector<FMTeventcontainer::const_iterator>aroundevents;
		aroundevents.reserve(8);
		try{
				const FMTcoordinate ndistcalculator = FMTcoordinate(neighborsize+1,neighborsize+1);
				const double distneighbors = ndistcalculator.at(neighborsize).distance(ndistcalculator);
				for (FMTeventcontainer::const_iterator eventit : iterators)
				{
					if (eventit->within(distneighbors, coord))
					{
						aroundevents.push_back(eventit);
					}
				}
		}catch(...){
			_exhandler->raisefromcatch("","FMTeventcontainer::getaroundevents", __LINE__, __FILE__);
		}
		return aroundevents;
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


	void FMTeventcontainer::addaction (const FMTcoordinate& coord, const int& period,const int& actionid, const FMTbindingspatialaction& binding)
		{
		// If maxsize is numeric limits, it means that there is no evaluation for maxsize and neighborsize
		//So we push the action as an event for each cell.
		unsigned int maxsizeof = 0;
		if(binding.getmaximalsize()<std::numeric_limits<size_t>::max())
		{
			maxsizeof = static_cast<unsigned int>(binding.getmaximalsize());
		}
		const unsigned int minx = coord.getx() > maxsizeof ? coord.getx() - maxsizeof : 0;
		const unsigned int miny = coord.gety() > maxsizeof ? coord.gety() - maxsizeof : 0;
		const FMTcoordinate lower(minx, miny);
		const FMTcoordinate upper(coord.getx()+ maxsizeof, coord.gety() + maxsizeof);
		pushaction(getevents(period, actionid, lower, upper), coord, period, actionid, binding.getminimalneighborsize());
		}

	void FMTeventcontainer::addactions(const FMTcoordinate& coord, const int& period, const std::vector<int>& actionids, const std::vector<FMTbindingspatialaction>& bindings)
		{
		unsigned int maxsizeof = 0;
		for (const int& id : actionids)
		{
			const FMTbindingspatialaction& binding = bindings.at(id);
			if(binding.getmaximalsize() < std::numeric_limits<size_t>::max())
			{
				const unsigned int msize = static_cast<unsigned int>(binding.getmaximalsize());
				maxsizeof = msize > maxsizeof ? msize : maxsizeof;
			}
		}
		const unsigned int minx = coord.getx() > maxsizeof ? coord.getx() - maxsizeof : 0;
		const unsigned int miny = coord.gety() > maxsizeof ? coord.gety() - maxsizeof : 0;
		const FMTcoordinate lower(minx, miny);
		const FMTcoordinate upper(coord.getx() + maxsizeof, coord.gety() + maxsizeof);
		size_t id = 0;
		for (const std::vector<FMTeventcontainer::const_iterator>& eventits : getmultipleevents(period, actionids, lower, upper))
			{
			const int& actid = actionids.at(id);
			pushaction(eventits, coord, period, actid, bindings.at(actid).getminimalneighborsize());
			++id;
			}
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

	FMTeventcontainer FMTeventcontainer::geteventstoadd(const FMTcoordinate& coord, const int& period, const int& actionid,
		const FMTbindingspatialaction& binding, FMTeventcontainer& newevents) const
	{
		unsigned int maxsizeof = 0;
		if(binding.getmaximalsize()<std::numeric_limits<size_t>::max())
		{
			maxsizeof = static_cast<unsigned int>(binding.getmaximalsize());
		}
		const unsigned int minx = coord.getx() > maxsizeof ? coord.getx() - maxsizeof : 0;
		const unsigned int miny = coord.gety() > maxsizeof ? coord.gety() - maxsizeof : 0;
		const FMTcoordinate lower(minx, miny);
		const FMTcoordinate upper(coord.getx()+ maxsizeof, coord.gety() + maxsizeof);
		const std::vector<FMTeventcontainer::const_iterator> eventits = getevents(period, actionid, lower, upper);
		std::vector<FMTeventcontainer::const_iterator> aroundevents;
		const size_t neighborsize = binding.getmaximalneighborsize();
		if(neighborsize>0 && !eventits.empty())
		{
			aroundevents = getaroundevents(eventits,coord,neighborsize);
		}
		//std::vector<FMTeventcontainer::const_iterator>tocalculate;
		FMTeventcontainer tocalculate;
		if (aroundevents.empty())
		{
			FMTevent newevent(coord, actionid, period);
			//newevent.insert(coord);
			newevents.insert(newevent);
		}
		else
		{
			FMTevent combinedevents(coord, actionid, period);
			//combinedevents.insert(coord);
			for (FMTeventcontainer::const_iterator e : aroundevents)
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


	FMTeventcontainer FMTeventcontainer::addupdate(const FMTeventcontainer& newevents, const FMTeventcontainer& eventstoremove)
	{
		for (FMTeventcontainer::const_iterator it = eventstoremove.events.begin(); it != eventstoremove.events.end(); ++it)
			{
			events.erase(*it);
			}
		FMTeventcontainer newlyadded;
		for (FMTeventcontainer::const_iterator it = newevents.events.begin(); it != newevents.events.end(); ++it)
			{
			newlyadded.insert(*events.insert(*it).first);
			}
		return newlyadded;
	}	

	FMTeventcontainer FMTeventcontainer::getcontainer(std::vector<FMTcoordinate> coordinates,const int& minperiod,const int& maxperiod, const size_t& buffer) const
	{
		FMTeventcontainer newcontainer;
		if (!coordinates.empty())
			{
			const unsigned int bufferof = static_cast<unsigned int>(buffer);
			std::sort(coordinates.begin(), coordinates.end());
			const unsigned int minx = coordinates.begin()->getx() > bufferof ? coordinates.begin()->getx() - bufferof : 0;
			const unsigned int miny = coordinates.begin()->gety() > bufferof ? coordinates.begin()->gety() - bufferof : 0;
			const FMTcoordinate minimalcoord(minx, miny);
			const FMTcoordinate maximalcoord(coordinates.back().getx() + bufferof, coordinates.back().gety() + bufferof);
			for (int period = minperiod; period<=maxperiod ; ++period)
				{
				FMTeventcontainer::const_iterator lower = lower_bound(period, minimalcoord);
				FMTeventcontainer::const_iterator upper = upper_bound(period, maximalcoord);
				for (FMTeventcontainer::const_iterator it = lower; it != upper; ++it)
					{
					newcontainer.insert(*it);
					}

				}
			}
		return newcontainer;
	}

	FMTeventcontainer FMTeventcontainer::geteventstoerase(const int& fromperiod, const std::vector<std::vector<bool>>& actionstarget,
																				const FMTcoordinate& coord, const size_t& buffer, FMTeventcontainer& newevents) const
	{
		const unsigned int bufferof = static_cast<unsigned int>(buffer);
		const unsigned int minx = coord.getx() > bufferof ? coord.getx() - bufferof : 0;
		const unsigned int miny = coord.gety() > bufferof ? coord.gety() - bufferof : 0;
		const FMTcoordinate minimalcoord(minx, miny);
		const FMTcoordinate maximalcoord(coord.getx()+ bufferof,coord.gety()+ bufferof);
		const int lastperiod = static_cast<int>(actionstarget.size()) + fromperiod;
		size_t periodid = 0;
		FMTeventcontainer erased;
		for (int period = fromperiod; period < lastperiod ; ++period)
			{
			FMTeventcontainer::const_iterator lower = lower_bound(period, minimalcoord);
			FMTeventcontainer::const_iterator upper = upper_bound(period, maximalcoord);
			if (!actionstarget.at(periodid).empty())
			{
			for (FMTeventcontainer::const_iterator it = lower; it != upper; ++it)
				{
				if (actionstarget.at(periodid).at(it->getactionid()) && it->contain(coord))
					{
					std::queue<FMTevent>emodif;
					if (it->size() > 1)
						{
						emodif.push(*it);
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
								newevents.insert(event);
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

	std::vector<std::vector<FMTeventcontainer::const_iterator>> FMTeventcontainer::getmultipleevents(const int& period, const std::vector<int>& action_ids,
		const FMTcoordinate& minimalcoordinate, const FMTcoordinate& maximalcoordinate) const
		{
		FMTeventcontainer::const_iterator lower = lower_bound(period, minimalcoordinate);
		FMTeventcontainer::const_iterator upper = upper_bound(period, maximalcoordinate);
		std::vector<std::vector<FMTeventcontainer::const_iterator>> selectedevents(action_ids.size(), std::vector<FMTeventcontainer::const_iterator>());
		for (FMTeventcontainer::const_iterator it = lower; it != upper; ++it)
			{
			for (size_t id = 0 ; id < action_ids.size();++id)
				{
				if (it->getactionid() == action_ids.at(id))
					{
					selectedevents.at(id).push_back(it);
					}
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
											boost::unordered_set<FMTeventrelation>& relations) const
	{
	double distancevalue = 0;
	const unsigned int lowerdistance = static_cast<unsigned int>(lowerdistancetoevent);
	const unsigned int upperdistance = static_cast<unsigned int>(upperdistancetoevent);
	const bool testlower = (lowerdistancetoevent == -std::numeric_limits<double>::infinity()) ? false : true;
	const bool testupper = (upperdistancetoevent == std::numeric_limits<double>::infinity()) ? false : true;
	for (FMTeventcontainer::const_iterator eventit : getevents(period, actionsused))
		{
			if (&(*eventit) != &eventof)//They will have the same address if it's the same event!
			{
			const FMTeventrelation straightrelation = eventof.getrelation(*eventit);
			const FMTeventrelation reverserelation = eventit->getrelation(eventof);
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
