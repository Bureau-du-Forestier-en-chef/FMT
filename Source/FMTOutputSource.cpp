/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTOutputSource.h"
#include "FMTGraphVertexToYield.h"
#include "FMTAction.h"
#include "FMTYields.h"
#include "FMTDevelopment.h"
#include "FMTActualDevelopment.h"
#include "FMTOperator.h"

namespace Core{

FMTOutputSource::FMTOutputSource() : FMTSpec(), m_mask(),
		m_target(FMTotar::val),
		m_action(),
		m_yield(),
		m_levelname(),
		m_values(),
		m_average(),
		m_sum(),
		m_outputorigin(-1),
		m_themetarget(-1)
	{

	}


FMTOutputSource::FMTOutputSource(const FMTotar ltarget,double lvalue, std::string lyield,
	std::string laction,int origin,int ttarget) : FMTSpec(),m_mask(),
    m_target(ltarget),
    m_action(laction),
    m_yield(lyield),
	m_levelname(),
    m_values(1,lvalue),
	m_average(),
	m_sum(),
	m_outputorigin(origin),
	m_themetarget(ttarget)
    {
	if (isTimeYield())
		{
		m_values.clear();
		}

    }


FMTOutputSource::FMTOutputSource(const FMTotar ltarget,std::vector<double>lvalues, int origin, int ttarget, std::string llevelname): FMTSpec(),m_mask(),
    m_target(ltarget),
    m_action(),
    m_yield(),
	m_levelname(llevelname),
    m_values(lvalues),
	m_average(),
	m_sum(),
	m_outputorigin(origin),
	m_themetarget(ttarget)
    {



    }

FMTOutputSource::FMTOutputSource(const FMTSpec& spec,const FMTMask& lmask,
               const FMTotar ltarget,std::string lyield,
			std::string laction, int origin,int ttarget) :
                FMTSpec(spec),m_mask(lmask),
                m_target(ltarget),m_action(laction),m_yield(lyield), m_levelname(), m_values(), m_average(), m_sum(), m_outputorigin(origin), m_themetarget(ttarget)
    {


    }

FMTOutputSource::FMTOutputSource(const FMTOutputSource& rhs) : 
	FMTSpec(rhs),m_mask(rhs.m_mask),m_target(rhs.m_target),
    m_action(rhs.m_action),m_yield(rhs.m_yield), m_levelname(rhs.m_levelname),
	m_values(rhs.m_values), m_average(rhs.m_average),
	m_sum(rhs.m_sum), m_outputorigin(rhs.m_outputorigin), m_themetarget(rhs.m_themetarget)
    {

    }

FMTOutputSource& FMTOutputSource::operator = (const FMTOutputSource& rhs)
    {
    if(this!=&rhs)
        {
        m_target = rhs.m_target;
        m_values = rhs.m_values;
        m_action = rhs.m_action;
        m_yield = rhs.m_yield;
		m_levelname = rhs.m_levelname;
        FMTSpec::operator=(rhs);
        m_mask = rhs.m_mask;
		m_average = rhs.m_average;
		m_sum = rhs.m_sum;
		m_outputorigin = rhs.m_outputorigin;
		m_themetarget=rhs.m_themetarget;
        }
    return *this;
    }

void FMTOutputSource::fillHashMask(Core::FMTMask& baseMask) const
	{
		//baseMask.append(mask.getBitsetReference());
		baseMask.binarizedAppend<std::string>(std::string(m_mask));
		baseMask.binarizedAppend<int>(static_cast<int>(m_target));
		for (const double& lvalue : m_values)
		{
			baseMask.binarizedAppend<double>(lvalue);
		}
		baseMask.binarizedAppend<std::string>(m_yield);
		baseMask.binarizedAppend<std::string>(m_action);
	}

bool FMTOutputSource::isEqualByValue(const FMTOutputSource& rhs) const
	{
	return (FMTSpec::operator == (rhs) && m_mask == rhs.m_mask && m_target == rhs.m_target &&
		m_yield == rhs.m_yield && m_action == rhs.m_action && m_values == rhs.m_values &&
		m_levelname == rhs.m_levelname && m_average == rhs.m_average && m_sum == rhs.m_sum);
	}


void FMTOutputSource::fillHashSpec(Core::FMTMask& baseMask) const
	{
	baseMask.binarizedAppend<std::string>(FMTSpec::operator std::string());
	}


FMTOutputSource::operator std::string() const
    {
	std::string line = "";
	if (!m_mask.empty())
		{
		line += std::string(m_mask)+" ";
		}
	if (!FMTSpec::empty())
		{
		line += FMTSpec::operator std::string() + " ";
		}
    switch (m_target)
        {
        case FMTotar::val:
            for (const double lvalue : m_values)
                {
                line += FMTOutputSource::trimDouble(std::to_string(lvalue))  + " ";
                }
            if (!m_values.empty())
                {
                line.pop_back();
                }
        break;
		case FMTotar::timeyld:
			line += m_yield;
		break;
        case FMTotar::actual:
            if (!m_action.empty())
                {
                line += m_action +" ";
                }
            if (!m_yield.empty())
                {
                line += m_yield+" ";
                }else{
                line+="_AREA";
                }
        break;
        case FMTotar::inventory:
            if (!m_action.empty())
                {
                line+="_INVENT("+m_action+") ";
				}
				/*else if (this->lock.getLower() > 0)
				{
				line += "_INVLOCK ";

				}*/else if (emptyLock()) {
                line+= "_INVENT ";
                }
            if (!m_yield.empty())
                {
                line += m_yield;
                }else{
                line+="_AREA";
                }
        break;
        case FMTotar::level:
			line += (m_action + m_levelname);
			/*if (!action.empty())
                {
                line += m_action;
                }else{
                for (const double lvalue : m_values)
                    {
                    line += std::to_string(lvalue) + " ";
                    }
                 if (!m_values.empty())
                    {
                    line.pop_back();
                    }
                }*/
        break;
        default:
            line+="";
        break;
        };
    return line;
    }

bool FMTOutputSource::operator < (const FMTOutputSource& rhs) const
	{
	if (m_mask < rhs.m_mask)
		return true;
	if (rhs.m_mask < m_mask)
		return false;
	if (m_target < rhs.m_target)
		return true;
	if (rhs.m_target < m_target)
		return false;
	if (m_action < rhs.m_action)
		return true;
	if (rhs.m_action < m_action)
		return false;
	if (FMTSpec::operator < (rhs))
		return true;
	return false;
	}

bool FMTOutputSource::operator == (const FMTOutputSource& rhs) const
	{
	return (FMTSpec::operator == (rhs) && m_target == rhs.m_target && m_mask==rhs.m_mask  && m_action == rhs.m_action);
	}

bool FMTOutputSource::operator != (const FMTOutputSource& rhs) const
	{
	return (!(*this==rhs));
	}

void FMTOutputSource::setThemeTarget(const int& newttarget)
	{
		m_themetarget=newttarget;
	}
void FMTOutputSource::setOutputOrigin(const int& neworigin)
	{
		m_outputorigin=neworigin;
	}

void FMTOutputSource::resetValues(const FMTOperator& op,const FMTOutputSource& other)
	{
	std::vector<double>newvalues(std::max(other.m_values.size(), m_values.size()),0);
	for (int period = 1; period < static_cast<int>(newvalues.size()+1);++period)
		{
		const double othervalue = other.getValue(period);
		newvalues[period-1]=op.call(getValue(period), othervalue);
		}
	m_values.swap(newvalues);
	if (other.isTimeYield())
		{
		m_yield = other.m_yield;
		m_target = FMTotar::timeyld;
		}
	}

bool FMTOutputSource::isSubsetOf(const FMTOutputSource& rhs) const
	{
	if ((this->isVariable() && rhs.isVariable() &&
		m_target == rhs.m_target && FMTSpec::isSubsetOf(rhs) &&
		!((!m_action.empty() && rhs.m_action.empty()) || (!rhs.m_action.empty() && m_action.empty()))) &&
		(m_mask.isSubsetOf(rhs.m_mask) &&
		((m_action.empty() && rhs.m_action.empty()) ||
			(!m_action.empty() && !rhs.m_action.empty() &&
			(m_action == rhs.m_action)))))
		{
			return true;
		}
	return false;
	}

bool FMTOutputSource::isSameButDifferentAction(const FMTOutputSource& rhs) const
	{
	return (FMTSpec::operator == (rhs) && m_target == rhs.m_target && m_mask == rhs.m_mask && m_action != rhs.m_action);
	}


bool FMTOutputSource::isInAggregate(const FMTOutputSource& rhs, const std::vector<Core::FMTAction>& actions) const
	{
	if (isAction() && rhs.isAction())
		{
		const std::vector<const FMTAction*>allactions = FMTActionComparator(rhs.m_action).getAllAggregates(actions);
		for (const FMTAction* actptr : allactions)
			{
				if (actptr->getName() == m_action)
				{
					return true;
				}
			}
		}
	return false;
	}
	

bool FMTOutputSource::isSubsetOf(const FMTOutputSource& rhs,
	const std::vector<Core::FMTAction>& actions) const
	{
	
	if ((this->isVariable() && rhs.isVariable() && 
		m_target == rhs.m_target && FMTSpec::isSubsetOf(rhs) && 
		!((!m_action.empty() && rhs.m_action.empty()) || (!rhs.m_action.empty() && m_action.empty()))) && 
		(m_mask.isSubsetOf(rhs.m_mask) &&
		((m_action.empty() && rhs.m_action.empty()) || 
		(!m_action.empty() && !rhs.m_action.empty() && 
			isInAggregate(rhs,actions)))))
			{
			return true;
			}
	return false;
	}

FMTOutputSource FMTOutputSource::presolve(const FMTMaskFilter& filter, const std::vector<FMTTheme>& newthemes) const
	{
	FMTOutputSource newsource(*this);
	if (newsource.isVariable())
		{
		newsource.m_mask = newsource.m_mask.presolve(filter, newthemes);
		}
	return newsource;
	}

bool FMTOutputSource::canBeDeducedToConstant() const
	{
	return (isVariable() && isInventory() &&
		!emptyPeriod() && getPeriodLowerBound() == 0 &&
		getPeriodUpperBound() == 0);
	}

double FMTOutputSource::getConstantValue(const std::vector<Core::FMTActualDevelopment>& area,
	const std::vector<Core::FMTAction>&actions, const FMTYields& yields) const
	{
	std::vector<const FMTAction*> operabletoactions = this->targets(actions);
	double value = 0;
	for (const Core::FMTActualDevelopment& development : area)
		{
		if (use(development,yields)  && operabletoactions.empty()
			|| development.anyOperable(operabletoactions, yields))
			{
			double developmentvalue = development.getArea();
			if (!m_yield.empty())
				{
				developmentvalue*=development.getInventoryCoef(yields, m_yield);
				}
			value += developmentvalue;
			}
		}
	return value;
	}


void FMTOutputSource::setAverage()
	{
	m_average = true;
	}


void FMTOutputSource::setSum()
{
	m_sum = true;
}

bool FMTOutputSource::isNull(const FMTYields& ylds) const
	{
	if (!m_yield.empty())
		{
		return ylds.isNullYld(m_yield);
		}
	if (!isVariable())
		{
		return (getValue() == 0);
		}
	return false;
	}

void  FMTOutputSource::_pushValues(const std::vector<double>& newvalues)
	{
	m_values.insert(m_values.end(),newvalues.begin(), newvalues.end());
	}

double FMTOutputSource::getValue(int period) const
	{
	double returnvalue = 0;
	if (m_target == FMTotar::timeyld&&
		m_values.empty())
		{
		return 1;
		}
	if (m_target == FMTotar::val||m_target == FMTotar::level||m_target==FMTotar::timeyld)
		{
		--period;
		period = std::max(period, 0);//Cannot get negative period
        if (period >= static_cast<int>(m_values.size()))
            {
            period = static_cast<int>(m_values.size() - 1);
            }
		returnvalue= m_values.at(period);
		}
	return returnvalue;
	}

void FMTOutputSource::setMask(const FMTMask& newmask)
	{
	m_mask = newmask;
	}

void FMTOutputSource::setAction(const std::string& actionname)
	{
	m_action = actionname;
	}

void FMTOutputSource::setTarget(const FMTotar& newtype)
	{
	m_target = newtype;
	}

std::vector<const FMTAction*>FMTOutputSource::targets(const std::vector<FMTAction>& actions) const
	{
	if (m_target != FMTotar::level && !m_action.empty())
        {
		return FMTActionComparator(m_action).getAllAggregates(actions);
        }
	return std::vector<const FMTAction*>();
	}

std::unordered_set<int>FMTOutputSource::targetsSet(const std::vector<FMTAction>& actions) const
{
	if (m_target != FMTotar::level && !m_action.empty())
	{
		return FMTActionComparator(m_action).getAllAggregatesSet(actions);
	}
	return std::unordered_set<int>();
}

bool FMTOutputSource::use(const FMTDevelopment& development, const FMTYields& ylds,
	const Graph::FMTGraphVertexToYield* graphinfo) const
{
	return (development.getMask().isSubsetOf(m_mask) && development.is(*this, ylds, graphinfo));
}

std::string FMTOutputSource::trimDouble(const std::string& string_number)
{
	std::string trimmed = string_number;
	for (int i = static_cast<int>(string_number.size())-1; i > 0; i--)
	{
		if(trimmed[i] == '0')
		{
			trimmed.pop_back();
		}
		else if(trimmed[i] == '.')
		{
			trimmed.pop_back();
			break;
		}
		else
		{
			break;
		}
	}
	return trimmed;
}

void FMTOutputSource::setYield(const std::string& p_Yield)
	{
	m_yield = p_Yield;
	}

double FMTOutputSource::getCoef(const FMTDevelopment& development,
	const FMTYields& yields,
	const Graph::FMTGraphVertexToYield* graphinfo,
	std::vector<FMTDevelopmentPath> const * paths,
	FMTAction const * modelaction) const
	{
	double coef = 1;
	if (isVariable())
		{
		if (!m_yield.empty())
			{
			if (m_target == FMTotar::inventory)
			{
				coef = development.getInventoryCoef(yields, m_yield, graphinfo);
			}
			else {
				coef = development.getHarvestCoef(*paths, *modelaction, yields, m_yield, graphinfo);
			}
			}
		}else{
			if (!m_values.empty())
			{
				coef = getValue(development.getPeriod());
			}
			if (isTimeYield())
				{
				coef *= development.getInventoryCoef(yields, m_yield, graphinfo);
				}
			}
	return coef;
	}

size_t FMTOutputSource::hash(int period, bool withyield) const
	{
	size_t seed = 0;
	boost::hash_combine(seed,m_mask.hash());
	boost::hash_combine(seed,m_target);
	boost::hash_combine(seed,m_action);
	for (const double& lvalue : m_values)
        {
        boost::hash_combine(seed,lvalue);
        }
	boost::hash_combine(seed,FMTSpec::hash());
	if (period >= 0)
		{
		boost::hash_combine(seed, period);
		}
	if (withyield)
		{
		boost::hash_combine(seed,m_yield);
		}
	return seed;
	}


FMTOutputSourceComparator::FMTOutputSourceComparator(bool lvariable) : m_variable(lvariable) {}

bool FMTOutputSourceComparator::operator()(const FMTOutputSource& source) const
	{
	if (m_variable)
		{
		return source.isVariable();
	}else {
		return source.isConstant();
	}
	return false;
	}





}
