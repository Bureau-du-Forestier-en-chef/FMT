#ifndef FMTwarning_H_INCLUDED
#define FMTwarning_H_INCLUDED

#include "FMTexception.h"

namespace Exception
{
	class FMTwarning : public FMTexception
	{
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
		{
			ar &  boost::serialization::make_nvp("parent_exception", boost::serialization::base_object<FMTexception>(*this));
		}
	public:
		FMTwarning();
		FMTwarning(const FMTexception& rhs);
		FMTwarning(const FMTexc lexception, const string message);
		FMTwarning(const FMTexc lexception, const FMTwssect lsection, const string message);
		void warn() const;
	};
}
#endif