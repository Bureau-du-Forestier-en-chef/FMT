#ifndef FMTYLDBOUNDS_H_INCLUDED
#define FMTYLDBOUNDS_H_INCLUDED

#include "FMTBounds.hpp"

#include <string>
#include <boost/serialization/base_object.hpp>

namespace Core
{
class FMTSpec;

class FMTEXPORT FMTYldBounds : public FMTBounds<double>
{
    friend class FMTSpec;
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive& ar, const unsigned int)
    {
        ar & boost::serialization::make_nvp(
            "bounds",
            boost::serialization::base_object<FMTBounds<double>>(*this));
        ar & BOOST_SERIALIZATION_NVP(yield);
    }

    std::string yield;

public:
    FMTYldBounds();
    ~FMTYldBounds() = default;
    FMTYldBounds(FMTsection section, const std::string& yield, const double& upper, const double& lower);
    FMTYldBounds(FMTsection section, FMTkwor key, const std::string& yield, const double& upper, const double& lower);
    FMTYldBounds(const std::string& yield, const FMTBounds<double>& rhs);
    FMTYldBounds(const FMTYldBounds& rhs);
    FMTYldBounds& operator=(const FMTYldBounds& rhs);
    bool operator==(const FMTYldBounds& rhs) const;
    operator std::string() const;
};
}

#endif
