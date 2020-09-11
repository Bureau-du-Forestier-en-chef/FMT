/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTcut.h"

namespace Spatial
{
FMTcut::FMTcut():
    FMTevent()
    {

    }


    FMTcut::FMTcut(const FMTcut& rhs):
        FMTevent(rhs)
    {

    }
FMTcut& FMTcut::operator = (const FMTcut& rhs)
    {
    if(this!=&rhs)
        {
        FMTevent::operator = (rhs);
        }
    return *this;
    }
}
