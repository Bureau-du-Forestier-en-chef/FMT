/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTsaevent.h"
#include <queue>

namespace Spatial
{
    FMTsaevent::FMTsaevent():
                            FMTevent(),
                            action_id(),
                            period(){}

    FMTsaevent::FMTsaevent(const FMTevent& rhs):
                            FMTevent(rhs),
                            action_id(),
                            period(){}

    FMTsaevent::FMTsaevent(const FMTsaevent& rhs):
                            FMTevent(rhs),
                            action_id(rhs.action_id),
                            period(rhs.period){}

    FMTsaevent::FMTsaevent(const FMTcoordinate& location,
                           const int& laction_id,const int& lperiod):
                            FMTevent(location),
                            action_id(laction_id),
                            period(lperiod){}

    FMTsaevent& FMTsaevent::operator=(const FMTsaevent& rhs)
    {
        if (this!=&rhs)
            {
                FMTevent::operator = (rhs);
                action_id = rhs.action_id;
                period = rhs.period;
            }
            return *this;
    }

    bool FMTsaevent::operator == (const FMTsaevent& rhs) const
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

    bool FMTsaevent::operator < (const FMTsaevent& rhs) const
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

    bool FMTsaevent::potentialysplitevent(const FMTcoordinate& coord) const
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

    bool FMTsaevent::splitevent(const unsigned int& ldistance, std::vector<FMTsaevent>& splittedevents) const
    {
        std::queue<FMTcoordinate> active;
        std::set<FMTcoordinate> coordinates = elements;
        active.push(*coordinates.begin());
        while(!coordinates.empty())
        {
            FMTsaevent newevent(active.front(),action_id,period);
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
