/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTEXPONENTIALSCHEDULE_H
#define FMTEXPONENTIALSCHEDULE_H

#include "FMTsaschedule.h"
#include <string>
#include <memory>


namespace Spatial
{

class FMTexponentialschedule : public FMTsaschedule
    {
    public:
        FMTexponentialschedule();
        ~FMTexponentialschedule()=default;
        void ReduceTemp();
		std::unique_ptr<FMTsaschedule> Clone()const;
    private:
        static const double m_ANNEALING_RATE;

    };
}
#endif // FMTSASCHEDULE_H
