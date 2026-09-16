#ifndef FMTBOUNDS_HPP_INCLUDED
#define FMTBOUNDS_HPP_INCLUDED

#include <limits>
#include <string>
#include "FMTException.h"
#include "FMTutility.h"
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/serialization.hpp>

namespace Core
{
template<typename T>
class FMTBounds
{
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive& ar, const unsigned int)
    {
        ar & BOOST_SERIALIZATION_NVP(andbound);
        ar & BOOST_SERIALIZATION_NVP(use);
        ar & BOOST_SERIALIZATION_NVP(section);
        ar & BOOST_SERIALIZATION_NVP(keytype);
        ar & BOOST_SERIALIZATION_NVP(upper);
        ar & BOOST_SERIALIZATION_NVP(lower);
    }
    bool andbound;
    bool use;
protected:
    FMTsection section;
    FMTkwor keytype;
    T upper;
    T lower;
public:
    FMTBounds() : andbound(true), use(false), section(), keytype(), upper(), lower() {}
    FMTBounds(FMTsection lsection, const T& lupper, const T& llower) : andbound(true), use(true), section(lsection), keytype(), upper(lupper), lower(llower) {}
    FMTBounds(FMTsection lsection, FMTkwor key, const T& lupper, const T& llower) : andbound(true), use(true), section(lsection), keytype(key), upper(lupper), lower(llower) {}
    virtual ~FMTBounds() = default;
    FMTBounds(const FMTBounds<T>& rhs) : andbound(rhs.andbound), use(rhs.use), section(rhs.section), keytype(rhs.keytype), upper(rhs.upper), lower(rhs.lower) {}
    FMTBounds<T>& operator=(const FMTBounds<T>& rhs)
    {
        if (this != &rhs) { andbound = rhs.andbound; section = rhs.section; use = rhs.use; lower = rhs.lower; upper = rhs.upper; keytype = rhs.keytype; }
        return *this;
    }
    void setOrBound() { andbound = false; }
    T getLower() const { return lower; }
    T getUpper() const { return upper; }
    bool out(const T& value) const { return lower > value || upper < value; }
    bool in(const T& value) const { return empty() || (value <= upper && value >= lower); }
    bool operator==(const FMTBounds<T>& rhs) const { return andbound == rhs.andbound && use == rhs.use && section == rhs.section && keytype == rhs.keytype && upper == rhs.upper && lower == rhs.lower; }
    bool operator<(const FMTBounds<T>& rhs) const
    {
        if (andbound < rhs.andbound) return true; if (rhs.andbound < andbound) return false;
        if (use < rhs.use) return true; if (rhs.use < use) return false;
        if (section < rhs.section) return true; if (rhs.section < section) return false;
        if (keytype < rhs.keytype) return true; if (rhs.keytype < keytype) return false;
        if (upper < rhs.upper) return true; if (rhs.upper < upper) return false;
        if (lower < rhs.lower) return true; return false;
    }
    bool empty() const { return !use; }
    bool add(const FMTBounds<T>& rhs)
    {
        if (!empty()) { if (rhs.lower != std::numeric_limits<T>::lowest()) lower = rhs.lower; if (rhs.upper != std::numeric_limits<T>::max()) upper = rhs.upper; }
        else *this = rhs;
        use = true; return true;
    }
    std::string toString(const std::string& name) const
    {
        std::string line; const std::string slower = std::to_string(lower); std::string supper = std::to_string(upper); bool beenuse = false;
        if (section == FMTsection::Action) {
            if (name.find("_LOCK") != std::string::npos && upper > 0) line += "_LOCKEXEMPT";
            else if (upper == lower) line = name + " = " + supper;
            else { if (lower != std::numeric_limits<T>::lowest()) { line = name + " >= " + slower; beenuse = true; } if (upper != std::numeric_limits<T>::max()) line += (beenuse ? " AND " : "") + name + " <= " + supper; }
        } else if (section == FMTsection::Transition || section == FMTsection::Outputs) {
            if (upper == std::numeric_limits<T>::max()) supper = "_MAXAGE";
            if (keytype == FMTkwor::Source) {
                line = name == "_AGE" ? "@AGE(" : "@YLD(" + name + ",";
                line += upper == lower ? slower + ")" : slower + ".." + supper + ")";
                if (section == FMTsection::Outputs) { if (name.find("LOCK") != std::string::npos && lower >= 1) line = "_INVLOCK"; else if (name.find("_CP") != std::string::npos && lower == 0 && upper == 0) line = "[0]"; }
            } else if (keytype == FMTkwor::Target) line = name.find("_LOCK") != std::string::npos ? "_LOCK " + slower : name + " " + slower;
        }
        return line;
    }
};
}
#endif
