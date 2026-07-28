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

	// DocString: FMTLpHeuristicMtHandler
	/**
	@brief Multithreading handler running several LP heuristics in parallel and keeping the best solution.
	*/
	class FMTEXPORT FMTLpHeuristicMtHandler
		{
		public:
		// DocString: FMTLpHeuristicMtHandler()
		/**
		@brief Default constructor for FMTLpHeuristicMtHandler.
		*/
		FMTLpHeuristicMtHandler();
		// DocString: FMTLpHeuristicMtHandler(std::vector<FMTOperatingAreaScheduler>&,const double&)
		/**
		@brief Construct a FMTLpHeuristicMtHandler from operating area schedulers and an initial solution.
		@param[in] lheuristics the scheduler heuristics.
		@param[in] linitialsolution the initial solution value.
		*/
		FMTLpHeuristicMtHandler(std::vector<FMTOperatingAreaScheduler>& lheuristics, const double& linitialsolution);
		// DocString: FMTLpHeuristicMtHandler(std::vector<FMTOperatingAreaClusterer>&,const double&)
		/**
		@brief Construct a FMTLpHeuristicMtHandler from operating area clusterers and an initial solution.
		@param[in] lheuristics the clusterer heuristics.
		@param[in] linitialsolution the initial solution value.
		*/
		FMTLpHeuristicMtHandler(std::vector<FMTOperatingAreaClusterer>& lheuristics, const double& linitialsolution);
		#if defined FMTWITHPYTHON
			// DocString: FMTLpHeuristicMtHandler(boost::python::list&,const double&)
			/**
			@brief Construct a FMTLpHeuristicMtHandler from a Python list of heuristics and an initial solution.
			@param[in] lheuristics the heuristics.
			@param[in] linitialsolution the initial solution value.
			*/
			FMTLpHeuristicMtHandler(boost::python::list& lheuristics, const double& linitialsolution);
		#endif
		// DocString: FMTLpHeuristicMtHandler::initialSolve
		/**
		@brief Solve each heuristic in parallel to find an initial solution and return the index of the best one.
		@return the best heuristic index.
		*/
		size_t initialSolve() const;
		// DocString: FMTLpHeuristicMtHandler::greedySolve
		/**
		@brief Improve the solution of each heuristic in parallel with greedy passes and return the index of the best one.
		@param[in] iterations the number of iterations.
		@param[in] maxtime the maximum time.
		@return the best heuristic index.
		*/
		size_t greedySolve(const unsigned int& iterations, const double& maxtime) const;
		private:
		double m_initialsolution;
		std::vector<FMTLpHeuristic*> m_heuristics;
		// DocString: FMTLpHeuristicMtHandler::_bestHeuristic
		/**
		@brief Return the index of the heuristic with the best solution.
		@return the best heuristic index.
		*/
		size_t _bestHeuristic() const;
		// DocString: FMTLpHeuristicMtHandler::_resetNumberOfThreads
		/**
		@brief Reset the number of threads used by each heuristic.
		@param[in] ncpu the number of cpus.
		*/
		void _resetNumberOfThreads(const unsigned int& ncpu) const;
	    };
}
//BOOST_CLASS_EXPORT_KEY(Heuristics::FMTLpHeuristicMtHandler)
#endif
#endif
