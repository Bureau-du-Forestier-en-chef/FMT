/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTAction.h"
#include "FMTExceptionHandler.h"
#include <bitset>
#include <boost/algorithm/string/split.hpp>
#include <algorithm>

namespace Core{

	FMTAction& FMTAction::operator+=(const FMTAction& OtherAction)
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
					_exhandler->raise(Exception::FMTexc::FMTignore, "Losing partiel on "+getName()+" with "+OtherAction.getName(),
						"FMTAction::operator+=", __LINE__, __FILE__, Core::FMTsection::Action);
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
			
			FMTList<FMTSpec>::operator+=(OtherAction);
			for (const std::string& Aggregate : OtherAction.aggregates)
				{
				if (std::find(aggregates.begin(),aggregates.end(), Aggregate)== aggregates.end())
					{
					aggregates.push_back(Aggregate);
					}

				}

		}catch (...)
			{
			_exhandler->raiseFromCatch("for action " + this->getName(),
				"FMTAction::operator+=", __LINE__, __FILE__, Core::FMTsection::Action);
			}
		return *this;
		}


	FMTAction::FMTAction():FMTList<FMTSpec>(),
					aggregates(),
					partials(),
					agelowerbound(), ageupperbound(), periodlowerbound(), periodupperbound(),
					name(""),
					lock(false),
					reset(false),
					m_series(),
					m_InSerie(false){}

    FMTAction::FMTAction(const std::string& lname): FMTList<FMTSpec>(),
						aggregates(),
                        partials(),
						agelowerbound(), ageupperbound(), periodlowerbound(), periodupperbound(),
                        name(lname),
                        lock(false),
                        reset(false),
						m_series(),
						m_InSerie(false) {}

    FMTAction::FMTAction(
		const std::string& lname, 
		const bool& lock,
		const bool& reset): 
		FMTList<FMTSpec>(), aggregates(), partials(),
		agelowerbound(), ageupperbound(), periodlowerbound(), periodupperbound(),
		name(lname),lock(lock),reset(reset), m_series(), m_InSerie(false)
        {

        }

	void FMTAction::pushAggregate(const std::string& aggregate)
		{
		aggregates.push_back(aggregate);
		}

    void FMTAction::pushPartials(const std::string& yield)
        {
        partials.push_back(yield);
        }
	FMTAction::FMTAction(const FMTAction& rhs) : FMTList<FMTSpec>(rhs),
		aggregates(rhs.aggregates),
		partials(rhs.partials),
		agelowerbound(rhs.agelowerbound), ageupperbound(rhs.ageupperbound), periodlowerbound(rhs.periodlowerbound), periodupperbound(rhs.periodupperbound),
                        name(rhs.name),
                        lock(rhs.lock),
                        reset(rhs.reset),
						m_series(rhs.m_series),
		m_InSerie(rhs.m_InSerie)
        {

        }
    FMTAction& FMTAction::operator = (const FMTAction& rhs)
        {
        if (this!=&rhs)
            {
			FMTList<FMTSpec>::operator = (rhs);
            name = rhs.name;
            lock = rhs.lock;
            reset = rhs.reset;
			m_series = rhs.m_series;
            partials = rhs.partials;
			aggregates = rhs.aggregates;
			agelowerbound = rhs.agelowerbound;
			ageupperbound = rhs.ageupperbound;
			periodlowerbound = rhs.periodlowerbound;
			periodupperbound = rhs.periodupperbound;
			m_InSerie = rhs.m_InSerie;
            }
        return *this;
        }

	void FMTAction::setBounds()
		{
		try {
			ageupperbound = 0;
			agelowerbound = std::numeric_limits<int>::max();
			periodupperbound = 0;
			periodlowerbound = std::numeric_limits<int>::max();
			std::vector<std::pair<FMTMask, FMTSpec>>::const_iterator datait = this->begin();
			for (size_t id = 0; id < this->size(); ++id)
			{
				if (!datait->second.emptyAge())
				{
					int upperbound = datait->second.getAgeUpperBound();
					if (upperbound > ageupperbound)
					{
						ageupperbound = upperbound;
					}
					int lowerbound = datait->second.getAgeLowerBound();
					if (lowerbound < agelowerbound)
					{
						agelowerbound = lowerbound;
					}
				}
				else {
					ageupperbound = std::numeric_limits<int>::max();
					agelowerbound = 0;
				}
				if (!datait->second.emptyPeriod())
				{
					int upperbound = datait->second.getPeriodUpperBound();
					if (upperbound > periodupperbound)
					{
						periodupperbound = upperbound;
					}
					int lowerbound = datait->second.getPeriodLowerBound();
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
			_exhandler->raiseFromCatch("for action "+this->getName(),
				"FMTAction::setBounds", __LINE__, __FILE__,Core::FMTsection::Action);
			}
		}

	void FMTAction::update()
		{
		try {
			FMTList<FMTSpec>::update();
			this->setBounds();
		}catch (...)
			{
			_exhandler->raiseFromCatch("for action " + this->getName(),
				"FMTAction::update", __LINE__, __FILE__, Core::FMTsection::Action);
			}
		}

	std::vector<std::string>FMTAction::getPartials() const
        {
        return partials;
        }

	bool FMTAction::isAllowedInSerie(const std::vector<std::string>& seriemask) const
	{
		try {
			if (!seriemask.empty())
			{
				for (const FMTSerie& SERIE : m_series)
				{
					if (SERIE.isAllowedInSerie(seriemask))
					{
						return true;
					}
				}
			}
		}catch (...)
			{
			_exhandler->raiseFromCatch("for action " + this->getName(),
				"FMTAction::isAllowedInSerie", __LINE__, __FILE__, Core::FMTsection::Action);
			}
		return false;
	}

	const FMTSerie* FMTAction::getSerie(const std::vector<std::string>& p_SerieMask) const
	{
		try {
			if (!p_SerieMask.empty())
			{
				for (const FMTSerie& SERIE : m_series)
				{
					if (SERIE.isAllowedInSerie(p_SerieMask))
					{
						return &SERIE;
					}
				}
			}
		}catch (...)
			{
			_exhandler->raiseFromCatch("for action " + this->getName(),
				"FMTAction::isAllowedInSerie", __LINE__, __FILE__, Core::FMTsection::Action);
			}
		return nullptr;
	}

	const std::vector<FMTSerie>& FMTAction::getSeries() const
	{
		return  m_series;
	}

	std::vector<std::string>FMTAction::getSeriesNames() const
	{
		std::vector<std::string>seriesnames;
		try {
			for (const FMTSerie& SERIE : m_series)
				{
				std::string seriename = SERIE.getSerie();
				if (!seriename.empty())
					{
					seriename += getName();
					}
				seriesnames.push_back(seriename);
				}
		}catch (...)
			{
			_exhandler->raiseFromCatch("for action " + this->getName(),
				"FMTAction::getSeriesNames", __LINE__, __FILE__, Core::FMTsection::Action);
			}
		return seriesnames;
	}

	size_t FMTAction::getLargestSerieSize() const
	{
		size_t sizeofserie = 0;
		try {
			if (isPartOfASerie())
				{
				for (const FMTSerie& SERIE : m_series)
					{
					sizeofserie = std::max(sizeofserie, 
										SERIE.getActions().size());
					}
				}
		}catch (...)
			{
			_exhandler->raiseFromCatch("for action " + this->getName(),
				"FMTAction::getLargestSerieSize", __LINE__, __FILE__, Core::FMTsection::Action);
			}
		return sizeofserie;
	}

	void FMTAction::setSeries(std::vector<Core::FMTSerie> p_series)
	{
		try {
			m_series.clear();
			for (const FMTSerie& SERIE : p_series)
				{
				std::vector<std::string>  actionsname = SERIE.getActions();
				std::vector<std::string>::iterator ait = std::find(actionsname.begin(), actionsname.end(), getName());
				if (ait!= actionsname.end())
					{
					if (!m_InSerie)
						{
						m_InSerie = true;
						}
					while (ait != actionsname.end())
						{
							if (ait != actionsname.end() && std::distance(actionsname.begin(), ait + 1) > 1)//Ok in serie
								{
								m_series.push_back(FMTSerie(std::vector<std::string>(actionsname.begin(), ait + 1),SERIE.getLength()));
								}
							if (!actionsname.empty())
								{
								actionsname.erase(actionsname.begin(), ait + 1);
								}
						ait = std::find(actionsname.begin(), actionsname.end(), getName());
						}
					}
				}
		}catch (...){
			_exhandler->raiseFromCatch("for action " + this->getName(),
				"FMTAction::setSeries", __LINE__, __FILE__, Core::FMTsection::Action);
		}
	}

    FMTAction::operator std::string() const
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

bool FMTAction::useYield(const std::string& yldname) const
	{
	try {
		for (const auto& speclitit : *this)
		{
			if (!speclitit.second.emptyYlds())
			{
				const std::vector<std::string> specyields = speclitit.second.getYlds();
				if (std::find(specyields.begin(), specyields.end(), yldname) != specyields.end())
				{
					return true;
				}
			}

		}
	}
	catch (...)
	{
		_exhandler->raiseFromCatch("for action " + this->getName(),
			"FMTAction::useYield", __LINE__, __FILE__, Core::FMTsection::Action);
	}
	return false;
	}

std::vector<std::string>FMTAction::getAggregates() const
	{
	return aggregates;
	}

void FMTAction::setAggregates(const std::vector<std::string>& p_aggregates)
	{
	aggregates = p_aggregates;
	}


bool FMTAction::operator < (const FMTAction& rhs) const
    {
    return name < rhs.name;
    }
bool FMTAction::operator == (const FMTAction& rhs) const
    {
	return (name == rhs.name &&
		partials == rhs.partials &&
		lock == rhs.lock &&
		reset == rhs.reset &&
		FMTList<FMTSpec>::operator == (rhs));

    }
bool FMTAction::operator != (const FMTAction& rhs) const
    {
    return !(*this== rhs);
    }

bool FMTAction::partial(const std::string& yield) const
	{
	return (std::find(partials.begin(), partials.end(),yield)!=partials.end());
	}

FMTActionComparator::FMTActionComparator(
	std::string name, bool lcheckaggregate) : action_name(name),checkaggregate(lcheckaggregate) {}

bool FMTActionComparator::operator()(const FMTAction& action) const
	{
	return (action.name == action_name || (checkaggregate && 
		std::find(action.aggregates.begin(), action.aggregates.end(),action_name)!= action.aggregates.end()));
	}

std::vector<const FMTAction*>FMTActionComparator::getAllAggregates(
	const std::vector<FMTAction>&actions,
	bool aggregateonly) const
	{
	std::vector<const FMTAction*> actionsptr;
	std::vector<FMTAction>::const_iterator actit = actions.begin();
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

std::unordered_set<int>FMTActionComparator::getAllAggregatesSet(
	const std::vector<FMTAction>&actions, 
	bool aggregateonly) const
{
	std::unordered_set<int>actionsptr;
	std::vector<FMTAction>::const_iterator actit = actions.begin();
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

std::vector<std::string>FMTAction::getGCBMActionDef() const
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
		_exhandler->raise(Exception::FMTexc::FMTempty_action, "Missing GCBM action for action " + this->getName(),
			"FMTAction::getGCBMActionDef", __LINE__, __FILE__, Core::FMTsection::Action);
	}
	catch (...)
	{
		_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "for action " + this->getName(),
			"FMTAction::getGCBMActionDef", __LINE__, __FILE__, Core::FMTsection::Action);
	}
	return allvalues;
}

int FMTAction::getGCBMActionId() const
{
	try {
		return std::atoi(getGCBMActionDef().at(1).c_str());
	}
	catch (...)
	{
		_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "for action " + this->getName(),
			"FMTAction::getGCBMActionId", __LINE__, __FILE__, Core::FMTsection::Action);
	}
	return 0;
}

std::string FMTAction::getGCBMActionName() const
{
	try {
		return getGCBMActionDef().at(2).c_str();
	}
	catch (...)
	{
		_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "for action " + this->getName(),
			"FMTAction::getGCBMActionName", __LINE__, __FILE__, Core::FMTsection::Action);
	}
	return std::string();
}

bool FMTAction::notUse() const
	{
	size_t count = 0;
	for (const auto& value : *this)
		{
		if (!value.second.emptyPeriod()&&
			value.second.getPeriodLowerBound() == 0 &&
			value.second.getPeriodUpperBound() == 0)
			{
			++count;
			}
		}
	return (size() == count);
	}

bool FMTAction::isPartOf(const std::string& p_name) const
	{
	return (name == p_name ||
		std::find(aggregates.begin(), aggregates.end(), p_name) != aggregates.end());
	}

bool FMTAction::isInSeries() const
	{
	return m_InSerie;
	}

std::vector<Core::FMTAction>FMTAction::split(const std::vector<Core::FMTMask>& p_mask,
											const std::vector<Core::FMTTheme>& p_themes) const
{
	std::vector<Core::FMTAction>Splitted;
	try {
		for (const Core::FMTMask& MASK : p_mask)
			{
			std::string NewName = std::string(MASK);
			NewName.erase(std::remove(NewName.begin(), NewName.end(), ' '), NewName.end());
			NewName.erase(std::remove(NewName.begin(), NewName.end(), '?'), NewName.end());
			NewName = getName() + "_" + NewName;
			Core::FMTAction NewAction(NewName,lock,reset);
			for (const auto& data : *this)
				{
				Core::FMTMask subMAsk =  Core::FMTMask(std::string(data.first), p_themes);
				if (!subMAsk.isNotThemesSubset(MASK,p_themes))
					{
					subMAsk = MASK.getIntersect(subMAsk);
					subMAsk.update(p_themes);
					NewAction.push_back(subMAsk, data.second);
					}
				}
			NewAction.aggregates = aggregates;
			NewAction.aggregates.insert(NewAction.aggregates.begin(),getName());
			NewAction.update();
			Splitted.push_back(NewAction);
			}
	}catch (...)
		{
		_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "for action " + this->getName(),
			"FMTAction::split", __LINE__, __FILE__, Core::FMTsection::Action);
		}
	return Splitted;
}

void FMTAction::presolveRef(
	const FMTMaskFilter& p_filter,
	const std::vector<FMTTheme>& p_originalthemes,
	std::vector<FMTTheme>& p_newthemes,
	 bool p_compressdata)
{
	try {
		presolveList(p_filter, p_originalthemes, p_newthemes);
		if (p_compressdata)
			{
			compressMasks(p_newthemes);
			}
		update();
	}catch (...)
		{
		_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "for action " + this->getName(),
			"FMTAction::presolveRef", __LINE__, __FILE__, Core::FMTsection::Action);
		}
}

FMTAction FMTAction::presolve(const FMTMaskFilter& filter,
	const std::vector<FMTTheme>& originalthemes,
	std::vector<FMTTheme>& newthemes,bool compressdata) const
	{
	FMTAction newaction(*this);
	try {
		newaction.presolveRef(filter, originalthemes, newthemes, compressdata);
	}catch (...)
		{
		_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,"for action "+this->getName(),
			"FMTAction::presolve", __LINE__, __FILE__, Core::FMTsection::Action);
		}
	return newaction;
	}

}

BOOST_CLASS_EXPORT_IMPLEMENT(Core::FMTAction)