#ifndef FMTLIF_H_INCLUDED
#define FMTLIF_H_INCLUDED

#include "FMTlist.h"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
namespace Core
{

class FMTlifespans : public FMTlist<int>
    {
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & boost::serialization::make_nvp("data", boost::serialization::base_object<FMTlist<int>>(*this));
	}
    public:
        FMTlifespans();
        FMTlifespans(const FMTlifespans& rhs);
        FMTlifespans& operator = (const FMTlifespans& rhs);
		bool operator == (const FMTlifespans& rhs) const;
        operator string() const;
    };

}
#endif // FMTLIF_H_INCLUDED
