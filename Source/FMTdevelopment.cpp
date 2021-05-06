/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTdevelopment.h"
#include "FMTfuturdevelopment.h"

namespace Core{

	

  FMTdevelopment::FMTdevelopment():FMTobject(), mask(),age(),lock(),period(0){}



  FMTdevelopment::FMTdevelopment(const FMTmask& mask,const int& age,const int& lock) : FMTobject(),mask(mask),age(age),lock(lock),period(0)
        {

        }
  FMTdevelopment::FMTdevelopment(const FMTmask&  lmask,const int& lage,const int& llock,const int& lperiod): FMTobject(),mask(lmask),age(lage),lock(llock),period(lperiod)
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
        ++newdev.age;
        if(newdev.lock>0)
            {
            --newdev.lock;
            }
        ++newdev.period;
        return newdev;
        }

	bool FMTdevelopment::worthtestingoperability(const FMTaction& action) const
		{
		return (((action.dorespectlock() && lock == 0) || !action.dorespectlock()) &&
			action.getagelowerbound() <= age && age <= action.getageupperbound() &&
			action.getperiodlowerbound() <= period && period <= action.getperiodupperbound());
		}

     bool FMTdevelopment::operable(const FMTaction& action,const FMTyields& ylds) const
        {
		 try{
			if (worthtestingoperability(action))
				{
				for (const FMTspec* spec : action.findsets(mask))
					{
					if (is(*spec, ylds))
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
	 bool FMTdevelopment::anyoperable(const std::vector<const FMTaction*>& actions, const FMTyields& ylds) const
		{
		 try{
		 for (const FMTaction* action : actions)
			{
			 if (this->operable(*action,ylds))
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
		 std::vector<bool>returnvalues(allactions.size(),false);
		 try {
			 bool clear = true;
			 const FMTaction* firstaction = &(*allactions.begin());
			 for (const FMTaction* action : actions)
			 {
				 if (this->worthtestingoperability(*action))
				 {
					 returnvalues[std::distance(firstaction, action)] = true;
					 clear = false;
				 }
			 }
			 if (clear)
			 {
				 returnvalues.clear();
			 }
		 }
		 catch (...)
		 {
			 _exhandler->raisefromcatch("for " + std::string(*this), "FMTdevelopment::isanyworthtestingoperability", __LINE__, __FILE__);
		 }
		 return returnvalues;
	 }

	 std::vector<FMTdevelopmentpath> FMTdevelopment::operate(const FMTaction& action,
		 const FMTtransition& Transition,
		 const FMTyields& ylds,
		 const std::vector<FMTtheme>& themes) const
		{
		 std::vector<FMTdevelopmentpath>newpaths;
		 try {
			 const FMTfork* fork = Transition.getfork(*this, ylds);
			 if (fork)
			 {
				 newpaths = fork->getpaths(*this, ylds, themes, action.isresetage());
		
			 }
			 else {
				 _exhandler->raise(Exception::FMTexc::FMTinvalid_transition_case,Transition.getname() + " for " + std::string(*this),
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
        return (mask==rhs.mask && age == rhs.age && lock == rhs.lock && period == rhs.period);
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
        line+=std::to_string(age)+" ";
        line+=std::to_string(lock)+" ";
        line+=std::to_string(period)+" ";
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

	bool FMTdevelopment::is(const FMTspec& specification, const FMTyields& ylds) const
		{
		bool allow = false;
		try {
			allow = specification.allowwithoutyield(period, age, lock);
			if (allow && !specification.emptyylds())
				{
				const std::vector<double> yields = ylds.getylds(*this, specification);
				allow = specification.allowyields(yields);
				}
		}catch (...)
			{
			_exhandler->raisefromcatch(
				"for " + std::string(*this),
				"FMTdevelopment::is",__LINE__, __FILE__);
			}
		return allow;
		}
	double FMTdevelopment::getinventorycoef(const FMTyields& ylds, const std::string& target_yield) const
	{
		try {
		return ylds.getsingle(*this, target_yield);
		}catch (...)
		{
			_exhandler->raisefromcatch("for " + std::string(*this) , "FMTdevelopment::getinventorycoef", __LINE__, __FILE__);
		}
		return 0;
		}
	double FMTdevelopment::getharvestcoef(const std::vector<FMTdevelopmentpath>& topaths,
			const FMTaction& action,const FMTyields& ylds,const std::string& target_yield) const
		{
		double value = 0;
		try {
			const double actual_value = this->getinventorycoef(ylds, target_yield);
			if (action.partial(target_yield))
			{
				for (const FMTdevelopmentpath& path : topaths)
				{
					const double dif_value = (actual_value - path.development->getinventorycoef(ylds, target_yield));
					value += (dif_value * (path.proportion / 100));
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