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

#ifndef FMTFIRE_H_INCLUDED
#define FMTFIRE_H_INCLUDED


#include <random>
#include <vector>
#include "FMTcoordinate.h"
#include "FMTevent.h"
#include <boost/serialization/serialization.hpp>


namespace Spatial
{

class FMTfire : public FMTevent<FMTdevelopment>
    {
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & boost::serialization::make_nvp("event", boost::serialization::base_object<FMTevent<Core::FMTdevelopment>>(*this));
	}
    protected:
		std::normal_distribution<unsigned int>spread_distribution;
		std::weibull_distribution<unsigned int>size_distribuion;
    public:
    FMTfire():FMTevent<Core::FMTdevelopment>(){};
    FMTfire& operator = (const FMTfire& rhs){};
    FMTfire(const FMTfire& rhs):FMTevent<Core::FMTdevelopment>(rhs),
    spread_distribution(rhs.spread_distribution),
    size_distribuion(rhs.size_distribuion){}
    };
}

#endif // FMTFIRE_H_INCLUDED
