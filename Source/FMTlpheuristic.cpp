/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifdef FMTWITHOSI

#include "FMTlpheuristic.h"
#include <algorithm>
#include <random>
#include "FMTsolverinterface.h"
#include "FMTexceptionhandler.h"

namespace Heuristics
{

	FMTlpheuristic::FMTlpheuristic(const FMTlpheuristic& rhs) :
		Models::FMTlpsolver(rhs),
		generator(rhs.generator),seed(rhs.seed),usingsolvercopy(true)
		{

		}
	FMTlpheuristic& FMTlpheuristic::operator = (const FMTlpheuristic& rhs)
		{
		if (this!=&rhs)
			{
			FMTlpsolver::operator = (rhs);
			generator=rhs.generator;
			seed = rhs.seed;
			usingsolvercopy = true;
			}
		return *this;
		}

    FMTlpheuristic::FMTlpheuristic(const Models::FMTsolverinterface& interfacetype,const size_t& lseed):
       Models::FMTlpsolver(interfacetype,std::string(), std::string(),"LpHeuristic_"+std::to_string(lseed)),
		generator(static_cast<unsigned int>(lseed)), seed(lseed), usingsolvercopy(true/*false*/)
        {
        //this->buildSolverInterface(interfacetype);
        }

	FMTlpheuristic::FMTlpheuristic(Models::FMTlpsolver& basesolve, size_t lseed,bool copysolver):
		Models::FMTlpsolver(),generator(static_cast<unsigned int>(lseed)),seed(lseed), usingsolvercopy(copysolver)
		{
		try {
			if (copysolver)
				{
				FMTlpsolver::operator=(basesolve);
			}else {
				FMTlpsolver::passInSolver(basesolve);
				}
		}catch (...)
			{
			_exhandler->raisefromcatch("","FMTlpheuristic::FMTlpheuristic", __LINE__, __FILE__);
			}
		}


	void FMTlpheuristic::setGeneratorSeed(const size_t& lseed)
		{
		seed = lseed;
		generator.seed(static_cast<unsigned int>(lseed));
		}

	bool FMTlpheuristic::branchNBoundSolve()
		{
		try {
			Models::FMTlpsolver::branchAndBound();
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTlpheuristic::branchNBoundSolve", __LINE__, __FILE__);
			}
		return Models::FMTlpsolver::isProvenOptimal();
       }
	bool FMTlpheuristic::greedyPass(const double& initsol,const unsigned int& iteration)
	{
		///Need to be implemented
		try {
			initialSolve();
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTlpheuristic::greedyPass", __LINE__, __FILE__);
			}
		return Models::FMTlpsolver::isProvenOptimal();
	  }

	void FMTlpheuristic::parallelOptimize(const double& initbestsolution, const unsigned int& iterations,const double& maxtime, const std::chrono::steady_clock::time_point& Starttime)
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
BOOST_CLASS_EXPORT_IMPLEMENT(Heuristics::FMTlpheuristic)
#endif
