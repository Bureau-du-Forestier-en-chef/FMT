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

#ifndef FMTCON_H_INCLUDED
#define FMTCON_H_INCLUDED

#include <boost/unordered_map.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <vector>
#include <string>

namespace Core
{
// DocString: FMTconstants
/**
FMTconstants is a class only used by the FMTparsers.
When a model is read sometime the user uses constants defined in the constants section.
The constant is represented by a string in this section and keeps double values.
*/
class FMTconstants
    {
	// DocString: FMTconstants::serialize
	/**
	Serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
	*/
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
		{
		ar & BOOST_SERIALIZATION_NVP(data);
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
    T get(std::string key,int period = 0) const
        {
		if (key.find("#") != std::string::npos)
			{
			key.erase(0, 1);
			}
		boost::unordered_map<std::string, std::vector<double>>::const_iterator it = data.find(key);
		std::vector<double> const* location = &it->second;
		if (period >= static_cast<int>(location->size()))
			{
			period = static_cast<int>(location->size()) - 1;
			}
		return static_cast<T>(location->at(period));
        }
	// DocString: FMTconstants::getall
	/**
	For a given constant (key) the function gets all the values of the constant.
	*/
    template<typename T>
	std::vector<T>getall(std::string key) const
        {
		std::vector<T>values;
		for (size_t period = 0; period < length(key); ++period)
			{
			values.push_back(get<T>(key, period));
			}
		return values;
        }
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

#endif // FMTCON_H_INCLUDED
