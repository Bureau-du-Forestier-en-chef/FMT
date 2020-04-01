/*
MIT License

Copyright (c) [2019] [Bureau du forestier en chef]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
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
    for(const std::map<std::string, std::vector<FMTevent<Core::FMTdevelopment>>>& dmap : data)
        {
        for(std::map<std::string,std::vector<FMTevent<Core::FMTdevelopment>>>::const_iterator eit = dmap.begin(); eit != dmap.end(); eit++)
            {
            mapping[eit->first] = eit->first;
            }
        }
    return mapping;
    }

void FMTdisturbancestack::push(const std::map<std::string, std::vector<FMTevent<Core::FMTdevelopment>>>& element)
    {
    data.push_back(element);
    }

void FMTdisturbancestack::add(const std::string& action,const std::vector<FMTevent<Core::FMTdevelopment>>& events)
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
        const std::map<std::string, std::vector<FMTevent<Core::FMTdevelopment>>>* mapping = &data.at(green_up);
        for(const std::string& actname : action.neighbors)
            {
			std::map<std::string, std::vector<FMTevent<Core::FMTdevelopment>>>::const_iterator evit = mapping->find(actname);
            if (evit!=mapping->end())
                {
                for(const FMTevent<Core::FMTdevelopment>& event : evit->second)
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
	for (const std::map<std::string, std::vector<FMTevent<Core::FMTdevelopment>>>& dmap : data)
		{
		for (std::map<std::string, std::vector<FMTevent<Core::FMTdevelopment>>>::const_iterator eit = dmap.begin(); eit != dmap.end(); eit++)
			{
			const std::string action = eit->first;
			for(const FMTevent<Core::FMTdevelopment>& ev : eit->second)
				{ 
				result += std::to_string(period) + " " + action +" "+ ev.getstats() + "\n";
				}
			}
		++period;
		}
	return result;
	}
}
