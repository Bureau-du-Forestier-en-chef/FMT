/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/
#ifdef FMTWITHOSI

#ifndef FMTlpheuristicmthandler_Hm_included
#define FMTlpheuristicmthandler_Hm_included


#if defined FMTWITHPYTHON
	#include "boost/python.h"
#endif

#include "FMTObject.h"
#include <vector>




namespace Heuristics
{
	class FMTOperatingAreaScheduler;
	class FMTOperatingAreaClusterer;
	class FMTLpHeuristic;

	class FMTEXPORT FMTLpHeuristicMtHandler
		{
		double initialsolution;
		std::vector<FMTLpHeuristic*> heuristics;
		size_t bestHeuristic() const;
		void resetNumberOfThreads(const unsigned int& ncpu) const;
		public:
		FMTLpHeuristicMtHandler();
		FMTLpHeuristicMtHandler(std::vector<FMTOperatingAreaScheduler>& lheuristics, const double& linitialsolution);
		FMTLpHeuristicMtHandler(std::vector<FMTOperatingAreaClusterer>& lheuristics, const double& linitialsolution);
		#if defined FMTWITHPYTHON
			FMTLpHeuristicMtHandler(boost::python::list& lheuristics, const double& linitialsolution);
		#endif
		size_t initialSolve() const;
		size_t greedySolve(const unsigned int& iterations, const double& maxtime) const;
	    };
}
//BOOST_CLASS_EXPORT_KEY(Heuristics::FMTLpHeuristicMtHandler)
#endif
#endif
