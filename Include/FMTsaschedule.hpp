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
		std::string schedule_type;
    public:
        FMTsaschedule();//Constructor
        virtual~FMTsaschedule()=default;//Destructor
        virtual double reduce_temp(double& temp);//
        virtual std::string get_schedule_type()const;
        virtual std::unique_ptr<FMTsaschedule> Clone()const;
    };

class FMTexponentialschedule : public FMTsaschedule
    {
    protected:
        double alpha;
		std::string schedule_type;
    public:
        FMTexponentialschedule(double lalpha = 0.9);
        ~FMTexponentialschedule()=default;
        double reduce_temp(double& temp);
		std::string get_schedule_type()const;
		std::unique_ptr<FMTsaschedule> Clone()const;

    };
}
#endif // FMTSASCHEDULE_H
