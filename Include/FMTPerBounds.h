#ifndef FMTPERBOUNDS_H_INCLUDED
#define FMTPERBOUNDS_H_INCLUDED

#include "FMTBounds.hpp"

#include <boost/serialization/base_object.hpp>

namespace Core
{
class FMTSpec;

class FMTEXPORT FMTPerBounds : public FMTBounds<int>
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
    FMTPerBounds();
    ~FMTPerBounds() = default;
    FMTPerBounds(FMTsection section, const int& upper, const int& lower);
    FMTPerBounds(const FMTPerBounds& rhs);
    FMTPerBounds(const FMTBounds<int>& rhs);
    FMTPerBounds& operator=(const FMTPerBounds& rhs);
    bool operator==(const FMTPerBounds& rhs) const;
    operator std::string() const;
};
}

#endif
