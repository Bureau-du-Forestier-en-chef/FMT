/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTreplanningtask.hpp"
#include "FMTparallelwriter.hpp"
#include "FMTmodel.hpp"
#include "FMToutput.hpp"
#include "FMTlpmodel.hpp"

namespace Parallel
{


	FMTreplanningtask::FMTreplanningtask(
		const Models::FMTmodel& globalm,
		const Models::FMTmodel& stochasticm,
		const Models::FMTmodel& localm,
		const std::vector<Core::FMToutput>& outputs,
		const std::string& outputlocation,
		const std::string& gdaldriver,
		const size_t& replicates,
		const int& replanningperiodssize,
		const double& globalwweight):
		resultswriter(new FMTparallelwriter(outputlocation, gdaldriver, outputs, globalm)),
		baseschedule(),
		global(),
		stochastic(),
		local(),
		replicateids(),
		dynamicarea(globalm.getarea()),
		iterationglobalschedule(),
		localconstraints(),
		replanningperiods(replanningperiodssize),
		globalsolutionweight(globalwweight)
	{
		try {
			passinobject(globalm);
			global = std::move(globalm.clone());
			stochastic = std::move(stochasticm.clone());
			local = std::move(localm.clone());
			std::unique_ptr<Models::FMTmodel>modelcpy = local->clone();
			if (!modelcpy->doplanning(true))
			{
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
					"Infeasible Global model",
					"FMTreplanningtask::FMTreplanningtask", __LINE__, __FILE__);
			}
			replicateids.push(0);
			writeresults(modelcpy);
			replicateids.pop();
			baseschedule = std::shared_ptr<Core::FMTschedule>(new Core::FMTschedule(modelcpy->getsolution(1, true)));
			localconstraints = modelcpy->getlocalconstraints(local->getconstraints(), 1);
			for (size_t replicateid = 0; replicateid < replicates; ++replicateid)
			{
				replicateids.push(replicateid);
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTreplanningtask::FMTreplanningtask", __LINE__, __FILE__);
		}
	}

	std::unique_ptr<FMTtask> FMTreplanningtask::clone() const
		{
		return std::unique_ptr<FMTtask>(new FMTreplanningtask(*this));
		}

	std::vector<std::unique_ptr<FMTtask>>FMTreplanningtask::split(const size_t& numberoftasks) const
	{
		std::vector<std::unique_ptr<FMTtask>>tasks;
		try {
			size_t replicate = 0;
			const size_t replicatepertask = (replicateids.size() / numberoftasks);
			std::queue<size_t>allreplicates=this->replicateids;
			for (size_t taskid = 0; taskid < numberoftasks; ++taskid)
				{
				FMTreplanningtask newtask(*this);
				std::queue<size_t>replicatesoftask;
				for (size_t taskreplicate = 0 ; taskreplicate < replicatepertask;++taskreplicate)
					{
					replicatesoftask.push(allreplicates.front());
					allreplicates.pop();
					}
				if ((taskid == numberoftasks - 1) && !allreplicates.empty())
					{
					replicatesoftask.push(allreplicates.front());
					allreplicates.pop();
					}
				newtask.replicateids = replicatesoftask;
				tasks.push_back(newtask.clone());
				}
			
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTreplanningtask::gettasks", __LINE__, __FILE__);
			}
		return tasks;
	}

	void FMTreplanningtask::writeresults(const std::unique_ptr<Models::FMTmodel>& modelptr, bool onlyfirstperiod)
	{
		try {
			int firstperiod = 1;
			int lastperiod = 1;
			if (!onlyfirstperiod)
				{
				lastperiod = modelptr->getparameter(Models::FMTintmodelparameters::LENGTH);
				}
			resultswriter->write(modelptr, firstperiod, lastperiod,getiteration());
		}catch (...)
		{
			_exhandler->raisefromcatch("", "FMTreplanningtask::writeresults", __LINE__, __FILE__);
		}
	}

	const size_t FMTreplanningtask::getiteration() const
		{
		return replicateids.front();
		}

	void FMTreplanningtask::work()
	{
		try {
			while (!replicateids.empty())
			{
				for (int replanningperiod = 1; replanningperiod <= replanningperiods; ++replanningperiod)
				{
					if (replanningperiod != 1)
					{
						const std::unique_ptr<Models::FMTmodel>globalcopy = domodelplanning(global,replanningperiod,true);
					}
					const std::unique_ptr<Models::FMTmodel> stochasticcopy = domodelplanning(stochastic,replanningperiod);
					for (Core::FMTactualdevelopment& developement : dynamicarea)
						{
						developement.setperiod(0);
						}
					local->setconstraints(localconstraints);
					const std::unique_ptr<Models::FMTmodel> localcopy = domodelplanning(local, replanningperiod,false,true);
					dynamicarea = localcopy->getarea(2);
					for (Core::FMTactualdevelopment& developement : dynamicarea)
					{
						developement.setperiod(replanningperiod);
					}
				}
				dynamicarea = global->getarea();
				iterationglobalschedule = *baseschedule;
				replicateids.pop();
			}
		}catch (...)
		{
			_exhandler->raisefromcatch("", "FMTreplanningtask::work", __LINE__, __FILE__);
		}

	}


	std::unique_ptr<Models::FMTmodel>FMTreplanningtask::domodelplanning(
		const std::shared_ptr<Models::FMTmodel>model,
		const int& replanningperiod,
		bool getsolutionandlocal,
		bool applyscheduleweight)
	{
		bool optimal = false;
		try {
			std::unique_ptr<Models::FMTmodel>modelcpy = model->clone();
			modelcpy->setarea(dynamicarea);
			if (applyscheduleweight)
			{
				Models::FMTlpmodel* lpmodel = dynamic_cast<Models::FMTlpmodel*>(modelcpy.get());
				lpmodel->doplanning(false);
				lpmodel->addscheduletoobjective(iterationglobalschedule, globalsolutionweight);
				if (lpmodel->initialsolve())
					{
					optimal = true;
				}else {
					modelcpy = std::unique_ptr<Models::FMTmodel>(nullptr);
				}
			}else {
				if (modelcpy->doplanning(true))
				{
					optimal = true;
					if (getsolutionandlocal)
					{
						localconstraints = modelcpy->getlocalconstraints(local->getconstraints(), replanningperiod + 1);
						iterationglobalschedule = modelcpy->getsolution(1, true);
					}
				}
				else {
					modelcpy = std::unique_ptr<Models::FMTmodel>(nullptr);
				}
			}
			writeresults(modelcpy,replanningperiod);
			return modelcpy;
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTreplanningtask::domodelplanning", __LINE__, __FILE__);
			}
		return std::unique_ptr<Models::FMTmodel>(nullptr);
	}

}