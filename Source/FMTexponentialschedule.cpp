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
    const double FMTexponentialschedule::m_ANNEALING_RATE = 0.9;

    FMTexponentialschedule::FMTexponentialschedule(): FMTsaschedule("Exponential")
    {
        //Constructor
    }

    void FMTexponentialschedule::ReduceTemp()
    {
        const double exponant = std::max(static_cast<double>(m_Level), 1.0);
        ++m_Level;
        m_Temperature = m_Temperature * pow(m_ANNEALING_RATE, exponant);
    }

	std::unique_ptr<FMTsaschedule> FMTexponentialschedule::Clone()const
    {
        return std::unique_ptr<FMTsaschedule>(new FMTexponentialschedule(*this));
    }

}
