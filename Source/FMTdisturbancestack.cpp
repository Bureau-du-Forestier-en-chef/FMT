/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTdisturbancestack.h"

namespace Spatial
{

FMTdisturbancestack::FMTdisturbancestack() : data()
    {

    }
FMTdisturbancestack::FMTdisturbancestack(const FMTdisturbancestack & rhs) : data(rhs.data)
    {

    }
FMTdisturbancestack& FMTdisturbancestack::operator = (const FMTdisturbancestack & rhs)
    {
    if(this!=&rhs)
        {
        data = rhs.data;
        }
    return *this;
    }

std::map<std::string, std::string>FMTdisturbancestack::directmapping() const
    {
	std::map<std::string, std::string>mapping;
    for(const std::map<std::string, std::vector<FMTsesevent<Core::FMTdevelopment>>>& dmap : data)
        {
        for(std::map<std::string,std::vector<FMTsesevent<Core::FMTdevelopment>>>::const_iterator eit = dmap.begin(); eit != dmap.end(); eit++)
            {
            mapping[eit->first] = eit->first;
            }
        }
    return mapping;
    }

void FMTdisturbancestack::push(const std::map<std::string, std::vector<FMTsesevent<Core::FMTdevelopment>>>& element)
    {
    data.push_back(element);
    }

void FMTdisturbancestack::add(const std::string& action,const std::vector<FMTsesevent<Core::FMTdevelopment>>& events)
    {
    if (data.back().find(action)==data.back().end())
        {
        data.back()[action] = events;
        }else{
        data.back()[action].insert(data.back()[action].end(),events.begin(),events.end());
        }
    }

bool FMTdisturbancestack::allow(const FMTspatialaction& action,const FMTcoordinate& location) const
    {
    int MINGU = static_cast<int>((data.size() - action.green_up));
    for(size_t green_up = std::max(0,MINGU); green_up < data.size(); ++green_up)
        {
        const std::map<std::string, std::vector<FMTsesevent<Core::FMTdevelopment>>>* mapping = &data.at(green_up);
        for(const std::string& actname : action.neighbors)
            {
			std::map<std::string, std::vector<FMTsesevent<Core::FMTdevelopment>>>::const_iterator evit = mapping->find(actname);
            if (evit!=mapping->end())
                {
                for(const FMTsesevent<Core::FMTdevelopment>& event : evit->second)
                    {
                    if (event.withinc(static_cast<unsigned int>(action.adjacency),location))
                        {
                        return false;
                        }
                    }

                }
            }
        }
    return true;
    }


std::string FMTdisturbancestack::getpatchstats() const
	{
	std::string result = "";
	size_t period = 1;
	for (const std::map<std::string, std::vector<FMTsesevent<Core::FMTdevelopment>>>& dmap : data)
		{
		for (std::map<std::string, std::vector<FMTsesevent<Core::FMTdevelopment>>>::const_iterator eit = dmap.begin(); eit != dmap.end(); eit++)
			{
			const std::string action = eit->first;
			for(const FMTsesevent<Core::FMTdevelopment>& ev : eit->second)
				{
				result += std::to_string(period) + " " + action +" "+ ev.getstats() + "\n";
				}
			}
		++period;
		}
	return result;
	}

}
