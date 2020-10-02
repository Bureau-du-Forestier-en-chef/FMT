/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTschedule.h"

namespace Core{

FMTschedule::FMTschedule():FMTobject(),period(),elements(){}

FMTschedule::FMTschedule(const int& lperiod, std::vector<FMTschedule>& schedules) : FMTobject(),period(lperiod),elements()
	{
	if (!schedules.empty())
		{
		elements = schedules.at(0).elements;
		for (size_t scheduleid = 1; scheduleid < schedules.size();++scheduleid)
			{
			*this += schedules.at(scheduleid);
			}
		}
	}

FMTschedule::FMTschedule(int lperiod, std::map<FMTaction, std::map<FMTdevelopment, std::vector<double>>> mapping) : FMTobject(),period(lperiod),elements(mapping)
    {

    }

FMTschedule::FMTschedule(int lperiod, const std::map<FMTaction, std::map<FMTdevelopment, std::map<int, double>>>& mapping) : FMTobject(),period(lperiod), elements()
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

FMTschedule::FMTschedule(const FMTschedule& rhs) : FMTobject(rhs),period(rhs.period),elements(rhs.elements)
    {

    }

FMTschedule& FMTschedule::operator = (const FMTschedule& rhs)
    {
    if (this!=&rhs)
        {
		FMTobject::operator=(rhs);
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
		if (elements.find(action) != elements.end())
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


	double FMTschedule::area() const
	{
		double value = 0;
		for (std::map<FMTaction, std::map<FMTdevelopment, std::vector<double>>>::const_iterator actit = elements.begin(); actit != elements.end(); actit++)
		{
			for (std::map<FMTdevelopment, std::vector<double>>::const_iterator devit = actit->second.begin(); devit != actit->second.end(); devit++)
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
			}
			else {
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

	FMTschedule FMTschedule::presolve(
		const FMTmask& presolvedmask,
		const std::vector<FMTtheme>& newthemes,
		const std::vector<FMTaction>&presolvedaction) const
	{
		FMTschedule newschedule(*this);
		try {
			newschedule.elements.clear();
			for (std::map<FMTaction, std::map<FMTdevelopment, std::vector<double>>>::const_iterator actit = elements.begin(); actit != elements.end(); actit++)
			{
				std::map<FMTdevelopment, std::vector<double>>newmapping;
				for (std::map<FMTdevelopment, std::vector<double>>::const_iterator devit = actit->second.begin(); devit != actit->second.end(); devit++)
				{
					FMTdevelopment newdev(devit->first);

					newdev.mask = newdev.mask.presolve(presolvedmask, newthemes);
					newmapping[newdev] = devit->second;
				}
				if (!newmapping.empty())
				{
					std::vector<FMTaction>::const_iterator actfit = std::find_if(presolvedaction.begin(), presolvedaction.end(), FMTactioncomparator(actit->first.getname()));
					if (actfit != presolvedaction.end())
					{
						newschedule.elements[*actfit] = newmapping;
					}
				}
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("","FMTschedule::presolve", __LINE__, __FILE__, _section);
			}
	return newschedule;
	}

	FMTschedule FMTschedule::postsolve(const FMTmask& presolvedmask,
		const std::vector<FMTtheme>& originalbasethemes, const std::vector<FMTaction>&originalbasebaseactions) const
	{
		FMTschedule newschedule(*this);
		try {
			newschedule.elements.clear();
			for (std::map<FMTaction, std::map<FMTdevelopment, std::vector<double>>>::const_iterator actit = elements.begin(); actit != elements.end(); actit++)
			{
				std::map<FMTdevelopment, std::vector<double>>newmapping;
				for (std::map<FMTdevelopment, std::vector<double>>::const_iterator devit = actit->second.begin(); devit != actit->second.end(); devit++)
				{
					FMTdevelopment newdev(devit->first);

					newdev.mask = newdev.mask.postsolve(presolvedmask, originalbasethemes);
					newmapping[newdev] = devit->second;
				}
				if (!newmapping.empty())
				{
					std::vector<FMTaction>::const_iterator actfit = std::find_if(originalbasebaseactions.begin(), originalbasebaseactions.end(), FMTactioncomparator(actit->first.getname()));
					if (actfit != originalbasebaseactions.end())
					{
						newschedule.elements[*actfit] = newmapping;
					}
				}
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("","FMTschedule::postsolve", __LINE__, __FILE__, _section);
			}
		return newschedule;
	}

	void FMTschedule::setperiod(const int& newperiod)
		{
			period = newperiod;
			for (std::map<FMTaction, std::map<FMTdevelopment, std::vector<double>>>::iterator actit = elements.begin();
				actit != elements.end(); actit++)
			{
				std::map<FMTdevelopment, std::vector<double>>newmapping;
				for (std::map<FMTdevelopment, std::vector<double>>::iterator devit = actit->second.begin();
					devit != actit->second.end(); devit++)
					{
					FMTdevelopment newdev(devit->first);
					newdev.period = newperiod;
					newmapping[newdev] = devit->second;
					}
				actit->second = newmapping;
			}
		}

	FMTschedule FMTschedule::getnewschedule(const double& factor) const
		{
		FMTschedule newscedule(*this);
		for (std::map<FMTaction, std::map<FMTdevelopment, std::vector<double>>>::iterator actit = newscedule.elements.begin();
			actit != newscedule.elements.end(); actit++)
			{
				for (std::map<FMTdevelopment, std::vector<double>>::iterator devit = actit->second.begin();devit != actit->second.end(); devit++)
				{
					for (double& value : devit->second)
					{
						value *= factor;
					}

				}

			}
		return newscedule;
		}

	std::map<FMTdevelopment, std::vector<double>>& FMTschedule::operator[](const FMTaction& action)
	{
		return elements[action];
	}
	const std::map<FMTdevelopment, std::vector<double>>& FMTschedule::at(const FMTaction& action) const
	{
		return elements.at(action);
	}
	size_t FMTschedule::size() const
	{
		return elements.size();
	}
	FMTschedule::iterator FMTschedule::find(const FMTaction& actionkey)
	{
		return elements.find(actionkey);
	}

	FMTschedule::const_iterator FMTschedule::find(const FMTaction& actionkey) const
	{
		return elements.find(actionkey);
	}

	FMTschedule::iterator FMTschedule::begin()
	{
		return elements.begin();
	}

	FMTschedule::const_iterator FMTschedule::begin() const
	{
		return elements.begin();
	}

	FMTschedule::iterator  FMTschedule::end()
	{
		return elements.end();
	}

	FMTschedule::const_iterator FMTschedule::end() const
	{
		return elements.end();
	}

}

BOOST_CLASS_EXPORT_IMPLEMENT(Core::FMTschedule)
