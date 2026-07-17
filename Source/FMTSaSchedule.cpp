/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTSaSchedule.h"

#include <memory>
#include <string>
#include <vector>
#include <limits>


namespace Spatial
{


//FMTSaSchedule base class

    FMTSaSchedule::FMTSaSchedule(const std::string& typesc) :
        m_ScheduleType(typesc),
        m_Level(0),
        m_Temperature(std::numeric_limits<double>::max()),
        m_InitialTemperature(std::numeric_limits<double>::max())
    {

    }


    FMTSaSchedule::~FMTSaSchedule(){}

    void FMTSaSchedule::setInitialTemperature(const double& Temp)
    {
        m_InitialTemperature = Temp;
        m_Temperature = Temp;
    }


    void FMTSaSchedule::reduceTemp()
    {
        ++m_Level;
    }

    double FMTSaSchedule::getTemp() const
    {
        return m_Temperature;
    }

    void FMTSaSchedule::setTemp(double p_Temp)
    {
        m_Temperature = p_Temp;
    }

    size_t FMTSaSchedule::getLevel()const
    {
        return m_Level;
    }
   

	std::string FMTSaSchedule::getScheduleType()const
    {
        return m_ScheduleType;
    }

	std::unique_ptr<FMTSaSchedule> FMTSaSchedule::Clone()const
    {
        return std::unique_ptr<FMTSaSchedule>(new FMTSaSchedule(*this));
    }

    double FMTSaSchedule::getInitialTemp() const
        {
        return m_InitialTemperature;
        }

}
