/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/
#ifdef FMTWITHOSI
#include "FMTlpheuristicmthandler.h"
#include <thread>
#include <functional>
#if defined FMTWITHPYTHON
	#include "boost/python.hpp"
#endif

namespace Heuristics
{
		FMTlpheuristicmthandler::FMTlpheuristicmthandler():heuristics()
		{

		}

		FMTlpheuristicmthandler::FMTlpheuristicmthandler(std::vector<FMToperatingareascheduler>& lheuristics):heuristics()
		{
			for (FMToperatingareascheduler& heur : lheuristics)
			{
				heuristics.push_back(&heur);
			}
		}
		FMTlpheuristicmthandler::FMTlpheuristicmthandler(std::vector<FMToperatingareaclusterer>& lheuristics):heuristics()
		{
			for (FMToperatingareaclusterer& heur : lheuristics)
			{
				heuristics.push_back(&heur);
			}
		}

		#if defined FMTWITHPYTHON
		FMTlpheuristicmthandler::FMTlpheuristicmthandler(boost::python::list& lheuristics):heuristics()
		{
			for (int i = 0; i < len(lheuristics); ++i)
			{
				heuristics.push_back(boost::python::extract<FMTlpheuristic*>(lheuristics[i]));
			}
		}
		#endif
		size_t FMTlpheuristicmthandler::initialsolve() const
		{
			const unsigned int processor_count = std::thread::hardware_concurrency();
			int mosek_process = processor_count/heuristics.size();
			if (mosek_process<1){mosek_process=1;}
			std::vector<std::thread>threads;
			for(std::size_t i = 0; i < heuristics.size(); ++i)
			{
				FMTlpheuristic* heuristic = heuristics[i];
				heuristic->setnumberofthreads(static_cast<size_t>(mosek_process));
				//threads.push_back(std::thread([heuristic]{heuristic->parallelinitialsolve();}));
				threads.push_back(std::thread(std::bind(&FMTlpheuristic::parallelinitialsolve, heuristic,i)));
			}
			for(std::thread& thr : threads)
			{
				thr.join();
			}
			double sol=0;
			size_t bestsol=0;
			const double sens = heuristics.at(0)->getObjSense();
			for(std::size_t i = 0; i < heuristics.size(); ++i)
			{
				heuristics[i]->setnumberofthreads(static_cast<size_t>(processor_count));
			    const double value = heuristics[i]->getObjValue();
			    if(value*sens<sol*sens)
			    {
			    	sol=value;
			    	bestsol=i;
			    }
			}
			return bestsol;
		}
}
BOOST_CLASS_EXPORT_IMPLEMENT(Heuristics::FMTlpheuristicmthandler)
#endif
