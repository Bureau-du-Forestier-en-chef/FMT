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

class FMTconstants
    {
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
		{
		ar & BOOST_SERIALIZATION_NVP(data);
		}
    boost::unordered_map<std::string,std::vector<std::string>>data;
    public:
        FMTconstants();
    FMTconstants(const FMTconstants& rhs);
    void set(std::string key, std::vector<std::string>values);
	std::string getstr(std::string key,int period = 0) const;
    template<typename T>
    T get(std::string key,int period = 0) const
        {
        T value;
        value = T(stod(getstr(key,period)));
        return value;
        }
    template<typename T>
	std::vector<T>getall(std::string key) const
        {
		std::vector<T>values;
        const size_t klength = length(key);
        for (size_t period = 0 ; period < klength; ++period)
            {
            values.push_back(T(stod(getstr(key,period))));
            }
        return values;
        }
    bool isconstant(std::string value) const;
    size_t length(std::string value) const;

    FMTconstants& operator = (const FMTconstants& rhs);
    operator std::string() const;
    };
}

#endif // FMTCON_H_INCLUDED
