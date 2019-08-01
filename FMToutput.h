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
	FMToutput boundto(const vector<FMTtheme>& themes, const FMTperbounds& bound,string attribute = "") const;
	vector<FMToutputnode> getnodes() const;
	bool issingleperiod() const;
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
