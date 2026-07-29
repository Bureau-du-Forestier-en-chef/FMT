/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTDevelopment.h"
#include "FMTFuturDevelopment.h"
#include "FMTYieldRequest.h"
#include "FMTGraphVertexToYield.h"
#include "FMTLifespans.h"
#include "FMTTransition.h"
#include "FMTAction.h"
#include "FMTTransitionMask.h"
#include "FMTYields.h"
#include "FMTDevelopmentPath.h"
#include "FMTBounds.hpp"
#include "FMTExceptionHandler.h"
#include <array>

namespace Core{

	FMTDevelopment::FMTDevelopment(FMTDevelopment&& rhs) :
		FMTObject(std::move(rhs)),
		m_mask(std::move(rhs.m_mask)),
		m_age(std::move(rhs.m_age)),
		m_lock(std::move(rhs.m_lock)),
		m_period(std::move(rhs.m_period))
	{

	}

	FMTDevelopment& FMTDevelopment::operator=(FMTDevelopment&& rhs)
	{
		FMTObject::operator=(std::move(rhs));
		m_mask = std::move(rhs.m_mask);
		m_age = std::move(rhs.m_age);
		m_lock = std::move(rhs.m_lock);
		m_period = std::move(rhs.m_period);
		return *this;
	}

	void FMTDevelopment::setAge(const int& lage)
	{
		try {
			m_age = static_cast<uint8_t>(lage);
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("for " + std::string(*this), "FMTDevelopment::setAge", __LINE__, __FILE__);
		}
	}

	void FMTDevelopment::setLock(const int& llock)
	{
		try{
		m_lock = static_cast<uint8_t>(llock);
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("for " + std::string(*this), "FMTDevelopment::setLock", __LINE__, __FILE__);
		}
	}

	void FMTDevelopment::setPeriod(const int& lperiod)
	{
		try{
		m_period = static_cast<uint8_t>(lperiod);
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("for " + std::string(*this), "FMTDevelopment::setPeriod", __LINE__, __FILE__);
		}
	}

	void FMTDevelopment::setMask(const Core::FMTMask& lmask)
	{
		m_mask = lmask;
	}

  FMTDevelopment::FMTDevelopment():FMTObject(), m_mask(),m_age(),m_lock(),m_period(0)
  {


  }



  FMTDevelopment::FMTDevelopment(const FMTMask& lmask,const int& lage,const int& llock) : FMTObject(),
	  m_mask(lmask),
	  m_age(static_cast<uint8_t>(lage)),
	  m_lock(static_cast<uint8_t>(llock)),
	  m_period(0)
        {

        }
  FMTDevelopment::FMTDevelopment(const FMTMask&  lmask,const int& lage,const int& llock,const int& lperiod): 
	  FMTObject(),
	  m_mask(lmask),
	  m_age(static_cast<uint8_t>(lage)),
	  m_lock(static_cast<uint8_t>(llock)),
	  m_period(static_cast<uint8_t>(lperiod))
        {

        }
  
    FMTDevelopment::FMTDevelopment(const FMTDevelopment& rhs) :
		FMTObject(rhs),
        m_mask(rhs.m_mask),
        m_age(rhs.m_age),
        m_lock(rhs.m_lock),
        m_period(rhs.m_period)
        {


        }

     FMTDevelopment& FMTDevelopment::operator = (const FMTDevelopment& rhs)
        {
        if (this!=&rhs)
            {
			FMTObject::operator=(rhs);
            m_mask = rhs.m_mask;
            m_age = rhs.m_age;
            m_lock = rhs.m_lock;
            m_period = rhs.m_period;
            }
        return *this;
        }
    FMTFuturDevelopment FMTDevelopment::grow() const
        {
        FMTFuturDevelopment newDev(*this);
		try {
			++newDev.m_age;
			if (newDev.m_lock > 0)
			{
				--newDev.m_lock;
			}
			++newDev.m_period;
		}catch (...)
			{
			_exhandler->raiseFromCatch("for " + std::string(*this), "FMTDevelopment::grow", __LINE__, __FILE__);
			}
        return newDev;
        }

	bool FMTDevelopment::worthTestingOperability(const FMTAction& action) const
		{
		return (((action.doRespectLock() && m_lock == 0) || !action.doRespectLock()) &&
			action.getAgeLowerBound() <= getAge() && getAge() <= action.getAgeUpperBound() &&
			action.getPeriodLowerBound() <= getPeriod() && getPeriod() <= action.getPeriodUpperBound());
		}

	FMTYieldRequest FMTDevelopment::getYieldRequest(const Graph::FMTGraphVertexToYield* graphyieldrequest) const
		{
			if (graphyieldrequest != nullptr)
			{
				return FMTYieldRequest(*this,*graphyieldrequest);
				
			}
			return FMTYieldRequest(*this);
		}

	std::string FMTDevelopment::getNonOperabilityInfo(const FMTAction& p_action,
		const Core::FMTYields& p_yields, const Graph::FMTGraphVertexToYield* p_yieldRequest) const
	{
		std::string Info;
		if (!operable(p_action, p_yields))
			{
			Info += "For action " + p_action.getName()+"\n";
			if ((p_action.doRespectLock() && m_lock != 0))
				{
				Info += "Lock of " + std::to_string(getLock()) + " so it is not operable!" + "\n";
				}
			if (p_action.getAgeLowerBound() > getAge())
				{
				Info += "Age of " + std::to_string(getAge()) + " vs Lower bound of "+ std::to_string(p_action.getAgeLowerBound()) + "\n";
				}
			if (p_action.getAgeUpperBound() < getAge())
				{
				Info += "Age of " + std::to_string(getAge()) + " vs Upper bound of " + std::to_string(p_action.getAgeUpperBound()) + "\n";
				}
			if (p_action.getPeriodLowerBound() > getPeriod())
				{
				Info += "Period of " + std::to_string(getPeriod()) + " vs Lower bound of " + std::to_string(p_action.getPeriodLowerBound()) + "\n";
				}
			if (p_action.getPeriodUpperBound() < getPeriod())
				{
				Info += "Period of " + std::to_string(getPeriod()) + " vs Upper bound of " + std::to_string(p_action.getPeriodUpperBound()) + "\n";
				}
			std::string yldsStr;
			for (const auto& OPERABILITY : p_action.findSets(getMask()))
			{
				for (const std::string& YLD_NAME : OPERABILITY->second.getYlds())
				{
					if (yldsStr.find(YLD_NAME) == std::string::npos)
					{
						yldsStr += YLD_NAME + ":" + std::to_string(p_yields.get(getYieldRequest(), YLD_NAME)) + "\n";
					}
				}
			}
			if (!yldsStr.empty())
				{
				yldsStr.pop_back();
				yldsStr.insert(0, "\n");
				Info +=  "With Yields values of " + yldsStr;
				}
			}
		return Info;
	}



     bool FMTDevelopment::operable(const FMTAction& action,const FMTYields& ylds, const Graph::FMTGraphVertexToYield* graphyieldrequest) const
        {
		 try{
			if (worthTestingOperability(action))
				{
				for (const FMTAction::const_iterator spec: action.findSets(m_mask))
					{
					if (is(spec->second, ylds,graphyieldrequest))
						{
						return true;
						}
					}
				}
		 }catch (...)
			{
			 _exhandler->raiseFromCatch("for " + std::string(*this)+" for action "+action.getName(), "FMTDevelopment::operable", __LINE__, __FILE__);
			}
        return false;
        }
	 bool FMTDevelopment::anyOperable(const std::vector<const FMTAction*>& actions, const FMTYields& ylds,
		 const Graph::FMTGraphVertexToYield* graphyieldrequest) const
		{
		 try{
		 for (const FMTAction* action : actions)
			{
			 if (this->operable(*action,ylds,graphyieldrequest))
				{
				return true;
				}
			}
		 }
		 catch (...)
		 {
			 _exhandler->raiseFromCatch("for " + std::string(*this), "FMTDevelopment::anyOperable", __LINE__, __FILE__);
		 }
		 return false;
		}

	 std::vector<int> FMTDevelopment::anyWorthTestingOperability(const std::vector<const FMTAction*>& actions, const FMTAction& firstaction) const
		{
		 std::vector<int>potentials;
		 try{
			for (const FMTAction* action : actions)
				{
				 if (this->worthTestingOperability(*action))
					{
					 const int location = static_cast<int>(std::distance(&firstaction, action));
					 potentials.push_back(location);
					}
				}
		 }
		 catch (...)
		 {
			 _exhandler->raiseFromCatch("for " + std::string(*this), "FMTDevelopment::anyWorthTestingOperability", __LINE__, __FILE__);
		 }
		 return potentials;
		}

	 std::vector<bool> FMTDevelopment::isAnyWorthTestingOperability(const std::vector<const FMTAction*>& actions,
		 const std::vector<FMTAction>& allactions) const noexcept
	 {
		 try {
			 const FMTAction* firstaction = &(*allactions.begin());
			 std::vector<int8_t>worth;
			 worth.reserve(actions.size());
			 for (const FMTAction* action : actions)
			 {
				 if (this->worthTestingOperability(*action))
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
			 _exhandler->raiseFromCatch("for " + std::string(*this), "FMTDevelopment::isAnyWorthTestingOperability", __LINE__, __FILE__);
		 }
		 return std::vector<bool>();
	 }

	 std::vector<FMTDevelopmentPath> FMTDevelopment::operate(const FMTAction& action,
		 const FMTTransition& Transition,
		 const FMTYields& ylds,
		 const std::vector<FMTTheme>& themes) const
		{
		 std::vector<FMTDevelopmentPath>newpaths;
		 try {
			 const FMTFork* fork = Transition.getFork(*this,ylds);
			 if (fork)
			 {
				 newpaths = fork->getPaths(*this, ylds, themes, action.isResetAge());
		
			 }
			 else {
				 std::string yldsStr;
				 for (const FMTTransition::const_iterator fork : Transition.findSets(getMask()))
				 {
					 for (const std::string& YLD_NAME : fork->second.getYlds())
					 {
						 if (yldsStr.find(YLD_NAME)==std::string::npos)
						 {
							 yldsStr += YLD_NAME + ":" + std::to_string(ylds.get(getYieldRequest(), YLD_NAME)) + "\n";
						 }
					 }
				 }
				 if (!yldsStr.empty())
				 {
					 yldsStr.pop_back();
					 yldsStr.insert(0, "\n");
				 }
				 _exhandler->raise(Exception::FMTexc::FMTinvalid_transition_case,Transition.getName() + " for " + std::string(*this) + yldsStr,
					 "FMTDevelopment::operate",__LINE__, __FILE__);
			 }
		 }catch (...)
			{
			 _exhandler->raiseFromCatch("for " + std::string(*this)+" for action "+action.getName(),"FMTDevelopment::operate", __LINE__, __FILE__);
			}
		 return newpaths;
		}


    bool FMTDevelopment::operator == (const FMTDevelopment& rhs) const
        {
        return (m_age == rhs.m_age && m_lock == rhs.m_lock && m_period == rhs.m_period && m_mask == rhs.m_mask);
        }
    bool FMTDevelopment::operator != (const FMTDevelopment& rhs) const
        {
        return !(*this==rhs);
        }
     bool FMTDevelopment::operator < (const FMTDevelopment& rhs) const
        {
		 //strict ordering
		 if (m_mask < rhs.m_mask)
			 return true;
		 if (rhs.m_mask < m_mask)
			 return false;
		 if (m_age < rhs.m_age)
			 return true;
		 if (rhs.m_age < m_age)
			 return false;
		 if (m_lock < rhs.m_lock)
			 return true;
		 if (rhs.m_lock < m_lock)
			 return false;
		 if (m_period < rhs.m_period)
			 return true;
		 if (rhs.m_period < m_period)
			 return false;
		 return false;
        }
    FMTDevelopment::operator std::string() const
        {
		std::string line = "";
        line+=std::string(m_mask)+" ";
        line+=std::to_string(getAge())+" ";
        line+=std::to_string(getLock())+" ";
        line+=std::to_string(getPeriod())+" ";
        return line;
        }
	double FMTDevelopment::getArea() const
		{
		return 0;
		}

	FMTDevelopment FMTDevelopment::clearLock() const
		{
		FMTDevelopment nolock(*this);
		nolock.m_lock = 0;
		return nolock;
		}

	bool FMTDevelopment::_isOutOfBound(const FMTYieldRequest& p_request,
									const FMTYields& p_yields,
									const Core::FMTYldBounds& p_bound,
									const std::string& p_yield) const
		{
		bool value = false;
		try {
			const double YIELD_VALUE = p_yields.get(p_request, p_yield);
			value = (p_bound.out(YIELD_VALUE));
		}catch (...)
			{
			_exhandler->raiseFromCatch(
				"for " + std::string(*this) + " on yield "+p_yield+" on bound "+ std::string(p_bound),
				"FMTDevelopment::_isOutOfBound", __LINE__, __FILE__);
			}
		return value;
		}

	bool FMTDevelopment::is(const FMTSpec& specification, const FMTYields& ylds,
		const Graph::FMTGraphVertexToYield* graphyieldrequest) const
		{
		bool allow = false;
		try {
			allow = specification.allowWithoutYield(getPeriod(), getAge(), getLock());
			if (allow && !specification.emptyYlds())
				{
				const FMTYieldRequest REQUEST = getYieldRequest(graphyieldrequest);
				const std::vector<Core::FMTYldBounds>& BOUNDS= specification.getYldBounds();
				const std::vector<std::string>& YLD_NAMES = specification.getYlds();
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
			_exhandler->raiseFromCatch(
				"for " + std::string(*this)+" with spec "+ specStr,
				"FMTDevelopment::is",__LINE__, __FILE__);
			}
		return allow;
		}
	double FMTDevelopment::getInventoryCoef(const FMTYields& ylds, const std::string& target_yield,
		const Graph::FMTGraphVertexToYield* graphyieldrequest) const
	{
		try {
			const FMTYieldRequest request = getYieldRequest(graphyieldrequest);
			return ylds.get(request,target_yield);
		}catch (...)
		{
			_exhandler->raiseFromCatch("for " + std::string(*this) , "FMTDevelopment::getInventoryCoef", __LINE__, __FILE__);
		}
		return 0;
		}
	double FMTDevelopment::getHarvestCoef(const std::vector<FMTDevelopmentPath>& topaths,
			const FMTAction& action,const FMTYields& ylds,const std::string& target_yield,
		const Graph::FMTGraphVertexToYield* graphyieldrequest) const
		{
		double value = 0;
		try {
			const double actual_value = this->getInventoryCoef(ylds, target_yield, graphyieldrequest);
			if (action.partial(target_yield))
			{
				for (const FMTDevelopmentPath& path : topaths)
				{
					const double dif_value = (actual_value - path.getDevelopment().getInventoryCoef(ylds, target_yield, graphyieldrequest));
					value += (dif_value * (path.getProportion() / 100));
				}
			}
			else {
				value = actual_value;
			}
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("for " + std::string(*this)+" for action "+action.getName(), "FMTDevelopment::getHarvestCoef", __LINE__, __FILE__);
		}
		return value;
		}

	std::unique_ptr<FMTDevelopment> FMTDevelopment::clone() const
		{
		return std::unique_ptr<FMTDevelopment>(new FMTDevelopment(*this));
		}
    
    
}

BOOST_CLASS_EXPORT_IMPLEMENT(Core::FMTDevelopment)