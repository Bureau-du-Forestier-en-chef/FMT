#ifndef FMTFORK_H_INCLUDED
#define FMTFORK_H_INCLUDED

#include "FMTtransitionmask.h"
#include "FMTbounds.h"
#include "FMTdevelopment.h"
#include "FMTdevelopmentpath.h"
#include "FMTyields.h"
#include <vector>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>

using namespace std;

namespace Core
{

class FMTdevelopment;
class FMTtransitionmask;
class FMTdevelopmentpath;

class FMTfork : public FMTspec
    {
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
		{
		ar & boost::serialization::make_nvp("specifications", boost::serialization::base_object<FMTspec>(*this));
		ar & BOOST_SERIALIZATION_NVP(transitions);
		}
        vector<FMTtransitionmask>transitions;
    public:
        FMTfork();
        FMTfork(const FMTfork& rhs);
        FMTfork& operator = (const FMTfork& rhs);
        void add(FMTtransitionmask& transition);
        //vector<FMTdevelopment>get(const FMTdevelopment& base,const FMTyields& ylds,const vector<FMTtheme>& themes) const;
		vector<FMTdevelopmentpath> getpaths(const FMTdevelopment& base, 
			const FMTyields& ylds, const vector<FMTtheme>& themes,const bool& reset_age) const;
		//FMTtransitionmask getmasktran(const FMTdevelopment& base,const FMTyields& ylds) const;
        FMTdevelopment getmax(const FMTdevelopment& base,const FMTyields& ylds,const vector<FMTtheme>& themes, const bool& reset_age) const;
        vector<FMTtransitionmask> getmasktrans() const;
		bool operator == (const FMTfork& rhs) const;
        FMTfork single() const;
        double sumprop() const;
        size_t size() const;
        operator string() const override;
    };

}

#endif // FMTFORK_H_INCLUDED
