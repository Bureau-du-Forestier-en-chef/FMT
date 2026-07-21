/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTSchedule.h"
#include "FMTConstraint.h"
#include "FMTExceptionHandler.h"

namespace Core{

FMTSchedule::FMTSchedule(const int& lperiod,const FMTObject& obj, const bool& luselock) :FMTObject(obj), period(lperiod), uselock(luselock), elements() {}

FMTSchedule::FMTSchedule():FMTObject(),period(), uselock(false),elements(){}

FMTSchedule::FMTSchedule(const int& lperiod, std::vector<FMTSchedule>& schedules) : FMTObject(),period(lperiod), uselock(false),elements()
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

FMTSchedule::FMTSchedule(int lperiod, std::map<FMTAction, std::map<FMTDevelopment, std::vector<double>>> mapping) : FMTObject(),period(lperiod), uselock(false),elements(mapping)
    {

    }

FMTSchedule::FMTSchedule(int lperiod, const std::map<FMTAction, std::map<FMTDevelopment, std::map<int, double>>>& mapping) : FMTObject(),period(lperiod), uselock(false), elements()
	{
	for (std::map<FMTAction, std::map<FMTDevelopment, std::map<int, double>>>::const_iterator its = mapping.begin(); its != mapping.end(); its++)
		{
		elements[its->first] = std::map<FMTDevelopment, std::vector<double>>();
		for (std::map<FMTDevelopment, std::map<int, double>>::const_iterator elits = its->second.begin(); elits != its->second.end(); elits++)
			{
			elements[its->first][elits->first] = std::vector<double>();
			for (std::map<int, double>::const_iterator valit = elits->second.begin(); valit!= elits->second.end(); valit++)
				{
				elements[its->first][elits->first].push_back(valit->second);
				}
			}
		}
	}

FMTSchedule::FMTSchedule(const FMTSchedule& rhs) : FMTObject(rhs),period(rhs.period), uselock(rhs.uselock),elements(rhs.elements)
    {

    }

FMTSchedule& FMTSchedule::operator = (const FMTSchedule& rhs)
    {
    if (this!=&rhs)
        {
		FMTObject::operator=(rhs);
		uselock = rhs.uselock;
        elements = rhs.elements;
        period = rhs.period;
        }
    return *this;
    }

bool FMTSchedule::sameElements(const FMTSchedule& rhs) const
	{
	try {
		if (elements.size() != rhs.elements.size())
		{
			return false;
		}
		for (std::map<FMTAction, std::map<FMTDevelopment, std::vector<double>>>::const_iterator actit = elements.begin(); actit != elements.end(); actit++)
		{
			std::map<FMTAction, std::map<FMTDevelopment, std::vector<double>>>::const_iterator rhsactit = rhs.elements.find(actit->first);
			if (rhsactit == rhs.elements.end())
			{
				return false;
			}
			if (actit->second.size() != rhsactit->second.size())
			{
				return false;
			}
			for (std::map<FMTDevelopment, std::vector<double>>::const_iterator devit = actit->second.begin(); devit != actit->second.end(); devit++)
			{
				std::map<FMTDevelopment, std::vector<double>>::const_iterator rhsdevit = rhsactit->second.find(devit->first);
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
	}catch (...)
		{
		_exhandler->raiseFromCatch("", "FMTSchedule::sameElements", __LINE__, __FILE__, Core::FMTsection::Schedule);
		}
	return true;
	}

bool FMTSchedule::operator == (const FMTSchedule& rhs) const
	{
	return (period == rhs.period&& uselock ==rhs.uselock && sameElements(rhs));
	}

bool FMTSchedule::operator != (const FMTSchedule& rhs) const
	{
	return (!(*this==rhs));
	}

FMTSchedule& FMTSchedule::operator += (const FMTSchedule& rhs)
    {
	try{
    for(std::map<FMTAction, std::map<FMTDevelopment, std::vector<double>>>::const_iterator actit = rhs.elements.begin(); actit != rhs.elements.end(); actit++)
        {
        if (elements.find(actit->first)==elements.end())
            {
            elements[actit->first] = actit->second;
            }else{
             for(std::map<FMTDevelopment, std::vector<double>>::const_iterator devit = actit->second.begin(); devit != actit->second.end(); devit++)
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
	}
	catch (...)
	{
		_exhandler->raiseFromCatch("", "FMTSchedule::operator+=", __LINE__, __FILE__, Core::FMTsection::Schedule);
	}
    return *this;
    }

FMTSchedule FMTSchedule::operator + (const FMTSchedule& rhs) const
    {
    FMTSchedule newSchedule(*this);
	try {
		newSchedule += rhs;
	}catch (...)
		{
		_exhandler->raiseFromCatch("", "FMTSchedule::operator+", __LINE__, __FILE__, Core::FMTsection::Schedule);
		}
    return newSchedule;
    }


FMTSchedule::operator std::string() const
    {
	std::string line = "";
	try{
    for(std::map<FMTAction, std::map<FMTDevelopment, std::vector<double>>>::const_iterator actit = elements.begin(); actit != elements.end(); actit++)
        {
        for(std::map<FMTDevelopment, std::vector<double>>::const_iterator devit = actit->second.begin(); devit != actit->second.end(); devit++)
            {
            const FMTDevelopment* dev = &devit->first;
			for (const double & value : devit->second)
				{
				line += std::string(dev->getMask()) + " " + std::to_string(dev->getAge()) + " " + std::to_string(value);
				if (uselock)
					{
					line += " " + std::to_string(dev->getLock());
					}
				line += +" " + actit->first.getName() + " " + std::to_string(period);
				line+=+"\n";
				}
            }
		}
	}
	catch (...)
	{
		_exhandler->raiseFromCatch("", "FMTSchedule::operatorstd::string()", __LINE__, __FILE__, Core::FMTsection::Schedule);
	}
		return line;
	}

	void FMTSchedule::setUseLock(const bool& lock)
	{
		uselock = lock;
	}

	void FMTSchedule::clean()
	{
		try {
			if (!uselock)
			{
				for (std::map<FMTAction, std::map<FMTDevelopment, std::vector<double>>>::iterator actit = elements.begin(); actit != elements.end(); actit++)
				{
					for (std::map<FMTDevelopment, std::vector<double>>::iterator devit = actit->second.begin(); devit != actit->second.end(); devit++)
						{
						std::vector<double>cleaned;
						for (const double& value : devit->second)
							{
							if (value>0)
								{
								cleaned.push_back(value);
								}
							}
						devit->second = cleaned;
						}
				}
				
			}
			
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTSchedule::clean", __LINE__, __FILE__, Core::FMTsection::Schedule);
		}
	}

	void FMTSchedule::addEvent(const Core::FMTDevelopment& dev, const double& area, const Core::FMTAction& action)
	{
		try {
			iterator actit = elements.find(action);
			if (actit ==elements.end())
				{
				actit = elements.insert(std::pair<Core::FMTAction, std::map<Core::FMTDevelopment, std::vector<double>>>(action, std::map<Core::FMTDevelopment, std::vector<double>>())).first;
				}
			std::map<FMTDevelopment, std::vector<double>>::iterator devit = actit->second.end(); 
			if (uselock)
			{
				devit = actit->second.find(dev);
				if (devit == actit->second.end())
					{
					devit = actit->second.insert(std::pair<Core::FMTDevelopment, std::vector<double>>(dev, std::vector<double>(1, 0))).first;
					}
				devit->second[0] += area;
			}else {
				const Core::FMTDevelopment lockout = dev.clearLock();
				const int leveltarget = dev.getLock();
				devit = actit->second.find(lockout);
				if (devit == actit->second.end())
					{
					devit = actit->second.insert(std::pair<Core::FMTDevelopment, std::vector<double>>(lockout, std::vector<double>(1, 0))).first;
					}
				if (devit->second.size()< leveltarget + 1)
					{
					devit->second.resize(leveltarget + 1, 0.0);
					}
				devit->second[leveltarget] += area;
			}
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTSchedule::addEvent", __LINE__, __FILE__, Core::FMTsection::Schedule);
		}

	}


	double FMTSchedule::actionArea(const FMTAction& action) const
	{
		double value = 0;
		try{
		if (elements.find(action) != elements.end())
		{
			for (std::map<FMTDevelopment, std::vector<double>>::const_iterator devit = elements.at(action).begin(); devit != elements.at(action).end(); devit++)
			{
				for (const double& val : devit->second)
				{
					value += val;
				}
			}
		}
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTSchedule::actionArea", __LINE__, __FILE__, Core::FMTsection::Schedule);
		}
		return value;
	}


	double FMTSchedule::area() const
	{
		double value = 0;
		try{
		for (std::map<FMTAction, std::map<FMTDevelopment, std::vector<double>>>::const_iterator actit = elements.begin(); actit != elements.end(); actit++)
		{
			for (std::map<FMTDevelopment, std::vector<double>>::const_iterator devit = actit->second.begin(); devit != actit->second.end(); devit++)
			{
				for (const double& val : devit->second)
				{
					value += val;
				}
			}
		}
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTSchedule::area", __LINE__, __FILE__, Core::FMTsection::Schedule);
		}
		return value;
	}

	std::vector<boost::unordered_set<FMTDevelopment>> FMTSchedule::getOperabilities(const std::vector<FMTAction>& actions) const
	{
		std::vector<boost::unordered_set<FMTDevelopment>>table(actions.size(),boost::unordered_set<FMTDevelopment>());
		try {
			size_t actionid = 0;
			for (const FMTAction& action : actions)
				{
				std::map<FMTAction, std::map<FMTDevelopment, std::vector<double>>>::const_iterator actit = elements.find(action);
				if (actit != elements.end())
					{
					for (std::map<FMTDevelopment, std::vector<double>>::const_iterator devit = actit->second.begin(); devit != actit->second.end(); ++devit)
						{
						if (table.at(actionid).find(devit->first) == table.at(actionid).end())
							{
							table[actionid].insert(devit->first);
							}
						}
					}
				++actionid;
				}
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTSchedule::getOperabilities", __LINE__, __FILE__, Core::FMTsection::Schedule);
			}
		return table;
	}

	bool FMTSchedule::operated(const FMTAction& action,
		const FMTDevelopment& development) const
	{
		bool value = false;
		try{
		std::map<FMTAction, std::map<FMTDevelopment, std::vector<double>>>::const_iterator actit = elements.find(action);
		if (actit != elements.end())
		{
			std::map<FMTDevelopment, std::vector<double>>::const_iterator devit;
			if (uselock||action.doRespectLock())
			{
				devit = actit->second.find(development);
			}
			else {
				devit = actit->second.find(development.clearLock());
			}
			if (devit != actit->second.end())
			{
				value = true;
			}
		}
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTSchedule::operated", __LINE__, __FILE__, Core::FMTsection::Schedule);
		}
		return value;
	}

	bool FMTSchedule::empty() const
	{
		return elements.empty();
	}

	void FMTSchedule::sort()
	{
		try{
		for (std::map<FMTAction, std::map<FMTDevelopment, std::vector<double>>>::iterator actit = elements.begin(); actit != elements.end(); actit++)
		{
			for (std::map<FMTDevelopment, std::vector<double>>::iterator devit = actit->second.begin(); devit != actit->second.end(); devit++)
			{
				std::sort(devit->second.begin(), devit->second.end());
			}
		}
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTSchedule::sort", __LINE__, __FILE__, Core::FMTsection::Schedule);
		}
	}

	FMTSchedule FMTSchedule::presolve(
		const FMTMaskFilter& filter,
		const std::vector<FMTTheme>& newthemes,
		const std::vector<FMTAction>&presolvedaction) const
	{
		FMTSchedule newSchedule(*this);
		try {
			newSchedule.elements.clear();
			for (std::map<FMTAction, std::map<FMTDevelopment, std::vector<double>>>::const_iterator actit = elements.begin(); actit != elements.end(); actit++)
			{
				std::map<FMTDevelopment, std::vector<double>>newmapping;
				for (std::map<FMTDevelopment, std::vector<double>>::const_iterator devit = actit->second.begin(); devit != actit->second.end(); devit++)
				{
					FMTDevelopment newDev(devit->first);
					const Core::FMTMask& BASE = newDev.getMask();
					if (BASE.canPresolve(filter, newthemes))
						{
						newDev.setMask(newDev.getMask().presolve(filter, newthemes));
						newmapping[newDev] = devit->second;
					}else {
						_exhandler->raise(Exception::FMTexc::FMTignore,
							"Presolve Removed "+std::string(newDev)+" from the base solution",
							"FMTSchedule::presolve", __LINE__, __FILE__);
					}
					
				}
				if (!newmapping.empty())
				{
					std::vector<FMTAction>::const_iterator actfit = std::find_if(presolvedaction.begin(), presolvedaction.end(), FMTActionComparator(actit->first.getName()));
					if (actfit != presolvedaction.end())
					{
						newSchedule.elements[*actfit] = newmapping;
					}
				}
			}
		}catch (...)
			{
			_exhandler->raiseFromCatch("","FMTSchedule::presolve", __LINE__, __FILE__,Core::FMTsection::Schedule);
			}
	return newSchedule;
	}

	FMTSchedule FMTSchedule::postSolve(const FMTMaskFilter& filter,
		const std::vector<FMTTheme>& originalbasethemes, const std::vector<FMTAction>&originalbasebaseactions) const
	{
		FMTSchedule newSchedule(*this);
		try {
			newSchedule.elements.clear();
			for (std::map<FMTAction, std::map<FMTDevelopment, std::vector<double>>>::const_iterator actit = elements.begin(); actit != elements.end(); actit++)
			{
				std::map<FMTDevelopment, std::vector<double>>newmapping;
				for (std::map<FMTDevelopment, std::vector<double>>::const_iterator devit = actit->second.begin(); devit != actit->second.end(); devit++)
				{
					FMTDevelopment newDev(devit->first);

					newDev.setMask(newDev.getMask().postSolve(filter, originalbasethemes));
					newmapping[newDev] = devit->second;
				}
				if (!newmapping.empty())
				{
					std::vector<FMTAction>::const_iterator actfit = std::find_if(originalbasebaseactions.begin(), originalbasebaseactions.end(), FMTActionComparator(actit->first.getName()));
					if (actfit != originalbasebaseactions.end())
					{
						newSchedule.elements[*actfit] = newmapping;
					}
				}
			}
		}catch (...)
			{
			_exhandler->raiseFromCatch("","FMTSchedule::postSolve", __LINE__, __FILE__, Core::FMTsection::Schedule);
			}
		return newSchedule;
	}

	void FMTSchedule::setPeriod(const int& newperiod)
		{
			period = newperiod;
			try{
			for (std::map<FMTAction, std::map<FMTDevelopment, std::vector<double>>>::iterator actit = elements.begin();
				actit != elements.end(); actit++)
			{
				std::map<FMTDevelopment, std::vector<double>>newmapping;
				for (std::map<FMTDevelopment, std::vector<double>>::iterator devit = actit->second.begin();
					devit != actit->second.end(); devit++)
					{
					FMTDevelopment newDev(devit->first);
					newDev.setPeriod(newperiod);
					newmapping[newDev] = devit->second;
					}
				actit->second = newmapping;
			}
			}
			catch (...)
			{
				_exhandler->raiseFromCatch("", "FMTSchedule::setPeriod", __LINE__, __FILE__,Core::FMTsection::Schedule);
			}
		}

	FMTSchedule FMTSchedule::getNewSchedule(const double& factor) const
		{
		FMTSchedule newscedule(*this);
		try{
		for (std::map<FMTAction, std::map<FMTDevelopment, std::vector<double>>>::iterator actit = newscedule.elements.begin();
			actit != newscedule.elements.end(); actit++)
			{
				for (std::map<FMTDevelopment, std::vector<double>>::iterator devit = actit->second.begin();devit != actit->second.end(); devit++)
				{
					for (double& value : devit->second)
					{
						value *= factor;
					}

				}

			}
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTSchedule::getNewSchedule", __LINE__, __FILE__, Core::FMTsection::Schedule);
		}
		return newscedule;
		}

	bool FMTSchedule::isFuturConstraints(const std::vector<Core::FMTConstraint>& constraints) const
		{
		try{
		for (const Core::FMTConstraint& constraint : constraints)
			{
			if (constraint.acrossPeriod()&&constraint.getPeriodLowerBound()==period)
				{
				return true;
				}
			}
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTSchedule::isFuturConstraints", __LINE__, __FILE__, Core::FMTsection::Schedule);
		}
		return false;
		}

	std::map<FMTDevelopment, std::vector<double>>& FMTSchedule::operator[](const FMTAction& action)
	{
		return elements[action];
	}
	const std::map<FMTDevelopment, std::vector<double>>& FMTSchedule::at(const FMTAction& action) const
	{
		return elements.at(action);
	}
	size_t FMTSchedule::size() const
	{
		return elements.size();
	}
	FMTSchedule::iterator FMTSchedule::find(const FMTAction& actionkey)
	{
		return elements.find(actionkey);
	}

	FMTSchedule::const_iterator FMTSchedule::find(const FMTAction& actionkey) const
	{
		return elements.find(actionkey);
	}

	FMTSchedule::iterator FMTSchedule::begin()
	{
		return elements.begin();
	}

	FMTSchedule::const_iterator FMTSchedule::begin() const
	{
		return elements.begin();
	}

	FMTSchedule::iterator  FMTSchedule::end()
	{
		return elements.end();
	}

	FMTSchedule::const_iterator FMTSchedule::end() const
	{
		return elements.end();
	}

}

BOOST_CLASS_EXPORT_IMPLEMENT(Core::FMTSchedule)
