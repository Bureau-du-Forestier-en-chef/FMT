#include "FMTAgeBounds.h"

namespace Core
{
FMTAgeBounds::FMTAgeBounds()
    : FMTBounds<int>()
{
}

FMTAgeBounds::FMTAgeBounds(FMTsection section, const int& upper, const int& lower)
    : FMTBounds<int>(section, upper, lower)
{
}

FMTAgeBounds::FMTAgeBounds(FMTsection section, FMTkwor key, const int& upper, const int& lower)
    : FMTBounds<int>(section, key, upper, lower)
{
}

FMTAgeBounds::FMTAgeBounds(const FMTAgeBounds& rhs)
    : FMTBounds(rhs)
{
}

FMTAgeBounds::FMTAgeBounds(const FMTBounds<int>& rhs)
    : FMTBounds(rhs)
{
}

FMTAgeBounds& FMTAgeBounds::operator=(const FMTAgeBounds& rhs)
{
    if (this != &rhs)
    {
        FMTBounds::operator=(rhs);
    }
    return *this;
}

FMTAgeBounds::operator std::string() const
{
    return FMTBounds::toString("_AGE");
}

bool FMTAgeBounds::operator==(const FMTAgeBounds& rhs) const
{
    return FMTBounds::operator==(rhs);
}
}
