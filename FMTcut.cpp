#include "FMTcut.h"

namespace Spatial
{
FMTcut::FMTcut():
    FMTevent<FMTdevelopment>()
    {

    }



FMTcut::FMTcut(const map<FMTcoordinate,FMTdevelopment>& lterritory, const int& pass):
                 FMTevent<FMTdevelopment>(lterritory,pass)
                 {

                 }

    FMTcut::FMTcut(const FMTcut& rhs):
        FMTevent<FMTdevelopment>(rhs)
    {

    }
FMTcut& FMTcut::operator = (const FMTcut& rhs)
    {
    if(this!=&rhs)
        {
        FMTevent<FMTdevelopment>::operator = (rhs);
        }
    return *this;
    }
}
