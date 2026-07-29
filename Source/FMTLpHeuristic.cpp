/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifdef FMTWITHOSI

#include "FMTLpHeuristic.h"
#include <algorithm>
#include <random>
#include "FMTsolverinterface.h"
#include "FMTExceptionHandler.h"

namespace Heuristics
{

	FMTLpHeuristic::FMTLpHeuristic(const FMTLpHeuristic& rhs) :
		Models::FMTLpSolver(rhs),
		m_generator(rhs.m_generator),m_seed(rhs.m_seed),m_usingsolvercopy(true)
		{

		}
	FMTLpHeuristic& FMTLpHeuristic::operator = (const FMTLpHeuristic& rhs)
		{
		if (this!=&rhs)
			{
			FMTLpSolver::operator = (rhs);
			m_generator=rhs.m_generator;
			m_seed = rhs.m_seed;
			m_usingsolvercopy = true;
			}
		return *this;
		}

    FMTLpHeuristic::FMTLpHeuristic(const Models::FMTsolverinterface& interfacetype,const size_t& lseed):
       Models::FMTLpSolver(interfacetype,std::string(), std::string(),"LpHeuristic_"+std::to_string(lseed)),
		m_generator(static_cast<unsigned int>(lseed)), m_seed(lseed), m_usingsolvercopy(true/*false*/)
        {
        //this->buildSolverInterface(interfacetype);
        }

	FMTLpHeuristic::FMTLpHeuristic(Models::FMTLpSolver& basesolve, size_t lseed,bool copysolver):
		Models::FMTLpSolver(),m_generator(static_cast<unsigned int>(lseed)),m_seed(lseed), m_usingsolvercopy(copysolver)
		{
		try {
			if (copysolver)
				{
				FMTLpSolver::operator=(basesolve);
			}else {
				FMTLpSolver::passInSolver(basesolve);
				}
		}catch (...)
			{
			_exhandler->raiseFromCatch("","FMTLpHeuristic::FMTLpHeuristic", __LINE__, __FILE__);
			}
		}


	void FMTLpHeuristic::setGeneratorSeed(const size_t& lseed)
		{
		m_seed = lseed;
		m_generator.seed(static_cast<unsigned int>(lseed));
		}

	bool FMTLpHeuristic::branchNBoundSolve()
		{
		try {
			Models::FMTLpSolver::branchAndBound();
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTLpHeuristic::branchNBoundSolve", __LINE__, __FILE__);
			}
		return Models::FMTLpSolver::isProvenOptimal();
       }
	bool FMTLpHeuristic::greedyPass(const double& initsol,const unsigned int& iteration)
	{
		///Need to be implemented
		try {
			initialSolve();
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTLpHeuristic::greedyPass", __LINE__, __FILE__);
			}
		return Models::FMTLpSolver::isProvenOptimal();
	  }

	void FMTLpHeuristic::parallelOptimize(const double& initbestsolution, const unsigned int& iterations,const double& maxtime, const std::chrono::steady_clock::time_point& Starttime)
		{
			unsigned int pass = 0;
			bool mxtreach = false;
			double mxt;
			while(pass <= iterations &&  !mxtreach)
			{
				greedyPass(initbestsolution,pass);
				const std::chrono::steady_clock::time_point Enditer = std::chrono::steady_clock::now();
				std::chrono::duration<double,std::milli> Elapsed = Enditer - Starttime;
				if(pass==0)
				{
					// Only to not start a new iteration if its gonna take long time and convert time in milliseconds.. I know not the most esthetic way
					mxt = maxtime*1000 - Elapsed.count();
				}
				if (Elapsed.count() > mxt)
				{
					mxtreach = true;
				}
				++pass;
			}
			(*_logger) << "Best solution found objective: "+std::to_string(getObjValue())+" after "+std::to_string(pass-1)+" iterations --- Waiting for other threads." << "\n";
		}

}
BOOST_CLASS_EXPORT_IMPLEMENT(Heuristics::FMTLpHeuristic)
#endif
