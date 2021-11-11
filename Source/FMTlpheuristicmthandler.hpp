/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/
#ifdef FMTWITHOSI

#ifndef FMTlpheuristicmthandler_H_INCLUDED
#define FMTlpheuristicmthandler_H_INCLUDED


#if defined FMTWITHPYTHON
	#include "boost/python.hpp"
#endif

#include "FMTobject.hpp"
#include <vector>




namespace Heuristics
{
	class FMToperatingareascheduler;
	class FMToperatingareaclusterer;
	class FMTlpheuristic;

	class FMTEXPORT FMTlpheuristicmthandler
		{
		double initialsolution;
		std::vector<FMTlpheuristic*> heuristics;
		size_t best_heuristic() const;
		void reset_number_of_threads(const unsigned int& ncpu) const;
		public:
		FMTlpheuristicmthandler();
		FMTlpheuristicmthandler(std::vector<FMToperatingareascheduler>& lheuristics, const double& linitialsolution);
		FMTlpheuristicmthandler(std::vector<FMToperatingareaclusterer>& lheuristics, const double& linitialsolution);
		#if defined FMTWITHPYTHON
			FMTlpheuristicmthandler(boost::python::list& lheuristics, const double& linitialsolution);
		#endif
		size_t initialsolve() const;
		size_t greedysolve(const unsigned int& iterations, const double& maxtime) const;
	    };
}
BOOST_CLASS_EXPORT_KEY(Heuristics::FMTlpheuristicmthandler)
#endif
#endif
