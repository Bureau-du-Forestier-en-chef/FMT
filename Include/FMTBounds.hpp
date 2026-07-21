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


// DocString: FMTBounds
/**
@brief Templated lower and upper bounds for a value, used to characterize developments.
@tparam T the type of the bounded value.
*/
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
        // DocString: FMTBounds()
        /**
        @brief Default constructor for FMTBounds.
        */
        FMTBounds() : andbound(true),use(false),section(),keytype(),upper(),lower()
            {

            }
        // DocString: FMTBounds(const FMTsection,const T&,const T&)
        /**
        @brief Construct bounds from a section, an upper and a lower bound.
        @param[in] lsection the section.
        @param[in] lupper the upper bound.
        @param[in] llower the lower bound.
        */
        FMTBounds(const FMTsection lsection, const T& lupper, const T& llower):andbound(true),use(true),section(lsection),keytype(),upper(lupper),lower(llower)
            {

            }
        // DocString: FMTBounds(const FMTsection,const FMTkwor,const T&,const T&)
        /**
        @brief Construct bounds from a section, a keyword, an upper and a lower bound.
        @param[in] lsection the section.
        @param[in] key the keyword.
        @param[in] lupper the upper bound.
        @param[in] llower the lower bound.
        */
        FMTBounds(const FMTsection lsection,const FMTkwor key, const T& lupper,const T& llower):andbound(true),use(true),section(lsection),keytype(key),upper(lupper),lower(llower)
            {

            }
		// DocString: ~FMTBounds()
		/**
		@brief Default virtual destructor for FMTBounds.
		*/
		virtual ~FMTBounds() = default;
        // DocString: FMTBounds::setOrBound
        /**
        @brief Set the bounds to be combined with an OR instead of an AND.
        */
        void setOrBound()
            {
            andbound = false;
            }
        // DocString: FMTBounds::getLower
        /**
        @brief Return the lower bound.
        @return the lower bound.
        */
        T getLower() const
            {
            return lower;
            }
         // DocString: FMTBounds::getUpper
         /**
         @brief Return the upper bound.
         @return the upper bound.
         */
         T getUpper() const
            {
            return upper;
            }
		 // DocString: FMTBounds::out
		 /**
		 @brief Return true if a value is outside the bounds.
		 @param[in] value the value.
		 @return true if the value is outside the bounds else false.
		 */
		 inline bool out(const T&  value) const
			{
			 return ((lower > value) || (upper < value));
			}
		 // DocString: FMTBounds::in
		 /**
		 @brief Return true if a value is within the bounds.
		 @param[in] value the value.
		 @return true if the value is within the bounds else false.
		 */
		 inline bool in(const T&  value) const
			{
			 return (empty() || (value <= upper && value >= lower));
			}
        // DocString: FMTBounds(const FMTBounds<T>&)
        /**
        @brief Copy constructor for FMTBounds.
        @param[in] rhs the FMTBounds to copy.
        */
        FMTBounds(const FMTBounds<T>& rhs) :
            andbound(rhs.andbound),
            use(rhs.use),
            section(rhs.section),
            keytype(rhs.keytype),
            upper(rhs.upper),
            lower(rhs.lower)
            {

            }
		// DocString: FMTBounds::operator==
		/**
		@brief Equality comparison operator of FMTBounds.
		@param[in] rhs the bounds to compare with.
		@return true if both bounds are equal else false.
		*/
		bool operator == (const FMTBounds<T>& rhs) const
			{
			return (andbound == rhs.andbound &&
				use == rhs.use &&
				section == rhs.section &&
				keytype == rhs.keytype &&
				upper == rhs.upper &&
				lower == rhs.lower);
			}

		// DocString: FMTBounds::operator<
		/**
		@brief Less than comparison operator of FMTBounds.
		@param[in] rhs the bounds to compare with.
		@return true if this bounds is less than rhs else false.
		*/
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

        // DocString: FMTBounds::operator=
        /**
        @brief Copy assignment operator for FMTBounds.
        @param[in] rhs the FMTBounds to copy.
        @return a reference to this FMTBounds.
        */
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
        // DocString: FMTBounds::empty
        /**
        @brief Return true if the bounds are empty (not used).
        @return true if the bounds are empty else false.
        */
        bool empty() const
            {
            return !use;
            }
        // DocString: FMTBounds::add
        /**
        @brief Add another bounds to this bounds.
        @param[in] rhs the bounds to add.
        @return true if the bounds are added else false.
        */
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
		// DocString: FMTBounds::toString
		/**
		@brief Return the string representation of the bounds for a given name.
		@param[in] name the name.
		@return the string representation of the bounds.
		*/
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

// DocString: FMTYldBounds
/**
@brief Yield bounds of a development, a FMTBounds of double with an associated yield name.
*/
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
	// DocString: FMTYldBounds()
	/**
	@brief Default constructor for FMTYldBounds.
	*/
	FMTYldBounds();
	// DocString: ~FMTYldBounds()
	/**
	@brief Default destructor for FMTYldBounds.
	*/
	~FMTYldBounds() = default;
	// DocString: FMTYldBounds(const FMTsection,const std::string&,const double&,const double&)
	/**
	@brief Construct yield bounds from a section, a yield, an upper and a lower bound.
	@param[in] lsection the section.
	@param[in] lyield the yield.
	@param[in] lupper the upper bound.
	@param[in] llower the lower bound.
	*/
	FMTYldBounds(const FMTsection lsection, const std::string& lyield, const double& lupper, const double& llower);
    // DocString: FMTYldBounds(const FMTsection,const FMTkwor,const std::string&,const double&,const double&)
    /**
    @brief Construct yield bounds from a section, a keyword, a yield, an upper and a lower bound.
    @param[in] lsection the section.
    @param[in] key the keyword.
    @param[in] lyield the yield.
    @param[in] lupper the upper bound.
    @param[in] llower the lower bound.
    */
    FMTYldBounds(const FMTsection lsection,const FMTkwor key,const std::string& lyield, const double& lupper,const double& llower);
    // DocString: FMTYldBounds(const std::string&,const FMTBounds<double>&)
    /**
    @brief Construct yield bounds from a yield and a FMTBounds.
    @param[in] lyield the yield.
    @param[in] rhs the bounds.
    */
    FMTYldBounds(const std::string& lyield,const FMTBounds<double>& rhs);
	// DocString: FMTYldBounds(const FMTYldBounds&)
	/**
	@brief Copy constructor for FMTYldBounds.
	@param[in] rhs the FMTYldBounds to copy.
	*/
	FMTYldBounds(const FMTYldBounds& rhs);
    // DocString: FMTYldBounds::operator=
    /**
    @brief Copy assignment operator for FMTYldBounds.
    @param[in] rhs the FMTYldBounds to copy.
    @return a reference to this FMTYldBounds.
    */
    FMTYldBounds& operator = (const FMTYldBounds& rhs);
	// DocString: FMTYldBounds::operator==
	/**
	@brief Equality comparison operator of FMTYldBounds.
	@param[in] rhs the bounds to compare with.
	@return true if both bounds are equal else false.
	*/
	bool operator == (const FMTYldBounds& rhs) const;
    // DocString: FMTYldBounds::operator std::string
    /**
    @brief Return the string representation of the yld bounds.
    @return the string representation of the bounds.
    */
    operator std::string() const;
    };

// DocString: FMTAgeBounds
/**
@brief Age bounds of a development, a FMTBounds of int.
*/
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
    // DocString: FMTAgeBounds()
    /**
    @brief Default constructor for FMTAgeBounds.
    */
    FMTAgeBounds();
	// DocString: ~FMTAgeBounds()
	/**
	@brief Default destructor for FMTAgeBounds.
	*/
	~FMTAgeBounds() = default;
    // DocString: FMTAgeBounds(FMTsection,const int&,const int&)
    /**
    @brief Construct age bounds from a section, an upper and a lower bound.
    @param[in] lsection the section.
    @param[in] lupper the upper bound.
    @param[in] llower the lower bound.
    */
    FMTAgeBounds(FMTsection lsection,const int& lupper, const int& llower);
    // DocString: FMTAgeBounds(FMTsection,FMTkwor,const int&,const int&)
    /**
    @brief Construct age bounds from a section, a keyword, an upper and a lower bound.
    @param[in] lsection the section.
    @param[in] key the keyword.
    @param[in] lupper the upper bound.
    @param[in] llower the lower bound.
    */
    FMTAgeBounds(FMTsection lsection,FMTkwor key, const int& lupper, const int& llower);
    // DocString: FMTAgeBounds(const FMTAgeBounds&)
    /**
    @brief Copy constructor for FMTAgeBounds.
    @param[in] rhs the FMTAgeBounds to copy.
    */
    FMTAgeBounds(const FMTAgeBounds& rhs);
    // DocString: FMTAgeBounds(const FMTBounds<int>&)
    /**
    @brief Construct age bounds from a FMTBounds.
    @param[in] rhs the bounds.
    */
    FMTAgeBounds(const FMTBounds<int>& rhs);
    // DocString: FMTAgeBounds::operator=
    /**
    @brief Copy assignment operator for FMTAgeBounds.
    @param[in] rhs the FMTAgeBounds to copy.
    @return a reference to this FMTAgeBounds.
    */
    FMTAgeBounds& operator = (const FMTAgeBounds& rhs);
	// DocString: FMTAgeBounds::operator==
	/**
	@brief Equality comparison operator of FMTAgeBounds.
	@param[in] rhs the bounds to compare with.
	@return true if both bounds are equal else false.
	*/
	bool operator == (const FMTAgeBounds& rhs) const;
    // DocString: FMTAgeBounds::operator std::string
    /**
    @brief Return the string representation of the age bounds.
    @return the string representation of the bounds.
    */
    operator std::string() const;
    };

// DocString: FMTPerBounds
/**
@brief Period bounds of a development, a FMTBounds of int.
*/
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
    // DocString: FMTPerBounds()
    /**
    @brief Default constructor for FMTPerBounds.
    */
    FMTPerBounds();
	// DocString: ~FMTPerBounds()
	/**
	@brief Default destructor for FMTPerBounds.
	*/
	~FMTPerBounds() = default;
    // DocString: FMTPerBounds(const FMTsection,const int&,const int&)
    /**
    @brief Construct period bounds from a section, an upper and a lower bound.
    @param[in] lsection the section.
    @param[in] lupper the upper bound.
    @param[in] llower the lower bound.
    */
    FMTPerBounds(const FMTsection lsection,const int& lupper,const int& llower);
    // DocString: FMTPerBounds(const FMTPerBounds&)
    /**
    @brief Copy constructor for FMTPerBounds.
    @param[in] rhs the FMTPerBounds to copy.
    */
    FMTPerBounds(const FMTPerBounds& rhs);
	// DocString: FMTPerBounds(const FMTBounds<int>&)
	/**
	@brief Construct period bounds from a FMTBounds.
	@param[in] rhs the bounds.
	*/
	FMTPerBounds(const FMTBounds<int>& rhs);
    // DocString: FMTPerBounds::operator=
    /**
    @brief Copy assignment operator for FMTPerBounds.
    @param[in] rhs the FMTPerBounds to copy.
    @return a reference to this FMTPerBounds.
    */
    FMTPerBounds& operator = (const FMTPerBounds& rhs);
	// DocString: FMTPerBounds::operator==
	/**
	@brief Equality comparison operator of FMTPerBounds.
	@param[in] rhs the bounds to compare with.
	@return true if both bounds are equal else false.
	*/
	bool operator == (const FMTPerBounds& rhs) const;
    // DocString: FMTPerBounds::operator std::string
    /**
    @brief Return the string representation of the per bounds.
    @return the string representation of the bounds.
    */
    operator std::string() const;
    };


// DocString: FMTLockBounds
/**
@brief Lock bounds of a development, a FMTBounds of int.
*/
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
    // DocString: FMTLockBounds()
    /**
    @brief Default constructor for FMTLockBounds.
    */
    FMTLockBounds();
	// DocString: ~FMTLockBounds()
	/**
	@brief Default destructor for FMTLockBounds.
	*/
	~FMTLockBounds() = default;
    // DocString: FMTLockBounds(const FMTsection,const FMTkwor,const int&,const int&)
    /**
    @brief Construct lock bounds from a section, a keyword, an upper and a lower bound.
    @param[in] lsection the section.
    @param[in] key the keyword.
    @param[in] lupper the upper bound.
    @param[in] llower the lower bound.
    */
    FMTLockBounds(const FMTsection lsection,const FMTkwor key,const int& lupper, const int& llower);
	// DocString: FMTLockBounds(const FMTsection,const int&,const int&)
	/**
	@brief Construct lock bounds from a section, an upper and a lower bound.
	@param[in] lsection the section.
	@param[in] lupper the upper bound.
	@param[in] llower the lower bound.
	*/
	FMTLockBounds(const FMTsection lsection, const int& lupper, const int& llower);
    // DocString: FMTLockBounds(const FMTLockBounds&)
    /**
    @brief Copy constructor for FMTLockBounds.
    @param[in] rhs the FMTLockBounds to copy.
    */
    FMTLockBounds(const FMTLockBounds& rhs);
    // DocString: FMTLockBounds::operator=
    /**
    @brief Copy assignment operator for FMTLockBounds.
    @param[in] rhs the FMTLockBounds to copy.
    @return a reference to this FMTLockBounds.
    */
    FMTLockBounds& operator = (const FMTLockBounds& rhs);
	// DocString: FMTLockBounds::operator==
	/**
	@brief Equality comparison operator of FMTLockBounds.
	@param[in] rhs the bounds to compare with.
	@return true if both bounds are equal else false.
	*/
	bool operator == (const FMTLockBounds& rhs) const;
    // DocString: FMTLockBounds::operator std::string
    /**
    @brief Return the string representation of the lock bounds.
    @return the string representation of the bounds.
    */
    operator std::string() const;
    };

class FMTYields;
// DocString: FMTSpec
/**
@brief Specifications of a group of developments, characterized by period, age, lock and yield bounds.
@details Example: 1 <= age <= 10, 5 <= period <= 20, 0 <= lock <= 10 and 145 <= yield <= 700.
*/
class FMTEXPORT FMTSpec
    {
	// DocString: FMTSpec::serialize
	/**
	@brief Serialize the FMTSpec for multiprocessing across multiple cpus (pickle in Python).
	@tparam Archive the archive type.
	@param[in,out] ar the archive to serialize to or from.
	@param[in] version the serialization version.
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
	@brief Default constructor for FMTSpec.
	*/
    FMTSpec();
	// DocString: ~FMTSpec()
	/**
	@brief Default virtual destructor for FMTSpec.
	*/
    virtual ~FMTSpec()=default;
	// DocString: FMTSpec(const FMTSpec&)
	/**
	@brief Copy constructor for FMTSpec.
	@param[in] rhs the FMTSpec to copy.
	*/
    FMTSpec(const FMTSpec& rhs);
	// DocString: FMTSpec::operator=
	/**
	@brief Copy assignment operator for FMTSpec.
	@param[in] rhs the FMTSpec to copy.
	@return a reference to this FMTSpec.
	*/
    FMTSpec& operator = (const FMTSpec& rhs);
	// DocString: FMTSpec::add
	/**
	@brief Add the yield, age, period or lock specifications from another spec.
	@param[in] rhs the other spec.
	@return true if the specifications are added else false.
	*/
    bool add(const FMTSpec& rhs);
	// DocString: FMTSpec::setBounds
	/**
	@brief Set the period bounds.
	@param[in] bound the period bounds.
	@return true if the bounds are set else false.
	*/
    bool setBounds(const FMTPerBounds& bound);
	// DocString: FMTSpec::setBounds
	/**
	@brief Add the age bounds.
	@param[in] bound the age bounds.
	@return true if the bounds are added else false.
	*/
    bool addBounds(const FMTAgeBounds& bound);
	// DocString: FMTSpec::setyldbounds
	/**
	@brief Add the yield bounds.
	@param[in] bound the yield bounds.
	@return true if the bounds are added else false.
	*/
    bool addBounds(const FMTYldBounds& bound);
	// DocString: FMTSpec::setlockbounds
	/**
	@brief Add the lock bounds.
	@param[in] bound the lock bounds.
	@return true if the bounds are added else false.
	*/
    bool addBounds(const FMTLockBounds& bound);
	// DocString: FMTSpec::allowWithoutYield
	/**
	@brief Return true if the period, age and lock respect their bounds.
	@param[in] tperiod the period.
	@param[in] tage the age.
	@param[in] tlock the lock.
	@return true if allowed else false.
	*/
	inline bool allowWithoutYield(const int& tperiod, const int& tage, const int& tlock) const
		{
		return (per.in(tperiod) &&
			age.in(tage) &&
			(lock.empty() || (tlock >= lock.lower)));
		}
	// DocString: FMTSpec::getYieldBound
	/**
	@brief Return the reference of a yield bound by its name.
	@param[in] name the yield name.
	@return a const reference to the yield bound.
	*/
	inline const FMTYldBounds& getYieldBound(const std::string& name) const
		{
		return yieldbounds.at(std::distance(yieldnames.begin(), std::find(yieldnames.begin(), yieldnames.end(), name)));
		}
	// DocString: FMTSpec::allowYields
	/**
	@brief Return true if all the yields are within the yield bounds.
	@param[in] values the yield values.
	@return true if the yields are allowed else false.
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
	@brief Return true if the period, age and lock are within their bounds and the yields are within the yield bounds.
	@param[in] tperiod the period.
	@param[in] tage the age.
	@param[in] tlock the lock.
	@param[in] values the yield values.
	@return true if allowed else false.
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
	@brief Return the yield names of the yield bounds.
	@return the yield names.
	*/
	inline const std::vector<std::string>& getYlds() const
		{
		return yieldnames;
		}
	// DocString: FMTSpec::getYldBounds
	/**
	@brief Return the yield bounds of the spec.
	@return the yield bounds.
	*/
	inline const std::vector<FMTYldBounds>& getYldBounds() const
		{
		return yieldbounds;
		}
	// DocString: FMTSpec::operator std::string
	/**
	@brief Return the string representation of the spec.
	@return the string representation of the spec.
	*/
    virtual operator std::string() const;
	// DocString: FMTSpec::operator==
	/**
	@brief Equality comparison operator of FMTSpec.
	@param[in] rhs the spec to compare with.
	@return true if both specs are equal else false.
	*/
	bool operator == (const FMTSpec& rhs) const;
	// DocString: FMTSpec::operator<
	/**
	@brief Less than comparison operator of FMTSpec.
	@param[in] rhs the spec to compare with.
	@return true if this spec is less than rhs else false.
	*/
	bool operator < (const FMTSpec& rhs) const;
	// DocString: FMTSpec::hash
	/**
	@brief Return the hash of the spec.
	@return the hash value.
	*/
	size_t hash() const;
	// DocString: FMTSpec::empty
	/**
	@brief Return true if the spec (all bounds) is empty.
	@return true if the spec (all bounds) is empty else false.
	*/
    bool empty() const;
	// DocString: FMTSpec::emptyAge
	/**
	@brief Return true if the age bound is empty.
	@return true if the age bound is empty else false.
	*/
	bool emptyAge() const;
	// DocString: FMTSpec::emptyYlds
	/**
	@brief Return true if the yield bounds is empty.
	@return true if the yield bounds is empty else false.
	*/
	bool emptyYlds() const;
	// DocString: FMTSpec::emptyPeriod
	/**
	@brief Return true if the period bound is empty.
	@return true if the period bound is empty else false.
	*/
	bool emptyPeriod() const;
	// DocString: FMTSpec::emptyLock
	/**
	@brief Return true if the lock bound is empty.
	@return true if the lock bound is empty else false.
	*/
	bool emptyLock() const;
	// DocString: FMTSpec::getAgeUpperBound
	/**
	@brief Return the upper bound of the age bounds.
	@return the age upper bound.
	*/
	int getAgeUpperBound() const;
	// DocString: FMTSpec::getAgeLowerBound
	/**
	@brief Return the lower bound of the age bounds.
	@return the age lower bound.
	*/
	int getAgeLowerBound() const;
	// DocString: FMTSpec::getPeriodUpperBound
	/**
	@brief Return the upper bound of the period bounds.
	@return the period upper bound.
	*/
	int getPeriodUpperBound() const;
	// DocString: FMTSpec::getPeriodLowerBound
	/**
	@brief Return the lower bound of the period bounds.
	@return the period lower bound.
	*/
	int getPeriodLowerBound() const;
	// DocString: FMTSpec::getLockUpperBound
	/**
	@brief Return the upper bound of the lock bounds.
	@return the lock upper bound.
	*/
	int getLockUpperBound() const;
	// DocString: FMTSpec::getLockLowerBound
	/**
	@brief Return the lower bound of the lock bounds.
	@return the lock lower bound.
	*/
	int getLockLowerBound() const;
	// DocString: FMTSpec::isSubsetOf
	/**
	@brief Return true if this spec is a subset of another.
	@param[in] rhs the other spec.
	@return true if this spec is a subset of rhs else false.
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
