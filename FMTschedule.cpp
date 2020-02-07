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

#include "FMTschedule.h"

namespace Core{

FMTschedule::FMTschedule():period(),elements(){}

FMTschedule::FMTschedule(int lperiod, std::map<FMTaction, std::map<FMTdevelopment, std::vector<double>>> mapping) : period(lperiod),elements(mapping)
    {

    }

FMTschedule::FMTschedule(int lperiod, const std::map<FMTaction, std::map<FMTdevelopment, std::map<int, double>>>& mapping) : period(lperiod), elements()
	{
	for (std::map<FMTaction, std::map<FMTdevelopment, std::map<int, double>>>::const_iterator its = mapping.begin(); its != mapping.end(); its++)
		{
		elements[its->first] = std::map<FMTdevelopment, std::vector<double>>();
		for (std::map<FMTdevelopment, std::map<int, double>>::const_iterator elits = its->second.begin(); elits != its->second.end(); elits++)
			{
			elements[its->first][elits->first] = std::vector<double>();
			for (std::map<int, double>::const_iterator valit = elits->second.begin(); valit!= elits->second.end(); valit++)
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
	for (std::map<FMTaction, std::map<FMTdevelopment, std::vector<double>>>::const_iterator actit = elements.begin(); actit != elements.end(); actit++)
		{
		std::map<FMTaction, std::map<FMTdevelopment, std::vector<double>>>::const_iterator rhsactit = rhs.elements.find(actit->first);
		if (rhsactit == rhs.elements.end())
			{
			return false;
			}
		if (actit->second.size() != rhsactit->second.size())
			{
			return false;
			}
		for (std::map<FMTdevelopment, std::vector<double>>::const_iterator devit = actit->second.begin(); devit != actit->second.end(); devit++)
			{
			std::map<FMTdevelopment, std::vector<double>>::const_iterator rhsdevit = rhsactit->second.find(devit->first);
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
    for(std::map<FMTaction, std::map<FMTdevelopment, std::vector<double>>>::const_iterator actit = rhs.elements.begin(); actit != rhs.elements.end(); actit++)
        {
        if (elements.find(actit->first)==elements.end())
            {
            elements[actit->first] = actit->second;
            }else{
             for(std::map<FMTdevelopment, std::vector<double>>::const_iterator devit = actit->second.begin(); devit != actit->second.end(); devit++)
                {
                if (elements.at(actit->first).find(devit->first) == elements.at(actit->first).end())
                    {
                    elements[actit->first][devit->first] = devit->second;
                    }else{
                    const size_t max_size = std::max(devit->second.size(),elements[actit->first][devit->first].size());
					std::vector<double>values(max_size,0);
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

std::map<FMTdevelopment, std::vector<double>>FMTschedule::getfaction(const FMTaction& action) const
    {
    if (elements.find(action)!=elements.end())
        {
        return elements.at(action);
        }
    return std::map<FMTdevelopment, std::vector<double>>();
    }

std::map<FMTaction, std::map<FMTdevelopment, std::vector<double>>>FMTschedule::get() const
    {
    return elements;
    }

FMTschedule::operator std::string() const
    {
	std::string line = "";
    for(std::map<FMTaction, std::map<FMTdevelopment, std::vector<double>>>::const_iterator actit = elements.begin(); actit != elements.end(); actit++)
        {
        for(std::map<FMTdevelopment, std::vector<double>>::const_iterator devit = actit->second.begin(); devit != actit->second.end(); devit++)
            {
            const FMTdevelopment* dev = &devit->first;
			for (const double & value : devit->second)
				{
				line += std::string(dev->mask) + " " + std::to_string(dev->age) + " " + std::to_string(value) + " " + actit->first.getname() + " " + std::to_string(period) + "\n";
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
		for (std::map<FMTdevelopment, std::vector<double>>::const_iterator devit = elements.at(action).begin(); devit != elements.at(action).end(); devit++)
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
    for(std::map<FMTaction, std::map<FMTdevelopment, std::vector<double>>>::const_iterator actit = elements.begin(); actit != elements.end(); actit++)
        {
        for(std::map<FMTdevelopment, std::vector<double>>::const_iterator devit = actit->second.begin(); devit != actit->second.end(); devit++)
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
	std::map<FMTaction, std::map<FMTdevelopment, std::vector<double>>>::const_iterator actit = elements.find(action);
	if (actit != elements.end())
		{
		std::map<FMTdevelopment, std::vector<double>>::const_iterator devit;
		if (action.dorespectlock())
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
	for (std::map<FMTaction, std::map<FMTdevelopment, std::vector<double>>>::iterator actit = elements.begin(); actit != elements.end(); actit++)
		{
		for (std::map<FMTdevelopment, std::vector<double>>::iterator devit = actit->second.begin(); devit != actit->second.end(); devit++)
			{
			std::sort(devit->second.begin(), devit->second.end());
			}
		}
	}
}
