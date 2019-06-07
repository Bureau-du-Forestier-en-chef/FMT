#ifndef FMTDATA_H_INCLUDED
#define FMTDATA_H_INCLUDED

#include <vector>
#include <string>
//#include <unordered_map>
#include "FMToperator.h"
#include "FMTmask.h"
#include "FMTexpression.h"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/binary_object.hpp>
#include "FMTutility.h"

using namespace std;
using namespace boost;

namespace Core
{


class FMTdata
    {
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & BOOST_SERIALIZATION_NVP(ops);
		ar & BOOST_SERIALIZATION_NVP(source);
		ar & BOOST_SERIALIZATION_NVP(stacking);
		ar & BOOST_SERIALIZATION_NVP(data);
		/*ar & BOOST_SERIALIZATION_NVP(_cache);
		ar & BOOST_SERIALIZATION_NVP(_agebase);*/
	}
    FMTyieldparserop ops;
    vector<string>source;
	vector<bool>stacking;//Stacking a string = true stacking a number = false
	mutable map<size_t,double> _cache; //hash base on (age and/or period and/or mask) //only for complex yield!!!!
	mutable bool _agebase;
	size_t hashdata(const int& period, const int& age, const FMTmask& mask) const;
    public:
        vector<double> data;
        FMTdata();
        FMTdata(const FMTdata& rhs);
		FMTdata(const vector<double>& lvalues,
			const FMTyieldparserop& lops,
			const vector<string>& lsource);
        FMTdata(const vector<double>& lvalues,
                const FMTyieldparserop& lops,
                const vector<string>& lsource,
				const vector<bool>& varstack);
        FMTdata& operator = (const FMTdata& rhs);
        FMTyieldparserop getop() const;
		bool constant() const;
		bool nulldata() const;
		bool cachevalue(const FMTmask& mask, const int& age, const int& period) const;
		double get(const FMTmask& mask, const int& age, const int& period) const;
		void set(const double& value, const FMTmask& mask, const int& age, const int& period, const bool& age_only) const;
        vector<string> getsource() const;
		vector<double>tovalues(const map<string, double>& sources) const;
		FMTexpression toexpression() const;
		bool operator == (const FMTdata& rhs) const;
        operator string() const;
    };
}


#endif // FMTDATA_H_INCLUDED
