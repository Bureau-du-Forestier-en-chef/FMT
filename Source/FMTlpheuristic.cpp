/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifdef FMTWITHOSI

#include "FMTlpheuristic.h"
#include <algorithm>
#include <random>
#include "OsiClpSolverInterface.hpp"
#ifdef  FMTWITHMOSEK
	#include "OsiMskSolverInterface.hpp"
	#include "mosek.h"
#endif
//#include "FMTmatrixbuild.h"

namespace Heuristics
{

	FMTlpheuristic::FMTlpheuristic(const FMTlpheuristic& rhs) :
		Models::FMTlpsolver(rhs),
		generator(rhs.generator),seed(rhs.seed),usingsolvercopy(true),nothread(-1)
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
			nothread = -1;
			}
		return *this;
		}

    FMTlpheuristic::FMTlpheuristic(const Models::FMTsolverinterface& interfacetype,const size_t& lseed):
       Models::FMTlpsolver(interfacetype),generator(static_cast<unsigned int>(lseed)),seed(lseed), usingsolvercopy(true/*false*/),nothread(-1)
        {
        //this->buildsolverinterface(interfacetype);
        }

	FMTlpheuristic::FMTlpheuristic(Models::FMTlpsolver& basesolve, size_t lseed,bool copysolver):
		Models::FMTlpsolver(),generator(static_cast<unsigned int>(lseed)),seed(lseed), usingsolvercopy(copysolver),nothread(-1)
		{
		try {
			if (copysolver)
				{
				FMTlpsolver::operator=(basesolve);
			}else {
				FMTlpsolver::passinsolver(basesolve);
				}
		}catch (...)
			{
			_exhandler->raisefromcatch("","FMTlpheuristic::FMTlpheuristic", __LINE__, __FILE__);
			}
		}


	void FMTlpheuristic::setgeneratorseed(const size_t& lseed)
		{
		seed = lseed;
		generator.seed(static_cast<unsigned int>(lseed));
		}

	bool FMTlpheuristic::branchnboundsolve()
		{
		try {
			Models::FMTlpsolver::branchAndBound();
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTlpheuristic::branchnboundsolve", __LINE__, __FILE__);
			}
		return Models::FMTlpsolver::isProvenOptimal();
       }
	bool FMTlpheuristic::greedypass(const double& initsol,const unsigned int& iteration)
	{
		///Need to be implemented
		try {
			initialsolve();
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTlpheuristic::greedypass", __LINE__, __FILE__);
			}
		return Models::FMTlpsolver::isProvenOptimal();
	  }

	void FMTlpheuristic::paralleloptimize(const double& initbestsolution, const unsigned int& iterations,const double& maxtime, const std::chrono::steady_clock::time_point& Starttime)
		{
			unsigned int pass = 0;
			bool mxtreach = false;
			double mxt;
			while(pass <= iterations &&  !mxtreach)
			{
				greedypass(initbestsolution,pass);
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
			(*_logger) << addonthreadno()+"Best solution found objective: "+std::to_string(getObjValue())+" after "+std::to_string(pass-1)+" iterations --- Waiting for other threads." << "\n";
		}

    void FMTlpheuristic::passinobject(const FMTobject& rhs)
        {
		FMTlpsolver::passinobject(rhs);
        FMTlpsolver::passinmessagehandler(*_logger);
        }

    void FMTlpheuristic::setnothread(const int& lnothread)
    {
    	nothread=lnothread;
    }


}
BOOST_CLASS_EXPORT_IMPLEMENT(Heuristics::FMTlpheuristic)
#endif
