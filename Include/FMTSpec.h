#ifndef FMTSPEC_H_INCLUDED
#define FMTSPEC_H_INCLUDED

#include "FMTPerBounds.h"
#include "FMTAgeBounds.h"
#include "FMTLockBounds.h"
#include "FMTYldBounds.h"
#include "FMTutility.h"

#include <algorithm>
#include <cstddef>
#include <string>
#include <vector>

#include <boost/functional/hash.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>

namespace Core
{
class FMTYields;

class FMTEXPORT FMTSpec
{
    friend class Core::FMTYields;
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive& ar, const unsigned int)
    {
        ar & BOOST_SERIALIZATION_NVP(per);
        ar & BOOST_SERIALIZATION_NVP(age);
        ar & BOOST_SERIALIZATION_NVP(lock);
        ar & BOOST_SERIALIZATION_NVP(yieldnames);
        ar & BOOST_SERIALIZATION_NVP(yieldbounds);
    }

protected:
    FMTPerBounds per;
    FMTAgeBounds age;
    FMTLockBounds lock;
    std::vector<std::string> yieldnames;
    std::vector<FMTYldBounds> yieldbounds;

public:
    FMTSpec();
    virtual ~FMTSpec() = default;
    FMTSpec(const FMTSpec& rhs);
    FMTSpec& operator=(const FMTSpec& rhs);

    bool add(const FMTSpec& rhs);
    bool setBounds(const FMTPerBounds& bound);
    bool addBounds(const FMTAgeBounds& bound);
    bool addBounds(const FMTYldBounds& bound);
    bool addBounds(const FMTLockBounds& bound);

    inline bool allowWithoutYield(const int& tperiod, const int& tage, const int& tlock) const
    {
        return per.in(tperiod) &&
               age.in(tage) &&
               (lock.empty() || tlock >= lock.lower);
    }

    inline const FMTYldBounds& getYieldBound(const std::string& name) const
    {
        return yieldbounds.at(
            std::distance(
                yieldnames.begin(),
                std::find(yieldnames.begin(), yieldnames.end(), name)));
    }

    inline bool allowYields(const std::vector<double>& values) const
    {
        for (size_t location = 0; location < yieldnames.size(); ++location)
        {
            if (yieldbounds.at(location).out(values.at(location)))
            {
                return false;
            }
        }
        return true;
    }

    inline bool allow(
        const int& tperiod,
        const int& tage,
        const int& tlock,
        const std::vector<double>& values) const
    {
        for (size_t location = 0; location < yieldnames.size(); ++location)
        {
            if (yieldbounds.at(location).out(values.at(location)))
            {
                return false;
            }
        }
        return allowWithoutYield(tperiod, tage, tlock);
    }

    inline const std::vector<std::string>& getYlds() const
    {
        return yieldnames;
    }

    inline const std::vector<FMTYldBounds>& getYldBounds() const
    {
        return yieldbounds;
    }

    virtual operator std::string() const;
    bool operator==(const FMTSpec& rhs) const;
    bool operator<(const FMTSpec& rhs) const;
    size_t hash() const;
    bool empty() const;
    bool emptyAge() const;
    bool emptyYlds() const;
    bool emptyPeriod() const;
    bool emptyLock() const;
    int getAgeUpperBound() const;
    int getAgeLowerBound() const;
    int getPeriodUpperBound() const;
    int getPeriodLowerBound() const;
    int getLockUpperBound() const;
    int getLockLowerBound() const;
    bool isSubsetOf(const FMTSpec& rhs) const;
};
}

namespace boost
{
template<>
struct hash<Core::FMTSpec>
{
    std::size_t operator()(const Core::FMTSpec& spec) const
    {
        return spec.hash();
    }
};
}

#endif
