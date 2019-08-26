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

#ifndef FMTOSR_H_INCLUDED
#define FMTOSR_H_INCLUDED

#include "FMTbounds.h"
#include "FMTmask.h"
#include "FMTaction.h"
#include "FMTyields.h"
#include <vector>
#include "FMTdevelopment.h"
#include <boost/serialization/serialization.hpp>

#include <string>

namespace Core
{

class FMToutputsource : public FMTspec
    {
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & boost::serialization::make_nvp("specification", boost::serialization::base_object<FMTspec>(*this));
		ar & BOOST_SERIALIZATION_NVP(mask);
		ar & BOOST_SERIALIZATION_NVP(target);
		ar & BOOST_SERIALIZATION_NVP(action);
		ar & BOOST_SERIALIZATION_NVP(yield);
		//ar & BOOST_SERIALIZATION_NVP(value);
		ar & BOOST_SERIALIZATION_NVP(values);
	}
    FMTmask mask;
    FMTotar target;
    string action;
    string yield;
    vector<double>values;
	bool average;
    public:
		FMToutputsource();
        FMToutputsource(const FMTotar ltarget,double lvalue = 0, string lyield= "",string laction = "");
        FMToutputsource(const FMTotar ltarget,vector<double>lvalues);
        FMToutputsource(const FMTspec& spec,const FMTmask& lmask,
               const FMTotar ltarget,string lyield= "",
               string laction = "");
        FMToutputsource(const FMToutputsource& rhs);
        FMToutputsource& operator = (const FMToutputsource& rhs);
        operator string() const override;
		bool operator == (const FMToutputsource& rhs) const;
		bool operator != (const FMToutputsource& rhs) const;
		bool issubsetof(const FMToutputsource& rhs,
			const map<string,vector<string>>& actaggregates) const;
		const FMTmask& getmask() const;
		void setmask(const FMTmask& newmask);
		string getaction() const;
		string getyield() const;
		FMTotar gettarget() const;
		double getvalue(int period = 0) const;
		bool isvariable() const;
		bool islevel() const;
		bool isconstant() const;
		bool isvariablelevel() const;
		string getlevel() const;
		bool isnull(const FMTyields& ylds) const;
		bool istimeyield() const;
		vector<int>targets(const vector<FMTaction>& actions,
			const map<string, vector<string>>& aggregates) const;
		bool useinedges() const;
		bool useoutedges() const;
		double getcoef(const FMTdevelopment& development,
			const FMTyields& yields, const FMTaction& action,
			const vector<FMTdevelopmentpath>& paths) const;
		bool use(const FMTdevelopment& development, const FMTyields& ylds) const;
		void setaverage();
		bool isaverage() const;
		size_t hash(int period = -1) const;
		/*vector<boost::dynamic_bitset<>> getclassifiers(const vector<FMTaction>& actions,
			const map<string, vector<string>>& aggregates,
			const int& maxage, const int& maxperiod,const int& period) const;*/
    };

class FMToutputsourcecomparator
{
	bool variable;
public:
	FMToutputsourcecomparator(bool lvariable);
	bool operator()(const FMToutputsource& source) const;

};

}



#endif // FMTOSR_H_INCLUDED
