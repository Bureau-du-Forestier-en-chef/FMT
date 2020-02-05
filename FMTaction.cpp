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

#include "FMTaction.h"

namespace Core{


FMTaction::FMTaction():FMTlist<FMTspec>(),
                partials(std::vector<std::string>()),
				agelowerbound(), ageupperbound(), periodlowerbound(), periodupperbound(),
                name(""),
                lock(false),
                reset(false){}

    FMTaction:: FMTaction(std::string& lname):FMTlist<FMTspec>(),
                        partials(std::vector<std::string>()),
						agelowerbound(), ageupperbound(), periodlowerbound(), periodupperbound(),
                        name(lname),
                        lock(false),
                        reset(false){}
    FMTaction::FMTaction(const std::string& lname): FMTlist<FMTspec>(),
                        partials(std::vector<std::string>()),
						agelowerbound(), ageupperbound(), periodlowerbound(), periodupperbound(),
                        name(lname),
                        lock(false),
                        reset(false){}
    FMTaction::FMTaction(const std::string& lname, bool lock,bool reset): FMTlist<FMTspec>(),partials(std::vector<std::string>()),
		agelowerbound(), ageupperbound(), periodlowerbound(), periodupperbound(),
		name(lname),lock(lock),reset(reset)
        {
        partials = std::vector<std::string>();
        }
    bool FMTaction::push_partials(std::string yield)
        {
        partials.push_back(yield);
        return true;
        }
	FMTaction::FMTaction(const FMTaction& rhs) : FMTlist<FMTspec>(rhs),
		partials(rhs.partials),
		agelowerbound(rhs.agelowerbound), ageupperbound(rhs.ageupperbound), periodlowerbound(rhs.periodlowerbound), periodupperbound(rhs.periodupperbound),
                        name(rhs.name),
                        lock(rhs.lock),
                        reset(rhs.reset)
        {

        }
    FMTaction& FMTaction::operator = (const FMTaction& rhs)
        {
        if (this!=&rhs)
            {
			FMTlist<FMTspec>::operator = (rhs);
            name = rhs.name;
            lock = rhs.lock;
            reset = rhs.reset;
            partials = rhs.partials;
			agelowerbound = rhs.agelowerbound;
			ageupperbound = rhs.ageupperbound;
			periodlowerbound = rhs.periodlowerbound;
			periodupperbound = rhs.periodupperbound;
            }
        return *this;
        }

	void FMTaction::setbounds()
		{
		ageupperbound = 0;
		agelowerbound = std::numeric_limits<int>::max();
		periodupperbound = 0;
		periodlowerbound = std::numeric_limits<int>::max();
		std::vector<FMTspec>::const_iterator datait = this->databegin();
		for (size_t id = 0; id < this->size(); ++id)
			{
				if (!datait->emptyage())
					{
					int upperbound = datait->getageupperbound();
					if (upperbound > ageupperbound)
						{
						ageupperbound = upperbound;
						}
					int lowerbound = datait->getagelowerbound();
					if (lowerbound < agelowerbound)
						{
						agelowerbound = lowerbound;
						}
					}else{
					ageupperbound = std::numeric_limits<int>::max();
					agelowerbound = 0;
					}
				if (!datait->emptyperiod())
					{
					int upperbound = datait->getperiodupperbound();
					if (upperbound > periodupperbound)
						{
						periodupperbound = upperbound;
						}
					int lowerbound = datait->getperiodlowerbound();
					if (lowerbound < periodlowerbound)
						{
						periodlowerbound = lowerbound;
						}
				}else {
					periodupperbound = std::numeric_limits<int>::max();
					periodlowerbound = 0;
					}
				++datait;
			}
		}


	int FMTaction::getagelowerbound() const
		{
		return agelowerbound;
		}

	int FMTaction::getageupperbound() const
		{
		return ageupperbound;
		}
	int FMTaction::getperiodlowerbound() const
		{
		return periodlowerbound;
		}
	int FMTaction::getperiodupperbound() const
		{
		return periodupperbound;
		}

	std::vector<std::string>FMTaction::getpartials() const
        {
        return partials;
        }

	bool FMTaction::inperiod() const
		{
		std::vector<FMTspec>::const_iterator datait = this->databegin();
		for (size_t id = 0; id < this->size(); ++id)
			{
			if (!datait->emptyage())
				{
				if (datait->getageupperbound() != 0)
					{
					return false;
					}
			}else {
				return false;
				}
			++datait;
			}
		return true;
		}

    FMTaction::operator std::string() const
        {
		const std::string resetyield = (reset) ? "Y" : "N";
		const std::string locked = (lock) ? "" : " _LOCKEXEMPT";
		std::string line="*ACTION "+name +" "+resetyield+locked+"\n";
        line+="*OPERABLE "+name+"\n";
		std::vector<FMTspec>::const_iterator datait = this->databegin();
		std::vector<FMTmask>::const_iterator maskit = this->maskbegin();
        for (size_t id = 0 ; id < this->size(); ++id)
            {
            line+= std::string(*maskit)+" ";
            line+= std::string(*datait);
            line+="\n";
			++datait;
			++maskit;
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
bool FMTaction::operator < (const FMTaction& rhs) const
    {
    return name < rhs.name;
    }
bool FMTaction::operator == (const FMTaction& rhs) const
    {
	return (name == rhs.name &&
		partials == rhs.partials &&
		lock == rhs.lock &&
		reset == rhs.reset &&
		FMTlist<FMTspec>::operator == (rhs));

    }
bool FMTaction::operator != (const FMTaction& rhs) const
    {
    return !(*this== rhs);
    }

bool FMTaction::partial(const std::string& yield) const
	{
	return (std::find(partials.begin(), partials.end(),yield)!=partials.end());
	}

FMTactioncomparator::FMTactioncomparator(std::string name) : action_name(name) {}

bool FMTactioncomparator::operator()(const FMTaction& action) const
	{
	return action.name == action_name;
	}
}
