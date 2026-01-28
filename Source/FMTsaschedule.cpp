/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTsaschedule.h"

#include <memory>
#include <string>
#include <vector>
#include <limits>


namespace Spatial
{


//FMTsaschedule base class

    FMTsaschedule::FMTsaschedule(const std::string& typesc) :
        ScheduleType(typesc),
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

    void FMTsaschedule::SetTemp(double p_Temp)
    {
        Temperature = p_Temp;
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
