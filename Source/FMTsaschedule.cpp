/*
Copyright (c) 2019 Gouvernement du Qu�bec

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

    FMTsaschedule::FMTsaschedule(const std::string& typeof) :
        ScheduleType(typeof),
        Level(0),
        Temperature(std::numeric_limits<double>::max()),
        InitialTemperature(std::numeric_limits<double>::max())
    {

    }


    FMTsaschedule::FMTsaschedule():
        ScheduleType("None"),
        Level(0),
        Temperature(std::numeric_limits<double>::max()),
        InitialTemperature(std::numeric_limits<double>::max())
    {

    }

    FMTsaschedule::~FMTsaschedule(){}

    void FMTsaschedule::SetInitialTemperature(const double& Temp)
    {
        InitialTemperature = Temp;
        Temperature = Temp;
    }


    void FMTsaschedule::ReduceTemp()
    {
        ++Level;
    }

    double FMTsaschedule::GetTemp() const
    {
        return Temperature;
    }

    size_t FMTsaschedule::GetLevel()const
    {
        return Level;
    }
   

	std::string FMTsaschedule::GetScheduleType()const
    {
        return ScheduleType;
    }

	std::unique_ptr<FMTsaschedule> FMTsaschedule::Clone()const
    {
        return std::unique_ptr<FMTsaschedule>(new FMTsaschedule(*this));
    }

    double FMTsaschedule::GetInitialTemp() const
        {
        return InitialTemperature;
        }

}
