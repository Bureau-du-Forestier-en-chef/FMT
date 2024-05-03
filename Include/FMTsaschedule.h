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

class FMTsaschedule
//Abstract class for cooling schedule
    {
    protected:
		std::string ScheduleType;
        size_t Level;
        double Temperature;
        double InitialTemperature;
    public:
        FMTsaschedule(const std::string& typesc);//Constructor
        FMTsaschedule();//Constructor
        std::string GetScheduleType()const;
        size_t GetLevel()const;
        void SetInitialTemperature(const double& Temp);
        virtual~FMTsaschedule();//Destructor
        virtual void ReduceTemp();//
        double GetTemp() const;//
        double GetInitialTemp() const;//
        virtual std::unique_ptr<FMTsaschedule> Clone()const;
    };
}
#endif // FMTSASCHEDULE_H
