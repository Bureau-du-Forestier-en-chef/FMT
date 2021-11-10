/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTGCBMTRANSITION_H_INCLUDED
#define FMTGCBMTRANSITION_H_INCLUDED

#include <map>
#include <string>
#include <boost/serialization/string.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/export.hpp>
#include "FMTobject.hpp"



namespace Core
{
	class FMTEXPORT FMTGCBMtransition
	{
		// DocString: FMTGCBMtransition::serialize
	/**
	serialize function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
	*/
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
		{
		ar & BOOST_SERIALIZATION_NVP(ageafter);
		ar & BOOST_SERIALIZATION_NVP(themes);
		ar & BOOST_SERIALIZATION_NVP(name);
		}
	public:
		int ageafter;
		std::map<std::string, std::string>themes;
		std::string name;
		FMTGCBMtransition();
		FMTGCBMtransition(const int& lageafter, const std::map<std::string, std::string>& lthemes, const std::string& lname);
		FMTGCBMtransition(const FMTGCBMtransition& rhs);
		FMTGCBMtransition& operator = (const FMTGCBMtransition& rhs);
		~FMTGCBMtransition()=default;
	};
}
#endif
