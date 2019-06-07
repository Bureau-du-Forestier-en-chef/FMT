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
