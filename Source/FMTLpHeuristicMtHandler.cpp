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
		FMTLpHeuristicMtHandler::FMTLpHeuristicMtHandler():m_heuristics()
		{

		}

		FMTLpHeuristicMtHandler::FMTLpHeuristicMtHandler(std::vector<FMTOperatingAreaScheduler>& lheuristics, const double& linitialsolution):m_heuristics(),m_initialsolution(linitialsolution)
		{
			for (FMTOperatingAreaScheduler& heur : lheuristics)
			{
				m_heuristics.push_back(&heur);
			}
		}
		FMTLpHeuristicMtHandler::FMTLpHeuristicMtHandler(std::vector<FMTOperatingAreaClusterer>& lheuristics, const double& linitialsolution):m_heuristics(),m_initialsolution(linitialsolution)
		{
			for (FMTOperatingAreaClusterer& heur : lheuristics)
			{
				m_heuristics.push_back(&heur);
			}
		}

		#if defined FMTWITHPYTHON
		FMTLpHeuristicMtHandler::FMTLpHeuristicMtHandler(boost::python::list& lheuristics, const double& linitialsolution):m_heuristics(),m_initialsolution(linitialsolution)
		{
			for (int i = 0; i < len(lheuristics); ++i)
			{
				m_heuristics.push_back(boost::python::extract<FMTLpHeuristic*>(lheuristics[i]));
			}
		}
		#endif

		size_t FMTLpHeuristicMtHandler::_bestHeuristic() const
		{
			double sol=m_heuristics[0]->getObjValue();
			size_t bestsol=0;
			const double sens = m_heuristics.at(0)->getObjSense();
			for(std::size_t i = 1; i < m_heuristics.size(); ++i)
			{
				const double value = m_heuristics[i]->getObjValue();
				if(value*sens<sol*sens)
				{
					sol=value;
					bestsol=i;
				}
			}
			return bestsol;

		}

		void FMTLpHeuristicMtHandler::_resetNumberOfThreads(const unsigned int& ncpu) const
		{
			for(std::size_t i = 0; i < m_heuristics.size(); ++i)
			{
				m_heuristics[i]->setNumberOfThreads(static_cast<size_t>(ncpu));
			}

		}

		size_t FMTLpHeuristicMtHandler::initialSolve() const
		{
			const unsigned int processor_count = boost::thread::hardware_concurrency();
			int mosek_process = static_cast<int>(processor_count/m_heuristics.size());
			if (mosek_process<1){mosek_process=1;}
			std::vector<boost::thread>threads;
			for(std::size_t i = 0; i < m_heuristics.size(); ++i)
			{
				FMTLpHeuristic* heuristic = m_heuristics[i];
				heuristic->setNumberOfThreads(static_cast<size_t>(mosek_process));
				threads.push_back(boost::thread(std::bind(&FMTLpHeuristic::initialSolve, heuristic)));
			}
			for(boost::thread& thr : threads)
			{
				thr.join();
			}
			_resetNumberOfThreads(processor_count);
			return _bestHeuristic();
		}

		size_t FMTLpHeuristicMtHandler::greedySolve(const unsigned int& iterations, const double& maxtime) const
		{
			const unsigned int processor_count = boost::thread::hardware_concurrency();
			int mosek_process = static_cast<int>(processor_count/m_heuristics.size());
			if (mosek_process<1){mosek_process=1;}
			std::vector<boost::thread>threads;
			const std::chrono::steady_clock::time_point Start = std::chrono::steady_clock::now();
			for(std::size_t i = 0; i < m_heuristics.size(); ++i)
			{
				FMTLpHeuristic* heuristic = m_heuristics[i];
				heuristic->setNumberOfThreads(static_cast<size_t>(mosek_process));
				threads.push_back(boost::thread(std::bind(&FMTLpHeuristic::parallelOptimize, heuristic, m_initialsolution,iterations,maxtime,Start)));
			}
			for(boost::thread& thr : threads)
			{
				thr.join();
			}
			_resetNumberOfThreads(processor_count);
			return _bestHeuristic();
		}
}
//BOOST_CLASS_EXPORT_IMPLEMENT(Heuristics::FMTLpHeuristicMtHandler)
#endif
