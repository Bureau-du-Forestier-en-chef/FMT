/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMToutputsource.h"
#include "FMTgraphvertextoyield.h"
#include "FMTaction.h"
#include "FMTyields.h"
#include "FMTdevelopment.h"
#include "FMTactualdevelopment.h"
#include "FMToperator.h"

namespace Core{

FMTOutputSource::FMTOutputSource() : FMTSpec(), mask(),
		target(FMTotar::val),
		action(),
		yield(),
		levelname(),
		values(),
		average(),
		sum(),
		outputorigin(-1),
		themetarget(-1)
	{

	}


FMTOutputSource::FMTOutputSource(const FMTotar ltarget,double lvalue, std::string lyield,
	std::string laction,int origin,int ttarget) : FMTSpec(),mask(),
    target(ltarget),
    action(laction),
    yield(lyield),
	levelname(),
    values(1,lvalue),
	average(),
	sum(),
	outputorigin(origin),
	themetarget(ttarget)
    {
	if (isTimeYield())
		{
		values.clear();
		}

    }


FMTOutputSource::FMTOutputSource(const FMTotar ltarget,std::vector<double>lvalues, int origin, int ttarget, std::string llevelname): FMTSpec(),mask(),
    target(ltarget),
    action(),
    yield(),
	levelname(llevelname),
    values(lvalues),
	average(),
	sum(),
	outputorigin(origin),
	themetarget(ttarget)
    {



    }

FMTOutputSource::FMTOutputSource(const FMTSpec& spec,const FMTMask& lmask,
               const FMTotar ltarget,std::string lyield,
			std::string laction, int origin,int ttarget) :
                FMTSpec(spec),mask(lmask),
                target(ltarget),action(laction),yield(lyield), levelname(), values(), average(), sum(), outputorigin(origin), themetarget(ttarget)
    {


    }

FMTOutputSource::FMTOutputSource(const FMTOutputSource& rhs) : 
	FMTSpec(rhs),mask(rhs.mask),target(rhs.target),
    action(rhs.action),yield(rhs.yield), levelname(rhs.levelname),
	values(rhs.values), average(rhs.average),
	sum(rhs.sum), outputorigin(rhs.outputorigin), themetarget(rhs.themetarget)
    {

    }

FMTOutputSource& FMTOutputSource::operator = (const FMTOutputSource& rhs)
    {
    if(this!=&rhs)
        {
        target = rhs.target;
        values = rhs.values;
        action = rhs.action;
        yield = rhs.yield;
		levelname = rhs.levelname;
        FMTSpec::operator=(rhs);
        mask = rhs.mask;
		average = rhs.average;
		sum = rhs.sum;
		outputorigin = rhs.outputorigin;
		themetarget=rhs.themetarget;
        }
    return *this;
    }

void FMTOutputSource::fillHashMask(Core::FMTMask& baseMask) const
	{
		//baseMask.append(mask.getBitsetReference());
		baseMask.binarizedAppend<std::string>(std::string(mask));
		baseMask.binarizedAppend<int>(static_cast<int>(target));
		for (const double& lvalue : values)
		{
			baseMask.binarizedAppend<double>(lvalue);
		}
		baseMask.binarizedAppend<std::string>(yield);
		baseMask.binarizedAppend<std::string>(action);
	}

bool FMTOutputSource::isEqualByValue(const FMTOutputSource& rhs) const
	{
	return (FMTSpec::operator == (rhs) && mask == rhs.mask && target == rhs.target &&
		yield == rhs.yield && action == rhs.action && values == rhs.values &&
		levelname == rhs.levelname && average == rhs.average && sum == rhs.sum);
	}


void FMTOutputSource::fillHashSpec(Core::FMTMask& baseMask) const
	{
	baseMask.binarizedAppend<std::string>(FMTSpec::operator std::string());
	}


FMTOutputSource::operator std::string() const
    {
	std::string line = "";
	if (!mask.empty())
		{
		line += std::string(mask)+" ";
		}
	if (!FMTSpec::empty())
		{
		line += FMTSpec::operator std::string() + " ";
		}
    switch (target)
        {
        case FMTotar::val:
            for (const double lvalue : values)
                {
                line += FMTOutputSource::trimDouble(std::to_string(lvalue))  + " ";
                }
            if (!values.empty())
                {
                line.pop_back();
                }
        break;
		case FMTotar::timeyld:
			line += yield;
		break;
        case FMTotar::actual:
            if (!action.empty())
                {
                line += action +" ";
                }
            if (!yield.empty())
                {
                line += yield+" ";
                }else{
                line+="_AREA";
                }
        break;
        case FMTotar::inventory:
            if (!action.empty())
                {
                line+="_INVENT("+action+") ";
				}
				/*else if (this->lock.getLower() > 0)
				{
				line += "_INVLOCK ";

				}*/else if (emptyLock()) {
                line+= "_INVENT ";
                }
            if (!yield.empty())
                {
                line += yield;
                }else{
                line+="_AREA";
                }
        break;
        case FMTotar::level:
			line += (action + levelname);
			/*if (!action.empty())
                {
                line += action;
                }else{
                for (const double lvalue : values)
                    {
                    line += std::to_string(lvalue) + " ";
                    }
                 if (!values.empty())
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
	if (mask < rhs.mask)
		return true;
	if (rhs.mask < mask)
		return false;
	if (target < rhs.target)
		return true;
	if (rhs.target < target)
		return false;
	if (action < rhs.action)
		return true;
	if (rhs.action < action)
		return false;
	if (FMTSpec::operator < (rhs))
		return true;
	return false;
	}

bool FMTOutputSource::operator == (const FMTOutputSource& rhs) const
	{
	return (FMTSpec::operator == (rhs) && target == rhs.target && mask==rhs.mask  && action == rhs.action);
	}

bool FMTOutputSource::operator != (const FMTOutputSource& rhs) const
	{
	return (!(*this==rhs));
	}

void FMTOutputSource::setThemeTarget(const int& newttarget)
	{
		themetarget=newttarget;
	}
void FMTOutputSource::setOutputOrigin(const int& neworigin)
	{
		outputorigin=neworigin;
	}

void FMTOutputSource::resetValues(const FMTOperator& op,const FMTOutputSource& other)
	{
	std::vector<double>newvalues(std::max(other.values.size(), values.size()),0);
	for (int period = 1; period < static_cast<int>(newvalues.size()+1);++period)
		{
		const double othervalue = other.getValue(period);
		newvalues[period-1]=op.call(getValue(period), othervalue);
		}
	values.swap(newvalues);
	if (other.isTimeYield())
		{
		yield = other.yield;
		target = FMTotar::timeyld;
		}
	}

bool FMTOutputSource::isSubsetOf(const FMTOutputSource& rhs) const
	{
	if ((this->isVariable() && rhs.isVariable() &&
		target == rhs.target && FMTSpec::isSubsetOf(rhs) &&
		!((!action.empty() && rhs.action.empty()) || (!rhs.action.empty() && action.empty()))) &&
		(mask.isSubsetOf(rhs.mask) &&
		((action.empty() && rhs.action.empty()) ||
			(!action.empty() && !rhs.action.empty() &&
			(action == rhs.action)))))
		{
			return true;
		}
	return false;
	}

bool FMTOutputSource::isSameButDifferentAction(const FMTOutputSource& rhs) const
	{
	return (FMTSpec::operator == (rhs) && target == rhs.target && mask == rhs.mask && action != rhs.action);
	}


bool FMTOutputSource::isInAggregate(const FMTOutputSource& rhs, const std::vector<Core::FMTAction>& actions) const
	{
	if (isAction() && rhs.isAction())
		{
		const std::vector<const FMTAction*>allactions = FMTActionComparator(rhs.action).getAllAggregates(actions);
		for (const FMTAction* actptr : allactions)
			{
				if (actptr->getName() == action)
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
		target == rhs.target && FMTSpec::isSubsetOf(rhs) && 
		!((!action.empty() && rhs.action.empty()) || (!rhs.action.empty() && action.empty()))) && 
		(mask.isSubsetOf(rhs.mask) &&
		((action.empty() && rhs.action.empty()) || 
		(!action.empty() && !rhs.action.empty() && 
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
		newsource.mask = newsource.mask.presolve(filter, newthemes);
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
			if (!yield.empty())
				{
				developmentvalue*=development.getInventoryCoef(yields, yield);
				}
			value += developmentvalue;
			}
		}
	return value;
	}


void FMTOutputSource::setAverage()
	{
	average = true;
	}


void FMTOutputSource::setSum()
{
	sum = true;
}

bool FMTOutputSource::isNull(const FMTYields& ylds) const
	{
	if (!yield.empty())
		{
		return ylds.isNullYld(yield);
		}
	if (!isVariable())
		{
		return (getValue() == 0);
		}
	return false;
	}

void  FMTOutputSource::pushValues(const std::vector<double>& newvalues)
	{
	values.insert(values.end(),newvalues.begin(), newvalues.end());
	}

double FMTOutputSource::getValue(int period) const
	{
	double returnvalue = 0;
	if (target == FMTotar::timeyld&&
		values.empty())
		{
		return 1;
		}
	if (target == FMTotar::val||target == FMTotar::level||target==FMTotar::timeyld)
		{
		--period;
		period = std::max(period, 0);//Cannot get negative period
        if (period >= static_cast<int>(values.size()))
            {
            period = static_cast<int>(values.size() - 1);
            }
		returnvalue= values.at(period);
		}
	return returnvalue;
	}

void FMTOutputSource::setMask(const FMTMask& newmask)
	{
	mask = newmask;
	}

void FMTOutputSource::setAction(const std::string& actionname)
	{
	action = actionname;
	}

void FMTOutputSource::setTarget(const FMTotar& newtype)
	{
	target = newtype;
	}

std::vector<const FMTAction*>FMTOutputSource::targets(const std::vector<FMTAction>& actions) const
	{
	if (target != FMTotar::level && !action.empty())
        {
		return FMTActionComparator(action).getAllAggregates(actions);
        }
	return std::vector<const FMTAction*>();
	}

std::unordered_set<int>FMTOutputSource::targetsSet(const std::vector<FMTAction>& actions) const
{
	if (target != FMTotar::level && !action.empty())
	{
		return FMTActionComparator(action).getAllAggregatesSet(actions);
	}
	return std::unordered_set<int>();
}

bool FMTOutputSource::use(const FMTDevelopment& development, const FMTYields& ylds,
	const Graph::FMTgraphvertextoyield* graphinfo) const
{
	return (development.getMask().isSubsetOf(mask) && development.is(*this, ylds, graphinfo));
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
	yield = p_Yield;
	}

double FMTOutputSource::getCoef(const FMTDevelopment& development,
	const FMTYields& yields,
	const Graph::FMTgraphvertextoyield* graphinfo,
	std::vector<FMTDevelopmentPath> const * paths,
	FMTAction const * modelaction) const
	{
	double coef = 1;
	if (isVariable())
		{
		if (!yield.empty())
			{
			if (target == FMTotar::inventory)
			{
				coef = development.getInventoryCoef(yields, yield, graphinfo);
			}
			else {
				coef = development.getHarvestCoef(*paths, *modelaction, yields, yield, graphinfo);
			}
			}
		}else{
			if (!values.empty())
			{
				coef = getValue(development.getPeriod());
			}
			if (isTimeYield())
				{
				coef *= development.getInventoryCoef(yields, yield, graphinfo);
				}
			}
	return coef;
	}

size_t FMTOutputSource::hash(int period, bool withyield) const
	{
	size_t seed = 0;
	boost::hash_combine(seed,mask.hash());
	boost::hash_combine(seed,target);
	boost::hash_combine(seed,action);
	for (const double& lvalue : values)
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
		boost::hash_combine(seed,yield);
		}
	return seed;
	}


FMTOutputSourceComparator::FMTOutputSourceComparator(bool lvariable) : variable(lvariable) {}

bool FMTOutputSourceComparator::operator()(const FMTOutputSource& source) const
	{
	if (variable)
		{
		return source.isVariable();
	}else {
		return source.isConstant();
	}
	return false;
	}





}
