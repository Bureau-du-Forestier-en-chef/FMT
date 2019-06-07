#ifndef FMTCUT_H_INCLUDED
#define FMTCUT_H_INCLUDED

#include "FMTcoordinate.h"
#include "FMTevent.h"
#include "FMTdevelopment.h"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>

using namespace Core;

namespace Spatial
{

class FMTcut : public FMTevent<FMTdevelopment>
    {
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & boost::serialization::make_nvp("event", boost::serialization::base_object<FMTevent<FMTdevelopment>>(*this));
	}
    public:
        FMTcut();
        ~FMTcut()=default;
        FMTcut(const map<FMTcoordinate,FMTdevelopment>& lterritory,const int& pass);
        FMTcut(const FMTcut& rhs);
        FMTcut& operator = (const FMTcut& rhs);
    };

}


#endif // FMTCUT_H_INCLUDED
