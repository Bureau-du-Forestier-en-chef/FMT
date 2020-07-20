/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTlpheuristic.h"
#include "FMToperatingareacluster.h"
#include <vector>

#ifdef FMTWITHOSI

#ifndef FMToperatingareaclusterer_H_INCLUDED
#define FMToperatingareaclusterer_H_INCLUDED
namespace Heuristics
{
class FMToperatingareascheduler : public FMTlpheuristic
	{
	std::vector<FMToperatingareacluster>clusters;
	public:
		FMToperatingareascheduler() = default;
		FMToperatingareascheduler(const FMToperatingareascheduler&) = default;
		FMToperatingareascheduler& operator = (const FMToperatingareascheduler&) = default;
		~FMToperatingareascheduler() = default;

	};
}


#endif
#endif
