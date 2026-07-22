/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTBounds.hpp"
#include <algorithm>

namespace Core{

	FMTYldBounds::FMTYldBounds():FMTBounds<double>(),yield(){}


	FMTYldBounds::FMTYldBounds(const FMTsection lsection, const std::string& lyield, const double& lupper, const double& llower) : FMTBounds<double>(lsection, lupper, llower), yield(lyield)
		{

		}

    FMTYldBounds::FMTYldBounds(const FMTsection lsection,const FMTkwor key,const std::string& lyield, const double& lupper, const double& llower):FMTBounds<double>(lsection,key,lupper,llower),yield(lyield)
        {

        }

        FMTYldBounds::FMTYldBounds(const FMTYldBounds& rhs) : FMTBounds<double>(rhs), yield(rhs.yield)
            {

            }

        FMTYldBounds::FMTYldBounds(const std::string& lyield,const FMTBounds<double>& rhs) : FMTBounds(rhs) , yield(lyield)
            {

            }
        FMTYldBounds& FMTYldBounds::operator = (const FMTYldBounds& rhs)
            {
            if (this!=&rhs)
                {
                FMTBounds::operator=(rhs);
                yield = rhs.yield;
                }
            return *this;
            }

		bool FMTYldBounds::operator == (const FMTYldBounds& rhs) const
			{
			return (FMTBounds::operator==(rhs) &&
					yield == rhs.yield);
			}
        FMTYldBounds::operator std::string() const
            {
            return FMTBounds::toString(yield);
            }



    FMTAgeBounds::FMTAgeBounds():FMTBounds<int>(){}
    FMTAgeBounds::FMTAgeBounds(const FMTsection lsection,const int& lupper, const int& llower):FMTBounds<int>(lsection,lupper,llower)
        {

        }
   FMTAgeBounds::FMTAgeBounds(const FMTsection lsection, const FMTkwor key, const int& lupper, const int& llower):FMTBounds<int>(lsection,key,lupper,llower)
        {

        }
    FMTAgeBounds::FMTAgeBounds(const FMTAgeBounds& rhs) : FMTBounds(rhs)
        {

        }
    FMTAgeBounds::FMTAgeBounds(const FMTBounds<int>& rhs) : FMTBounds(rhs)
        {

        }
    FMTAgeBounds& FMTAgeBounds::operator = (const FMTAgeBounds& rhs)
        {
        if (this!=&rhs)
            {
            FMTBounds::operator=(rhs);
            }
        return *this;
        }
    FMTAgeBounds::operator std::string() const
        {
        return FMTBounds::toString("_AGE");
        }
	bool  FMTAgeBounds::operator == (const FMTAgeBounds& rhs) const
		{
		return (FMTBounds::operator==(rhs));
		}

    FMTPerBounds::FMTPerBounds():FMTBounds<int>(){}
    FMTPerBounds::FMTPerBounds(const FMTsection lsection, const int& lupper, const int& llower):FMTBounds<int>(lsection,lupper,llower)
        {

        }
 
    FMTPerBounds::FMTPerBounds(const FMTPerBounds& rhs) : FMTBounds(rhs)
        {

        }
	FMTPerBounds::FMTPerBounds(const FMTBounds<int>& rhs) : FMTBounds(rhs)
		{

		}
    FMTPerBounds& FMTPerBounds::operator = (const FMTPerBounds& rhs)
        {
        if (this!=&rhs)
            {
            FMTBounds::operator=(rhs);
            }
        return *this;
        }
    FMTPerBounds::operator std::string() const
        {
        return FMTBounds::toString("_CP");
        }
	bool FMTPerBounds::operator == (const FMTPerBounds& rhs) const
		{
		return (FMTBounds::operator==(rhs));
		}

FMTLockBounds::FMTLockBounds():FMTBounds<int>(){}

FMTLockBounds::FMTLockBounds(const FMTsection lsection, const int& lupper, const int& llower) : FMTBounds<int>(lsection,lupper, llower)
{

}

FMTLockBounds::FMTLockBounds(const FMTsection lsection,const FMTkwor key,const int& lupper, const int& llower):FMTBounds<int>(lsection,key,lupper,llower)
    {

    }
FMTLockBounds::FMTLockBounds(const FMTLockBounds& rhs):FMTBounds<int>(rhs)
    {

    }
FMTLockBounds& FMTLockBounds::operator = (const FMTLockBounds& rhs)
    {
     if (this!=&rhs)
        {
        FMTBounds::operator=(rhs);
        }
    return *this;
    }
FMTLockBounds::operator std::string() const
    {
    return FMTBounds::toString("_LOCK");
    }

bool FMTLockBounds::operator == (const FMTLockBounds& rhs) const
	{
	return FMTBounds<int>::operator==(rhs);
	}


FMTSpec::FMTSpec():per(),age(),lock(), yieldnames(), yieldbounds(){}
    FMTSpec::FMTSpec(const FMTSpec& rhs):per(rhs.per),age(rhs.age),
                                        lock(rhs.lock),
									yieldnames(rhs.yieldnames), yieldbounds(rhs.yieldbounds)
        {

        }
    FMTSpec& FMTSpec::operator = (const FMTSpec& rhs)
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
    bool FMTSpec::add(const FMTSpec& rhs)
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
        /*for(std::map<std::string,FMTYldBounds>::const_iterator it = rhs.ylds.begin(); it != rhs.ylds.end() ; it++)
            {
            ylds[it->first] = it->second;
            }*/
        return true;
        }
    bool FMTSpec::setBounds(const FMTPerBounds& bound)
        {
        return per.add(bound);
        }
    bool FMTSpec::addBounds(const FMTAgeBounds& bound)
        {
        return age.add(bound);
        }
    bool FMTSpec::addBounds(const FMTYldBounds& bound)
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
    bool FMTSpec::addBounds(const FMTLockBounds& bound)
        {
        return lock.add(bound);
        }
    FMTSpec::operator std::string() const
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
            for(std::map<std::string,FMTYldBounds>::const_iterator it = ylds.begin(); it != ylds.end() ; it++)
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


	bool FMTSpec::operator == (const FMTSpec& rhs) const
		{
		return (per == rhs.per &&
			age == rhs.age &&
			lock == rhs.lock &&
			yieldnames == rhs.yieldnames &&
			yieldbounds == rhs.yieldbounds);
		}

	bool FMTSpec::operator < (const FMTSpec& rhs) const
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

	/*std::vector<std::string>FMTSpec::getYlds() const
        {
		std::vector<std::string>values;
        if(!ylds.empty())
            {
            values.reserve(ylds.size());
            for(std::map<std::string,FMTYldBounds>::const_iterator it = ylds.begin();it!= ylds.end();++it)
                {
                values.push_back(it->first);
                }
            }
        return values;
        }
	std::map<std::string, FMTYldBounds>FMTSpec::getYldsBounds() const
		{
		return ylds;
		}*/


    bool FMTSpec::empty() const
        {
        return (per.empty() && age.empty() && yieldnames.empty() && lock.empty());
        }

	size_t FMTSpec::hash() const
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
			for (std::map<std::string, FMTYldBounds>::const_iterator it = ylds.begin(); it != ylds.end(); ++it)
				{
				boost::hash_combine(seed, std::hash<double>()(it->second.getLower()));
				boost::hash_combine(seed, std::hash<double>()(it->second.getUpper()));
				}
			}*/
		return seed;
		}

	bool FMTSpec::emptyLock() const
		{
		return lock.empty();
		}

	bool FMTSpec::emptyAge() const
		{
		return age.empty();
		}
	bool FMTSpec::emptyYlds() const
		{
		return yieldnames.empty();
		}

	bool FMTSpec::emptyPeriod() const
		{
		return per.empty();
		}
	int FMTSpec::getPeriodUpperBound() const
	{
		return per.upper;
	}
	int FMTSpec::getPeriodLowerBound() const
	{
		return per.lower;
	}
	int FMTSpec::getLockUpperBound() const
	{
		return lock.upper;
	}

	int FMTSpec::getLockLowerBound() const
	{
		return lock.lower;
	}



	int FMTSpec::getAgeUpperBound() const
		{
		return age.upper;
		}
	int FMTSpec::getAgeLowerBound() const
		{
		return age.lower;
		}

	bool FMTSpec::isSubsetOf(const FMTSpec& rhs) const
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
		/*for (std::map<std::string, FMTYldBounds>::const_iterator it = ylds.begin(); it != ylds.end(); ++it)
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
