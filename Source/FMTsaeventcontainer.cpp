/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTsaeventcontainer.h"
#include <limits>
#include <queue>

namespace Spatial
{
    FMTsaeventcontainer::FMTsaeventcontainer():events(){}

    FMTsaeventcontainer::FMTsaeventcontainer(const FMTsaeventcontainer& rhs):
                                            events(rhs.events){}

    FMTsaeventcontainer& FMTsaeventcontainer::operator = (const FMTsaeventcontainer& rhs)
    {
        if (this!=&rhs)
            {
                events = rhs.events;
            }
        return *this;
    }

    bool FMTsaeventcontainer::operator==(const FMTsaeventcontainer& rhs) const
    {
        return events==rhs.events;
    }

    void FMTsaeventcontainer::swap(FMTsaeventcontainer& rhs)
    {
        events.swap(rhs.events);
    }

    void FMTsaeventcontainer::erase(const FMTsaevent& event)
    {
        events.erase(event);
    }

    FMTsaeventcontainer::const_iterator FMTsaeventcontainer::erase(const FMTsaeventcontainer::const_iterator event)
    {
        return events.erase(event);
    }

    int FMTsaeventcontainer::lastperiod()const
    {
        return events.rbegin()->getperiod();
    }

    int FMTsaeventcontainer::firstperiod()const
    {
        return events.begin()->getperiod();
    }

    void FMTsaeventcontainer::erasecoordinate (const FMTcoordinate coord, const int& periodstart)
    {
        const int lperiod=lastperiod();
        for (int period=periodstart;period<=lperiod;++period)
        {
            const FMTsaeventcontainer::const_iterator upper = upper_bound(period);
            FMTsaeventcontainer::const_iterator lower = lower_bound(period);
            std::queue<FMTsaevent> emodif;
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
                FMTsaevent event = emodif.front();
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
                    std::vector<FMTsaevent> splittedevents;
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
    }

    std::pair<FMTsaeventcontainer::const_iterator,FMTsaeventcontainer::const_iterator> FMTsaeventcontainer::getbounds(const int& period) const
    {
        return std::pair<FMTsaeventcontainer::const_iterator,FMTsaeventcontainer::const_iterator>(lower_bound(period),upper_bound(period));
    }

    void FMTsaeventcontainer::addaction (const FMTcoordinate coord, const int& period,const int& actionid)
    {
        std::vector<FMTsaeventcontainer::const_iterator> events = getevents(period,actionid);
        std::vector<FMTsaevent> aroundevents;
        for (auto eventit : events)
        {
            if (eventit->whithinelements(1,coord))
            {
                aroundevents.push_back(*eventit);
            }
        }
        if (aroundevents.empty())
        {
            FMTsaevent newevent(coord,actionid,period);
            newevent.insert(coord);
            insert(newevent);
        }
        else
        {
            FMTsaevent combinedevents(coord,actionid,period);
            combinedevents.insert(coord);
            for (auto e : aroundevents)
            {
                combinedevents.merge(e);
                erase(e);
            }
            insert(combinedevents);
        }
    }

    std::vector<FMTsaeventcontainer::const_iterator> FMTsaeventcontainer::getevents(const int& period,
                                                                                    const int& action_id) const
    {
        FMTsaeventcontainer::const_iterator upper = upper_bound(period);
        FMTsaeventcontainer::const_iterator lower = lower_bound(period);
        std::vector<FMTsaeventcontainer::const_iterator> selectedevents;
        for  (FMTsaeventcontainer::const_iterator it = lower ; it != upper ; ++it)
        {
            if (it->getactionid()==action_id && it->getperiod()==period)
            {
                selectedevents.push_back(it);
            }
        }
        return selectedevents;
    }

    std::vector<FMTsaeventcontainer::const_iterator> FMTsaeventcontainer::getevents(const int& period,
                                                                                    const std::vector<int>& actions_id) const
    {
        FMTsaeventcontainer::const_iterator upper = upper_bound(period);
        FMTsaeventcontainer::const_iterator lower = lower_bound(period);
        std::vector<FMTsaeventcontainer::const_iterator> selectedevents;
        for  (FMTsaeventcontainer::const_iterator it = lower ; it != upper ; ++it)
        {
            if (std::find(actions_id.begin(),actions_id.end(),it->getactionid())!=actions_id.end() && it->getperiod()==period)
            {
                selectedevents.push_back(it);
            }
        }
        return selectedevents;
    }

    std::vector<FMTsaeventcontainer::const_iterator> FMTsaeventcontainer::getevents(const int& period,
                                                                                    const FMTcoordinate& coord) const
    {
        FMTsaeventcontainer::const_iterator upper = upper_bound(period);
        FMTsaeventcontainer::const_iterator lower = lower_bound(period);
        std::vector<FMTsaeventcontainer::const_iterator> selectedevents;
        for  (FMTsaeventcontainer::const_iterator it = lower ; it != upper ; ++it)
        {
            if (it->contain(coord))
            {
                selectedevents.push_back(it);
            }
        }
        return selectedevents;

    }

    std::vector<FMTsaeventcontainer::const_iterator> FMTsaeventcontainer::getevents(const int& period,
                                                                                    const std::set<FMTcoordinate>& territory) const
    {
        std::vector<FMTsaeventcontainer::const_iterator> selectedevents;
        if (!territory.empty())
        {
            const FMTcoordinate minxy = *territory.begin();
            const FMTcoordinate maxxy = *(--territory.end());
            FMTsaeventcontainer::const_iterator lower = lower_bound(period,minxy);
            FMTsaeventcontainer::const_iterator upper = upper_bound(period,maxxy);
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



    FMTsaeventcontainer::const_iterator FMTsaeventcontainer::upper_bound(const int& period) const
    {
        FMTsaeventcontainer::const_iterator upper = events.upper_bound(FMTsaevent(FMTcoordinate(UINT_MAX,UINT_MAX),
                                                             INT_MAX,
                                                             period));
        return upper;
    }

    FMTsaeventcontainer::const_iterator FMTsaeventcontainer::upper_bound(const int& period, const FMTcoordinate& maxxycoord) const
    {
        FMTsaeventcontainer::const_iterator upper = events.upper_bound(FMTsaevent(maxxycoord,
                                                                                  INT_MAX,
                                                                                  period));
        return upper;
    }

    FMTsaeventcontainer::const_iterator FMTsaeventcontainer::lower_bound(const int& period) const
    {
        FMTsaeventcontainer::const_iterator lower = events.lower_bound(FMTsaevent(FMTcoordinate(0,0),
                                                             0,
                                                             period));
        return lower;
    }

    FMTsaeventcontainer::const_iterator FMTsaeventcontainer::lower_bound(const int& period,const FMTcoordinate& minxycoord) const
    {
        FMTsaeventcontainer::const_iterator lower = events.lower_bound(FMTsaevent(minxycoord,
                                                             0,
                                                             period));
        return lower;
    }

    double FMTsaeventcontainer::minimaldistance(const FMTsaevent& event, const unsigned int& distancel,
                                                const int& period, const std::vector<int>& actionsid) const
    {
    double distancevalue = static_cast<double>(distancel) + 1.0;
    std::vector<FMTsaeventcontainer::const_iterator> potentialneighbors = getevents(period,actionsid);
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
}
