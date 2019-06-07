#ifndef FMTdefaultexceptionhandler_H_INCLUDED
#define FMTdefaultexceptionhandler_H_INCLUDED

#include "FMTexceptionhandler.h"
#include "FMTexception.h"

namespace Exception
{
class FMTdefaultexceptionhandler : public FMTexceptionhandler
	{
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar &  boost::serialization::make_nvp("parent_handler", boost::serialization::base_object<FMTexceptionhandler>(*this));
	}
	public:
		FMTdefaultexceptionhandler();
		~FMTdefaultexceptionhandler() = default;
		FMTlev raise(FMTexc lexception, FMTwssect lsection, string text,
			const int& line, const string& file) override;
	};
}

#endif

