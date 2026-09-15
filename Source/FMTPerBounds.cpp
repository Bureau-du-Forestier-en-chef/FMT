#include "FMTPerBounds.h"

namespace Core
{
FMTPerBounds::FMTPerBounds()
    : FMTBounds<int>()
{
}

FMTPerBounds::FMTPerBounds(FMTsection section, const int& upper, const int& lower)
    : FMTBounds<int>(section, upper, lower)
{
}

FMTPerBounds::FMTPerBounds(const FMTPerBounds& rhs)
    : FMTBounds(rhs)
{
}

FMTPerBounds::FMTPerBounds(const FMTBounds<int>& rhs)
    : FMTBounds(rhs)
{
}

FMTPerBounds& FMTPerBounds::operator=(const FMTPerBounds& rhs)
{
    if (this != &rhs)
    {
        FMTBounds::operator=(rhs);
    }
    return *this;
}

FMTPerBounds::operator std::string() const
{
    return FMTBounds::toString("_CP");
}

bool FMTPerBounds::operator==(const FMTPerBounds& rhs) const
{
    return FMTBounds::operator==(rhs);
}
}
