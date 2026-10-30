/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef PERFORMANCE_MODELSETUP_H_INCLUDED
#define PERFORMANCE_MODELSETUP_H_INCLUDED

#ifdef FMTWITHOSI
	#include "FMTsolverinterface.h"

	#include <vector>

namespace Core
{
	class FMTSchedule;
}

namespace Models
{
	class FMTLpModel;
}

namespace Performance
{
	// MOSEK, the solver of production, when FMT is built with it; CLP otherwise.
	Models::FMTSolverInterface getSolver();

	// Builds a model with the schedule of its scenario over p_length periods, without solving it, as
	// SetSolution does.
	void buildWithSchedule(Models::FMTLpModel& p_model, const std::vector<Core::FMTSchedule>& p_schedules, int p_length);
}
#endif

#endif
