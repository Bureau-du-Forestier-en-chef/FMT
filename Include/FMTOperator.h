/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTOPE_Hm_included
#define FMTOPE_Hm_included

#include "FMTutility.h"
#include <string>
#include <vector>
#include <boost/functional/hash.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include "FMTException.h"



namespace Core
{

	enum class FMTokey
	{
		multiply = 1,
		divide = 2,
		add = 3,
		sub = 4,
		comma = 5,
		pow = 6,
		notvalid = 7
	};

class FMTEXPORT FMTOperator
{
    FMTokey key;
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & BOOST_SERIALIZATION_NVP(key);
	}
public:
	FMTOperator();
	~FMTOperator() = default;
    FMTOperator(FMTokey lkey);
    FMTOperator(std::string strkey);
	bool operator == (const FMTOperator& rhs) const;
	bool operator != (const FMTOperator& rhs) const;
    double call(const double& rhs1, const double& rhs2) const;
    FMTOperator(const FMTOperator& rhs);
    FMTOperator& operator = (const FMTOperator& rhs);
	int precedence() const;
	std::string associativity() const;
	FMTOperator reverse() const;
	bool valid() const;
	size_t hash() const;
	bool isFactor() const;
	bool isDivide() const;
	FMTokey getKey() const;
    operator std::string() const;
};

}

#endif // FMTOPE_Hm_included
