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

#include "FMTbounds.h"

namespace Core{

FMTyldbounds::FMTyldbounds():FMTbounds<double>(),yield(){}


	FMTyldbounds::FMTyldbounds(const FMTwssect lsection, const std::string& lyield, const double& lupper, const double& llower) : FMTbounds<double>(lsection, lupper, llower), yield(lyield)
		{

		}

    FMTyldbounds::FMTyldbounds(const FMTwssect lsection,const FMTwskwor key,const std::string& lyield, const double& lupper, const double& llower):FMTbounds<double>(lsection,key,lupper,llower),yield(lyield)
        {

        }

        FMTyldbounds::FMTyldbounds(const FMTyldbounds& rhs) : FMTbounds(rhs) , yield(rhs.yield)
            {

            }

        FMTyldbounds::FMTyldbounds(const std::string& lyield,const FMTbounds<double>& rhs) : FMTbounds(rhs) , yield(lyield)
            {

            }
        FMTyldbounds& FMTyldbounds::operator = (const FMTyldbounds& rhs)
            {
            if (this!=&rhs)
                {
                FMTbounds::operator=(rhs);
                yield = rhs.yield;
                }
            return *this;
            }

		bool FMTyldbounds::operator == (const FMTyldbounds& rhs) const
			{
			return (FMTbounds::operator==(rhs) &&
					yield == rhs.yield);
			}
        FMTyldbounds::operator std::string() const
            {
            return FMTbounds::tostring(yield);
            }



    FMTagebounds::FMTagebounds():FMTbounds<int>(){}
    FMTagebounds::FMTagebounds(const FMTwssect lsection,const int& lupper, const int& llower):FMTbounds<int>(lsection,lupper,llower)
        {

        }
   FMTagebounds::FMTagebounds(const FMTwssect lsection, const FMTwskwor key, const int& lupper, const int& llower):FMTbounds<int>(lsection,key,lupper,llower)
        {

        }
    FMTagebounds::FMTagebounds(const FMTagebounds& rhs) : FMTbounds(rhs)
        {

        }
    FMTagebounds::FMTagebounds(const FMTbounds<int>& rhs) : FMTbounds(rhs)
        {

        }
    FMTagebounds& FMTagebounds::operator = (const FMTagebounds& rhs)
        {
        if (this!=&rhs)
            {
            FMTbounds::operator=(rhs);
            }
        return *this;
        }
    FMTagebounds::operator std::string() const
        {
        return FMTbounds::tostring("_AGE");
        }
	bool  FMTagebounds::operator == (const FMTagebounds& rhs) const
		{
		return (FMTbounds::operator==(rhs));
		}

    FMTperbounds::FMTperbounds():FMTbounds<int>(){}
    FMTperbounds::FMTperbounds(const FMTwssect lsection, const int& lupper, const int& llower):FMTbounds<int>(lsection,lupper,llower)
        {

        }
 
    FMTperbounds::FMTperbounds(const FMTperbounds& rhs) : FMTbounds(rhs)
        {

        }
	FMTperbounds::FMTperbounds(const FMTbounds<int>& rhs) : FMTbounds(rhs)
		{

		}
    FMTperbounds& FMTperbounds::operator = (const FMTperbounds& rhs)
        {
        if (this!=&rhs)
            {
            FMTbounds::operator=(rhs);
            }
        return *this;
        }
    FMTperbounds::operator std::string() const
        {
        return FMTbounds::tostring("_CP");
        }
	bool FMTperbounds::operator == (const FMTperbounds& rhs) const
		{
		return (FMTbounds::operator==(rhs));
		}

FMTlockbounds::FMTlockbounds():FMTbounds<int>(){}
FMTlockbounds::FMTlockbounds(const FMTwssect lsection,const FMTwskwor key,const int& lupper, const int& llower):FMTbounds<int>(lsection,key,lupper,llower)
    {

    }
FMTlockbounds::FMTlockbounds(const FMTlockbounds& rhs):FMTbounds<int>(rhs)
    {

    }
FMTlockbounds& FMTlockbounds::operator = (const FMTlockbounds& rhs)
    {
     if (this!=&rhs)
        {
        FMTbounds::operator=(rhs);
        }
    return *this;
    }
FMTlockbounds::operator std::string() const
    {
    return FMTbounds::tostring("_LOCK");
    }

bool FMTlockbounds::operator == (const FMTlockbounds& rhs) const
	{
	return FMTbounds<int>::operator==(rhs);
	}


FMTspec::FMTspec():per(),age(),lock(),ylds(){}
    FMTspec::FMTspec(const FMTspec& rhs):per(rhs.per),age(rhs.age),
                                        lock(rhs.lock),ylds(rhs.ylds)
        {
        }
    FMTspec& FMTspec::operator = (const FMTspec& rhs)
        {
        if (this!=&rhs)
            {
            per = rhs.per;
            age = rhs.age;
            ylds = rhs.ylds;
            lock = rhs.lock;
            }
        return *this;
        }
    bool FMTspec::add(const FMTspec& rhs)
        {
        if (!rhs.per.empty())
            {
            per = rhs.per;
            }
        if (!rhs.age.empty())
            {
            age = rhs.age;
            }
        for(std::map<std::string,FMTyldbounds>::const_iterator it = rhs.ylds.begin(); it != rhs.ylds.end() ; it++)
            {
            ylds[it->first] = it->second;
            }
        return true;
        }
    bool FMTspec::setbounds(const FMTperbounds& bound)
        {
        return per.add(bound);
        }
    bool FMTspec::addbounds(const FMTagebounds& bound)
        {
        return age.add(bound);
        }
    bool FMTspec::addbounds(const FMTyldbounds& bound)
        {
        if (ylds.find(bound.yield)!=ylds.end())
            {
            ylds[bound.yield].add(bound);
            }else{
            ylds[bound.yield] = bound;
            }
        return true;
        }
    bool FMTspec::addbounds(const FMTlockbounds& bound)
        {
        return lock.add(bound);
        }
    FMTspec::operator std::string() const
        {
		std::string line;
        bool andstuff = false;
        if (!per.empty())
            {
            line+=std::string(per)+" ";
            andstuff = true;
            }
        if (!age.empty())
            {
            if(andstuff)
                {
                line+="AND ";
                }
            line+=std::string(age)+" ";
            andstuff = true;
            }
        if (!ylds.empty())
            {
            if(andstuff)
                {
                line+="AND ";
                }
            size_t nyld = 1;
            for(std::map<std::string,FMTyldbounds>::const_iterator it = ylds.begin(); it != ylds.end() ; it++)
                {
                if (nyld!=ylds.size())
                    {
                    line+=std::string(it->second)+ " AND ";
                    }else{
                    line+=std::string(it->second)+ " ";
                    }
                ++nyld;
                }
            }
        if (!lock.empty())
            {
            line+=std::string(lock);
            }
        return line;
        }


	bool FMTspec::operator == (const FMTspec& rhs) const
		{
		return (per == rhs.per &&
			age == rhs.age &&
			lock == rhs.lock &&
			ylds == rhs.ylds);
		}

	bool FMTspec::operator < (const FMTspec& rhs) const
		{
		//strict ordering
		if (per < rhs.per)
			return true;
		if (rhs.per < per)
			return false;
		if (age < rhs.age)
			return true;
		if (rhs.age < age)
			return false;
		if (lock < rhs.lock)
			return true;
		if (rhs.lock < lock)
			return false;
		if (ylds < rhs.ylds)
			return true;
		if (rhs.ylds < ylds)
			return false;
		return false;
		}

    std::vector<std::string>FMTspec::getylds() const
        {
		std::vector<std::string>values;
        if(!ylds.empty())
            {
            values.reserve(ylds.size());
            for(std::map<std::string,FMTyldbounds>::const_iterator it = ylds.begin();it!= ylds.end();++it)
                {
                values.push_back(it->first);
                }
            }
        return values;
        }
	std::map<std::string, FMTyldbounds>FMTspec::getyldsbounds() const
		{
		return ylds;
		}


    bool FMTspec::empty() const
        {
        return (per.empty() && age.empty() && ylds.empty());
        }

	size_t FMTspec::hash() const
		{
		std::size_t seed = 0;
		boost::hash_combine(seed, std::hash<int>()(per.getlower()));
		boost::hash_combine(seed, std::hash<int>()(per.getupper()));
		boost::hash_combine(seed, std::hash<int>()(age.getlower()));
		boost::hash_combine(seed, std::hash<int>()(age.getupper()));
		boost::hash_combine(seed, std::hash<int>()(lock.getlower()));
		boost::hash_combine(seed, std::hash<int>()(lock.getupper()));
		if (!ylds.empty())
			{
			for (std::map<std::string, FMTyldbounds>::const_iterator it = ylds.begin(); it != ylds.end(); ++it)
				{
				boost::hash_combine(seed, std::hash<double>()(it->second.getlower()));
				boost::hash_combine(seed, std::hash<double>()(it->second.getupper()));
				}
			}
		return seed;
		}

	bool FMTspec::emptylock() const
		{
		return lock.empty();
		}

	bool FMTspec::emptyage() const
		{
		return age.empty();
		}
	bool FMTspec::emptyylds() const
		{
		return ylds.empty();
		}

	bool FMTspec::emptyperiod() const
		{
		return per.empty();
		}
	int FMTspec::getperiodupperbound() const
	{
		return per.upper;
	}
	int FMTspec::getperiodlowerbound() const
	{
		return per.lower;
	}


	int FMTspec::getageupperbound() const
		{
		return age.upper;
		}
	int FMTspec::getagelowerbound() const
		{
		return age.lower;
		}

	bool FMTspec::issubsetof(const FMTspec& rhs) const
		{
		bool persubset = true;
		if (!per.empty() && !rhs.per.empty())
			{
			persubset = (per.getlower() >= rhs.per.getlower()) && (per.getupper() <= rhs.per.getupper());
		}else if (!rhs.per.empty() && per.empty())
			{
			persubset = false;
			}
		bool agesubset = true;
		if (!age.empty() && !rhs.age.empty())
			{
			agesubset = (age.getlower() >= rhs.age.getlower()) && (age.getupper() <= rhs.age.getupper());
			}
			else if (!rhs.age.empty() && age.empty())
			{
			agesubset = false;
			}
		bool yldssubset = true;
		if (!ylds.empty() && !rhs.ylds.empty())
		{
		for (std::map<std::string, FMTyldbounds>::const_iterator it = ylds.begin(); it != ylds.end(); ++it)
			{
			if (rhs.ylds.find(it->first)== rhs.ylds.end())
				{
				yldssubset = false;
				break;
			}else {
				yldssubset = (it->second.getlower() >= it->second.getlower()) && (it->second.getupper() <= it->second.getupper());
				}

			}
		}else if (!rhs.ylds.empty() && ylds.empty())
			{
			yldssubset = false;
			}
		return persubset && agesubset && yldssubset;
		}

}
