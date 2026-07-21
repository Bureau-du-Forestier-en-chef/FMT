/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/
#ifdef FMTWITHOSI
#include <boost/thread.hpp>
#include "FMTLpHeuristicMtHandler.h"

#include <functional>
#if defined FMTWITHPYTHON
	#include "boost/python.h"
#endif
#include "FMTLpHeuristic.h"
#include "FMTOperatingAreaScheduler.h"
#include "FMTOperatingAreaClusterer.h"
namespace Heuristics
{
		FMTLpHeuristicMtHandler::FMTLpHeuristicMtHandler():heuristics()
		{

		}

		FMTLpHeuristicMtHandler::FMTLpHeuristicMtHandler(std::vector<FMTOperatingAreaScheduler>& lheuristics, const double& linitialsolution):heuristics(),initialsolution(linitialsolution)
		{
			for (FMTOperatingAreaScheduler& heur : lheuristics)
			{
				heuristics.push_back(&heur);
			}
		}
		FMTLpHeuristicMtHandler::FMTLpHeuristicMtHandler(std::vector<FMTOperatingAreaClusterer>& lheuristics, const double& linitialsolution):heuristics(),initialsolution(linitialsolution)
		{
			for (FMTOperatingAreaClusterer& heur : lheuristics)
			{
				heuristics.push_back(&heur);
			}
		}

		#if defined FMTWITHPYTHON
		FMTLpHeuristicMtHandler::FMTLpHeuristicMtHandler(boost::python::list& lheuristics, const double& linitialsolution):heuristics(),initialsolution(linitialsolution)
		{
			for (int i = 0; i < len(lheuristics); ++i)
			{
				heuristics.push_back(boost::python::extract<FMTLpHeuristic*>(lheuristics[i]));
			}
		}
		#endif

		size_t FMTLpHeuristicMtHandler::bestHeuristic() const
		{
			double sol=heuristics[0]->getObjValue();
			size_t bestsol=0;
			const double sens = heuristics.at(0)->getObjSense();
			for(std::size_t i = 1; i < heuristics.size(); ++i)
			{
				const double value = heuristics[i]->getObjValue();
				if(value*sens<sol*sens)
				{
					sol=value;
					bestsol=i;
				}
			}
			return bestsol;

		}

		void FMTLpHeuristicMtHandler::resetNumberOfThreads(const unsigned int& ncpu) const
		{
			for(std::size_t i = 0; i < heuristics.size(); ++i)
			{
				heuristics[i]->setNumberOfThreads(static_cast<size_t>(ncpu));
			}

		}

		size_t FMTLpHeuristicMtHandler::initialSolve() const
		{
			const unsigned int processor_count = boost::thread::hardware_concurrency();
			int mosek_process = static_cast<int>(processor_count/heuristics.size());
			if (mosek_process<1){mosek_process=1;}
			std::vector<boost::thread>threads;
			for(std::size_t i = 0; i < heuristics.size(); ++i)
			{
				FMTLpHeuristic* heuristic = heuristics[i];
				heuristic->setNumberOfThreads(static_cast<size_t>(mosek_process));
				threads.push_back(boost::thread(std::bind(&FMTLpHeuristic::initialSolve, heuristic)));
			}
			for(boost::thread& thr : threads)
			{
				thr.join();
			}
			resetNumberOfThreads(processor_count);
			return bestHeuristic();
		}

		size_t FMTLpHeuristicMtHandler::greedySolve(const unsigned int& iterations, const double& maxtime) const
		{
			const unsigned int processor_count = boost::thread::hardware_concurrency();
			int mosek_process = static_cast<int>(processor_count/heuristics.size());
			if (mosek_process<1){mosek_process=1;}
			std::vector<boost::thread>threads;
			const std::chrono::steady_clock::time_point Start = std::chrono::steady_clock::now();
			for(std::size_t i = 0; i < heuristics.size(); ++i)
			{
				FMTLpHeuristic* heuristic = heuristics[i];
				heuristic->setNumberOfThreads(static_cast<size_t>(mosek_process));
				threads.push_back(boost::thread(std::bind(&FMTLpHeuristic::parallelOptimize, heuristic, initialsolution,iterations,maxtime,Start)));
			}
			for(boost::thread& thr : threads)
			{
				thr.join();
			}
			resetNumberOfThreads(processor_count);
			return bestHeuristic();
		}
}
//BOOST_CLASS_EXPORT_IMPLEMENT(Heuristics::FMTLpHeuristicMtHandler)
#endif
