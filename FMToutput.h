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

#ifndef FMToutput_H_INCLUDED
#define FMToutput_H_INCLUDED

#include <vector>
#include <string>
#include <stack>
#include "FMToutputsource.h"
#include "FMToperator.h"
#include "FMTexpression.h"
#include "FMTmaskfilter.h"
#include "FMToutputnode.h"
#include "FMTtheme.h"
#include <boost/serialization/serialization.hpp>

using namespace std;

namespace Core
{


class FMToutput
    {
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & BOOST_SERIALIZATION_NVP(sources);
		ar & BOOST_SERIALIZATION_NVP(operators);
		ar & BOOST_SERIALIZATION_NVP(theme_target);
		ar & BOOST_SERIALIZATION_NVP(name);
		ar & BOOST_SERIALIZATION_NVP(description);
	}
	protected:
    vector<FMToutputsource>sources;
    vector<FMToperator>operators;
	int theme_target;
	//bool level;
    public:
        string name,description;
    FMToutput();
    virtual ~FMToutput()=default;
    FMToutput(const string& lname,const string& ldescription,
		const int& ltheme_target, vector<FMToutputsource>& lsources,vector<FMToperator>& loperators/*,bool islevel = false*/);
    FMToutput(const string& lname);
    FMToutput(const FMToutput& rhs);
    FMToutput& operator = (const FMToutput& rhs);
    bool operator == (const FMToutput& rhs) const;
    bool operator != (const FMToutput& rhs) const;
	FMToutput& operator +=(const FMToutput& rhs);
	FMToutput& operator -=(const FMToutput& rhs);
	FMToutput& operator *=(const double& rhs);
	FMToutput& operator /=(const double& rhs);
    operator string() const;
	bool empty() const;
	bool linear() const;
	bool islevel() const;
	bool isconstant() const;
	double getconstantvalue() const;
	bool containslevel() const;
	double shuntingyard(const vector<double>& sourcevalues, const vector<FMToperator>& simple_operators) const;
	FMToutput boundto(const vector<FMTtheme>& themes, const FMTperbounds& bound,const string& specialbound,string attribute = "") const;
	vector<FMToutputnode> getnodes(double multiplier = 1) const;
	bool issingleperiod() const;
	bool hasaverage() const;
	int gettargetperiod() const;
	size_t hash() const;
    vector<FMToutputsource> getsources() const;
    vector<FMToperator> getopes() const;
	FMTtheme targettheme(const vector<FMTtheme>& themes) const;
	int targetthemeid() const;
	vector<string>getdecomposition(const vector<FMTtheme>& themes) const;
    };

class FMToutputcomparator
{
	string output_name;
public:
	FMToutputcomparator(string name);
	bool operator()(const FMToutput& output) const;

};

}


#endif // FMToutput_H_INCLUDED
