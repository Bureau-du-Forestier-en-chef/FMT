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
@brief Class holding the constants defined in the constants section of a model, used by the parsers.
@details A constant is represented by a string and holds double values.
*/
class FMTEXPORT FMTConstants : public FMTObject
    {
	// DocString: FMTConstants::serialize
	/**
	@brief Serialize the FMTConstants for multiprocessing across multiple cpus (pickle in Python).
	@tparam Archive the archive type.
	@param[in,out] ar the archive to serialize to or from.
	@param[in] version the serialization version.
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
	@brief Default constructor for FMTConstants.
	*/
    FMTConstants();
	// DocString: ~FMTConstants()
	/**
	@brief Default destructor for FMTConstants.
	*/
	~FMTConstants()=default;
	// DocString: FMTConstants(const FMTConstants&)
	/**
	@brief Copy constructor for FMTConstants.
	@param[in] rhs the FMTConstants to copy.
	*/
    FMTConstants(const FMTConstants& rhs);
	// DocString: FMTConstants::set
	/**
	@brief Set the values of a given constant.
	@param[in] key the constant definition string.
	@param[in] values the values of the constant.
	*/
    void set(const std::string& key, std::vector<double>values);
	// DocString: FMTConstants::get
	/**
	@brief Get the value of a given constant for a given period.
	@tparam T the value type.
	@param[in] key the constant definition string.
	@param[in] period the period.
	@return the value of the constant for the period.
	*/
    template<typename T>
	T get(std::string key, int period = 0) const;
	// DocString: FMTConstants::getAll
	/**
	@brief Get all the values of a given constant.
	@tparam T the value type.
	@param[in] key the constant definition string.
	@return all the values of the constant.
	*/
    template<typename T>
	std::vector<T>getAll(std::string key) const;
	// DocString: FMTConstants::isConstant
	/**
	@brief Return true if the given value is considered a constant.
	@param[in] value the value to check.
	@return true if the value is a constant else false.
	*/
    bool isConstant(std::string value) const;
	// DocString: FMTConstants::length
	/**
	@brief Return the number of double values kept by a given constant.
	@param[in] value the constant definition string.
	@return the number of values of the constant.
	*/
    size_t length(std::string value) const;
	// DocString: FMTConstants::operator=
	/**
	@brief Copy assignment operator for FMTConstants.
	@param[in] rhs the FMTConstants to copy.
	@return a reference to this FMTConstants.
	*/
    FMTConstants& operator = (const FMTConstants& rhs);
	// DocString: FMTConstants::operator std::string
	/**
	@brief Return the constants as a constant section (.con) string.
	@return the string representation of the constants.
	*/
    operator std::string() const;
    };
}

BOOST_CLASS_EXPORT_KEY(Core::FMTConstants)

#endif // FMTCON_Hm_included
