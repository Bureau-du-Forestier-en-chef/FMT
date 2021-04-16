/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTaction.h"

namespace Core{


FMTaction::FMTaction():FMTlist<FMTspec>(),
				aggregates(),
                partials(),
				agelowerbound(), ageupperbound(), periodlowerbound(), periodupperbound(),
                name(""),
                lock(false),
                reset(false){}

    FMTaction::FMTaction(const std::string& lname): FMTlist<FMTspec>(),
						aggregates(),
                        partials(),
						agelowerbound(), ageupperbound(), periodlowerbound(), periodupperbound(),
                        name(lname),
                        lock(false),
                        reset(false){}
    FMTaction::FMTaction(const std::string& lname, const bool& lock,const bool& reset): FMTlist<FMTspec>(), aggregates(), partials(),
		agelowerbound(), ageupperbound(), periodlowerbound(), periodupperbound(),
		name(lname),lock(lock),reset(reset)
        {

        }
	void FMTaction::push_aggregate(const std::string& aggregate)
		{
		aggregates.push_back(aggregate);
		}

    void FMTaction::push_partials(const std::string& yield)
        {
        partials.push_back(yield);
        }
	FMTaction::FMTaction(const FMTaction& rhs) : FMTlist<FMTspec>(rhs),
		aggregates(rhs.aggregates),
		partials(rhs.partials),
		agelowerbound(rhs.agelowerbound), ageupperbound(rhs.ageupperbound), periodlowerbound(rhs.periodlowerbound), periodupperbound(rhs.periodupperbound),
                        name(rhs.name),
                        lock(rhs.lock),
                        reset(rhs.reset)
        {

        }
    FMTaction& FMTaction::operator = (const FMTaction& rhs)
        {
        if (this!=&rhs)
            {
			FMTlist<FMTspec>::operator = (rhs);
            name = rhs.name;
            lock = rhs.lock;
            reset = rhs.reset;
            partials = rhs.partials;
			aggregates = rhs.aggregates;
			agelowerbound = rhs.agelowerbound;
			ageupperbound = rhs.ageupperbound;
			periodlowerbound = rhs.periodlowerbound;
			periodupperbound = rhs.periodupperbound;
            }
        return *this;
        }

	void FMTaction::setbounds()
		{
		try {
			ageupperbound = 0;
			agelowerbound = std::numeric_limits<int>::max();
			periodupperbound = 0;
			periodlowerbound = std::numeric_limits<int>::max();
			std::vector<std::pair<FMTmask, FMTspec>>::const_iterator datait = this->begin();
			for (size_t id = 0; id < this->size(); ++id)
			{
				if (!datait->second.emptyage())
				{
					int upperbound = datait->second.getageupperbound();
					if (upperbound > ageupperbound)
					{
						ageupperbound = upperbound;
					}
					int lowerbound = datait->second.getagelowerbound();
					if (lowerbound < agelowerbound)
					{
						agelowerbound = lowerbound;
					}
				}
				else {
					ageupperbound = std::numeric_limits<int>::max();
					agelowerbound = 0;
				}
				if (!datait->second.emptyperiod())
				{
					int upperbound = datait->second.getperiodupperbound();
					if (upperbound > periodupperbound)
					{
						periodupperbound = upperbound;
					}
					int lowerbound = datait->second.getperiodlowerbound();
					if (lowerbound < periodlowerbound)
					{
						periodlowerbound = lowerbound;
					}
				}
				else {
					periodupperbound = std::numeric_limits<int>::max();
					periodlowerbound = 0;
				}
				++datait;
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("for action "+this->getname(),
				"FMTaction::setbounds", __LINE__, __FILE__,Core::FMTsection::Action);
			}
		}

	void FMTaction::update()
		{
		try {
			FMTlist<FMTspec>::update();
			this->setbounds();
		}catch (...)
			{
			_exhandler->raisefromcatch("for action " + this->getname(),
				"FMTaction::update", __LINE__, __FILE__, Core::FMTsection::Action);
			}
		}


	

	std::vector<std::string>FMTaction::getpartials() const
        {
        return partials;
        }


    FMTaction::operator std::string() const
        {
		const std::string resetyield = (reset) ? "Y" : "N";
		const std::string locked = (lock) ? "" : " _LOCKEXEMPT";
		std::string line="*ACTION "+name +" "+resetyield+locked+"\n";
        line+="*OPERABLE "+name+"\n";
		for (const auto& yieldobject : *this)
			{
			line += std::string(yieldobject.first) + " ";
			line += std::string(yieldobject.second);
			line += "\n";
			}
        if(!partials.empty())
            {
            line+="*PARTIAL "+name+"\n";
            int lid = 0;
            for(const std::string& partial : partials)
                {
                line+=partial+" ";
                if (lid==20)
                    {
                    line+="\n";
                    lid=0;
                    }
                ++lid;
                }
            }
        return line;
        }

bool FMTaction::useyield(const std::string& yldname) const
	{
	for (const auto& speclitit : *this)
		{
		if (!speclitit.second.emptyylds())
			{
			const std::vector<std::string> specyields = speclitit.second.getylds();
			if (std::find(specyields.begin(), specyields.end(),yldname)!= specyields.end())
				{
				return true;
				}
			}

		}
	return false;
	}

std::vector<std::string>FMTaction::getaggregates() const
	{
	return aggregates;
	}


bool FMTaction::operator < (const FMTaction& rhs) const
    {
    return name < rhs.name;
    }
bool FMTaction::operator == (const FMTaction& rhs) const
    {
	return (name == rhs.name &&
		partials == rhs.partials &&
		lock == rhs.lock &&
		reset == rhs.reset &&
		FMTlist<FMTspec>::operator == (rhs));

    }
bool FMTaction::operator != (const FMTaction& rhs) const
    {
    return !(*this== rhs);
    }

bool FMTaction::partial(const std::string& yield) const
	{
	return (std::find(partials.begin(), partials.end(),yield)!=partials.end());
	}

FMTactioncomparator::FMTactioncomparator(std::string name, bool lcheckaggregate) : action_name(name),checkaggregate(lcheckaggregate) {}

bool FMTactioncomparator::operator()(const FMTaction& action) const
	{
	return (action.name == action_name || (checkaggregate && 
		std::find(action.aggregates.begin(), action.aggregates.end(),action_name)!= action.aggregates.end()));
	}

std::vector<const FMTaction*>FMTactioncomparator::getallaggregates(const std::vector<FMTaction>&actions,bool aggregateonly) const
	{
	std::vector<const FMTaction*>actionsptr;
	std::vector<FMTaction>::const_iterator actit = actions.begin();
	while (actit!= actions.end())
		{
		if ((!aggregateonly && actit->name == action_name) || (std::find(actit->aggregates.begin(), actit->aggregates.end(), action_name) != actit->aggregates.end()))
			{
			actionsptr.push_back(&(*actit));
			}
		++actit;
		}
	return actionsptr;
	}

std::unordered_set<int>FMTactioncomparator::getallaggregatesset(const std::vector<FMTaction>&actions, bool aggregateonly) const
{
	std::unordered_set<int>actionsptr;
	std::vector<FMTaction>::const_iterator actit = actions.begin();
	int actionid = 0;
	while (actit != actions.end())
	{
		if ((!aggregateonly && actit->name == action_name) || (std::find(actit->aggregates.begin(), actit->aggregates.end(), action_name) != actit->aggregates.end()))
		{
			actionsptr.insert(actionid);
		}
		++actit;
	++actionid;
	}
	return actionsptr;
}


FMTaction FMTaction::presolve(const FMTmask& basemask,
	const std::vector<FMTtheme>& originalthemes,
	const FMTmask& presolvedmask,
	const std::vector<FMTtheme>& newthemes) const
	{
	FMTaction newaction(*this);
	try {
		newaction.presolvelist(basemask, originalthemes, presolvedmask, newthemes);
		newaction.update();
	}catch (...)
		{
		_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,"for action "+this->getname(),
			"FMTaction::presolve", __LINE__, __FILE__, Core::FMTsection::Action);
		}
	return newaction;
	}

}

BOOST_CLASS_EXPORT_IMPLEMENT(Core::FMTaction)