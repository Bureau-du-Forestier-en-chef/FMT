#ifndef FMTconstraint_H_INCLUDED
#define FMTconstraint_H_INCLUDED

#include "FMToutput.h"

#include <vector>
#include <string>
#include "FMToperator.h"
#include "FMTbounds.h"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
//#include <CoinFinite.hpp>

using namespace Core;
using namespace std;

namespace Core
{

enum  FMTconstrainttype
{
	FMTMAXobjective = 1,
	FMTMINobjective = 2,
	FMTMAXMINobjective = 3,
	FMTMINMAXobjective = 4,
	FMTevenflow = 5,
	FMTnondeclining = 6,
	FMTsequence = 7,
	FMTstandard = 8,
	FMTspatialsize = 9,
	FMTspatialadjacency = 10,
	FMTspatialgreenup = 11
};




class FMTconstraint: public FMToutput,public FMTspec
	{
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & boost::serialization::make_nvp("output", boost::serialization::base_object<FMToutput>(*this));
		ar & boost::serialization::make_nvp("specification", boost::serialization::base_object<FMTspec>(*this));
		ar & BOOST_SERIALIZATION_NVP(type);
	}
	FMTconstrainttype type;
	public:
		FMTconstraint();
		FMTconstraint(FMTconstrainttype ltype, const FMToutput& loutput);
		FMTconstraint(const FMTconstraint& rhs);
		FMTconstraint& operator = (const FMTconstraint& rhs);
		bool operator == (const FMTconstraint& rhs) const;
		bool operator != (const FMTconstraint& rhs) const;
		bool extravariables() const;
		bool isgoal() const;
		vector<string>getpenalties(double & sense) const;
		vector<string>getvariablelevels() const;
		void getgoal(string& name,double& value) const;
		bool isobjective() const;
		double sense() const;
		void getbounds(double& lower, double& upper,int period = 0) const;
		void getvariations(double& lower, double& upper) const;
		bool ismultiple() const;
		bool acrossperiod() const;
		size_t hash() const;
		void setoutput(const FMToutput& out);
		void setconstrainttype(FMTconstrainttype ltype);
		FMTconstrainttype getconstrainttype() const;
		operator string() const;
		standardstring(string& line, string& period_bounds, string& goal) const;
		~FMTconstraint()=default;
	};
}


#endif
