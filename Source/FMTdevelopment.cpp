/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTdevelopment.h"
#include "FMTfuturdevelopment.h"
#include "FMTyieldrequest.h"
#include "FMTgraphvertextoyield.h"
#include "FMTlifespans.h"
#include "FMTtransition.h"
#include "FMTaction.h"
#include "FMTtransitionmask.h"
#include "FMTyields.h"
#include "FMTdevelopmentpath.h"
#include "FMTbounds.hpp"
#include "FMTexceptionhandler.h"
#include <array>

namespace Core{

	FMTdevelopment::FMTdevelopment(FMTdevelopment&& rhs) :
		FMTobject(std::move(rhs)),
		mask(std::move(rhs.mask)),
		age(std::move(rhs.age)),
		lock(std::move(rhs.lock)),
		period(std::move(rhs.period))
	{

	}

	FMTdevelopment& FMTdevelopment::operator=(FMTdevelopment&& rhs)
	{
		FMTobject::operator=(std::move(rhs));
		mask = std::move(rhs.mask);
		age = std::move(rhs.age);
		lock = std::move(rhs.lock);
		period = std::move(rhs.period);
		return *this;
	}

	void FMTdevelopment::setage(const int& lage)
	{
		try {
			age = static_cast<uint8_t>(lage);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("for " + std::string(*this), "FMTdevelopment::setage", __LINE__, __FILE__);
		}
	}

	void FMTdevelopment::setlock(const int& llock)
	{
		try{
		lock = static_cast<uint8_t>(llock);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("for " + std::string(*this), "FMTdevelopment::setlock", __LINE__, __FILE__);
		}
	}

	void FMTdevelopment::setperiod(const int& lperiod)
	{
		try{
		period = static_cast<uint8_t>(lperiod);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("for " + std::string(*this), "FMTdevelopment::setperiod", __LINE__, __FILE__);
		}
	}

	void FMTdevelopment::setmask(const Core::FMTmask& lmask)
	{
		mask = lmask;
	}

  FMTdevelopment::FMTdevelopment():FMTobject(), mask(),age(),lock(),period(0)
  {


  }



  FMTdevelopment::FMTdevelopment(const FMTmask& lmask,const int& lage,const int& llock) : FMTobject(),
	  mask(lmask),
	  age(static_cast<uint8_t>(lage)),
	  lock(static_cast<uint8_t>(llock)),
	  period(0)
        {

        }
  FMTdevelopment::FMTdevelopment(const FMTmask&  lmask,const int& lage,const int& llock,const int& lperiod): 
	  FMTobject(),
	  mask(lmask),
	  age(static_cast<uint8_t>(lage)),
	  lock(static_cast<uint8_t>(llock)),
	  period(static_cast<uint8_t>(lperiod))
        {

        }
  
    FMTdevelopment::FMTdevelopment(const FMTdevelopment& rhs) :
		FMTobject(rhs),
        mask(rhs.mask),
        age(rhs.age),
        lock(rhs.lock),
        period(rhs.period)
        {


        }

     FMTdevelopment& FMTdevelopment::operator = (const FMTdevelopment& rhs)
        {
        if (this!=&rhs)
            {
			FMTobject::operator=(rhs);
            mask = rhs.mask;
            age = rhs.age;
            lock = rhs.lock;
            period = rhs.period;
            }
        return *this;
        }
    FMTfuturdevelopment FMTdevelopment::grow() const
        {
        FMTfuturdevelopment newdev(*this);
		try {
			++newdev.age;
			if (newdev.lock > 0)
			{
				--newdev.lock;
			}
			++newdev.period;
		}catch (...)
			{
			_exhandler->raisefromcatch("for " + std::string(*this), "FMTdevelopment::grow", __LINE__, __FILE__);
			}
        return newdev;
        }

	bool FMTdevelopment::worthtestingoperability(const FMTaction& action) const
		{
		return (((action.dorespectlock() && lock == 0) || !action.dorespectlock()) &&
			action.getagelowerbound() <= getage() && getage() <= action.getageupperbound() &&
			action.getperiodlowerbound() <= getperiod() && getperiod() <= action.getperiodupperbound());
		}

	FMTyieldrequest FMTdevelopment::getyieldrequest(const Graph::FMTgraphvertextoyield* graphyieldrequest) const
		{
			if (graphyieldrequest != nullptr)
			{
				return FMTyieldrequest(*this,*graphyieldrequest);
				
			}
			return FMTyieldrequest(*this);
		}

     bool FMTdevelopment::operable(const FMTaction& action,const FMTyields& ylds, const Graph::FMTgraphvertextoyield* graphyieldrequest) const
        {
		 try{
			if (worthtestingoperability(action))
				{
				for (const FMTaction::const_iterator spec: action.findsets(mask))
					{
					if (is(spec->second, ylds,graphyieldrequest))
						{
						return true;
						}
					}
				}
		 }catch (...)
			{
			 _exhandler->raisefromcatch("for " + std::string(*this)+" for action "+action.getname(), "FMTdevelopment::operable", __LINE__, __FILE__);
			}
        return false;
        }
	 bool FMTdevelopment::anyoperable(const std::vector<const FMTaction*>& actions, const FMTyields& ylds,
		 const Graph::FMTgraphvertextoyield* graphyieldrequest) const
		{
		 try{
		 for (const FMTaction* action : actions)
			{
			 if (this->operable(*action,ylds,graphyieldrequest))
				{
				return true;
				}
			}
		 }
		 catch (...)
		 {
			 _exhandler->raisefromcatch("for " + std::string(*this), "FMTdevelopment::anyoperable", __LINE__, __FILE__);
		 }
		 return false;
		}

	 std::vector<int> FMTdevelopment::anyworthtestingoperability(const std::vector<const FMTaction*>& actions, const FMTaction& firstaction) const
		{
		 std::vector<int>potentials;
		 try{
			for (const FMTaction* action : actions)
				{
				 if (this->worthtestingoperability(*action))
					{
					 const int location = static_cast<int>(std::distance(&firstaction, action));
					 potentials.push_back(location);
					}
				}
		 }
		 catch (...)
		 {
			 _exhandler->raisefromcatch("for " + std::string(*this), "FMTdevelopment::anyworthtestingoperability", __LINE__, __FILE__);
		 }
		 return potentials;
		}

	 std::vector<bool> FMTdevelopment::isanyworthtestingoperability(const std::vector<const FMTaction*>& actions,
		 const std::vector<FMTaction>& allactions) const noexcept
	 {
		 try {
			 const FMTaction* firstaction = &(*allactions.begin());
			 std::vector<int8_t>worth;
			 worth.reserve(actions.size());
			 for (const FMTaction* action : actions)
			 {
				 if (this->worthtestingoperability(*action))
				 {
					 worth.push_back(static_cast<int8_t>(std::distance(firstaction, action)));
				 }
			 }
			 if (!worth.empty())
			 {
				 std::vector<bool>returnvalues(allactions.size(), false);
				 for (const int8_t& index : worth)
					 {
					 returnvalues[index] = true;
					 }
				 return returnvalues;
			 }

		 }
		 catch (...)
		 {
			 _exhandler->raisefromcatch("for " + std::string(*this), "FMTdevelopment::isanyworthtestingoperability", __LINE__, __FILE__);
		 }
		 return std::vector<bool>();
	 }

	 std::vector<FMTdevelopmentpath> FMTdevelopment::operate(const FMTaction& action,
		 const FMTtransition& Transition,
		 const FMTyields& ylds,
		 const std::vector<FMTtheme>& themes) const
		{
		 std::vector<FMTdevelopmentpath>newpaths;
		 try {
			 const FMTfork* fork = Transition.getfork(*this,ylds);
			 if (fork)
			 {
				 newpaths = fork->getpaths(*this, ylds, themes, action.isresetage());
		
			 }
			 else {
				 std::string yldsStr;
				 for (const FMTtransition::const_iterator fork : Transition.findsets(getmask()))
				 {
					 for (const std::string& YLD_NAME : fork->second.getylds())
					 {
						 if (yldsStr.find(YLD_NAME)==std::string::npos)
						 {
							 yldsStr += YLD_NAME + ":" + std::to_string(ylds.get(getyieldrequest(), YLD_NAME)) + "\n";
						 }
					 }
				 }
				 if (!yldsStr.empty())
				 {
					 yldsStr.pop_back();
					 yldsStr.insert(0, "\n");
				 }
				 _exhandler->raise(Exception::FMTexc::FMTinvalid_transition_case,Transition.getname() + " for " + std::string(*this) + yldsStr,
					 "FMTdevelopment::operate",__LINE__, __FILE__);
			 }
		 }catch (...)
			{
			 _exhandler->raisefromcatch("for " + std::string(*this)+" for action "+action.getname(),"FMTdevelopment::operate", __LINE__, __FILE__);
			}
		 return newpaths;
		}


    bool FMTdevelopment::operator == (const FMTdevelopment& rhs) const
        {
        return (age == rhs.age && lock == rhs.lock && period == rhs.period && mask == rhs.mask);
        }
    bool FMTdevelopment::operator != (const FMTdevelopment& rhs) const
        {
        return !(*this==rhs);
        }
     bool FMTdevelopment::operator < (const FMTdevelopment& rhs) const
        {
		 //strict ordering
		 if (mask < rhs.mask)
			 return true;
		 if (rhs.mask < mask)
			 return false;
		 if (age < rhs.age)
			 return true;
		 if (rhs.age < age)
			 return false;
		 if (lock < rhs.lock)
			 return true;
		 if (rhs.lock < lock)
			 return false;
		 if (period < rhs.period)
			 return true;
		 if (rhs.period < period)
			 return false;
		 return false;
        }
    FMTdevelopment::operator std::string() const
        {
		std::string line = "";
        line+=std::string(mask)+" ";
        line+=std::to_string(getage())+" ";
        line+=std::to_string(getlock())+" ";
        line+=std::to_string(getperiod())+" ";
        return line;
        }
	double FMTdevelopment::getarea() const
		{
		return 0;
		}

	FMTdevelopment FMTdevelopment::clearlock() const
		{
		FMTdevelopment nolock(*this);
		nolock.lock = 0;
		return nolock;
		}

	bool FMTdevelopment::_isOutOfBound(const FMTyieldrequest& p_request,
									const FMTyields& p_yields,
									const Core::FMTyldbounds& p_bound,
									const std::string& p_yield) const
		{
		bool value = false;
		try {
			const double YIELD_VALUE = p_yields.get(p_request, p_yield);
			value = (p_bound.out(YIELD_VALUE));
		}catch (...)
			{
			_exhandler->raisefromcatch(
				"for " + std::string(*this) + " on yield "+p_yield+" on bound "+ std::string(p_bound),
				"FMTdevelopment::_isOutOfBound", __LINE__, __FILE__);
			}
		return value;
		}

	bool FMTdevelopment::is(const FMTspec& specification, const FMTyields& ylds,
		const Graph::FMTgraphvertextoyield* graphyieldrequest) const
		{
		bool allow = false;
		try {
			allow = specification.allowwithoutyield(getperiod(), getage(), getlock());
			if (allow && !specification.emptyylds())
				{
				const FMTyieldrequest REQUEST = getyieldrequest(graphyieldrequest);
				const std::vector<Core::FMTyldbounds>& BOUNDS= specification.getyldbounds();
				const std::vector<std::string>& YLD_NAMES = specification.getylds();
				size_t boundId = 0;
				while (allow && boundId < BOUNDS.size())
					{
					allow = !_isOutOfBound(REQUEST, ylds, BOUNDS.at(boundId), YLD_NAMES.at(boundId));
					++boundId;
					}
				}
		}catch (...)
			{
			std::string specStr;
			if (!specification.empty())
				{
				specStr = std::string(specification);
				}
			_exhandler->raisefromcatch(
				"for " + std::string(*this)+" with spec "+ specStr,
				"FMTdevelopment::is",__LINE__, __FILE__);
			}
		return allow;
		}
	double FMTdevelopment::getinventorycoef(const FMTyields& ylds, const std::string& target_yield,
		const Graph::FMTgraphvertextoyield* graphyieldrequest) const
	{
		try {
			const FMTyieldrequest request = getyieldrequest(graphyieldrequest);
			return ylds.get(request,target_yield);
		}catch (...)
		{
			_exhandler->raisefromcatch("for " + std::string(*this) , "FMTdevelopment::getinventorycoef", __LINE__, __FILE__);
		}
		return 0;
		}
	double FMTdevelopment::getharvestcoef(const std::vector<FMTdevelopmentpath>& topaths,
			const FMTaction& action,const FMTyields& ylds,const std::string& target_yield,
		const Graph::FMTgraphvertextoyield* graphyieldrequest) const
		{
		double value = 0;
		try {
			const double actual_value = this->getinventorycoef(ylds, target_yield, graphyieldrequest);
			if (action.partial(target_yield))
			{
				for (const FMTdevelopmentpath& path : topaths)
				{
					const double dif_value = (actual_value - path.getDevelopment().getinventorycoef(ylds, target_yield, graphyieldrequest));
					value += (dif_value * (path.getProportion() / 100));
				}
			}
			else {
				value = actual_value;
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("for " + std::string(*this)+" for action "+action.getname(), "FMTdevelopment::getharvestcoef", __LINE__, __FILE__);
		}
		return value;
		}

	std::unique_ptr<FMTdevelopment> FMTdevelopment::Clone() const
		{
		return std::unique_ptr<FMTdevelopment>(new FMTdevelopment(*this));
		}
    
    
}

BOOST_CLASS_EXPORT_IMPLEMENT(Core::FMTdevelopment)