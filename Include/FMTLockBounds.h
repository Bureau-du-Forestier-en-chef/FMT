#ifndef FMTLOCKBOUNDS_H_INCLUDED
#define FMTLOCKBOUNDS_H_INCLUDED

#include "FMTBounds.hpp"

#include <boost/serialization/base_object.hpp>

namespace Core
{
class FMTSpec;

class FMTEXPORT FMTLockBounds : public FMTBounds<int>
{
    friend class FMTSpec;
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive& ar, const unsigned int)
    {
        ar & boost::serialization::make_nvp(
            "bounds",
            boost::serialization::base_object<FMTBounds<int>>(*this));
    }

public:
    FMTLockBounds();
    ~FMTLockBounds() = default;
    FMTLockBounds(FMTsection section, FMTkwor key, const int& upper, const int& lower);
    FMTLockBounds(FMTsection section, const int& upper, const int& lower);
    FMTLockBounds(const FMTLockBounds& rhs);
    FMTLockBounds& operator=(const FMTLockBounds& rhs);
    bool operator==(const FMTLockBounds& rhs) const;
    operator std::string() const;
};
}

#endif
