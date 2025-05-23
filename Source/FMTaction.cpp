/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTaction.h"
#include "FMTexceptionhandler.h"
#include <bitset>
#include <boost/algorithm/string/split.hpp>
#include <algorithm>

namespace Core{

	FMTaction& FMTaction::operator+=(const FMTaction& OtherAction)
		{
		try {
			if (empty())
				{
				this->reset = OtherAction.reset;
				this->lock = OtherAction.lock;
				}
			if (OtherAction.reset)
				{
				this->reset = true;
				if (!this->partials.empty())
					{
					_exhandler->raise(Exception::FMTexc::FMTignore, "Losing partiel on "+getname()+" with "+OtherAction.getname(),
						"FMTaction::operator+=", __LINE__, __FILE__, Core::FMTsection::Action);
					this->partials.clear();
					}
			}else {
				for (const std::string& partial : OtherAction.partials)
					{
						if (std::find(this->partials.begin(), this->partials.end(), partial) == this->partials.end())
						{
							this->partials.push_back(partial);
						}

					}
				}
			if (!OtherAction.lock)
				{
				this->lock = false;
				}
			
			FMTlist<FMTspec>::operator+=(OtherAction);
			for (const std::string& Aggregate : OtherAction.aggregates)
				{
				if (std::find(aggregates.begin(),aggregates.end(), Aggregate)== aggregates.end())
					{
					aggregates.push_back(Aggregate);
					}

				}

		}catch (...)
			{
			_exhandler->raisefromcatch("for action " + this->getname(),
				"FMTaction::operator+=", __LINE__, __FILE__, Core::FMTsection::Action);
			}
		return *this;
		}


FMTaction::FMTaction():FMTlist<FMTspec>(),
				aggregates(),
                partials(),
				agelowerbound(), ageupperbound(), periodlowerbound(), periodupperbound(),
                name(""),
                lock(false),
                reset(false),
				series() {}

    FMTaction::FMTaction(const std::string& lname): FMTlist<FMTspec>(),
						aggregates(),
                        partials(),
						agelowerbound(), ageupperbound(), periodlowerbound(), periodupperbound(),
                        name(lname),
                        lock(false),
                        reset(false),
						series() {}
    FMTaction::FMTaction(const std::string& lname, const bool& lock,const bool& reset): FMTlist<FMTspec>(), aggregates(), partials(),
		agelowerbound(), ageupperbound(), periodlowerbound(), periodupperbound(),
		name(lname),lock(lock),reset(reset), series()
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
                        reset(rhs.reset),
						series(rhs.series)
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
			series = rhs.series;
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

	bool FMTaction::isallowedinserie(const std::vector<std::string>& seriemask) const
	{
		try {
			if (!seriemask.empty())
			{
				for (const std::vector<std::string>& serie : series)
				{
					std::vector<std::string>::const_iterator firstelement = std::find_first_of(seriemask.begin(), seriemask.end(),serie.begin(),serie.end());
					if (firstelement!= seriemask.end()&&
						std::equal(firstelement, seriemask.end(), serie.begin()))
					{
						return true;
					}
				}
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("for action " + this->getname(),
				"FMTaction::isallowedinserie", __LINE__, __FILE__, Core::FMTsection::Action);
			}
		return false;
	}

	std::vector<std::string>FMTaction::getseriesnames() const
	{
		std::vector<std::string>seriesnames;
		try {
			for (const std::vector<std::string>& serie : series)
				{
				std::string seriename;
				for (const std::string& val : serie)
					{
					if (val != getname())
						{
						seriename += val + " -> ";
						}
					}
				if (!seriename.empty())
					{
					seriename += getname();
					}
				seriesnames.push_back(seriename);
				}
		}catch (...)
			{
			_exhandler->raisefromcatch("for action " + this->getname(),
				"FMTaction::getseriesnames", __LINE__, __FILE__, Core::FMTsection::Action);
			}
		return seriesnames;
	}

	size_t FMTaction::getlargestseriesize() const
	{
		size_t sizeofserie = 0;
		try {
			if (ispartofaserie())
				{
				for (const std::vector<std::string>& serie : series)
					{
					sizeofserie = std::max(sizeofserie, serie.size());
					}
				}
		}catch (...)
			{
			_exhandler->raisefromcatch("for action " + this->getname(),
				"FMTaction::getlargestseriesize", __LINE__, __FILE__, Core::FMTsection::Action);
			}
		return sizeofserie;
	}


	void FMTaction::setseries(std::vector<std::vector<std::string>> seriesnames)
	{
		try {
			series.clear();
			for (std::vector<std::string> actionsname : seriesnames)
				{
				std::vector<std::string>::iterator ait = std::find(actionsname.begin(), actionsname.end(), getname());
				if (ait!= actionsname.end())
					{
					while (ait != actionsname.end())
						{
							if (ait != actionsname.end() && std::distance(actionsname.begin(), ait + 1) > 1)//Ok in serie
								{
								series.push_back(std::vector<std::string>(actionsname.begin(), ait + 1));
								}
							if (!actionsname.empty())
								{
								actionsname.erase(actionsname.begin(), ait + 1);
								}
						ait = std::find(actionsname.begin(), actionsname.end(), getname());
						}
					}
				}
		}catch (...){
			_exhandler->raisefromcatch("for action " + this->getname(),
				"FMTaction::setseries", __LINE__, __FILE__, Core::FMTsection::Action);
		}
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
	try {
		for (const auto& speclitit : *this)
		{
			if (!speclitit.second.emptyylds())
			{
				const std::vector<std::string> specyields = speclitit.second.getylds();
				if (std::find(specyields.begin(), specyields.end(), yldname) != specyields.end())
				{
					return true;
				}
			}

		}
	}
	catch (...)
	{
		_exhandler->raisefromcatch("for action " + this->getname(),
			"FMTaction::useyield", __LINE__, __FILE__, Core::FMTsection::Action);
	}
	return false;
	}

std::vector<std::string>FMTaction::getaggregates() const
	{
	return aggregates;
	}

void FMTaction::setaggregates(const std::vector<std::string>& p_aggregates)
	{
	aggregates = p_aggregates;
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

std::vector<std::string>FMTaction::getGCBMactiondef() const
{
	std::vector<std::string> allvalues;
	try {
		for (const std::string& aggregate : aggregates)
		{
			if (aggregate.find("~GCBM:") != std::string::npos)
			{
				boost::split(allvalues, aggregate, boost::is_any_of(":"), boost::token_compress_on);
				return allvalues;
			}
		}
		_exhandler->raise(Exception::FMTexc::FMTempty_action, "Missing GCBM action for action " + this->getname(),
			"FMTaction::getGCBMactiondef", __LINE__, __FILE__, Core::FMTsection::Action);
	}
	catch (...)
	{
		_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "for action " + this->getname(),
			"FMTaction::getGCBMactiondef", __LINE__, __FILE__, Core::FMTsection::Action);
	}
	return allvalues;
}

int FMTaction::getGCBMactionid() const
{
	try {
		return std::atoi(getGCBMactiondef().at(1).c_str());
	}
	catch (...)
	{
		_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "for action " + this->getname(),
			"FMTaction::getGCBMactionid", __LINE__, __FILE__, Core::FMTsection::Action);
	}
	return 0;
}

std::string FMTaction::getGCBMactionname() const
{
	try {
		return getGCBMactiondef().at(2).c_str();
	}
	catch (...)
	{
		_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "for action " + this->getname(),
			"FMTaction::getGCBMactionname", __LINE__, __FILE__, Core::FMTsection::Action);
	}
	return std::string();
}

bool FMTaction::notUse() const
	{
	size_t count = 0;
	for (const auto& value : *this)
		{
		if (!value.second.emptyperiod()&&
			value.second.getperiodlowerbound() == 0 &&
			value.second.getperiodupperbound() == 0)
			{
			++count;
			}
		}
	return (size() == count);
	}

bool FMTaction::isPartOf(const std::string& p_name) const
	{
	return (name == p_name ||
		std::find(aggregates.begin(), aggregates.end(), p_name) != aggregates.end());
	}

std::vector<Core::FMTaction>FMTaction::split(const std::vector<Core::FMTmask>& p_mask,
											const std::vector<Core::FMTtheme>& p_themes) const
{
	std::vector<Core::FMTaction>Splitted;
	try {
		for (const Core::FMTmask& MASK : p_mask)
			{
			std::string NewName = std::string(MASK);
			NewName.erase(std::remove(NewName.begin(), NewName.end(), ' '), NewName.end());
			NewName.erase(std::remove(NewName.begin(), NewName.end(), '?'), NewName.end());
			NewName = getname() + "_" + NewName;
			Core::FMTaction NewAction(NewName,lock,reset);
			for (const auto& data : *this)
				{
				Core::FMTmask subMAsk =  Core::FMTmask(std::string(data.first), p_themes);
				if (!subMAsk.isnotthemessubset(MASK,p_themes))
					{
					subMAsk = MASK.getintersect(subMAsk);
					subMAsk.update(p_themes);
					NewAction.push_back(subMAsk, data.second);
					}
				}
			NewAction.aggregates = aggregates;
			NewAction.aggregates.insert(NewAction.aggregates.begin(),getname());
			NewAction.update();
			Splitted.push_back(NewAction);
			}
	}catch (...)
		{
		_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "for action " + this->getname(),
			"FMTaction::split", __LINE__, __FILE__, Core::FMTsection::Action);
		}
	return Splitted;
}

void FMTaction::presolveRef(const FMTmaskfilter& p_filter,
	const std::vector<FMTtheme>& p_originalthemes,
	std::vector<FMTtheme>& p_newthemes, bool p_compressdata)
{
	try {
		presolvelist(p_filter, p_originalthemes, p_newthemes);
		if (p_compressdata)
			{
			compressmasks(p_newthemes);
			}
		update();
	}catch (...)
		{
		_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "for action " + this->getname(),
			"FMTaction::presolveRef", __LINE__, __FILE__, Core::FMTsection::Action);
		}
}


FMTaction FMTaction::presolve(const FMTmaskfilter& filter,
	const std::vector<FMTtheme>& originalthemes,
	std::vector<FMTtheme>& newthemes,bool compressdata) const
	{
	FMTaction newaction(*this);
	try {
		newaction.presolveRef(filter, originalthemes, newthemes, compressdata);
	}catch (...)
		{
		_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,"for action "+this->getname(),
			"FMTaction::presolve", __LINE__, __FILE__, Core::FMTsection::Action);
		}
	return newaction;
	}

}

BOOST_CLASS_EXPORT_IMPLEMENT(Core::FMTaction)