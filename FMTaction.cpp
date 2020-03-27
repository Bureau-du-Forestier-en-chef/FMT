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
		ageupperbound = 0;
		agelowerbound = std::numeric_limits<int>::max();
		periodupperbound = 0;
		periodlowerbound = std::numeric_limits<int>::max();
		std::vector<std::pair<FMTmask,FMTspec>>::const_iterator datait = this->begin();
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
					}else{
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
				}else {
					periodupperbound = std::numeric_limits<int>::max();
					periodlowerbound = 0;
					}
				++datait;
			}
		}

	void FMTaction::update()
		{
		FMTlist<FMTspec>::update();
		this->setbounds();
		}


	

	std::vector<std::string>FMTaction::getpartials() const
        {
        return partials;
        }

	/*bool FMTaction::inperiod() const
		{
		std::vector<std::pair<FMTmask, FMTspec>>::const_iterator datait = this->begin();
		for (size_t id = 0; id < this->size(); ++id)
			{
			if (!datait->second.emptyage())
				{
				if (datait->second.getageupperbound() != 0)
					{
					return false;
					}
			}else {
				return false;
				}
			++datait;
			}
		return true;
		}*/

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

FMTaction FMTaction::presolve(const FMTmask& basemask,
	const std::vector<FMTtheme>& originalthemes,
	const FMTmask& presolvedmask,
	const std::vector<FMTtheme>& newthemes) const
	{
	FMTaction newaction(*this);
	newaction.presolvelist(basemask, originalthemes, presolvedmask, newthemes);
	newaction.update();
	return newaction;
	}

}

BOOST_CLASS_EXPORT_IMPLEMENT(Core::FMTaction);