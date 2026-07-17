/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTbounds_Hm_included
#define FMTbounds_Hm_included
#include <limits>
#include <map>
#include "FMTException.h"
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
class FMTBounds
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
        FMTBounds() : andbound(true),use(false),section(),keytype(),upper(),lower()
            {

            }
        FMTBounds(const FMTsection lsection, const T& lupper, const T& llower):andbound(true),use(true),section(lsection),keytype(),upper(lupper),lower(llower)
            {

            }
        FMTBounds(const FMTsection lsection,const FMTkwor key, const T& lupper,const T& llower):andbound(true),use(true),section(lsection),keytype(key),upper(lupper),lower(llower)
            {

            }
		virtual ~FMTBounds() = default;
        void setOrBound()
            {
            andbound = false;
            }
        T getLower() const
            {
            return lower;
            }
         T getUpper() const
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
        FMTBounds(const FMTBounds<T>& rhs) :
            andbound(rhs.andbound),
            use(rhs.use),
            section(rhs.section),
            keytype(rhs.keytype),
            upper(rhs.upper),
            lower(rhs.lower)
            {

            }
		bool operator == (const FMTBounds<T>& rhs) const
			{
			return (andbound == rhs.andbound &&
				use == rhs.use &&
				section == rhs.section &&
				keytype == rhs.keytype &&
				upper == rhs.upper &&
				lower == rhs.lower);
			}

		bool operator < (const FMTBounds<T>& rhs) const
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

        FMTBounds<T>& operator = (const FMTBounds<T>& rhs)
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
        bool add(const FMTBounds<T>& rhs)
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
		std::string toString(const std::string& name) const
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
                    //if (name=="_AGE")
                       //{
                        if(upper== std::numeric_limits<T>::max())
                            {
                            supper = "_MAXAGE";
                            }
                       //}
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
                            }/*else if (upper == std::numeric_limits<T>::max() && name != "_AGE")
                                {
                                line+=slower+")";
                                }*/else{
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

class FMTEXPORT FMTYldBounds: public FMTBounds<double>
    {
    friend class FMTSpec;
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & boost::serialization::make_nvp("bounds",boost::serialization::base_object<FMTBounds<double>>(*this));
		ar & BOOST_SERIALIZATION_NVP(yield);
	}
    std::string yield;
    public:
	FMTYldBounds();
	~FMTYldBounds() = default;
	FMTYldBounds(const FMTsection lsection, const std::string& lyield, const double& lupper, const double& llower);
    FMTYldBounds(const FMTsection lsection,const FMTkwor key,const std::string& lyield, const double& lupper,const double& llower);
    FMTYldBounds(const std::string& lyield,const FMTBounds<double>& rhs);
	FMTYldBounds(const FMTYldBounds& rhs);
    FMTYldBounds& operator = (const FMTYldBounds& rhs);
	bool operator == (const FMTYldBounds& rhs) const;
    operator std::string() const;
    };

class FMTEXPORT FMTAgeBounds: public FMTBounds<int>
    {
    friend class FMTSpec;
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & boost::serialization::make_nvp("bounds", boost::serialization::base_object<FMTBounds<int>>(*this));
	}
    public:
    FMTAgeBounds();
	~FMTAgeBounds() = default;
    FMTAgeBounds(FMTsection lsection,const int& lupper, const int& llower);
    FMTAgeBounds(FMTsection lsection,FMTkwor key, const int& lupper, const int& llower);
    FMTAgeBounds(const FMTAgeBounds& rhs);
    FMTAgeBounds(const FMTBounds<int>& rhs);
    FMTAgeBounds& operator = (const FMTAgeBounds& rhs);
	bool operator == (const FMTAgeBounds& rhs) const;
    operator std::string() const;
    };

class FMTEXPORT FMTPerBounds: public FMTBounds<int>
    {
    friend class FMTSpec;
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & boost::serialization::make_nvp("bounds", boost::serialization::base_object<FMTBounds<int>>(*this));
	}
    public:
    FMTPerBounds();
	~FMTPerBounds() = default;
    FMTPerBounds(const FMTsection lsection,const int& lupper,const int& llower);
    FMTPerBounds(const FMTPerBounds& rhs);
	FMTPerBounds(const FMTBounds<int>& rhs);
    FMTPerBounds& operator = (const FMTPerBounds& rhs);
	bool operator == (const FMTPerBounds& rhs) const;
    operator std::string() const;
    };


class FMTEXPORT FMTLockBounds : public FMTBounds<int>
    {
    friend class FMTSpec;
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
		{
		ar & boost::serialization::make_nvp("bounds", boost::serialization::base_object<FMTBounds<int>>(*this));
		}
    public:
    FMTLockBounds();
	~FMTLockBounds() = default;
    FMTLockBounds(const FMTsection lsection,const FMTkwor key,const int& lupper, const int& llower);
	FMTLockBounds(const FMTsection lsection, const int& lupper, const int& llower);
    FMTLockBounds(const FMTLockBounds& rhs);
    FMTLockBounds& operator = (const FMTLockBounds& rhs);
	bool operator == (const FMTLockBounds& rhs) const;
    operator std::string() const;
    };

class FMTYields;
// DocString: FMTSpec
/**
FMTSpec represent the specifications of a group of FMTDevelopment. You can characterize a
FMTDevelopment by it's period (FMTPerBounds), it's age (FMTAgeBounds), it's lock (FMTLockBounds) and 
it's yields (yieldbounds). Bounds are "bounds" lower and upper bounds:
Example: 1>=age<=10, 5>=period<=20, 0>=lock<=10 and 145<=yield<=700
*/
class FMTEXPORT FMTSpec
    {
	// DocString: FMTSpec::serialize
	/**
	serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
	*/
	friend class Core::FMTYields;
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
	// DocString: FMTPerBounds::per
	///Period bounds so lower >= period <= upper 
    FMTPerBounds per;
	// DocString: FMTPerBounds::age
	///Age bounds so lower >= age <= upper 
    FMTAgeBounds age;
	// DocString: FMTLockBounds::lock
	///Lock bounds so lower >= lock <= upper 
    FMTLockBounds lock;
	// DocString: FMTLockBounds::yieldnames
	///The names of each yield in the vector of yieldbounds
	std::vector<std::string>yieldnames;
	// DocString: FMTLockBounds::yieldbounds
	///the yields bounds vector lower>=y1<=upper,lower>=y2<=upper
	std::vector<FMTYldBounds>yieldbounds;
public:
	// DocString: FMTSpec()
	/**
	Default constructor for FMTSpec
	*/
    FMTSpec();
	// DocString: ~FMTSpec()
	/**
	Destructor for FMTSpec
	*/
    virtual ~FMTSpec()=default;
	// DocString: FMTSpec(const FMTSpec&)
	/**
	FMTSpec copy constructor.
	*/
    FMTSpec(const FMTSpec& rhs);
	// DocString: FMTSpec::operator=
	/**
	Copy assignement of FMTSpec
	*/
    FMTSpec& operator = (const FMTSpec& rhs);
	// DocString: FMTSpec::add
	/**
	Add yields spec or age, period or lock spec from an other spec.
	*/
    bool add(const FMTSpec& rhs);
	// DocString: FMTSpec::setBounds
	/**
	Set period bounds
	*/
    bool setBounds(const FMTPerBounds& bound);
	// DocString: FMTSpec::setBounds
	/**
	Set age bounds
	*/
    bool addBounds(const FMTAgeBounds& bound);
	// DocString: FMTSpec::setyldbounds
	/**
	Set yield bounds
	*/
    bool addBounds(const FMTYldBounds& bound);
	// DocString: FMTSpec::setlockbounds
	/**
	Set lock bounds
	*/
    bool addBounds(const FMTLockBounds& bound);
	// DocString: FMTSpec::allowWithoutYield
	/**
	return true if the given period, age and lock respect the age,period and lock bounds.
	*/
	inline bool allowWithoutYield(const int& tperiod, const int& tage, const int& tlock) const
		{
		return (per.in(tperiod) &&
			age.in(tage) &&
			(lock.empty() || (tlock >= lock.lower)));
		}
	// DocString: FMTSpec::getYieldBound
	/**
	return the reference of a given FMTYldBounds using the name of the FMTYldBounds.
	*/
	inline const FMTYldBounds& getYieldBound(const std::string& name) const
		{
		return yieldbounds.at(std::distance(yieldnames.begin(), std::find(yieldnames.begin(), yieldnames.end(), name)));
		}
	// DocString: FMTSpec::allowYields
	/**
	Return true if all the yields (values) are within the yield bounds.
	*/
	inline bool allowYields(const std::vector<double>& values) const
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
	// DocString: FMTSpec::allow
	/**
	Return true if all age,period and lock are within the bounds and when the yields (values) are within the yield bounds
	*/
	inline bool allow(const int& tperiod, const int& tage, const int& tlock, const std::vector<double>& values) const
		{
		for (size_t location = 0; location < yieldnames.size(); ++location)
		{
			if (yieldbounds.at(location).out(values.at(location)))
				{
				return false;
				}
		}
		return (allowWithoutYield(tperiod,tage,tlock));
		}
	// DocString: FMTSpec::getYlds
	/**
	Return the yields names of the yield bounds.
	*/
	inline const std::vector<std::string>& getYlds() const
		{
		return yieldnames;
		}
	// DocString: FMTSpec::getYldBounds
	/**
	Return tthe yields bounds of the spec.
	*/
	inline const std::vector<FMTYldBounds>& getYldBounds() const
		{
		return yieldbounds;
		}
	// DocString: FMTSpec::operator std::string
	/**
	Returns the string reprensentation of the FMTSpec like _age >= lower and _Age <= upper and ....
	*/
    virtual operator std::string() const;
	// DocString: FMTSpec::operator==
	/**
	FMTSpec equality operator.
	*/
	bool operator == (const FMTSpec& rhs) const;
	// DocString: FMTSpec::operator<
	/**
	FMTSpec less than operator.
	*/
	bool operator < (const FMTSpec& rhs) const;
	// DocString: FMTSpec::hash
	/**
	Return the hashed value of FMTSpec.
	*/
	size_t hash() const;
	// DocString: FMTSpec::empty
	/**
	Return true if all bounds are empty.
	*/
    bool empty() const;
	// DocString: FMTSpec::emptyAge
	/**
	Return true if age bound is empty.
	*/
	bool emptyAge() const;
	// DocString: FMTSpec::emptyYlds
	/**
	Return true if yields bounds are empty.
	*/
	bool emptyYlds() const;
	// DocString: FMTSpec::emptyPeriod
	/**
	Return true if period bounds is empty.
	*/
	bool emptyPeriod() const;
	// DocString: FMTSpec::emptyLock
	/**
	Return true if lock bounds is empty.
	*/
	bool emptyLock() const;
	// DocString: FMTSpec::getAgeUpperBound
	/**
	Get the upper bounds of the age bounds.
	*/
	int getAgeUpperBound() const;
	// DocString: FMTSpec::getAgeLowerBound
	/**
	Get the lower bound of the age bounds.
	*/
	int getAgeLowerBound() const;
	// DocString: FMTSpec::getPeriodUpperBound
	/**
	Get the upper bound of the period bounds.
	*/
	int getPeriodUpperBound() const;
	// DocString: FMTSpec::getPeriodLowerBound
	/**
	Get the lower bound of the period bounds.
	*/
	int getPeriodLowerBound() const;
	// DocString: FMTSpec::getLockUpperBound
	/**
	Get the upper bounds of the age bounds.
	*/
	int getLockUpperBound() const;
	// DocString: FMTSpec::getLockLowerBound
	/**
	Get the lower bound of the age bounds.
	*/
	int getLockLowerBound() const;
	// DocString: FMTSpec::isSubsetOf
	/**
	return true if this spec is the subset of the rhs spec.
	*/
	bool isSubsetOf(const FMTSpec& rhs) const;
    };

}

namespace boost {

	template <>
	struct hash<Core::FMTSpec>
	{
		std::size_t operator()(const Core::FMTSpec& spec) const
		{
			return spec.hash();
		}
	};


}


#endif // FMTbounds_Hm_included
