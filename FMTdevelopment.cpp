
/*
MIT License

Copyright (c) [2019] [Bureau du forestier en chef]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "FMTdevelopment.h"
#include "FMTfuturdevelopment.h"

namespace Core{

	

  FMTdevelopment::FMTdevelopment():FMTobject(), mask(),age(),lock(),period(0){}

  FMTdevelopment::FMTdevelopment(FMTmask mask,int age,int lock) : FMTobject(),mask(mask),age(age),lock(lock),period(0)
        {

        }
  FMTdevelopment::FMTdevelopment(FMTmask lmask,int lage,int llock, int lperiod): FMTobject(),mask(lmask),age(lage),lock(llock),period(lperiod)
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
        //int maxage = lifespan[mask];
        //int maxage = lifespan.find(mask)->second;

        //if (newdev.age < maxage)
           // {
            ++newdev.age;
           // }
        if(newdev.lock>0)//yeah well wtf...  && newdev.period!= 1
            {
            --newdev.lock;
            }
        ++newdev.period;
        return newdev;
        }

	bool FMTdevelopment::worthtestingoperability(const FMTaction& action) const
		{
		return (((action.lock && lock == 0) || !action.lock) &&
			action.getagelowerbound() <= age && age <= action.getageupperbound() &&
			action.getperiodlowerbound() <= period && period <= action.getperiodupperbound());
		}

     bool FMTdevelopment::operable(const FMTaction& action,const FMTyields& ylds) const
        {
        if (worthtestingoperability(action))
            {
			const vector<const FMTspec*>it = action.findsets(mask);
            if (!it.empty())
                {
				for (const FMTspec* spec : it)
					{
					if (is(*spec, ylds))
						{
						return true;
						}
					}
                }
            }
        return false;
        }
	 bool FMTdevelopment::anyoperable(const vector<const FMTaction*>& actions, const FMTyields& ylds) const
		{
		 for (const FMTaction* action : actions)
			{
			 if (this->operable(*action,ylds))
				{
				return true;
				}
			}
		 return false;
		}

	 vector<int> FMTdevelopment::anyworthtestingoperability(const vector<const FMTaction*>& actions, const vector<int>& action_IDS) const
		{
		vector<int>potentials;
		int id = 0;
		for (const FMTaction* action : actions)
			{
			 if (this->worthtestingoperability(*action))
				{
				 potentials.push_back(action_IDS.at(id));
				}
			 ++id;
			}
		 return potentials;
		}

     /*vector<FMTdevelopment> FMTdevelopment::operate(const FMTaction& action,const FMTtransition& Transition,
                                    const FMTyields& ylds,const vector<FMTtheme>& themes) const
        {
        vector<FMTdevelopment>newdevs;
        FMTdevelopment newdev(*this);
        if(action.reset)
            {
            newdev.age = 0;
            }
        const FMTfork* fork = Transition.getfork(*this,ylds);
        if (fork)
            {
            vector<FMTdevelopment>devs = fork->get(newdev,ylds,themes);
            newdevs.insert(newdevs.end(), devs.begin(), devs.end());
            }else{
			_exhandler->raise(FMTexc::WSinvalid_transition_case, FMTwssect::Empty, Transition.name + " for " + string(*this), __LINE__, __FILE__);
            }
        return newdevs;
        }*/
	 vector<FMTdevelopmentpath> FMTdevelopment::operate(const FMTaction& action,
		 const FMTtransition& Transition,
		 const FMTyields& ylds,
		 const vector<FMTtheme>& themes) const
		{
		 vector<FMTdevelopmentpath>newpaths;
		/* FMTdevelopment newdev(*this);
		 if (action.reset)
		 {
			 newdev.age = 0;
		 }*/
		 const FMTfork* fork = Transition.getfork(*this, ylds);
		 if (fork)
		 {
			 newpaths = fork->getpaths(*this, ylds, themes,action.reset);
			 /*if (action.name == "ACPIL")
				 {
				 Logging::FMTlogger(Logging::FMTlogtype::FMT_Debug) << " AGE " << devs[0].age << "\n";
				 }*/
			// newpaths.insert(newpaths.end(), paths.begin(), paths.end());
		 }
		 else {
			 _exhandler->raise(FMTexc::WSinvalid_transition_case, FMTwssect::Empty, Transition.name + " for " + string(*this), __LINE__, __FILE__);
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
    FMTdevelopment::operator string() const
        {
        string line = "";
        line+=string(mask)+" ";
        line+=to_string(age)+" ";
        line+=to_string(lock)+" ";
        line+=to_string(period)+" ";
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
		if (specification.empty())
			{
			return true;
		}else{
			map<string, double> yields;
			if (!specification.emptyylds())
			{
				yields = ylds.getylds(*this, specification);
				for (const string& yield : specification.getylds())
				{
					if (yields.find(yield) == yields.end())
					{
						_exhandler->raise(FMTexc::FMTmissingyield,
							FMTwssect::Empty, yield + " for development type " + string(*this), __LINE__, __FILE__);
					}
				}
			}
			if (specification.allow(period, age, lock, yields))
				{
				return true;
				}
			
			}
		return false;
		}
	/*bool FMTdevelopment::within(const FMToutputsource& output_source, const FMTyields& ylds) const
		{
		const FMTmask& output_mask = output_source.getmask();
		if (output_mask)
			{
			if (mask.data.is_subset_of(output_mask.data) && is(output_source,ylds))
				{
				return true;
				}
			}
		return false;
		}*/
	double FMTdevelopment::getinventorycoef(const FMTyields& ylds, const string& target_yield) const
		{
		vector<string>targets(1, target_yield);
		return ylds.get(*this, targets)[target_yield];
		}
	double FMTdevelopment::getharvestcoef(const vector<FMTdevelopmentpath>& topaths,
			const FMTaction& action,const FMTyields& ylds,const string& target_yield) const
		{
		double value = 0;
		//vector<string>targets(1, target_yield);
		double actual_value = this->getinventorycoef(ylds,target_yield);//= ylds.get(*this, targets)[target_yield];
		if (action.partial(target_yield))
			{
			for (const FMTdevelopmentpath& path : topaths)
				{
				double dif_value = (actual_value - path.development->getinventorycoef(ylds,target_yield));
				value += (dif_value * (path.proportion/ 100));
				}
			}else {
			value = actual_value;
			}
		return value;
		}

	unique_ptr<FMTdevelopment> FMTdevelopment::Clone() const
		{
		return unique_ptr<FMTdevelopment>(new FMTdevelopment(*this));
		}
    
    
}
