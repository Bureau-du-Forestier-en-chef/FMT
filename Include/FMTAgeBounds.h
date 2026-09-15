#ifndef FMTAGEBOUNDS_H_INCLUDED
#define FMTAGEBOUNDS_H_INCLUDED

#include "FMTBounds.hpp"

#include <boost/serialization/base_object.hpp>

namespace Core
{
class FMTSpec;

class FMTEXPORT FMTAgeBounds : public FMTBounds<int>
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
    FMTAgeBounds();
    ~FMTAgeBounds() = default;
    FMTAgeBounds(FMTsection section, const int& upper, const int& lower);
    FMTAgeBounds(FMTsection section, FMTkwor key, const int& upper, const int& lower);
    FMTAgeBounds(const FMTAgeBounds& rhs);
    FMTAgeBounds(const FMTBounds<int>& rhs);
    FMTAgeBounds& operator=(const FMTAgeBounds& rhs);
    bool operator==(const FMTAgeBounds& rhs) const;
    operator std::string() const;
};
}

#endif
