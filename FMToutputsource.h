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
   //double value;
    vector<double>values;
	/*vector<boost::dynamic_bitset<>> actionscombination(const boost::dynamic_bitset<>& actioned) const;
	vector<boost::dynamic_bitset<>> linearcombination(const int& minclass, const int& maxclass,const int& lenght) const;
	boost::dynamic_bitset<> actionbit(const vector<FMTaction>& actions,
		const map<string, vector<string>>& aggregates) const;*/
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
		//size_t hash() const override;
		size_t hash(int period = -1, size_t typeofout = 0) const;
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
