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

#ifndef FMTYLDDATA_H_INCLUDED
#define FMTYLDDATA_H_INCLUDED

#include <string>
#include <map>
#include <algorithm>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include "FMTdata.h"
#include "FMTmask.h"

using namespace std;

namespace Core
{

class FMTyieldhandler
    {
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{

		ar & BOOST_SERIALIZATION_NVP(yldtype);
		ar & BOOST_SERIALIZATION_NVP(mask);
		ar & BOOST_SERIALIZATION_NVP(bases);
		ar & BOOST_SERIALIZATION_NVP(elements);
	}
        FMTyldwstype yldtype;
        FMTmask mask;
        vector<int>bases;

    public:
        map<string,FMTdata>elements;
        operator string() const;
		FMTyieldhandler();
        FMTyieldhandler(FMTyldwstype ltype,const FMTmask& lmask);
        FMTyieldhandler(const FMTyieldhandler& rhs);
        FMTyieldhandler& operator = (const FMTyieldhandler& rhs);
        vector<string> compare(const vector<string>& keys) const;
        bool push_base(const int& base);
        bool push_data(const string& yld,const double& value);
        bool push_data(const string& yld,const FMTdata& data);
        //vector<FMTyieldhandler>decomposeindexes(const vector<FMTtheme>& themes) const;
        vector<string> indexes(const vector<string>& names) const;
        double get(const vector<const FMTyieldhandler*>& datas,
                   const string yld,const int& age,const int& period, const FMTmask& resume_mask) const;
		int getlastbase() const;
		const vector<int>& getbases() const;
		double getlinearvalue(const vector<double>& dls, const int& agetarget) const;
		double getlastvalue(const string yld) const;
        map<string,double> getsources(const map<string, const FMTyieldhandler*>& srcdata, const vector<const FMTyieldhandler*>& datas,
										const int& age, const int& period ,const FMTmask& resume_mask, bool& age_only) const;
        map<string,const FMTyieldhandler*> getdata(const vector<const FMTyieldhandler*>& datas,
                                    const vector<string>& names) const;
		bool operator == (const FMTyieldhandler& rhs) const;
        double getpeak(const string& yld, const int& targetage) const;
        int getage(const string yld, const double& value) const;
        map<string,FMTdata>getdataelements() const;
        FMTyldwstype gettype() const;
    };
}
#endif // FMTYLDDATA_H_INCLUDED
