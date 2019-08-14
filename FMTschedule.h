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

#ifndef FMTschedule_H_INCLUDED
#define FMTschedule_H_INCLUDED

#include <map>
#include "FMTdevelopment.h"
#include "FMTaction.h"


using namespace std;

namespace Core
{

class FMTschedule
    {
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & BOOST_SERIALIZATION_NVP(period);
		ar & BOOST_SERIALIZATION_NVP(elements);
	}
    int period;
		bool sameelements(const FMTschedule& rhs) const;
    public:
        map<FMTaction,map<FMTdevelopment,vector<double>>> elements;
        FMTschedule(int lperiod,map<FMTaction,map<FMTdevelopment,vector<double>>> mapping);
		FMTschedule(int lperiod, const map<FMTaction, map<FMTdevelopment, map<int,double>>>& mapping);
        FMTschedule();
        FMTschedule(const FMTschedule& rhs);
        FMTschedule& operator = (const FMTschedule& rhs);
		bool operator == (const FMTschedule& rhs) const;
		bool operator != (const FMTschedule& rhs) const;
		FMTschedule& operator += (const FMTschedule& rhs);
		FMTschedule operator + (const FMTschedule& rhs) const;
        map<FMTdevelopment,vector<double>>getfaction(const FMTaction& action) const;
        map<FMTaction,map<FMTdevelopment,vector<double>>>get() const;
        operator string() const;
        int getperiod() const;
        double actionarea(const FMTaction& action) const;
        double area() const;
		bool operated(const FMTaction& action,
			const FMTdevelopment& developement) const;
		void sort();
		bool empty() const;
    };


}

#endif // FMTschedule_H_INCLUDED
