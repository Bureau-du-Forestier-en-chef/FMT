/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTOPE_H_INCLUDED
#define FMTOPE_H_INCLUDED

#include "FMTutility.hpp"
#include <string>
#include <vector>
#include <boost/functional/hash.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include "FMTexception.hpp"



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

class FMTEXPORT FMToperator
{
    FMTokey key;
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & BOOST_SERIALIZATION_NVP(key);
	}
public:
	FMToperator();
	~FMToperator() = default;
    FMToperator(FMTokey lkey);
    FMToperator(std::string strkey);
	bool operator == (const FMToperator& rhs) const;
	bool operator != (const FMToperator& rhs) const;
    double call(const double& rhs1, const double& rhs2) const;
    FMToperator(const FMToperator& rhs);
    FMToperator& operator = (const FMToperator& rhs);
	int precedence() const;
	std::string associativity() const;
	FMToperator reverse() const;
	bool valid() const;
	size_t hash() const;
	bool isfactor() const;
	bool isdivide() const;
	FMTokey getkey() const;
    operator std::string() const;
};

}

#endif // FMTOPE_H_INCLUDED
