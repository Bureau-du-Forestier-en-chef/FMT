/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTEXPONENTIALSCHEDULE_H
#define FMTEXPONENTIALSCHEDULE_H

#include "FMTSaSchedule.h"
#include <string>
#include <memory>


namespace Spatial
{

// DocString: FMTExponentialSchedule
/**
@brief Exponential cooling schedule for simulated annealing, reducing the temperature by a constant annealing rate.
*/
class FMTExponentialSchedule : public FMTSaSchedule
    {
    public:
        // DocString: FMTExponentialSchedule()
        /**
        @brief Default constructor for FMTExponentialSchedule.
        */
        FMTExponentialSchedule();
        // DocString: ~FMTExponentialSchedule()
        /**
        @brief Default destructor for FMTExponentialSchedule.
        */
        ~FMTExponentialSchedule()=default;
        // DocString: FMTExponentialSchedule::reduceTemp
        /**
        @brief Reduce the temperature using the exponential annealing rate.
        */
        void reduceTemp();
		// DocString: FMTExponentialSchedule::clone
		/**
		@brief Return a clone of the FMTExponentialSchedule.
		@return a unique pointer to the cloned schedule.
		*/
		std::unique_ptr<FMTSaSchedule> clone()const;
        // DocString: FMTExponentialSchedule::setAnnealingRate
        /**
        @brief Set the annealing rate.
        */
        void setAnnealingRate(const double& p_annealingRate);
    private:
        double m_ANNEALING_RATE;

    };
}
#endif // FMTSASCHEDULE_H
