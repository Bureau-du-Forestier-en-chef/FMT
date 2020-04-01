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

#ifndef FMTOPE_H_INCLUDED
#define FMTOPE_H_INCLUDED

#include "FMTutility.h"
#include <string>
#include <vector>
#include <algorithm>
#include <boost/functional/hash.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>



namespace Core
{

	enum class FMTokey
	{
		multiply = 1,
		divide = 2,
		add = 3,
		sub = 4,
		comma = 7,
		pow = 8,
		notvalid = 9
	};

class FMToperator
{
    FMTokey key;
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & BOOST_SERIALIZATION_NVP(key);
	}
public:
	FMToperator();
    FMToperator(FMTokey lkey);
    FMToperator(std::string strkey);
	bool operator == (const FMToperator& rhs) const;
    double call(const double& rhs1, const double& rhs2) const;
    FMToperator(const FMToperator& rhs);
    FMToperator& operator = (const FMToperator& rhs);
	int precedence() const;
	std::string associativity() const;
	FMToperator reverse() const;
	bool valid() const;
	size_t hash() const;
	bool isfactor() const;
	bool isdivide() const;
	FMTokey getkey() const;
    operator std::string() const;
};

}

#endif // FMTOPE_H_INCLUDED
