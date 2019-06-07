#ifndef FMTerror_H_INCLUDED
#define FMTerror_H_INCLUDED

#include "FMTexception.h"


namespace Exception
	{
	class FMTerror : public FMTexception
		{
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar &  boost::serialization::make_nvp("exception", boost::serialization::base_object<FMTexception>(*this));
		}
		public:
			FMTerror();
			FMTerror(const FMTexception& rhs);
			FMTerror(const FMTexc lexception, const string message);
			FMTerror(const FMTexc lexception, const FMTwssect lsection, const string message);
		};
	}
#endif

