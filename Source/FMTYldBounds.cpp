#include "FMTYldBounds.h"

namespace Core
{
FMTYldBounds::FMTYldBounds()
    : FMTBounds<double>(), yield()
{
}

FMTYldBounds::FMTYldBounds(
    FMTsection section,
    const std::string& lyield,
    const double& upper,
    const double& lower)
    : FMTBounds<double>(section, upper, lower), yield(lyield)
{
}

FMTYldBounds::FMTYldBounds(
    FMTsection section,
    FMTkwor key,
    const std::string& lyield,
    const double& upper,
    const double& lower)
    : FMTBounds<double>(section, key, upper, lower), yield(lyield)
{
}

FMTYldBounds::FMTYldBounds(const FMTYldBounds& rhs)
    : FMTBounds<double>(rhs), yield(rhs.yield)
{
}

FMTYldBounds::FMTYldBounds(const std::string& lyield, const FMTBounds<double>& rhs)
    : FMTBounds(rhs), yield(lyield)
{
}

FMTYldBounds& FMTYldBounds::operator=(const FMTYldBounds& rhs)
{
    if (this != &rhs)
    {
        FMTBounds::operator=(rhs);
        yield = rhs.yield;
    }
    return *this;
}

bool FMTYldBounds::operator==(const FMTYldBounds& rhs) const
{
    return FMTBounds::operator==(rhs) && yield == rhs.yield;
}

FMTYldBounds::operator std::string() const
{
    return FMTBounds::toString(yield);
}
}
