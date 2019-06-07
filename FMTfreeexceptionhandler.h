#ifndef FMTfreeexceptionhandler_H_INCLUDED
#define FMTfreeexceptionhandler_H_INCLUDED

#include "FMTexceptionhandler.h"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>

namespace Exception
{
class FMTfreeexceptionhandler : public FMTexceptionhandler
{
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & boost::serialization::make_nvp("parent_exception", boost::serialization::base_object<FMTexceptionhandler>(*this));
	}
public:
	FMTfreeexceptionhandler();
	~FMTfreeexceptionhandler() = default;
	FMTlev raise(FMTexc lexception, FMTwssect lsection, string text,
		const int& line, const string& file) override;
};
}
#endif