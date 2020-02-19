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
#include "FMTbounds.h"
#include <boost/serialization/serialization.hpp>


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
	std::vector<FMToutputsource>sources;
	std::vector<FMToperator>operators;
	int theme_target;
    public:
		std::string name,description;
    FMToutput();
    virtual ~FMToutput()=default;
    FMToutput(const std::string& lname,const std::string& ldescription,
		const int& ltheme_target, std::vector<FMToutputsource>& lsources, std::vector<FMToperator>& loperators);
    FMToutput(const std::string& lname);
    FMToutput(const FMToutput& rhs);
    FMToutput& operator = (const FMToutput& rhs);
    bool operator == (const FMToutput& rhs) const;
    bool operator != (const FMToutput& rhs) const;
	FMToutput& operator +=(const FMToutput& rhs);
	FMToutput& operator -=(const FMToutput& rhs);
	FMToutput& operator *=(const double& rhs);
	FMToutput& operator /=(const double& rhs);
    operator std::string() const;
	bool empty() const;
	size_t size() const;
	bool linear() const;
	bool islevel() const;
	bool isconstant() const;
	double getconstantvalue() const;
	bool containslevel() const;
	double shuntingyard(const std::vector<double>& sourcevalues, const std::vector<FMToperator>& simple_operators) const;
	FMToutput boundto(const std::vector<FMTtheme>& themes, const FMTperbounds& bound,const std::string& specialbound, std::string attribute = "") const;
	std::vector<FMToutputnode> getnodes(double multiplier = 1) const;
	bool issingleperiod() const;
	bool hasaverage() const;
	int gettargetperiod() const;
	size_t hash() const;
	std::vector<FMToutputsource> getsources() const;
	std::vector<FMToperator> getopes() const;
	FMTtheme targettheme(const std::vector<FMTtheme>& themes) const;
	int targetthemeid() const;
	std::vector<std::string>getdecomposition(const std::vector<FMTtheme>& themes) const;
	FMToutput presolve(const FMTmask& basemask,
		const std::vector<FMTtheme>& originalthemes,
		const FMTmask& presolvedmask,
		const std::vector<FMTtheme>& newthemes,
		const std::vector<FMTaction>& actions,const FMTyields& yields) const;
    };

class FMToutputcomparator
{
	std::string output_name;
public:
	FMToutputcomparator(std::string name);
	bool operator()(const FMToutput& output) const;

};

}


#endif // FMToutput_H_INCLUDED
