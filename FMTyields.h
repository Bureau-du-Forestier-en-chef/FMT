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

#ifndef FMTYLD_H_INCLUDED
#define FMTYLD_H_INCLUDED

#include "FMTlist.h"
#include "FMTyieldhandler.h"
#include "FMTdevelopment.h"
#include "FMTbounds.h"
#include <vector>
#include <string>
#include <boost/serialization/serialization.hpp>

using namespace std;

namespace Core
{

class FMTdevelopment;

class FMTyields : public FMTlist<FMTyieldhandler>
    {
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & boost::serialization::make_nvp("handlers", boost::serialization::base_object<FMTlist<FMTyieldhandler>>(*this));
		ar & BOOST_SERIALIZATION_NVP(names);
		ar & BOOST_SERIALIZATION_NVP(null_names);
	}
    vector<string>names;
	vector<string>null_names;
	vector<const FMTyieldhandler*> gethandleroftype(FMTyldwstype type) const;
	int getmaxbase(const vector<const FMTyieldhandler*>& handlers) const;
    public:
        FMTyields();
        FMTyields(const FMTyields& rhs);
        FMTyields& operator = (const FMTyields& rhs);
        bool isyld(const string& value) const;
		bool isnullyld(const string& value) const;
        vector<string>getyldsnames() const;
		vector<string>getnullyldsnames() const;
		map<string, double>get(const FMTdevelopment& dev,
			const vector<string>& targets) const;
        map<string,double>getylds(const FMTdevelopment& dev,const FMTspec& spec) const;
        int getage(const FMTdevelopment& dev,const FMTspec& spec) const;
		map<string, map<string, vector<double>>>getallyields(const FMTtheme& target,FMTyldwstype type) const;
		bool operator == (const FMTyields& rhs) const;
        void update();
    vector<string>getstacked() const;
    };
}

#endif // FMTYLD_H_INCLUDED
