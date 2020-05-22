/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTcut.h"

namespace Spatial
{
FMTcut::FMTcut():
    FMTsesevent<Core::FMTdevelopment>()
    {

    }



FMTcut::FMTcut(const std::map<FMTcoordinate,Core::FMTdevelopment>& lterritory, const int& pass):
                 FMTsesevent<Core::FMTdevelopment>(lterritory,pass)
                 {

                 }

    FMTcut::FMTcut(const FMTcut& rhs):
        FMTsesevent<Core::FMTdevelopment>(rhs)
    {

    }
FMTcut& FMTcut::operator = (const FMTcut& rhs)
    {
    if(this!=&rhs)
        {
        FMTsesevent<Core::FMTdevelopment>::operator = (rhs);
        }
    return *this;
    }
}
