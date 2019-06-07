#include "FMTschedule.h"

namespace Core{

FMTschedule::FMTschedule():period(),elements(){}

FMTschedule::FMTschedule(int lperiod,map<FMTaction,map<FMTdevelopment,vector<double>>> mapping) : period(lperiod),elements(mapping)
    {

    }

FMTschedule::FMTschedule(int lperiod, const map<FMTaction, map<FMTdevelopment, map<int, double>>>& mapping) : period(lperiod), elements()
	{
	for (map<FMTaction, map<FMTdevelopment, map<int, double>>>::const_iterator its = mapping.begin(); its != mapping.end(); its++)
		{
		elements[its->first] = map<FMTdevelopment, vector<double>>();
		for (map<FMTdevelopment, map<int, double>>::const_iterator elits = its->second.begin(); elits != its->second.end(); elits++)
			{
			elements[its->first][elits->first] = vector<double>();
			for (map<int, double>::const_iterator valit = elits->second.begin(); valit!= elits->second.end(); valit++)
				{
				elements[its->first][elits->first].push_back(valit->second);
				}
			}
		}
	}

FMTschedule::FMTschedule(const FMTschedule& rhs) : period(rhs.period),elements(rhs.elements)
    {

    }

FMTschedule& FMTschedule::operator = (const FMTschedule& rhs)
    {
    if (this!=&rhs)
        {
        elements = rhs.elements;
        period = rhs.period;
        }
    return *this;
    }

bool FMTschedule::sameelements(const FMTschedule& rhs) const
	{
	if (elements.size() != rhs.elements.size())
		{
		return false;
		}
	for (map<FMTaction, map<FMTdevelopment, vector<double>>>::const_iterator actit = elements.begin(); actit != elements.end(); actit++)
		{
		map<FMTaction, map<FMTdevelopment, vector<double>>>::const_iterator rhsactit = rhs.elements.find(actit->first);
		if (rhsactit == rhs.elements.end())
			{
			return false;
			}
		if (actit->second.size() != rhsactit->second.size())
			{
			return false;
			}
		for (map<FMTdevelopment, vector<double>>::const_iterator devit = actit->second.begin(); devit != actit->second.end(); devit++)
			{
			map<FMTdevelopment, vector<double>>::const_iterator rhsdevit = rhsactit->second.find(devit->first);
			if (rhsdevit == rhsactit->second.end())
				{
				return false;
				}
			double total = 0;
			for (const double& value : devit->second)
				{
				total += value;
				}
			double rhstotal = 0;
			for (const double& value : rhsdevit->second)
				{
				rhstotal += value;
				}
			if (total != rhstotal)
				{
				return false;
				}
			}
		}
	return true;
	}

bool FMTschedule::operator == (const FMTschedule& rhs) const
	{
	return (period == rhs.period && sameelements(rhs));
	}

bool FMTschedule::operator != (const FMTschedule& rhs) const
	{
	return (!(*this==rhs));
	}

FMTschedule& FMTschedule::operator += (const FMTschedule& rhs)
    {
    for(map<FMTaction,map<FMTdevelopment,vector<double>>>::const_iterator actit = rhs.elements.begin(); actit != rhs.elements.end(); actit++)
        {
        if (elements.find(actit->first)==elements.end())
            {
            elements[actit->first] = actit->second;
            }else{
             for(map<FMTdevelopment,vector<double>>::const_iterator devit = actit->second.begin(); devit != actit->second.end(); devit++)
                {
                if (elements.at(actit->first).find(devit->first) == elements.at(actit->first).end())
                    {
                    elements[actit->first][devit->first] = devit->second;
                    }else{
                    size_t max_size = max(devit->second.size(),elements[actit->first][devit->first].size());
                    vector<double>values(max_size,0);
                    for (size_t id = 0; id < max_size; ++id)
                        {
                        if (id < devit->second.size())
                            {
                            values[id]+=devit->second.at(id);
                            }
                        if (id < elements.at(actit->first).at(devit->first).size())
                            {
                            values[id]+= elements.at(actit->first).at(devit->first).at(id);
                            }
                        }
                    elements[actit->first][devit->first] = values;
                    }
                }
            }

        }
    return *this;
    }

FMTschedule FMTschedule::operator + (const FMTschedule& rhs) const
    {
    FMTschedule newschedule(*this);
    newschedule += rhs;
    return newschedule;
    }

map<FMTdevelopment,vector<double>>FMTschedule::getfaction(const FMTaction& action) const
    {
    if (elements.find(action)!=elements.end())
        {
        return elements.at(action);
        }
    return map<FMTdevelopment,vector<double>>();
    }

map<FMTaction,map<FMTdevelopment,vector<double>>>FMTschedule::get() const
    {
    return elements;
    }

FMTschedule::operator string() const
    {
    string line = "";
    for(map<FMTaction,map<FMTdevelopment,vector<double>>>::const_iterator actit = elements.begin(); actit != elements.end(); actit++)
        {
        for(map<FMTdevelopment,vector<double>>::const_iterator devit = actit->second.begin(); devit != actit->second.end(); devit++)
            {
            const FMTdevelopment* dev = &devit->first;
			for (const double & value : devit->second)
				{
				line += string(dev->mask) + " " + to_string(dev->age) + " " + to_string(value) + " " + actit->first.name + " " + to_string(period) + "\n";
				}
            }
        }
    return line;
    }


double FMTschedule::actionarea(const FMTaction& action) const
    {
    double value = 0;
	if (elements.find(action)!=elements.end())
		{
		for (map<FMTdevelopment, vector<double>>::const_iterator devit = elements.at(action).begin(); devit != elements.at(action).end(); devit++)
			{
			for (const double& val : devit->second)
				{
				value += val;
				}
			}
		}
    return value;
    }

int FMTschedule::getperiod() const
    {
    return period;
    }

double FMTschedule::area() const
    {
    double value = 0;
    for(map<FMTaction,map<FMTdevelopment,vector<double>>>::const_iterator actit = elements.begin(); actit != elements.end(); actit++)
        {
        for(map<FMTdevelopment, vector<double>>::const_iterator devit = actit->second.begin(); devit != actit->second.end(); devit++)
            {
			for (const double& val : devit->second)
				{
				value += val;
				}
            }
        }
    return value;
    }

bool FMTschedule::operated(const FMTaction& action,
	const FMTdevelopment& developement) const
	{
	bool value = false;
	map<FMTaction, map<FMTdevelopment, vector<double>>>::const_iterator actit = elements.find(action);
	if (actit != elements.end())
		{
		map<FMTdevelopment, vector<double>>::const_iterator devit;
		if (action.lock)
			{
			devit = actit->second.find(developement);
			}else {
			devit = actit->second.find(developement.clearlock());
			}
		if (devit != actit->second.end())
			{
			value = true;
			}
		}
	return value;
	}

bool FMTschedule::empty() const
	{
	return elements.empty();
	}

void FMTschedule::sort()
	{
	for (map<FMTaction, map<FMTdevelopment, vector<double>>>::iterator actit = elements.begin(); actit != elements.end(); actit++)
		{
		for (map<FMTdevelopment, vector<double>>::iterator devit = actit->second.begin(); devit != actit->second.end(); devit++)
			{
			std::sort(devit->second.begin(), devit->second.end());
			}
		}
	}
}
