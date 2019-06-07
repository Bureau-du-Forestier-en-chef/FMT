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
		bool operator == (const FMTyields& rhs) const;
        void update();
    vector<string>getstacked() const;
    };
}

#endif // FMTYLD_H_INCLUDED
