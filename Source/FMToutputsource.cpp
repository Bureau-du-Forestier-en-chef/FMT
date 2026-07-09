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

FMToutputsource::FMToutputsource() : FMTspec(), mask(),
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


FMToutputsource::FMToutputsource(const FMTotar ltarget,double lvalue, std::string lyield,
	std::string laction,int origin,int ttarget) : FMTspec(),mask(),
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


FMToutputsource::FMToutputsource(const FMTotar ltarget,std::vector<double>lvalues, int origin, int ttarget, std::string llevelname): FMTspec(),mask(),
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

FMToutputsource::FMToutputsource(const FMTspec& spec,const FMTmask& lmask,
               const FMTotar ltarget,std::string lyield,
			std::string laction, int origin,int ttarget) :
                FMTspec(spec),mask(lmask),
                target(ltarget),action(laction),yield(lyield), levelname(), values(), average(), sum(), outputorigin(origin), themetarget(ttarget)
    {


    }

FMToutputsource::FMToutputsource(const FMToutputsource& rhs) : 
	FMTspec(rhs),mask(rhs.mask),target(rhs.target),
    action(rhs.action),yield(rhs.yield), levelname(rhs.levelname),
	values(rhs.values), average(rhs.average),
	sum(rhs.sum), outputorigin(rhs.outputorigin), themetarget(rhs.themetarget)
    {

    }

FMToutputsource& FMToutputsource::operator = (const FMToutputsource& rhs)
    {
    if(this!=&rhs)
        {
        target = rhs.target;
        values = rhs.values;
        action = rhs.action;
        yield = rhs.yield;
		levelname = rhs.levelname;
        FMTspec::operator=(rhs);
        mask = rhs.mask;
		average = rhs.average;
		sum = rhs.sum;
		outputorigin = rhs.outputorigin;
		themetarget=rhs.themetarget;
        }
    return *this;
    }

void FMToutputsource::fillHashMask(Core::FMTmask& baseMask) const
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

bool FMToutputsource::isEqualByValue(const FMToutputsource& rhs) const
	{
	return (FMTspec::operator == (rhs) && mask == rhs.mask && target == rhs.target &&
		yield == rhs.yield && action == rhs.action && values == rhs.values &&
		levelname == rhs.levelname && average == rhs.average && sum == rhs.sum);
	}


void FMToutputsource::fillHashSpec(Core::FMTmask& baseMask) const
	{
	baseMask.binarizedAppend<std::string>(FMTspec::operator std::string());
	}


FMToutputsource::operator std::string() const
    {
	std::string line = "";
	if (!mask.empty())
		{
		line += std::string(mask)+" ";
		}
	if (!FMTspec::empty())
		{
		line += FMTspec::operator std::string() + " ";
		}
    switch (target)
        {
        case FMTotar::val:
            for (const double lvalue : values)
                {
                line += FMToutputsource::trimDouble(std::to_string(lvalue))  + " ";
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

bool FMToutputsource::operator < (const FMToutputsource& rhs) const
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
	if (FMTspec::operator < (rhs))
		return true;
	return false;
	}

bool FMToutputsource::operator == (const FMToutputsource& rhs) const
	{
	return (FMTspec::operator == (rhs) && target == rhs.target && mask==rhs.mask  && action == rhs.action);
	}

bool FMToutputsource::operator != (const FMToutputsource& rhs) const
	{
	return (!(*this==rhs));
	}

void FMToutputsource::setThemeTarget(const int& newttarget)
	{
		themetarget=newttarget;
	}
void FMToutputsource::setOutputOrigin(const int& neworigin)
	{
		outputorigin=neworigin;
	}

void FMToutputsource::resetValues(const FMToperator& op,const FMToutputsource& other)
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

bool FMToutputsource::isSubsetOf(const FMToutputsource& rhs) const
	{
	if ((this->isVariable() && rhs.isVariable() &&
		target == rhs.target && FMTspec::isSubsetOf(rhs) &&
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

bool FMToutputsource::isSameButDifferentAction(const FMToutputsource& rhs) const
	{
	return (FMTspec::operator == (rhs) && target == rhs.target && mask == rhs.mask && action != rhs.action);
	}


bool FMToutputsource::isInAggregate(const FMToutputsource& rhs, const std::vector<Core::FMTaction>& actions) const
	{
	if (isAction() && rhs.isAction())
		{
		const std::vector<const FMTaction*>allactions = FMTactioncomparator(rhs.action).getAllAggregates(actions);
		for (const FMTaction* actptr : allactions)
			{
				if (actptr->getname() == action)
				{
					return true;
				}
			}
		}
	return false;
	}
	

bool FMToutputsource::isSubsetOf(const FMToutputsource& rhs,
	const std::vector<Core::FMTaction>& actions) const
	{
	
	if ((this->isVariable() && rhs.isVariable() && 
		target == rhs.target && FMTspec::isSubsetOf(rhs) && 
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

FMToutputsource FMToutputsource::presolve(const FMTmaskfilter& filter, const std::vector<FMTtheme>& newthemes) const
	{
	FMToutputsource newsource(*this);
	if (newsource.isVariable())
		{
		newsource.mask = newsource.mask.presolve(filter, newthemes);
		}
	return newsource;
	}

bool FMToutputsource::canBeDeducedToConstant() const
	{
	return (isVariable() && isInventory() &&
		!emptyPeriod() && getPeriodLowerBound() == 0 &&
		getPeriodUpperBound() == 0);
	}

double FMToutputsource::getConstantValue(const std::vector<Core::FMTactualdevelopment>& area,
	const std::vector<Core::FMTaction>&actions, const FMTyields& yields) const
	{
	std::vector<const FMTaction*> operabletoactions = this->targets(actions);
	double value = 0;
	for (const Core::FMTactualdevelopment& development : area)
		{
		if (use(development,yields)  && operabletoactions.empty()
			|| development.anyOperable(operabletoactions, yields))
			{
			double developmentvalue = development.getarea();
			if (!yield.empty())
				{
				developmentvalue*=development.getInventoryCoef(yields, yield);
				}
			value += developmentvalue;
			}
		}
	return value;
	}


void FMToutputsource::setAverage()
	{
	average = true;
	}


void FMToutputsource::setSum()
{
	sum = true;
}

bool FMToutputsource::isNull(const FMTyields& ylds) const
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

void  FMToutputsource::pushValues(const std::vector<double>& newvalues)
	{
	values.insert(values.end(),newvalues.begin(), newvalues.end());
	}

double FMToutputsource::getValue(int period) const
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

void FMToutputsource::setMask(const FMTmask& newmask)
	{
	mask = newmask;
	}

void FMToutputsource::setAction(const std::string& actionname)
	{
	action = actionname;
	}

void FMToutputsource::setTarget(const FMTotar& newtype)
	{
	target = newtype;
	}

std::vector<const FMTaction*>FMToutputsource::targets(const std::vector<FMTaction>& actions) const
	{
	if (target != FMTotar::level && !action.empty())
        {
		return FMTactioncomparator(action).getAllAggregates(actions);
        }
	return std::vector<const FMTaction*>();
	}

std::unordered_set<int>FMToutputsource::targetsSet(const std::vector<FMTaction>& actions) const
{
	if (target != FMTotar::level && !action.empty())
	{
		return FMTactioncomparator(action).getAllAggregatesSet(actions);
	}
	return std::unordered_set<int>();
}

bool FMToutputsource::use(const FMTdevelopment& development, const FMTyields& ylds,
	const Graph::FMTgraphvertextoyield* graphinfo) const
{
	return (development.getmask().isSubsetOf(mask) && development.is(*this, ylds, graphinfo));
}

std::string FMToutputsource::trimDouble(const std::string& string_number)
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

void FMToutputsource::setYield(const std::string& p_Yield)
	{
	yield = p_Yield;
	}

double FMToutputsource::getCoef(const FMTdevelopment& development,
	const FMTyields& yields,
	const Graph::FMTgraphvertextoyield* graphinfo,
	std::vector<FMTdevelopmentpath> const * paths,
	FMTaction const * modelaction) const
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
				coef = getValue(development.getperiod());
			}
			if (isTimeYield())
				{
				coef *= development.getInventoryCoef(yields, yield, graphinfo);
				}
			}
	return coef;
	}

size_t FMToutputsource::hash(int period, bool withyield) const
	{
	size_t seed = 0;
	boost::hash_combine(seed,mask.hash());
	boost::hash_combine(seed,target);
	boost::hash_combine(seed,action);
	for (const double& lvalue : values)
        {
        boost::hash_combine(seed,lvalue);
        }
	boost::hash_combine(seed,FMTspec::hash());
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


FMToutputsourcecomparator::FMToutputsourcecomparator(bool lvariable) : variable(lvariable) {}

bool FMToutputsourcecomparator::operator()(const FMToutputsource& source) const
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
