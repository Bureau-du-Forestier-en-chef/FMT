#include "FMTbounds.h"

namespace Core{

FMTyldbounds::FMTyldbounds():FMTbounds<double>(),yield(){}


    FMTyldbounds::FMTyldbounds(FMTwssect lsection,string& lyield, double& lupper, double& llower):FMTbounds<double>(lsection,lupper,llower),yield(lyield)
        {
        priority = 2;
        }
    FMTyldbounds::FMTyldbounds(FMTwssect lsection,FMTwskwor key,string& lyield, double& lupper, double& llower):FMTbounds<double>(lsection,key,lupper,llower),yield(lyield)
        {

        priority = 2;
        }

        FMTyldbounds::FMTyldbounds(const FMTyldbounds& rhs) : FMTbounds(rhs) , yield(rhs.yield)
            {

            }

        FMTyldbounds::FMTyldbounds(string& lyield,const FMTbounds<double>& rhs) : FMTbounds(rhs) , yield(lyield)
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
        FMTyldbounds::operator string() const
            {
            return FMTbounds::tostring(yield);
            }


        /* bool FMTyldbounds::validate(const FMTdevelopment& dev)
            {

            }
         FMTdevelopment FMTyldbounds::get(FMTdevelopment& dev){}*/


    FMTagebounds::FMTagebounds():FMTbounds<int>(){}
    FMTagebounds::FMTagebounds(FMTwssect lsection,int lupper, int llower):FMTbounds<int>(lsection,lupper,llower)
        {
        priority = 1;
        }
   FMTagebounds::FMTagebounds(FMTwssect lsection,FMTwskwor key,int lupper, int llower):FMTbounds<int>(lsection,key,lupper,llower)
        {
        priority = 1;
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
    FMTagebounds::operator string() const
        {
        return FMTbounds::tostring("_AGE");
        }
	bool  FMTagebounds::operator == (const FMTagebounds& rhs) const
		{
		return (FMTbounds::operator==(rhs));
		}
   /* bool FMTagebounds::validate(FMTdevelopment& dev){}
    FMTdevelopment FMTagebounds::get(FMTdevelopment& dev){}*/


    FMTperbounds::FMTperbounds():FMTbounds<int>(){}
    FMTperbounds::FMTperbounds(FMTwssect lsection,int& lupper, int& llower):FMTbounds<int>(lsection,lupper,llower)
        {
        priority = 0;
        }
    /*FMTperbounds::FMTperbounds(FMTwssect lsection, string op, string value):FMTperbounds<int>(lsection,op,value)
         {
         priority = 0;
         }*/
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
    FMTperbounds::operator string() const
        {
        return FMTbounds::tostring("_CP");
        }
	bool FMTperbounds::operator == (const FMTperbounds& rhs) const
		{
		return (FMTbounds::operator==(rhs));
		}
    /*bool FMTperbounds::validate(FMTdevelopment& dev){}
    FMTdevelopment FMTperbounds::get(FMTdevelopment& dev){}*/


FMTlockbounds::FMTlockbounds():FMTbounds<int>(){}
FMTlockbounds::FMTlockbounds(FMTwssect lsection,FMTwskwor key,int& lupper, int& llower):FMTbounds<int>(lsection,key,lupper,llower)
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
FMTlockbounds::operator string() const
    {
    return FMTbounds::tostring("_LOCK");
    }

bool FMTlockbounds::operator == (const FMTlockbounds& rhs) const
	{
	return FMTbounds<int>::operator==(rhs);
	}
/*bool FMTlockbounds::validate(FMTdevelopment& dev){}
FMTdevelopment FMTlockbounds::get(FMTdevelopment& dev){}*/



bool compareFMTbounds(const FMTperbounds& rhs1, const FMTperbounds& rhs2)
    {
    return rhs1.priority < rhs2.priority;
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
    bool FMTspec::add(FMTspec& rhs)
        {
        if (!rhs.per.empty())
            {
            per = rhs.per;
            }
        if (!rhs.age.empty())
            {
            age = rhs.age;
            }
        for(map<string,FMTyldbounds>::iterator it = rhs.ylds.begin(); it != rhs.ylds.end() ; it++)
            {
            ylds[it->first] = it->second;
            }
        return true;
        }
    bool FMTspec::setbounds(FMTperbounds bound)
        {
        return per.add(bound);
        }
    bool FMTspec::addbounds(FMTagebounds bound)
        {
        return age.add(bound);
        }
    bool FMTspec::addbounds(FMTyldbounds bound)
        {
        if (ylds.find(bound.yield)!=ylds.end())
            {
            ylds[bound.yield].add(bound);
            }else{
            ylds[bound.yield] = bound;
            }
        return true;
        }
    bool FMTspec::addbounds(FMTlockbounds bound)
        {
        return lock.add(bound);
        }
    FMTspec::operator string() const
        {
        string line;
        bool andstuff = false;
        if (!per.empty())
            {
            line+=string(per)+" ";
            andstuff = true;
            }
        if (!age.empty())
            {
            if(andstuff)
                {
                line+="AND ";
                }
            line+=string(age)+" ";
            andstuff = true;
            }
        if (!ylds.empty())
            {
            if(andstuff)
                {
                line+="AND ";
                }
            size_t nyld = 1;
            for(map<string,FMTyldbounds>::const_iterator it = ylds.begin(); it != ylds.end() ; it++)
                {
                if (nyld!=ylds.size())
                    {
                    line+=string(it->second)+ " AND ";
                    }else{
                    line+=string(it->second)+ " ";
                    }
                ++nyld;
                }
            }
        if (!lock.empty())
            {
            line+=string(lock);
            }
        return line;
        }
    bool FMTspec::allow(const int& tperiod,const int& tage,const int& tlock,const map<string,double>& names) const
        {
        bool yldbounds = true;
        if (!ylds.empty())
            {
            for(map<string,FMTyldbounds>::const_iterator it = ylds.begin();it!= ylds.end();++it)
                {
                const FMTyldbounds* bnds  = &it->second;
                if((bnds->lower > names.at(it->first)) || (bnds->upper < names.at(it->first)))
                    {
                    return false;
                    }
                }
            }
        return ((per.empty() || (tperiod <= per.upper && tperiod >= per.lower)) &&
                (age.empty() || (tage <= age.upper && tage >= age.lower)) &&
                (lock.empty() || (tlock >= lock.lower)) && (yldbounds));
        }

	bool FMTspec::operator == (const FMTspec& rhs) const
		{
		return (per == rhs.per &&
			age == rhs.age &&
			lock == rhs.lock &&
			ylds == rhs.ylds);
		}

    vector<string>FMTspec::getylds() const
        {
        vector<string>values;
        if(!ylds.empty())
            {
            values.reserve(ylds.size());
            for(map<string,FMTyldbounds>::const_iterator it = ylds.begin();it!= ylds.end();++it)
                {
                values.push_back(it->first);
                }
            }
        return values;
        }
	map<string, FMTyldbounds>FMTspec::getyldsbounds() const
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
			for (map<string, FMTyldbounds>::const_iterator it = ylds.begin(); it != ylds.end(); ++it)
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
		for (map<string, FMTyldbounds>::const_iterator it = ylds.begin(); it != ylds.end(); ++it)
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
