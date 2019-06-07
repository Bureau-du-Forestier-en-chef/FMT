#ifndef FMTFIRE_H_INCLUDED
#define FMTFIRE_H_INCLUDED


#include <random>
#include <vector>
#include "FMTcoordinate.h"
#include "FMTevent.h"
#include <boost/serialization/serialization.hpp>

using namespace std;


namespace Spatial
{

class FMTfire : public FMTevent<FMTdevelopment>
    {
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & boost::serialization::make_nvp("event", boost::serialization::base_object<FMTevent<FMTdevelopment>>(*this));
	}
    protected:
    normal_distribution<unsigned int>spread_distribution;
    weibull_distribution<unsigned int>size_distribuion;
    public:
    FMTfire():FMTevent<FMTdevelopment>(){};
    FMTfire& operator = (const FMTfire& rhs){};
    FMTfire(const FMTfire& rhs):FMTevent<FMTdevelopment>(rhs),
    spread_distribution(rhs.spread_distribution),
    size_distribuion(rhs.size_distribuion){}
    };
}

#endif // FMTFIRE_H_INCLUDED
