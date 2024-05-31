/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTexponentialschedule.h"
#include "FMTsaschedule.h"

#include <memory>
#include <string>
#include <vector>
#include <math.h> 
#include <algorithm>


namespace Spatial
{

    FMTexponentialschedule::FMTexponentialschedule(double lalpha): FMTsaschedule("Exponential"), alpha(lalpha)
    {
        //Constructor
    }

    void FMTexponentialschedule::ReduceTemp()
    {
        const double exponant = std::max(static_cast<double>(Level), 1.0);
        ++Level;
        Temperature = Temperature * pow(alpha, exponant);
    }

	std::unique_ptr<FMTsaschedule> FMTexponentialschedule::Clone()const
    {
        return std::unique_ptr<FMTsaschedule>(new FMTexponentialschedule(*this));
    }

}
