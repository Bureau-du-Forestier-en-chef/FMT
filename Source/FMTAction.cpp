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
				this->m_reset = OtherAction.m_reset;
				this->m_lock = OtherAction.m_lock;
				}
			if (OtherAction.m_reset)
				{
				this->m_reset = true;
				if (!this->m_partials.empty())
					{
					_exhandler->raise(Exception::FMTexc::FMTignore, "Losing partiel on "+getName()+" with "+OtherAction.getName(),
						"FMTAction::operator+=", __LINE__, __FILE__, Core::FMTsection::Action);
					this->m_partials.clear();
					}
			}else {
				for (const std::string& partial : OtherAction.m_partials)
					{
						if (std::find(this->m_partials.begin(), this->m_partials.end(), partial) == this->m_partials.end())
						{
							this->m_partials.push_back(partial);
						}

					}
				}
			if (!OtherAction.m_lock)
				{
				this->m_lock = false;
				}
			
			FMTList<FMTSpec>::operator+=(OtherAction);
			for (const std::string& Aggregate : OtherAction.m_aggregates)
				{
				if (std::find(m_aggregates.begin(),m_aggregates.end(), Aggregate)== m_aggregates.end())
					{
					m_aggregates.push_back(Aggregate);
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
					m_aggregates(),
					m_partials(),
					m_agelowerbound(), m_ageupperbound(), m_periodlowerbound(), m_periodupperbound(),
					m_name(""),
					m_lock(false),
					m_reset(false),
					m_series(),
					m_InSerie(false){}

    FMTAction::FMTAction(const std::string& lname): FMTList<FMTSpec>(),
						m_aggregates(),
                        m_partials(),
						m_agelowerbound(), m_ageupperbound(), m_periodlowerbound(), m_periodupperbound(),
                        m_name(lname),
                        m_lock(false),
                        m_reset(false),
						m_series(),
						m_InSerie(false) {}

    FMTAction::FMTAction(
		const std::string& lname, 
		const bool& p_lock,
		const bool& p_reset): 
		FMTList<FMTSpec>(), m_aggregates(), m_partials(),
		m_agelowerbound(), m_ageupperbound(), m_periodlowerbound(), m_periodupperbound(),
		m_name(lname),m_lock(p_lock),m_reset(p_reset), m_series(), m_InSerie(false)
        {

        }

	void FMTAction::pushAggregate(const std::string& aggregate)
		{
		m_aggregates.push_back(aggregate);
		}

    void FMTAction::pushPartials(const std::string& yield)
        {
        m_partials.push_back(yield);
        }
	FMTAction::FMTAction(const FMTAction& rhs) : FMTList<FMTSpec>(rhs),
		m_aggregates(rhs.m_aggregates),
		m_partials(rhs.m_partials),
		m_agelowerbound(rhs.m_agelowerbound), m_ageupperbound(rhs.m_ageupperbound), m_periodlowerbound(rhs.m_periodlowerbound), m_periodupperbound(rhs.m_periodupperbound),
                        m_name(rhs.m_name),
                        m_lock(rhs.m_lock),
                        m_reset(rhs.m_reset),
						m_series(rhs.m_series),
		m_InSerie(rhs.m_InSerie)
        {

        }
    FMTAction& FMTAction::operator = (const FMTAction& rhs)
        {
        if (this!=&rhs)
            {
			FMTList<FMTSpec>::operator = (rhs);
            m_name = rhs.m_name;
            m_lock = rhs.m_lock;
            m_reset = rhs.m_reset;
			m_series = rhs.m_series;
            m_partials = rhs.m_partials;
			m_aggregates = rhs.m_aggregates;
			m_agelowerbound = rhs.m_agelowerbound;
			m_ageupperbound = rhs.m_ageupperbound;
			m_periodlowerbound = rhs.m_periodlowerbound;
			m_periodupperbound = rhs.m_periodupperbound;
			m_InSerie = rhs.m_InSerie;
            }
        return *this;
        }

	void FMTAction::_setBounds()
		{
		try {
			m_ageupperbound = 0;
			m_agelowerbound = std::numeric_limits<int>::max();
			m_periodupperbound = 0;
			m_periodlowerbound = std::numeric_limits<int>::max();
			std::vector<std::pair<FMTMask, FMTSpec>>::const_iterator datait = this->begin();
			for (size_t id = 0; id < this->size(); ++id)
			{
				if (!datait->second.emptyAge())
				{
					int upperbound = datait->second.getAgeUpperBound();
					if (upperbound > m_ageupperbound)
					{
						m_ageupperbound = upperbound;
					}
					int lowerbound = datait->second.getAgeLowerBound();
					if (lowerbound < m_agelowerbound)
					{
						m_agelowerbound = lowerbound;
					}
				}
				else {
					m_ageupperbound = std::numeric_limits<int>::max();
					m_agelowerbound = 0;
				}
				if (!datait->second.emptyPeriod())
				{
					int upperbound = datait->second.getPeriodUpperBound();
					if (upperbound > m_periodupperbound)
					{
						m_periodupperbound = upperbound;
					}
					int lowerbound = datait->second.getPeriodLowerBound();
					if (lowerbound < m_periodlowerbound)
					{
						m_periodlowerbound = lowerbound;
					}
				}
				else {
					m_periodupperbound = std::numeric_limits<int>::max();
					m_periodlowerbound = 0;
				}
				++datait;
			}
		}catch (...)
			{
			_exhandler->raiseFromCatch("for action "+this->getName(),
				"FMTAction::_setBounds", __LINE__, __FILE__,Core::FMTsection::Action);
			}
		}

	void FMTAction::update()
		{
		try {
			FMTList<FMTSpec>::update();
			this->_setBounds();
		}catch (...)
			{
			_exhandler->raiseFromCatch("for action " + this->getName(),
				"FMTAction::update", __LINE__, __FILE__, Core::FMTsection::Action);
			}
		}

	std::vector<std::string>FMTAction::getPartials() const
        {
        return m_partials;
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
		const std::string resetyield = (m_reset) ? "Y" : "N";
		const std::string locked = (m_lock) ? "" : " _LOCKEXEMPT";
		std::string line="*ACTION "+m_name +" "+resetyield+locked+"\n";
        line+="*OPERABLE "+m_name+"\n";
		for (const auto& yieldobject : *this)
			{
			line += std::string(yieldobject.first) + " ";
			line += std::string(yieldobject.second);
			line += "\n";
			}
        if(!m_partials.empty())
            {
            line+="*PARTIAL "+m_name+"\n";
            int lid = 0;
            for(const std::string& partial : m_partials)
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
	return m_aggregates;
	}

void FMTAction::setAggregates(const std::vector<std::string>& p_aggregates)
	{
	m_aggregates = p_aggregates;
	}


bool FMTAction::operator < (const FMTAction& rhs) const
    {
    return m_name < rhs.m_name;
    }
bool FMTAction::operator == (const FMTAction& rhs) const
    {
	return (m_name == rhs.m_name &&
		m_partials == rhs.m_partials &&
		m_lock == rhs.m_lock &&
		m_reset == rhs.m_reset &&
		FMTList<FMTSpec>::operator == (rhs));

    }
bool FMTAction::operator != (const FMTAction& rhs) const
    {
    return !(*this== rhs);
    }

bool FMTAction::partial(const std::string& yield) const
	{
	return (std::find(m_partials.begin(), m_partials.end(),yield)!=m_partials.end());
	}

FMTActionComparator::FMTActionComparator(
	std::string p_name, bool lcheckaggregate) : m_actionName(p_name),m_checkAggregate(lcheckaggregate) {}

bool FMTActionComparator::operator()(const FMTAction& action) const
	{
	return (action.m_name == m_actionName || (m_checkAggregate && 
		std::find(action.m_aggregates.begin(), action.m_aggregates.end(),m_actionName)!= action.m_aggregates.end()));
	}

std::vector<const FMTAction*>FMTActionComparator::getAllAggregates(
	const std::vector<FMTAction>&actions,
	bool aggregateonly) const
	{
	std::vector<const FMTAction*> actionsptr;
	std::vector<FMTAction>::const_iterator actit = actions.begin();
	while (actit!= actions.end())
		{
		if ((!aggregateonly && actit->m_name == m_actionName) || (std::find(actit->m_aggregates.begin(), actit->m_aggregates.end(), m_actionName) != actit->m_aggregates.end()))
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
		if ((!aggregateonly && actit->m_name == m_actionName) || (std::find(actit->m_aggregates.begin(), actit->m_aggregates.end(), m_actionName) != actit->m_aggregates.end()))
		{
			actionsptr.insert(actionid);
		}
		++actit;
	++actionid;
	}
	return actionsptr;
}

std::vector<std::string>FMTAction::_getGCBMActionDef() const
{
	std::vector<std::string> allvalues;
	try {
		for (const std::string& aggregate : m_aggregates)
		{
			if (aggregate.find("~GCBM:") != std::string::npos)
			{
				boost::split(allvalues, aggregate, boost::is_any_of(":"), boost::token_compress_on);
				return allvalues;
			}
		}
		_exhandler->raise(Exception::FMTexc::FMTempty_action, "Missing GCBM action for action " + this->getName(),
			"FMTAction::_getGCBMActionDef", __LINE__, __FILE__, Core::FMTsection::Action);
	}
	catch (...)
	{
		_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "for action " + this->getName(),
			"FMTAction::_getGCBMActionDef", __LINE__, __FILE__, Core::FMTsection::Action);
	}
	return allvalues;
}

int FMTAction::getGCBMActionId() const
{
	try {
		return std::atoi(_getGCBMActionDef().at(1).c_str());
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
		return _getGCBMActionDef().at(2).c_str();
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
			value.second.getPeriodLowerBound() <= 0 &&
			value.second.getPeriodUpperBound() == 0)
			{
			++count;
			}
		}
	return (size() == count);
	}

bool FMTAction::isPartOf(const std::string& p_name) const
	{
	return (m_name == p_name ||
		std::find(m_aggregates.begin(), m_aggregates.end(), p_name) != m_aggregates.end());
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
			Core::FMTAction NewAction(NewName,m_lock,m_reset);
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
			NewAction.m_aggregates = m_aggregates;
			NewAction.m_aggregates.insert(NewAction.m_aggregates.begin(),getName());
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
		_presolveList(p_filter, p_originalthemes, p_newthemes);
		if (p_compressdata)
			{
			_compressMasks(p_newthemes);
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