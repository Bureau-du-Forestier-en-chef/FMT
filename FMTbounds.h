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

using namespace Exception;
using namespace std;

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
		ar & BOOST_SERIALIZATION_NVP(priority);
	}
        bool andbound;
        bool use;
    protected:
        FMTwssect section;
        FMTwskwor keytype;
        T upper;
        T lower;
    public:
        int priority;
        FMTbounds() : andbound(true),use(false),section(),keytype(),upper(),lower(),priority(0)
            {

            }
        FMTbounds(FMTwssect lsection, T& lupper, T& llower):andbound(true),use(true),section(lsection),keytype(),upper(lupper),lower(llower),priority(0)
            {

            }
        FMTbounds(FMTwssect lsection,FMTwskwor key, T& lupper, T& llower):andbound(true),use(true),section(lsection),keytype(key),upper(lupper),lower(llower),priority(0)
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
            lower(rhs.lower),
            priority(rhs.priority)
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

        FMTbounds<T>& operator = (const FMTbounds<T>& rhs)
            {
            if (this!=&rhs)
                {
                priority = rhs.priority;
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
        bool add(FMTbounds<T>& rhs)
            {
            if (!this->empty())
                {
                 if (rhs.lower!=numeric_limits<T>::min())
                    {
                    lower = rhs.lower;
                    }
                if (rhs.upper!=numeric_limits<T>::max())
                    {
                    upper = rhs.upper;
                    }
                }else{
                *this = rhs;
                }
            use = true;
            return true;
            }
        string tostring(string name) const
            {
            string line;
            string slower = to_string(lower);
            string supper = to_string(upper);
            bool beenuse = false;
            if (section == FMTwssect::Action)
                {
                if (name.find("_LOCK")!=string::npos && upper > 0)
                    {
                    line+= "_LOCKEXEMPT";
                    }else{
                    if(upper==lower)
                        {
                        line=name+" = "+supper;
                        }else{
                            if(lower!=numeric_limits<T>::min())
                                {
                                line = name+" >= "+slower;
                                beenuse = true;
                                }
                            if(upper!=numeric_limits<T>::max())
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
                        //USEMAXAGE
                        if(upper==numeric_limits<T>::max())
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
                            }else if(upper==numeric_limits<T>::max() && name!="_AGE")
                                {
                                line+=slower+")";
                                }else{
                                line+=slower+".."+supper+")";
                                }
						if (section == FMTwssect::Outputs && name.find("LOCK") != string::npos && lower >= 1)
							{
								line = "_INVLOCK";
							}
                        }else if(keytype == FMTwskwor::Target)
                            {
                            if(name.find("_LOCK")!=string::npos)
                                {
                                line+= "_LOCK " + slower;
                                }else{
                                line=name + " " + slower;
                                }
                            }
                    }/*else if(section == FMTwssect::Outputs)
                        {
                        if (name.find("LOCK")!=string::npos)
                            {
                            if(lower >= 1)
                                {
                                line+="_INVLOCK";
                                }
                            }
                        }*/
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
    string yield;
    public:
    FMTyldbounds();
    FMTyldbounds(FMTwssect lsection,string& lyield, double& lupper, double& llower);
    FMTyldbounds(FMTwssect lsection,FMTwskwor key,string& lyield, double& lupper, double& llower);
    FMTyldbounds(string& lyield,const FMTbounds<double>& rhs);
    FMTyldbounds(const FMTyldbounds& rhs);
    FMTyldbounds& operator = (const FMTyldbounds& rhs);
	bool operator == (const FMTyldbounds& rhs) const;
    operator string() const;
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
    FMTagebounds(FMTwssect lsection,int lupper, int llower);
    FMTagebounds(FMTwssect lsection,FMTwskwor key,int lupper, int llower);
    FMTagebounds(const FMTagebounds& rhs);
    FMTagebounds(const FMTbounds<int>& rhs);
    FMTagebounds& operator = (const FMTagebounds& rhs);
	bool operator == (const FMTagebounds& rhs) const;
    operator string() const;
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
    FMTperbounds(FMTwssect lsection,int& lupper, int& llower);
    FMTperbounds(FMTwssect lsection, string op, string value);
    FMTperbounds(const FMTperbounds& rhs);
	FMTperbounds(const FMTbounds<int>& rhs);
    FMTperbounds& operator = (const FMTperbounds& rhs);
	bool operator == (const FMTperbounds& rhs) const;
    operator string() const;
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
    FMTlockbounds(FMTwssect lsection,FMTwskwor key,int& lupper, int& llower);
    FMTlockbounds(const FMTlockbounds& rhs);
    FMTlockbounds& operator = (const FMTlockbounds& rhs);
	bool operator == (const FMTlockbounds& rhs) const;
    operator string() const;
    };

class FMTspec
    {
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
public:
    map<string,FMTyldbounds>ylds;
    FMTspec();
    virtual ~FMTspec()=default;
    FMTspec(const FMTspec& rhs);
    FMTspec& operator = (const FMTspec& rhs);
    bool add(FMTspec& rhs);
    bool setbounds(FMTperbounds bound);
    bool addbounds(FMTagebounds bound);
    bool addbounds(FMTyldbounds bound);
    bool addbounds(FMTlockbounds bound);
    bool allow(const int& tperiod,const int& tage,const int& tlock,const map<string,double>& names) const;
    map<string,FMTyldbounds>getyldsbounds() const;
    vector<string>getylds() const;
    virtual operator string() const;
	bool operator == (const FMTspec& rhs) const;
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
