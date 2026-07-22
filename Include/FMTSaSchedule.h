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

// DocString: FMTSaSchedule
/**
@brief Abstract class for a simulated annealing cooling schedule.
*/
class FMTSaSchedule
    {
    public:
        // DocString: FMTSaSchedule(const std::string&)
        /**
        @brief Construct a FMTSaSchedule from a schedule type.
        @param[in] typesc the schedule type.
        */
        FMTSaSchedule(const std::string& typesc);
        // DocString: FMTSaSchedule()
        /**
        @brief Default constructor for FMTSaSchedule.
        */
        FMTSaSchedule()=default;
        // DocString: FMTSaSchedule::getScheduleType
        /**
        @brief Return the schedule type.
        @return the schedule type.
        */
        std::string getScheduleType()const;
        // DocString: FMTSaSchedule::getLevel
        /**
        @brief Return the level of the schedule.
        @return the level.
        */
        size_t getLevel()const;
        // DocString: FMTSaSchedule::setInitialTemperature
        /**
        @brief Set the initial temperature.
        @param[in] Temp the initial temperature.
        */
        void setInitialTemperature(const double& Temp);
        // DocString: ~FMTSaSchedule()
        /**
        @brief Virtual destructor for FMTSaSchedule.
        */
        virtual~FMTSaSchedule();
        // DocString: FMTSaSchedule::reduceTemp
        /**
        @brief Reduce the temperature according to the cooling schedule.
        */
        virtual void reduceTemp();
        // DocString: FMTSaSchedule::getTemp
        /**
        @brief Return the current temperature.
        @return the current temperature.
        */
        double getTemp() const;
        // DocString: FMTSaSchedule::setTemp
        /**
        @brief Set the current temperature.
        @param[in] p_Temp the temperature.
        */
        void setTemp( double p_Temp);
        // DocString: FMTSaSchedule::getInitialTemp
        /**
        @brief Return the initial temperature.
        @return the initial temperature.
        */
        double getInitialTemp() const;
        // DocString: FMTSaSchedule::Clone
        /**
        @brief Return a clone of the FMTSaSchedule.
        @return a unique pointer to the cloned schedule.
        */
        virtual std::unique_ptr<FMTSaSchedule> Clone()const;
    protected:
        std::string m_ScheduleType;
        size_t m_Level;
        double m_Temperature;
        double m_InitialTemperature;
    };
}
#endif // FMTSASCHEDULE_H
