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
        m_ScheduleType(typesc),
        m_Level(0),
        m_Temperature(std::numeric_limits<double>::max()),
        m_InitialTemperature(std::numeric_limits<double>::max())
    {

    }


    FMTsaschedule::~FMTsaschedule(){}

    void FMTsaschedule::SetInitialTemperature(const double& Temp)
    {
        m_InitialTemperature = Temp;
        m_Temperature = Temp;
    }


    void FMTsaschedule::ReduceTemp()
    {
        ++m_Level;
    }

    double FMTsaschedule::GetTemp() const
    {
        return m_Temperature;
    }

    void FMTsaschedule::SetTemp(double p_Temp)
    {
        m_Temperature = p_Temp;
    }

    size_t FMTsaschedule::GetLevel()const
    {
        return m_Level;
    }
   

	std::string FMTsaschedule::GetScheduleType()const
    {
        return m_ScheduleType;
    }

	std::unique_ptr<FMTsaschedule> FMTsaschedule::Clone()const
    {
        return std::unique_ptr<FMTsaschedule>(new FMTsaschedule(*this));
    }

    double FMTsaschedule::GetInitialTemp() const
        {
        return m_InitialTemperature;
        }

}
