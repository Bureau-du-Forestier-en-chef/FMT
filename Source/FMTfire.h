/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTFIRE_H_INCLUDED
#define FMTFIRE_H_INCLUDED


#include <random>
#include <vector>
#include "FMTcoordinate.h"
#include "FMTevent.h"
#include <boost/serialization/serialization.hpp>


namespace Spatial
{

class FMTfire : public FMTevent<FMTdevelopment>
    {
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & boost::serialization::make_nvp("event", boost::serialization::base_object<FMTevent<Core::FMTdevelopment>>(*this));
	}
    protected:
		std::normal_distribution<unsigned int>spread_distribution;
		std::weibull_distribution<unsigned int>size_distribuion;
    public:
    FMTfire():FMTevent<Core::FMTdevelopment>(){};
    FMTfire& operator = (const FMTfire& rhs){};
    FMTfire(const FMTfire& rhs):FMTevent<Core::FMTdevelopment>(rhs),
    spread_distribution(rhs.spread_distribution),
    size_distribuion(rhs.size_distribuion){}
    };
}

#endif // FMTFIRE_H_INCLUDED
