#ifndef FMTOPE_H_INCLUDED
#define FMTOPE_H_INCLUDED

#include "FMTutility.h"
#include <string>
#include <vector>
#include <algorithm>
#include <boost/functional/hash.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>

using namespace std;

namespace Core
{

	enum class FMTokey
	{
		multiply = 1,
		divide = 2,
		add = 3,
		sub = 4,
		/*leftparenthesis = 5,
		rightparenthesis = 6,*/
		comma = 7,
		notvalid = 8
	};

class FMToperator
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
    FMToperator(FMTokey lkey);
    FMToperator(string strkey);
	bool operator == (const FMToperator& rhs) const;
    double call(const double& rhs1, const double& rhs2) const;
    FMToperator(const FMToperator& rhs);
    FMToperator& operator = (const FMToperator& rhs);
	int precedence() const;
	string associativity() const;
	FMToperator reverse() const;
	bool valid() const;
	size_t hash() const;
	//bool isparenthesis() const;
	bool isfactor() const;
	bool isdivide() const;
	FMTokey getkey() const;
    operator string() const;
};

}

#endif // FMTOPE_H_INCLUDED
