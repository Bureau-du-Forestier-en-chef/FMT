/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/
#ifdef FMTWITHOSI
#include <boost/thread.hpp>
#include "FMTlpheuristicmthandler.hpp"

#include <functional>
#if defined FMTWITHPYTHON
	#include "boost/python.hpp"
#endif
#include "FMTlpheuristic.hpp"
#include "FMToperatingareascheduler.hpp"
#include "FMToperatingareaclusterer.hpp"
namespace Heuristics
{
		FMTlpheuristicmthandler::FMTlpheuristicmthandler():heuristics()
		{

		}

		FMTlpheuristicmthandler::FMTlpheuristicmthandler(std::vector<FMToperatingareascheduler>& lheuristics, const double& linitialsolution):heuristics(),initialsolution(linitialsolution)
		{
			for (FMToperatingareascheduler& heur : lheuristics)
			{
				heuristics.push_back(&heur);
			}
		}
		FMTlpheuristicmthandler::FMTlpheuristicmthandler(std::vector<FMToperatingareaclusterer>& lheuristics, const double& linitialsolution):heuristics(),initialsolution(linitialsolution)
		{
			for (FMToperatingareaclusterer& heur : lheuristics)
			{
				heuristics.push_back(&heur);
			}
		}

		#if defined FMTWITHPYTHON
		FMTlpheuristicmthandler::FMTlpheuristicmthandler(boost::python::list& lheuristics, const double& linitialsolution):heuristics(),initialsolution(linitialsolution)
		{
			for (int i = 0; i < len(lheuristics); ++i)
			{
				heuristics.push_back(boost::python::extract<FMTlpheuristic*>(lheuristics[i]));
			}
		}
		#endif

		size_t FMTlpheuristicmthandler::best_heuristic() const
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

		void FMTlpheuristicmthandler::reset_number_of_threads(const unsigned int& ncpu) const
		{
			for(std::size_t i = 0; i < heuristics.size(); ++i)
			{
				heuristics[i]->setnumberofthreads(static_cast<size_t>(ncpu));
			}

		}

		size_t FMTlpheuristicmthandler::initialsolve() const
		{
			const unsigned int processor_count = boost::thread::hardware_concurrency();
			int mosek_process = processor_count/heuristics.size();
			if (mosek_process<1){mosek_process=1;}
			std::vector<boost::thread>threads;
			for(std::size_t i = 0; i < heuristics.size(); ++i)
			{
				FMTlpheuristic* heuristic = heuristics[i];
				heuristic->setnumberofthreads(static_cast<size_t>(mosek_process));
				heuristic->setnothread(static_cast<int>(i));
				threads.push_back(boost::thread(std::bind(&FMTlpheuristic::initialsolve, heuristic)));
			}
			for(boost::thread& thr : threads)
			{
				thr.join();
			}
			reset_number_of_threads(processor_count);
			return best_heuristic();
		}

		size_t FMTlpheuristicmthandler::greedysolve(const unsigned int& iterations, const double& maxtime) const
		{
			const unsigned int processor_count = boost::thread::hardware_concurrency();
			int mosek_process = processor_count/heuristics.size();
			if (mosek_process<1){mosek_process=1;}
			std::vector<boost::thread>threads;
			const std::chrono::steady_clock::time_point Start = std::chrono::steady_clock::now();
			for(std::size_t i = 0; i < heuristics.size(); ++i)
			{
				FMTlpheuristic* heuristic = heuristics[i];
				heuristic->setnumberofthreads(static_cast<size_t>(mosek_process));
				heuristic->setnothread(static_cast<int>(i));
				threads.push_back(boost::thread(std::bind(&FMTlpheuristic::paralleloptimize, heuristic, initialsolution,iterations,maxtime,Start)));
			}
			for(boost::thread& thr : threads)
			{
				thr.join();
			}
			reset_number_of_threads(processor_count);
			return best_heuristic();
		}
}
BOOST_CLASS_EXPORT_IMPLEMENT(Heuristics::FMTlpheuristicmthandler)
#endif
