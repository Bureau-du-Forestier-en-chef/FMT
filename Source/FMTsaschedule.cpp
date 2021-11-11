/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTsaschedule.hpp"

#include <memory>
#include <string>
#include <vector>


namespace Spatial
{


//FMTsaschedule base class


    FMTsaschedule::FMTsaschedule():schedule_type()
    {

    }

    double FMTsaschedule::reduce_temp(double& temp)
    {
        return 0.0;
    }

	std::string FMTsaschedule::get_schedule_type()const
    {
        return schedule_type;
    }

	std::unique_ptr<FMTsaschedule> FMTsaschedule::Clone()const
    {
        return std::unique_ptr<FMTsaschedule>(new FMTsaschedule(*this));
    }


//FMTexponentialschedule


    FMTexponentialschedule::FMTexponentialschedule(double lalpha): FMTsaschedule(),alpha(lalpha),schedule_type("exponential")
    {
        //Constructor
    }

    double FMTexponentialschedule::reduce_temp(double& temp)
    {
        return temp*alpha;
    }

	std::string FMTexponentialschedule::get_schedule_type()const
    {
        return schedule_type;
    }

	std::unique_ptr<FMTsaschedule> FMTexponentialschedule::Clone()const
    {
        return std::unique_ptr<FMTsaschedule>(new FMTexponentialschedule(*this));
    }

}
