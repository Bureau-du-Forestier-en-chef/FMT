/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
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
#include <boost/serialization/string.hpp>
#include <algorithm>

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
        FMTsection section;
        FMTkwor keytype;
        T upper;
        T lower;
    public:
        FMTbounds() : andbound(true),use(false),section(),keytype(),upper(),lower()
            {

            }
        FMTbounds(const FMTsection lsection, const T& lupper, const T& llower):andbound(true),use(true),section(lsection),keytype(),upper(lupper),lower(llower)
            {

            }
        FMTbounds(const FMTsection lsection,const FMTkwor key, const T& lupper,const T& llower):andbound(true),use(true),section(lsection),keytype(key),upper(lupper),lower(llower)
            {

            }
		virtual ~FMTbounds() = default;
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
		 inline bool out(const T&  value) const
			{
			 return ((lower > value) || (upper < value));
			}
		 inline bool in(const T&  value) const
			{
			 return (empty() || (value <= upper && value >= lower));
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
                 if (rhs.lower!=std::numeric_limits<T>::lowest())
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
            if (section == FMTsection::Action)
                {
                if (name.find("_LOCK")!=std::string::npos && upper > 0)
                    {
                    line+= "_LOCKEXEMPT";
                    }else{
                    if(upper==lower)
                        {
                        line=name+" = "+supper;
                        }else{
                            if(lower!= std::numeric_limits<T>::lowest())
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
                }else if(section == FMTsection::Transition || section == FMTsection::Outputs)
                    {
                    if (name=="_AGE")
                       {
                        if(upper== std::numeric_limits<T>::max())
                            {
                            supper = "_MAXAGE";
                            }
                       }
                    if (keytype == FMTkwor::Source)
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
						if (section == FMTsection::Outputs)
							{
							if (name.find("LOCK") != std::string::npos && lower >= 1)
								{
								line = "_INVLOCK";
							}else if(name.find("_CP") != std::string::npos && lower == 0 && upper == 0)
								{
								line = "[0]";
								}	
							}

                        }else if(keytype == FMTkwor::Target)
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
	~FMTyldbounds() = default;
	FMTyldbounds(const FMTsection lsection, const std::string& lyield, const double& lupper, const double& llower);
    FMTyldbounds(const FMTsection lsection,const FMTkwor key,const std::string& lyield, const double& lupper,const double& llower);
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
	~FMTagebounds() = default;
    FMTagebounds(FMTsection lsection,const int& lupper, const int& llower);
    FMTagebounds(FMTsection lsection,FMTkwor key, const int& lupper, const int& llower);
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
	~FMTperbounds() = default;
    FMTperbounds(const FMTsection lsection,const int& lupper,const int& llower);
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
	~FMTlockbounds() = default;
    FMTlockbounds(const FMTsection lsection,const FMTkwor key,const int& lupper, const int& llower);
    FMTlockbounds(const FMTlockbounds& rhs);
    FMTlockbounds& operator = (const FMTlockbounds& rhs);
	bool operator == (const FMTlockbounds& rhs) const;
    operator std::string() const;
    };

class FMTyields;

class FMTEXPORT FMTspec
    {
	friend class Core::FMTyields;
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & BOOST_SERIALIZATION_NVP(per);
		ar & BOOST_SERIALIZATION_NVP(age);
		ar & BOOST_SERIALIZATION_NVP(lock);
		ar & BOOST_SERIALIZATION_NVP(yieldnames);
		ar & BOOST_SERIALIZATION_NVP(yieldbounds);
	}
protected:
    FMTperbounds per;
    FMTagebounds age;
    FMTlockbounds lock;
	std::vector<std::string>yieldnames;
	std::vector<FMTyldbounds>yieldbounds;
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
		return (per.in(tperiod) &&
			age.in(tage) &&
			(lock.empty() || (tlock >= lock.lower)));
		}
	inline const FMTyldbounds& getyieldbound(const std::string& name) const
		{
		return yieldbounds.at(std::distance(yieldnames.begin(), std::find(yieldnames.begin(), yieldnames.end(), name)));
		}
	inline bool allowyields(const std::vector<double>& values) const
	{
		for (size_t location = 0; location < yieldnames.size(); ++location)
		{
			if (yieldbounds.at(location).out(values.at(location)))
			{
				return false;
			}
		}

		return true;
	}
	inline bool allow(const int& tperiod, const int& tage, const int& tlock, const std::vector<double>& values) const
		{
		for (size_t location = 0; location < yieldnames.size(); ++location)
		{
			if (yieldbounds.at(location).out(values.at(location)))
				{
				return false;
				}
		}
		return (allowwithoutyield(tperiod,tage,tlock));
		}
	//std::map<std::string,FMTyldbounds>getyldsbounds() const;
	inline const std::vector<std::string>& getylds() const
		{
		return yieldnames;
		}
	inline const std::vector<FMTyldbounds>& getyldbounds() const
		{
		return yieldbounds;
		}
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
