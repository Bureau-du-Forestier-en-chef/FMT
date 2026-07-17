/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTSASCHEDULE_H
#define FMTSASCHEDULE_H

#include <memory>
#include <string>
#include <vector>


namespace Spatial
{

class FMTSaSchedule
//Abstract class for cooling schedule
    {
    public:
        FMTSaSchedule(const std::string& typesc);//Constructor
        FMTSaSchedule()=default;//Constructor
        std::string getScheduleType()const;
        size_t getLevel()const;
        void setInitialTemperature(const double& Temp);
        virtual~FMTSaSchedule();//Destructor
        virtual void reduceTemp();//
        double getTemp() const;//
        void setTemp( double p_Temp);
        double getInitialTemp() const;//
        virtual std::unique_ptr<FMTSaSchedule> Clone()const;
    protected:
        std::string m_ScheduleType;
        size_t m_Level;
        double m_Temperature;
        double m_InitialTemperature;
    };
}
#endif // FMTSASCHEDULE_H
