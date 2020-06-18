/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTCUT_H_INCLUDED
#define FMTCUT_H_INCLUDED

#include "FMTcoordinate.h"
#include "FMTsesevent.h"
#include "FMTdevelopment.h"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>


namespace Spatial
{

class FMTcut : public FMTsesevent<Core::FMTdevelopment>
    {
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & boost::serialization::make_nvp("event", boost::serialization::base_object<FMTsesevent<Core::FMTdevelopment>>(*this));
	}
    public:
        FMTcut();
        ~FMTcut()=default;
        FMTcut(const std::map<FMTcoordinate,Core::FMTdevelopment>& lterritory,const int& pass);
        FMTcut(const FMTcut& rhs);
        FMTcut& operator = (const FMTcut& rhs);
    };

}


#endif // FMTCUT_H_INCLUDED
