/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTCUT_H_INCLUDED
#define FMTCUT_H_INCLUDED

#include "FMTcoordinate.hpp"
#include "FMTevent.hpp"
#include "FMTdevelopment.hpp"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>


namespace Spatial
{

class FMTcut : public FMTevent
    {
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & boost::serialization::make_nvp("event", boost::serialization::base_object<FMTevent>(*this));
	}
    public:
        FMTcut();
        ~FMTcut()=default;
        FMTcut(const FMTcut& rhs);
        FMTcut& operator = (const FMTcut& rhs);
    };

}


#endif // FMTCUT_H_INCLUDED
