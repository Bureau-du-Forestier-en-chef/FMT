#include "FMTLockBounds.h"

namespace Core
{
FMTLockBounds::FMTLockBounds()
    : FMTBounds<int>()
{
}

FMTLockBounds::FMTLockBounds(FMTsection section, const int& upper, const int& lower)
    : FMTBounds<int>(section, upper, lower)
{
}

FMTLockBounds::FMTLockBounds(FMTsection section, FMTkwor key, const int& upper, const int& lower)
    : FMTBounds<int>(section, key, upper, lower)
{
}

FMTLockBounds::FMTLockBounds(const FMTLockBounds& rhs)
    : FMTBounds<int>(rhs)
{
}

FMTLockBounds& FMTLockBounds::operator=(const FMTLockBounds& rhs)
{
    if (this != &rhs)
    {
        FMTBounds::operator=(rhs);
    }
    return *this;
}

FMTLockBounds::operator std::string() const
{
    return FMTBounds::toString("_LOCK");
}

bool FMTLockBounds::operator==(const FMTLockBounds& rhs) const
{
    return FMTBounds<int>::operator==(rhs);
}
}
