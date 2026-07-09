/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTbounds.hpp"
#include <algorithm>

namespace Core{

	FMTyldbounds::FMTyldbounds():FMTbounds<double>(),yield(){}


	FMTyldbounds::FMTyldbounds(const FMTsection lsection, const std::string& lyield, const double& lupper, const double& llower) : FMTbounds<double>(lsection, lupper, llower), yield(lyield)
		{

		}

    FMTyldbounds::FMTyldbounds(const FMTsection lsection,const FMTkwor key,const std::string& lyield, const double& lupper, const double& llower):FMTbounds<double>(lsection,key,lupper,llower),yield(lyield)
        {

        }

        FMTyldbounds::FMTyldbounds(const FMTyldbounds& rhs) : FMTbounds<double>(rhs), yield(rhs.yield)
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
            return FMTbounds::toString(yield);
            }



    FMTagebounds::FMTagebounds():FMTbounds<int>(){}
    FMTagebounds::FMTagebounds(const FMTsection lsection,const int& lupper, const int& llower):FMTbounds<int>(lsection,lupper,llower)
        {

        }
   FMTagebounds::FMTagebounds(const FMTsection lsection, const FMTkwor key, const int& lupper, const int& llower):FMTbounds<int>(lsection,key,lupper,llower)
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
        return FMTbounds::toString("_AGE");
        }
	bool  FMTagebounds::operator == (const FMTagebounds& rhs) const
		{
		return (FMTbounds::operator==(rhs));
		}

    FMTperbounds::FMTperbounds():FMTbounds<int>(){}
    FMTperbounds::FMTperbounds(const FMTsection lsection, const int& lupper, const int& llower):FMTbounds<int>(lsection,lupper,llower)
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
        return FMTbounds::toString("_CP");
        }
	bool FMTperbounds::operator == (const FMTperbounds& rhs) const
		{
		return (FMTbounds::operator==(rhs));
		}

FMTlockbounds::FMTlockbounds():FMTbounds<int>(){}

FMTlockbounds::FMTlockbounds(const FMTsection lsection, const int& lupper, const int& llower) : FMTbounds<int>(lsection,lupper, llower)
{

}

FMTlockbounds::FMTlockbounds(const FMTsection lsection,const FMTkwor key,const int& lupper, const int& llower):FMTbounds<int>(lsection,key,lupper,llower)
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
    return FMTbounds::toString("_LOCK");
    }

bool FMTlockbounds::operator == (const FMTlockbounds& rhs) const
	{
	return FMTbounds<int>::operator==(rhs);
	}


FMTspec::FMTspec():per(),age(),lock(), yieldnames(), yieldbounds(){}
    FMTspec::FMTspec(const FMTspec& rhs):per(rhs.per),age(rhs.age),
                                        lock(rhs.lock),
									yieldnames(rhs.yieldnames), yieldbounds(rhs.yieldbounds)
        {

        }
    FMTspec& FMTspec::operator = (const FMTspec& rhs)
        {
        if (this!=&rhs)
            {
            per = rhs.per;
            age = rhs.age;
			yieldnames = rhs.yieldnames;
			yieldbounds = rhs.yieldbounds;
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
		for (size_t ylid = 0; ylid < rhs.yieldnames.size(); ++ylid)
			{
			std::vector<std::string>::const_iterator nameit = std::find(yieldnames.begin(), yieldnames.end(), rhs.yieldnames.at(ylid));
			if (nameit != yieldnames.end())
			{
				yieldbounds[std::distance(yieldnames.cbegin(), nameit)] = rhs.yieldbounds.at(ylid);
			}else{
				yieldbounds.push_back(rhs.yieldbounds.at(ylid));
				yieldnames.push_back(*nameit);
				}
			}
        /*for(std::map<std::string,FMTyldbounds>::const_iterator it = rhs.ylds.begin(); it != rhs.ylds.end() ; it++)
            {
            ylds[it->first] = it->second;
            }*/
        return true;
        }
    bool FMTspec::setBounds(const FMTperbounds& bound)
        {
        return per.add(bound);
        }
    bool FMTspec::addBounds(const FMTagebounds& bound)
        {
        return age.add(bound);
        }
    bool FMTspec::addBounds(const FMTyldbounds& bound)
        {
		std::vector<std::string>::const_iterator nameit = std::find(yieldnames.begin(), yieldnames.end(), bound.yield);
		if (nameit != yieldnames.end())
		{
			yieldbounds[std::distance(yieldnames.cbegin(), nameit)].add(bound);
		}
		else {
			yieldbounds.push_back(bound);
			yieldnames.push_back(bound.yield);
		}
        /*if (ylds.find(bound.yield)!=ylds.end())
            {
            ylds[bound.yield].add(bound);
            }else{
            ylds[bound.yield] = bound;
            }*/
        return true;
        }
    bool FMTspec::addBounds(const FMTlockbounds& bound)
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
        if (!yieldnames.empty())
            {
            if(andstuff)
                {
                line+="AND ";
                }
			size_t nyld = 1;
			for (size_t ylid = 0; ylid < yieldnames.size(); ++ylid)
				{
				if (nyld != yieldbounds.size())
				{
					line += std::string(yieldbounds.at(ylid)) + " AND ";
				}
				else {
					line += std::string(yieldbounds.at(ylid)) + " ";
				}
				++nyld;
				}
            /*size_t nyld = 1;
            for(std::map<std::string,FMTyldbounds>::const_iterator it = ylds.begin(); it != ylds.end() ; it++)
                {
                if (nyld!=ylds.size())
                    {
                    line+=std::string(it->second)+ " AND ";
                    }else{
                    line+=std::string(it->second)+ " ";
                    }
                ++nyld;
                }*/
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
			yieldnames == rhs.yieldnames &&
			yieldbounds == rhs.yieldbounds);
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
		if (yieldnames < rhs.yieldnames)
			return true;
		if (rhs.yieldnames < yieldnames)
			return false;
		if (yieldbounds < rhs.yieldbounds)
			return true;
		if (rhs.yieldbounds < yieldbounds)
			return false;
		return false;
		}

	/*std::vector<std::string>FMTspec::getYlds() const
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
	std::map<std::string, FMTyldbounds>FMTspec::getYldsBounds() const
		{
		return ylds;
		}*/


    bool FMTspec::empty() const
        {
        return (per.empty() && age.empty() && yieldnames.empty() && lock.empty());
        }

	size_t FMTspec::hash() const
		{
		std::size_t seed = 0;
		boost::hash_combine(seed, std::hash<int>()(per.getLower()));
		boost::hash_combine(seed, std::hash<int>()(per.getUpper()));
		boost::hash_combine(seed, std::hash<int>()(age.getLower()));
		boost::hash_combine(seed, std::hash<int>()(age.getUpper()));
		boost::hash_combine(seed, std::hash<int>()(lock.getLower()));
		boost::hash_combine(seed, std::hash<int>()(lock.getUpper()));
		for (size_t ylid = 0; ylid < yieldnames.size(); ++ylid)
		{
			boost::hash_combine(seed, std::hash<std::string>()(yieldnames.at(ylid)));
			boost::hash_combine(seed, std::hash<double>()(yieldbounds.at(ylid).getLower()));
			boost::hash_combine(seed, std::hash<double>()(yieldbounds.at(ylid).getUpper()));
		}
		/*if (!ylds.empty())
			{
			for (std::map<std::string, FMTyldbounds>::const_iterator it = ylds.begin(); it != ylds.end(); ++it)
				{
				boost::hash_combine(seed, std::hash<double>()(it->second.getLower()));
				boost::hash_combine(seed, std::hash<double>()(it->second.getUpper()));
				}
			}*/
		return seed;
		}

	bool FMTspec::emptyLock() const
		{
		return lock.empty();
		}

	bool FMTspec::emptyAge() const
		{
		return age.empty();
		}
	bool FMTspec::emptyYlds() const
		{
		return yieldnames.empty();
		}

	bool FMTspec::emptyPeriod() const
		{
		return per.empty();
		}
	int FMTspec::getPeriodUpperBound() const
	{
		return per.upper;
	}
	int FMTspec::getPeriodLowerBound() const
	{
		return per.lower;
	}
	int FMTspec::getLockUpperBound() const
	{
		return lock.upper;
	}

	int FMTspec::getLockLowerBound() const
	{
		return lock.lower;
	}



	int FMTspec::getAgeUpperBound() const
		{
		return age.upper;
		}
	int FMTspec::getAgeLowerBound() const
		{
		return age.lower;
		}

	bool FMTspec::isSubsetOf(const FMTspec& rhs) const
		{
		bool persubset = true;
		if (!per.empty() && !rhs.per.empty())
			{
			persubset = (per.getLower() >= rhs.per.getLower()) && (per.getUpper() <= rhs.per.getUpper());
		}else if (!rhs.per.empty() && per.empty())
			{
			persubset = false;
			}
		bool agesubset = true;
		if (!age.empty() && !rhs.age.empty())
			{
			agesubset = (age.getLower() >= rhs.age.getLower()) && (age.getUpper() <= rhs.age.getUpper());
			}
			else if (!rhs.age.empty() && age.empty())
			{
			agesubset = false;
			}
		bool yldssubset = true;
		if (!yieldnames.empty() && !rhs.yieldnames.empty())
		{
			for (size_t ylid = 0; ylid < yieldnames.size(); ++ylid)
			{
				std::vector<std::string>::const_iterator it = std::find(rhs.yieldnames.begin(), rhs.yieldnames.end(), yieldnames.at(ylid));
				if ( it == rhs.yieldnames.end())
				{
					yldssubset = false;
					break;
				}
				else {
					const size_t location = std::distance(rhs.yieldnames.begin(),it);
					yldssubset = (rhs.yieldbounds.at(location).getLower() >= yieldbounds.at(ylid).getLower()) && (yieldbounds.at(ylid).getUpper() <= rhs.yieldbounds.at(location).getUpper());
				}
			}
		/*for (std::map<std::string, FMTyldbounds>::const_iterator it = ylds.begin(); it != ylds.end(); ++it)
			{
			if (rhs.ylds.find(it->first)== rhs.ylds.end())
				{
				yldssubset = false;
				break;
			}else {
				yldssubset = (it->second.getLower() >= it->second.getLower()) && (it->second.getUpper() <= it->second.getUpper());
				}

			}*/
		}else if (!rhs.yieldnames.empty() && yieldnames.empty())
			{
			yldssubset = false;
			}
		return persubset && agesubset && yldssubset;
		}

}
