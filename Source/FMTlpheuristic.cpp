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
       Models::FMTlpsolver(interfacetype),generator(static_cast<unsigned int>(lseed)),seed(lseed), usingsolvercopy(true/*false*/)
        {
        //this->buildsolverinterface(interfacetype);
        }

	FMTlpheuristic::FMTlpheuristic(Models::FMTlpsolver& basesolve, size_t lseed,bool copysolver):
		Models::FMTlpsolver(),generator(static_cast<unsigned int>(lseed)),seed(lseed), usingsolvercopy(copysolver)
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

	void FMTlpheuristic::branchnboundsolve()
		{
		try {
			Models::FMTlpsolver::branchAndBound();
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTlpheuristic::branchnboundsolve", __LINE__, __FILE__);
			}
       }

    void FMTlpheuristic::passinobject(const FMTobject& rhs)
        {
		FMTlpsolver::passinobject(rhs);
        FMTlpsolver::passinmessagehandler(*_logger);
        }

}
BOOST_CLASS_EXPORT_IMPLEMENT(Heuristics::FMTlpheuristic)
#endif
