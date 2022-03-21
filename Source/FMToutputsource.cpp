/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMToutputsource.hpp"
#include "FMTgraphvertextoyield.hpp"
#include "FMTaction.hpp"
#include "FMTyields.hpp"
#include "FMTdevelopment.hpp"
#include "FMTactualdevelopment.hpp"
#include "FMToperator.hpp"

namespace Core{

FMToutputsource::FMToutputsource() : FMTspec(), mask(),
		target(FMTotar::val),
		action(),
		yield(),
		values(),
		average(),
		outputorigin(-1),
		themetarget(-1)
	{

	}


FMToutputsource::FMToutputsource(const FMTotar ltarget,double lvalue, std::string lyield,
	std::string laction,int origin,int ttarget) : FMTspec(),mask(),
    target(ltarget),
    action(laction),
    yield(lyield),
    values(1,lvalue),
	average(),
	outputorigin(origin),
	themetarget(ttarget)
    {
	if (istimeyield())
		{
		values.clear();
		}

    }


FMToutputsource::FMToutputsource(const FMTotar ltarget,std::vector<double>lvalues, int origin, int ttarget): FMTspec(),mask(),
    target(ltarget),
    action(),
    yield(),
    values(lvalues),
	average(),
	outputorigin(origin),
	themetarget(ttarget)
    {



    }

FMToutputsource::FMToutputsource(const FMTspec& spec,const FMTmask& lmask,
               const FMTotar ltarget,std::string lyield,
			std::string laction, int origin,int ttarget) :
                FMTspec(spec),mask(lmask),
                target(ltarget),action(laction),yield(lyield),values(),average(), outputorigin(origin),themetarget(ttarget)
    {


    }

FMToutputsource::FMToutputsource(const FMToutputsource& rhs) : FMTspec(rhs),mask(rhs.mask),target(rhs.target),
    action(rhs.action),yield(rhs.yield),values(rhs.values), average(rhs.average),outputorigin(rhs.outputorigin),themetarget(rhs.themetarget)
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
        FMTspec::operator=(rhs);
        mask = rhs.mask;
		average = rhs.average;
		outputorigin = rhs.outputorigin;
		themetarget=rhs.themetarget;
        }
    return *this;
    }

void FMToutputsource::fillhashmask(Core::FMTmask& basemask) const
	{
		//basemask.append(mask.getbitsetreference());
		basemask.binarizedappend<std::string>(std::string(mask));
		basemask.binarizedappend<int>(static_cast<int>(target));
		for (const double& lvalue : values)
		{
			basemask.binarizedappend<double>(lvalue);
		}
		basemask.binarizedappend<std::string>(yield);
		basemask.binarizedappend<std::string>(action);
	}

bool FMToutputsource::isequalbyvalue(const FMToutputsource& rhs) const
	{
	return (FMTspec::operator == (rhs) && mask == rhs.mask && target == rhs.target && yield == rhs.yield && action == rhs.action && values == rhs.values);
	}


void FMToutputsource::fillhashspec(Core::FMTmask& basemask) const
	{
	basemask.binarizedappend<std::string>(FMTspec::operator std::string());
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
                line += std::to_string(lvalue) + " ";
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
				else if (this->lock.getlower() > 0)
				{
				line += "_INVLOCK ";
				}else{
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
            if (!action.empty())
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
                }
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

void FMToutputsource::setthemetarget(const int& newttarget)
	{
		themetarget=newttarget;
	}
void FMToutputsource::setoutputorigin(const int& neworigin)
	{
		outputorigin=neworigin;
	}

void FMToutputsource::resetvalues(const FMToperator& op,const FMToutputsource& other)
	{
	std::vector<double>newvalues(std::max(other.values.size(), values.size()),0);
	for (int period = 1; period < static_cast<int>(newvalues.size()+1);++period)
		{
		const double othervalue = other.getvalue(period);
		newvalues[period-1]=op.call(getvalue(period), othervalue);
		}
	values.swap(newvalues);
	if (other.istimeyield())
		{
		yield = other.yield;
		target = FMTotar::timeyld;
		}
	}

bool FMToutputsource::issubsetof(const FMToutputsource& rhs) const
	{
	if ((this->isvariable() && rhs.isvariable() &&
		target == rhs.target && FMTspec::issubsetof(rhs) &&
		!((!action.empty() && rhs.action.empty()) || (!rhs.action.empty() && action.empty()))) &&
		(mask.issubsetof(rhs.mask) &&
		((action.empty() && rhs.action.empty()) ||
			(!action.empty() && !rhs.action.empty() &&
			(action == rhs.action)))))
		{
			return true;
		}
	return false;
	}

bool FMToutputsource::issamebutdifferentaction(const FMToutputsource& rhs) const
	{
	return (FMTspec::operator == (rhs) && target == rhs.target && mask == rhs.mask && action != rhs.action);
	}


bool FMToutputsource::isinaggregate(const FMToutputsource& rhs, const std::vector<Core::FMTaction>& actions) const
	{
	if (isaction() && rhs.isaction())
		{
		const std::vector<const FMTaction*>allactions = FMTactioncomparator(rhs.action).getallaggregates(actions);
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
	

bool FMToutputsource::issubsetof(const FMToutputsource& rhs,
	const std::vector<Core::FMTaction>& actions) const
	{
	
	if ((this->isvariable() && rhs.isvariable() && 
		target == rhs.target && FMTspec::issubsetof(rhs) && 
		!((!action.empty() && rhs.action.empty()) || (!rhs.action.empty() && action.empty()))) && 
		(mask.issubsetof(rhs.mask) &&
		((action.empty() && rhs.action.empty()) || 
		(!action.empty() && !rhs.action.empty() && 
			isinaggregate(rhs,actions)))))
			{
			return true;
			}
	return false;
	}

FMToutputsource FMToutputsource::presolve(const FMTmaskfilter& filter, const std::vector<FMTtheme>& newthemes) const
	{
	FMToutputsource newsource(*this);
	if (newsource.isvariable())
		{
		newsource.mask = newsource.mask.presolve(filter, newthemes);
		}
	return newsource;
	}

bool FMToutputsource::canbededucedtoconstant() const
	{
	return (isvariable() && isinventory() &&
		!emptyperiod() && getperiodlowerbound() == 0 &&
		getperiodupperbound() == 0);
	}

double FMToutputsource::getconstantvalue(const std::vector<Core::FMTactualdevelopment>& area,
	const std::vector<Core::FMTaction>&actions, const FMTyields& yields) const
	{
	std::vector<const FMTaction*> operabletoactions = this->targets(actions);
	double value = 0;
	for (const Core::FMTactualdevelopment& development : area)
		{
		if (use(development,yields)  && operabletoactions.empty()
			|| development.anyoperable(operabletoactions, yields))
			{
			double developmentvalue = development.getarea();
			if (!yield.empty())
				{
				developmentvalue*=development.getinventorycoef(yields, yield);
				}
			value += developmentvalue;
			}
		}
	return value;
	}


void FMToutputsource::setaverage()
	{
	average = true;
	}

bool FMToutputsource::isnull(const FMTyields& ylds) const
	{
	if (!yield.empty())
		{
		return ylds.isnullyld(yield);
		}
	if (!isvariable())
		{
		return (getvalue() == 0);
		}
	return false;
	}

void  FMToutputsource::pushvalues(const std::vector<double>& newvalues)
	{
	values.insert(values.end(),newvalues.begin(), newvalues.end());
	}

double FMToutputsource::getvalue(int period) const
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

void FMToutputsource::setmask(const FMTmask& newmask)
	{
	mask = newmask;
	}

void FMToutputsource::setaction(const std::string& actionname)
	{
	action = actionname;
	}

void FMToutputsource::settarget(const FMTotar& newtype)
	{
	target = newtype;
	}

std::vector<const FMTaction*>FMToutputsource::targets(const std::vector<FMTaction>& actions) const
	{
	if (target != FMTotar::level && !action.empty())
        {
		return FMTactioncomparator(action).getallaggregates(actions);
        }
	return std::vector<const FMTaction*>();
	}

std::unordered_set<int>FMToutputsource::targetsset(const std::vector<FMTaction>& actions) const
{
	if (target != FMTotar::level && !action.empty())
	{
		return FMTactioncomparator(action).getallaggregatesset(actions);
	}
	return std::unordered_set<int>();
}


double FMToutputsource::getcoef(const FMTdevelopment& development,
	const FMTyields& yields,
	const Graph::FMTgraphvertextoyield* graphinfo,
	std::vector<FMTdevelopmentpath> const * paths,
	FMTaction const * modelaction) const
	{
	double coef = 1;
	if (isvariable())
		{
		if (!yield.empty())
			{
			if (target == FMTotar::inventory)
			{
				coef = development.getinventorycoef(yields, yield, graphinfo);
			}
			else {
				coef = development.getharvestcoef(*paths, *modelaction, yields, yield, graphinfo);
			}
			}
		}else{
			if (!values.empty())
			{
				coef = getvalue(development.getperiod());
			}
			if (istimeyield())
				{
				coef*= development.getinventorycoef(yields, yield, graphinfo);
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
		return source.isvariable();
	}else {
		return source.isconstant();
	}
	return false;
	}





}
