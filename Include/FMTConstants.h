/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTCON_Hm_included
#define FMTCON_Hm_included

#include <boost/unordered_map.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>
#include <vector>
#include <string>
#include "FMTObject.h"
#include "FMTExceptionHandler.h"

namespace Core
{
// DocString: FMTConstants
/**
FMTConstants is a class only used by the FMTparsers.
When a model is read sometime the user uses constants defined in the constants section.
The constant is represented by a string in this section and keeps double values.
*/
class FMTEXPORT FMTConstants : public FMTObject
    {
	// DocString: FMTConstants::serialize
	/**
	Serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
	*/
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
		{
		try{
			ar & BOOST_SERIALIZATION_NVP(data);
		}catch (...)
			{
			_exhandler->printExceptions("", "FMTConstants::serialize", __LINE__, __FILE__);
			}
		}
	// DocString: FMTConstants::data
	///This unordered_map keeps uses the constant definition string has key and the double value has elements.
    boost::unordered_map<std::string,std::vector<double>>data;
    public:
	// DocString: FMTConstants()
	/**
	Default constructor for FMTConstants.
	*/
    FMTConstants();
	// DocString: ~FMTConstants()
	/**
	Default destructor for FMTConstants.
	*/
	~FMTConstants()=default;
	// DocString: FMTConstants(const FMTConstants&)
	/**
	Default copy constructor for FMTConstants.
	*/
    FMTConstants(const FMTConstants& rhs);
	// DocString: FMTConstants::set
	/**
	For a given constant (key) the function sets a (values) vector in the data unordered_map.
	*/
    void set(const std::string& key, std::vector<double>values);
	// DocString: FMTConstants::get
	/**
	For a given constant (key) the function gets a value for a given (period)
	*/
    template<typename T>
	T get(std::string key, int period = 0) const;
	// DocString: FMTConstants::getAll
	/**
	For a given constant (key) the function gets all the values of the constant.
	*/
    template<typename T>
	std::vector<T>getAll(std::string key) const;
	// DocString: FMTConstants::isConstant
	/**
	Returns true if the (value) is considered a constant.
	*/
    bool isConstant(std::string value) const;
	// DocString: FMTConstants::length
	/**
	Returns the length of the constant (value), which is the number of double values kept by the constant.
	*/
    size_t length(std::string value) const;
	// DocString: FMTConstants::operator=
	/**
	Copy assignment operator for FMTConstants.
	*/
    FMTConstants& operator = (const FMTConstants& rhs);
	// DocString: FMTConstants::operator std::string
	/**
	Returns the FMTConstants as a constant section (.con) in a string.
	*/
    operator std::string() const;
    };
}

BOOST_CLASS_EXPORT_KEY(Core::FMTConstants)

#endif // FMTCON_Hm_included
