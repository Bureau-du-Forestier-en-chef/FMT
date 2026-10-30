/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

// How the benchmarks that solve or replay a model set it up, shared by the model flows and the
// traversal of the graph.

#include "ModelSetup.h"

#ifdef FMTWITHOSI
	#include "FMTLpModel.h"
	#include "FMTSchedule.h"

namespace
{
	// Tolerance of a replayed schedule, as in SetSolution.
	constexpr double REPLAY_TOLERANCE = 0.01;
}

namespace Performance
{
	Models::FMTSolverInterface getSolver()
	{
	#ifdef FMTWITHMOSEK
		return Models::FMTSolverInterface::MOSEK;
	#else
		return Models::FMTSolverInterface::CLP;
	#endif
	}

	void buildWithSchedule(Models::FMTLpModel& p_model, const std::vector<Core::FMTSchedule>& p_schedules, int p_length)
	{
		p_model.setParameter(Models::FMTintmodelparameters::LENGTH, p_length);
		p_model.setParameter(Models::FMTboolmodelparameters::FORCE_PARTIAL_BUILD, true);
		p_model.setParameter(Models::FMTboolmodelparameters::QUIET_LOGGING, true);
		p_model.FMTModel::setParameter(Models::FMTdblmodelparameters::TOLERANCE, REPLAY_TOLERANCE);
		p_model.doPlanning(false, p_schedules);
	}
}
#endif
