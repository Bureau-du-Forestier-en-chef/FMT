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

#ifndef FMTbounds_H_INCLUDED
#define FMTbounds_H_INCLUDED
#include <limits>
#include <map>
#include "FMTexception.h"
#include "FMTutility.h"
#include <vector>
#include <boost/functional/hash.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>

namespace Core
{


template<typename T>
class FMTbounds
    {
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & BOOST_SERIALIZATION_NVP(andbound);
		ar & BOOST_SERIALIZATION_NVP(use);
		ar & BOOST_SERIALIZATION_NVP(section);
		ar &  BOOST_SERIALIZATION_NVP(keytype);
		ar &  BOOST_SERIALIZATION_NVP(upper);
		ar &  BOOST_SERIALIZATION_NVP(lower);
	}
        bool andbound;
        bool use;
    protected:
        FMTwssect section;
        FMTwskwor keytype;
        T upper;
        T lower;
    public:
        FMTbounds() : andbound(true),use(false),section(),keytype(),upper(),lower()
            {

            }
        FMTbounds(const FMTwssect lsection, const T& lupper, const T& llower):andbound(true),use(true),section(lsection),keytype(),upper(lupper),lower(llower)
            {

            }
        FMTbounds(const FMTwssect lsection,const FMTwskwor key, const T& lupper,const T& llower):andbound(true),use(true),section(lsection),keytype(key),upper(lupper),lower(llower)
            {

            }
        virtual ~FMTbounds(){}
        void setorbound()
            {
            andbound = false;
            }
        T getlower() const
            {
            return lower;
            }
         T getupper() const
            {
            return upper;
            }
        FMTbounds(const FMTbounds<T>& rhs) :
            andbound(rhs.andbound),
            use(rhs.use),
            section(rhs.section),
            keytype(rhs.keytype),
            upper(rhs.upper),
            lower(rhs.lower)
            {

            }
		bool operator == (const FMTbounds<T>& rhs) const
			{
			return (andbound == rhs.andbound &&
				use == rhs.use &&
				section == rhs.section &&
				keytype == rhs.keytype &&
				upper == rhs.upper &&
				lower == rhs.lower);
			}

		bool operator < (const FMTbounds<T>& rhs) const
			{
			//strict ordering
			if (andbound < rhs.andbound)
				return true;
			if (rhs.andbound < andbound)
				return false;
			if (use < rhs.use)
				return true;
			if (rhs.use < use)
				return false;
			if (section < rhs.section)
				return true;
			if (rhs.section < section)
				return false;
			if (keytype < rhs.keytype)
				return true;
			if (rhs.keytype < keytype)
				return false;
			if (upper < rhs.upper)
				return true;
			if (rhs.upper < upper)
				return false;
			if (lower < rhs.lower)
				return true;
			if (rhs.lower < lower)
				return false;
			return false;
			}

        FMTbounds<T>& operator = (const FMTbounds<T>& rhs)
            {
            if (this!=&rhs)
                {
                andbound = rhs.andbound;
                section = rhs.section;
                use = rhs.use;
                lower = rhs.lower;
                upper = rhs.upper;
                keytype = rhs.keytype;
                }
            return *this;
            }
        bool empty() const
            {
            return !use;
            }
        bool add(const FMTbounds<T>& rhs)
            {
            if (!this->empty())
                {
                 if (rhs.lower!=std::numeric_limits<T>::min())
                    {
                    lower = rhs.lower;
                    }
                if (rhs.upper!= std::numeric_limits<T>::max())
                    {
                    upper = rhs.upper;
                    }
                }else{
                *this = rhs;
                }
            use = true;
            return true;
            }
		std::string tostring(const std::string& name) const
            {
            std::string line;
            const std::string slower = std::to_string(lower);
            std::string supper = std::to_string(upper);
            bool beenuse = false;
            if (section == FMTwssect::Action)
                {
                if (name.find("_LOCK")!=std::string::npos && upper > 0)
                    {
                    line+= "_LOCKEXEMPT";
                    }else{
                    if(upper==lower)
                        {
                        line=name+" = "+supper;
                        }else{
                            if(lower!= std::numeric_limits<T>::min())
                                {
                                line = name+" >= "+slower;
                                beenuse = true;
                                }
                            if(upper!= std::numeric_limits<T>::max())
                                {
                                if (beenuse)
                                    {
                                    line+=" AND "+ name+" <= "+supper;
                                    }else{
                                    line=name+" <= "+supper;
                                    }
                                }
                            }
                        }
                }else if(section == FMTwssect::Transition || section == FMTwssect::Outputs)
                    {
                    if (name=="_AGE")
                       {
                        if(upper== std::numeric_limits<T>::max())
                            {
                            supper = "_MAXAGE";
                            }
                       }
                    if (keytype == FMTwskwor::Source)
                        {
                        if (name=="_AGE")
                            {
                            line = "@AGE(";
                            }else{
                            line = "@YLD("+name+",";
                            }
                        if(upper==lower)
                            {
                            line+=slower+")";
                            }else if(upper== std::numeric_limits<T>::max() && name!="_AGE")
                                {
                                line+=slower+")";
                                }else{
                                line+=slower+".."+supper+")";
                                }
						if (section == FMTwssect::Outputs && name.find("LOCK") != std::string::npos && lower >= 1)
							{
								line = "_INVLOCK";
							}
                        }else if(keytype == FMTwskwor::Target)
                            {
                            if(name.find("_LOCK")!= std::string::npos)
                                {
                                line+= "_LOCK " + slower;
                                }else{
                                line=name + " " + slower;
                                }
                            }
                    }
            return line;
        }
    };

class FMTyldbounds: public FMTbounds<double>
    {
    friend class FMTspec;
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & boost::serialization::make_nvp("bounds",boost::serialization::base_object<FMTbounds<double>>(*this));
		ar & BOOST_SERIALIZATION_NVP(yield);
	}
    std::string yield;
    public:
    FMTyldbounds();
	FMTyldbounds(const FMTwssect lsection, const std::string& lyield, const double& lupper, const double& llower);
    FMTyldbounds(const FMTwssect lsection,const FMTwskwor key,const std::string& lyield, const double& lupper,const double& llower);
    FMTyldbounds(const std::string& lyield,const FMTbounds<double>& rhs);
    FMTyldbounds(const FMTyldbounds& rhs);
    FMTyldbounds& operator = (const FMTyldbounds& rhs);
	bool operator == (const FMTyldbounds& rhs) const;
    operator std::string() const;
    };

class FMTagebounds: public FMTbounds<int>
    {
    friend class FMTspec;
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & boost::serialization::make_nvp("bounds", boost::serialization::base_object<FMTbounds<int>>(*this));
	}
    public:
    FMTagebounds();
    FMTagebounds(FMTwssect lsection,const int& lupper, const int& llower);
    FMTagebounds(FMTwssect lsection,FMTwskwor key, const int& lupper, const int& llower);
    FMTagebounds(const FMTagebounds& rhs);
    FMTagebounds(const FMTbounds<int>& rhs);
    FMTagebounds& operator = (const FMTagebounds& rhs);
	bool operator == (const FMTagebounds& rhs) const;
    operator std::string() const;
    };

class FMTperbounds: public FMTbounds<int>
    {
    friend class FMTspec;
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & boost::serialization::make_nvp("bounds", boost::serialization::base_object<FMTbounds<int>>(*this));
	}
    public:
    FMTperbounds();
    FMTperbounds(const FMTwssect lsection,const int& lupper,const int& llower);
    FMTperbounds(const FMTperbounds& rhs);
	FMTperbounds(const FMTbounds<int>& rhs);
    FMTperbounds& operator = (const FMTperbounds& rhs);
	bool operator == (const FMTperbounds& rhs) const;
    operator std::string() const;
    };


class FMTlockbounds : public FMTbounds<int>
    {
    friend class FMTspec;
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
		{
		ar & boost::serialization::make_nvp("bounds", boost::serialization::base_object<FMTbounds<int>>(*this));
		}
    public:
    FMTlockbounds();
    FMTlockbounds(const FMTwssect lsection,const FMTwskwor key,const int& lupper, const int& llower);
    FMTlockbounds(const FMTlockbounds& rhs);
    FMTlockbounds& operator = (const FMTlockbounds& rhs);
	bool operator == (const FMTlockbounds& rhs) const;
    operator std::string() const;
    };

class FMTyields;

class FMTspec
    {
	friend class Core::FMTyields;
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & BOOST_SERIALIZATION_NVP(per);
		ar & BOOST_SERIALIZATION_NVP(age);
		ar & BOOST_SERIALIZATION_NVP(lock);
		ar & BOOST_SERIALIZATION_NVP(ylds);
	}
protected:
    FMTperbounds per;
    FMTagebounds age;
    FMTlockbounds lock;
	std::map<std::string, FMTyldbounds>ylds;
public:
    FMTspec();
    virtual ~FMTspec()=default;
    FMTspec(const FMTspec& rhs);
    FMTspec& operator = (const FMTspec& rhs);
    bool add(const FMTspec& rhs);
    bool setbounds(const FMTperbounds& bound);
    bool addbounds(const FMTagebounds& bound);
    bool addbounds(const FMTyldbounds& bound);
    bool addbounds(const FMTlockbounds& bound);
	inline bool allowwithoutyield(const int& tperiod, const int& tage, const int& tlock) const
		{
		return ((per.empty() || (tperiod <= per.upper && tperiod >= per.lower)) &&
			(age.empty() || (tage <= age.upper && tage >= age.lower)) &&
			(lock.empty() || (tlock >= lock.lower)));
		}
	inline bool allow(const int& tperiod, const int& tage, const int& tlock, const std::map<std::string, double>& names) const
		{
		for (std::map<std::string, FMTyldbounds>::const_iterator it = ylds.begin(); it != ylds.end(); ++it)
			{
				const FMTyldbounds* bnds = &it->second;
				if ((bnds->lower > names.at(it->first)) || (bnds->upper < names.at(it->first)))
				{
					return false;
				}
			}
		return (allowwithoutyield(tperiod,tage,tlock));
		}
	std::map<std::string,FMTyldbounds>getyldsbounds() const;
	std::vector<std::string>getylds() const;
    virtual operator std::string() const;
	bool operator == (const FMTspec& rhs) const;
	bool operator < (const FMTspec& rhs) const;
	size_t hash() const;
    bool empty() const;
	bool emptyage() const;
	bool emptyylds() const;
	bool emptyperiod() const;
	bool emptylock() const;
	int getageupperbound() const;
	int getagelowerbound() const;
	int getperiodupperbound() const;
	int getperiodlowerbound() const;
	bool issubsetof(const FMTspec& rhs) const;
    };

}

namespace boost {

	template <>
	struct hash<Core::FMTspec>
	{
		std::size_t operator()(const Core::FMTspec& spec) const
		{
			return spec.hash();
		}
	};


}


#endif // FMTbounds_H_INCLUDED
