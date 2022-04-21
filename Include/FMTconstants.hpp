/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTCON_H_INCLUDED
#define FMTCON_H_INCLUDED

#include <boost/unordered_map.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>
#include <vector>
#include <string>
#include "FMTobject.hpp"

namespace Core
{
// DocString: FMTconstants
/**
FMTconstants is a class only used by the FMTparsers.
When a model is read sometime the user uses constants defined in the constants section.
The constant is represented by a string in this section and keeps double values.
*/
class FMTEXPORT FMTconstants : public FMTobject
    {
	// DocString: FMTconstants::serialize
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
			_exhandler->printexceptions("", "FMTconstants::serialize", __LINE__, __FILE__);
			}
		}
	// DocString: FMTconstants::data
	///This unordered_map keeps uses the constant definition string has key and the double value has elements.
    boost::unordered_map<std::string,std::vector<double>>data;
    public:
	// DocString: FMTconstants()
	/**
	Default constructor for FMTconstants.
	*/
    FMTconstants();
	// DocString: ~FMTconstants()
	/**
	Default destructor for FMTconstants.
	*/
	~FMTconstants()=default;
	// DocString: FMTconstants(const FMTconstants&)
	/**
	Default copy constructor for FMTconstants.
	*/
    FMTconstants(const FMTconstants& rhs);
	// DocString: FMTconstants::set
	/**
	For a given constant (key) the function sets a (values) vector in the data unordered_map.
	*/
    void set(const std::string& key, std::vector<double>values);
	// DocString: FMTconstants::get
	/**
	For a given constant (key) the function gets a value for a given (period)
	*/
    template<typename T>
	T get(std::string key, int period = 0) const;
	// DocString: FMTconstants::getall
	/**
	For a given constant (key) the function gets all the values of the constant.
	*/
    template<typename T>
	std::vector<T>getall(std::string key) const;
	// DocString: FMTconstants::isconstant
	/**
	Returns true if the (value) is considered a constant.
	*/
    bool isconstant(std::string value) const;
	// DocString: FMTconstants::length
	/**
	Returns the length of the constant (value), which is the number of double values kept by the constant.
	*/
    size_t length(std::string value) const;
	// DocString: FMTconstants::operator=
	/**
	Copy assignment operator for FMTconstants.
	*/
    FMTconstants& operator = (const FMTconstants& rhs);
	// DocString: FMTconstants::operator std::string
	/**
	Returns the FMTconstants as a constant section (.con) in a string.
	*/
    operator std::string() const;
    };
}

BOOST_CLASS_EXPORT_KEY(Core::FMTconstants)

#endif // FMTCON_H_INCLUDED
