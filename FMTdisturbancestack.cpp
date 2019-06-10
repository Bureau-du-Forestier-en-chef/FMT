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

map<string,string>FMTdisturbancestack::directmapping() const
    {
    map<string,string>mapping;
    for(const map<string,vector<FMTevent<FMTdevelopment>>>& dmap : data)
        {
        for(map<string,vector<FMTevent<FMTdevelopment>>>::const_iterator eit = dmap.begin(); eit != dmap.end(); eit++)
            {
            mapping[eit->first] = eit->first;
            }
        }
    return mapping;
    }

void FMTdisturbancestack::push(const map<string,vector<FMTevent<FMTdevelopment>>>& element)
    {
    data.push_back(element);
    }

void FMTdisturbancestack::add(const string& action,const vector<FMTevent<FMTdevelopment>>& events)
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
    int MINGU = (data.size() - action.green_up);
    for(size_t green_up = max(0,MINGU); green_up < data.size(); ++green_up)
        {
        const map<string,vector<FMTevent<FMTdevelopment>>>* mapping = &data.at(green_up);
        for(const string& actname : action.neighbors)
            {
            map<string,vector<FMTevent<FMTdevelopment>>>::const_iterator evit = mapping->find(actname);
            if (evit!=mapping->end())
                {
                for(const FMTevent<FMTdevelopment>& event : evit->second)
                    {
                    if (event.withinc(action.adjacency,location))
                        {
                        return false;
                        }
                    }

                }
            }
        }
    return true;
    }

FMTschedule FMTdisturbancestack::getlastschedule(double cellsize,
	const vector<FMTaction>& actions,
	const FMTlayer<FMTdevelopment>& forest) const
	{
	map<FMTaction, map<FMTdevelopment, vector<double>>> mapping;
	int period = static_cast<int>(data.size());
	for (map<string, vector<FMTevent<FMTdevelopment>>>::const_iterator acit = data.back().begin(); acit != data.back().end(); acit++)
	{
		string actionname = acit->first;
		vector<FMTaction>::const_iterator cntaction = find_if(actions.begin(), actions.end(), FMTactioncomparator(actionname));
		if (cntaction != actions.end())
		{
			mapping[*cntaction] = map<FMTdevelopment, vector<double>>();
			for (map<FMTcoordinate, const FMTdevelopment*>::const_iterator devit = acit->second.back().elements.begin();
				devit != acit->second.back().elements.end(); devit++)
			{
				const FMTdevelopment dev = forest.mapping.at(devit->first);
				if (mapping[*cntaction].find(dev.clearlock()) == mapping[*cntaction].end())
				{
					mapping[*cntaction][dev.clearlock()].push_back(cellsize);
				}
				else {
					mapping[*cntaction][dev.clearlock()][0] += cellsize;
				}
			}
		}
	}
	return FMTschedule(period, mapping);
	}

string FMTdisturbancestack::getpatchstats() const
	{
	string result = "";
	size_t period = 1;
	for (const map<string, vector<FMTevent<FMTdevelopment>>>& dmap : data)
		{
		for (map<string, vector<FMTevent<FMTdevelopment>>>::const_iterator eit = dmap.begin(); eit != dmap.end(); eit++)
			{
			const string action = eit->first;
			for(const FMTevent<FMTdevelopment>& ev : eit->second)
				{ 
				result += to_string(period) + " " + action +" "+ ev.getstats() + "\n";
				}
			}
		++period;
		}
	return result;
	}

}
