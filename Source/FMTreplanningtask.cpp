/*
Copyright (c) 2019 Gouvernement du Québec

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

	FMTreplanningtask::FMTreplanningtask(const Models::FMTmodel& globalm,
		const Models::FMTmodel& stochasticm,
		const Models::FMTmodel& localm,
		const std::string& outputlocation,
		const std::string& gdaldriver,
		const std::vector<std::string>& creationoptions,
		Core::FMToutputlevel outputlevel):
		resultswriter(),
		baseschedule(),
		global(),
		stochastic(),
		local(),
		replicateids(),
		dynamicarea(globalm.getarea()),
		iterationglobalschedule(),
		dynamicconstraints(),
		replanningperiods(globalm.getparameter(Models::FMTintmodelparameters::LENGTH)),
		globalsolutionweight(0)
	{
		try {
			//passinobject(globalm);
			global = std::move(globalm.clone());
			stochastic = std::move(stochasticm.clone());
			local = std::move(localm.clone());
			std::vector<Models::FMTmodel*>modelsptr;
			modelsptr.push_back(global.get());
			modelsptr.push_back(stochastic.get());
			modelsptr.push_back(local.get());
			resultswriter = std::shared_ptr<FMTparallelwriter>(new FMTparallelwriter(outputlocation, gdaldriver, outputs, modelsptr, creationoptions, minimaldrift, outputlevel));
			std::unique_ptr<Models::FMTmodel>modelcpy = global->clone();
			_logger->logwithlevel("Initial planning started\n", 0);
			if (!modelcpy->doplanning(true))
			{
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
					"Infeasible Global model",
					"FMTreplanningtask::FMTreplanningtask", __LINE__, __FILE__);
			}
			replicateids.push(0);
			writeresults(global->getname(),
				global->getparameter(Models::FMTintmodelparameters::LENGTH), modelcpy, 1);
			_logger->logwithlevel("Initial planning done\n", 0);
			replicateids.pop();
			baseschedule = std::shared_ptr<Core::FMTschedule>(new Core::FMTschedule(modelcpy->getsolution(1, true)));
			iterationglobalschedule = *baseschedule;
			dynamicconstraints = modelcpy->getreplanningconstraints("GLOBAL", local->getconstraints(), 1);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTreplanningtask::FMTreplanningtask", __LINE__, __FILE__);
		}
	}

	void setreplicates(const int& replicatesnumber)
	{
		replicateids=std::queue<int>();
		for (int replicateid = 1; replicateid < (replicatesnumber + 1); ++replicateid)
		{
			replicateids.push(replicateid);
		}
	}

	void setreplanningperiods(const int& periodsnumber)
		{
		replanningperiods = periodsnumber;
		}

	void setglobalweight(const double& weight)
		{
		globalsolutionweight = weight;
		}


	FMTreplanningtask::FMTreplanningtask(
		const Models::FMTmodel& globalm,
		const Models::FMTmodel& stochasticm,
		const Models::FMTmodel& localm,
		const std::vector<Core::FMToutput>& outputs,
		const std::string& outputlocation,
		const std::string& gdaldriver,
		const std::vector<std::string>& creationoptions,
		const int& replicates,
		const int& replanningperiodssize,
		const double& globalwweight,
		const double& minimaldrift,
		Core::FMToutputlevel outputlevel):
		resultswriter(),
		baseschedule(),
		global(),
		stochastic(),
		local(),
		replicateids(),
		dynamicarea(globalm.getarea()),
		iterationglobalschedule(),
		dynamicconstraints(),
		replanningperiods(replanningperiodssize),
		globalsolutionweight(globalwweight)
	{
		try {
			//passinobject(globalm);
			global = std::move(globalm.clone());
			stochastic = std::move(stochasticm.clone());
			local = std::move(localm.clone());
			std::vector<Models::FMTmodel*>modelsptr;
			modelsptr.push_back(global.get());
			modelsptr.push_back(stochastic.get());
			modelsptr.push_back(local.get());
			resultswriter = std::shared_ptr<FMTparallelwriter>(new FMTparallelwriter(outputlocation, gdaldriver, outputs, modelsptr, creationoptions,minimaldrift,outputlevel));
			std::unique_ptr<Models::FMTmodel>modelcpy = global->clone();
			_logger->logwithlevel("Initial planning started\n", 0);
			if (!modelcpy->doplanning(true))
			{
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
					"Infeasible Global model",
					"FMTreplanningtask::FMTreplanningtask", __LINE__, __FILE__);
			}
			replicateids.push(0);
			writeresults(global->getname(),
				global->getparameter(Models::FMTintmodelparameters::LENGTH),modelcpy,1);
			_logger->logwithlevel("Initial planning done\n", 0);
			replicateids.pop();
			baseschedule = std::shared_ptr<Core::FMTschedule>(new Core::FMTschedule(modelcpy->getsolution(1, true)));
			iterationglobalschedule = *baseschedule;
			dynamicconstraints = modelcpy->getreplanningconstraints("GLOBAL",local->getconstraints(), 1);
			for (int replicateid = 1; replicateid < (replicates+1); ++replicateid)
			{
				replicateids.push(replicateid);
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTreplanningtask::FMTreplanningtask", __LINE__, __FILE__);
		}
	}

	FMTreplanningtask::~FMTreplanningtask()
	{
		try {
			if (resultswriter&&
				resultswriter.use_count()==1)
				{
				/////
				resultswriter->setdriftprobability(global->getname(), local->getname());///testt
				////
				}
		}catch (...)
		{
			_exhandler->raisefromcatch("", "FMTreplanningtask::~FMTreplanningtask", __LINE__, __FILE__);
		}
	}

	std::unique_ptr<FMTtask> FMTreplanningtask::clone() const
		{
		return std::unique_ptr<FMTtask>(new FMTreplanningtask(*this));
		}

	std::vector<std::unique_ptr<FMTtask>>FMTreplanningtask::split(const unsigned int& numberoftasks) const
	{
		std::vector<std::unique_ptr<FMTtask>>tasks;
		try {
			size_t replicate = 0;
			int replicatepertask = (static_cast<int>(replicateids.size()) / numberoftasks);
			std::queue<int>allreplicates=this->replicateids;
			for (size_t taskid = 0; taskid < numberoftasks; ++taskid)
				{
				FMTreplanningtask newtask(*this);
				std::queue<int>replicatesoftask;
				for (int taskreplicate = 0 ; taskreplicate < replicatepertask;++taskreplicate)
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
				tasks.push_back(std::move(newtask.clone()));
				}
			
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTreplanningtask::gettasks", __LINE__, __FILE__);
			}
		return tasks;
	}

	std::unique_ptr<FMTtask>FMTreplanningtask::spawn()
	{
		try {
			if (!replicateids.empty())
				{
				FMTreplanningtask newtask(*this);
				std::queue<int>singlereplicate;
				singlereplicate.push(replicateids.front());
				replicateids.pop();
				newtask.replicateids.swap(singlereplicate);
				return std::move(newtask.clone());
				}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTreplanningtask::spawn", __LINE__, __FILE__);
			}
	return std::unique_ptr<FMTtask>(nullptr);
	}

	void FMTreplanningtask::writeresults(const std::string& modelname, const int& modellength,
		const std::unique_ptr<Models::FMTmodel>& modelptr,const int& replanningperiod,bool onlyfirstperiod)
	{
		try {
			if (replanningperiod <= replanningperiods)//Dont write outside the replanningsperiods
			{
				int modelsize = modellength;
				int firstperiod = dynamicarea.begin()->getperiod() + 1;
				int lastperiod = firstperiod;
				if (!modelptr)//infeasible!
				{
					//put NAN everywhere the size of 
					modelsize = (replanningperiods - replanningperiod) + 1;
					onlyfirstperiod = false;
					firstperiod = replanningperiod;
				}
				if (!onlyfirstperiod)
				{
					lastperiod += modelsize;
					if (firstperiod == 1)
					{
						--lastperiod;
					}
				}
				
				const std::map<std::string, std::vector<std::vector<double>>>results = resultswriter->getresults(modelptr, firstperiod, lastperiod);
				if (modelsize == 1)
				{
					firstperiod = replanningperiod;
					lastperiod = replanningperiod;
				}
				_logger->logwithlevel("Writing results for " + modelname + " first period at: " + 
					std::to_string(firstperiod)+" for iteration "+ std::to_string(getiteration()) + "\n", 1);
				resultswriter->write(modelname, results, firstperiod, lastperiod, getiteration());
			}
		}catch (...)
		{
			_exhandler->raisefromcatch("", "FMTreplanningtask::writeresults", __LINE__, __FILE__);
		}
	}

	const int FMTreplanningtask::getiteration() const
		{
		return replicateids.front();
		}

	void FMTreplanningtask::passinlogger(const std::shared_ptr<Logging::FMTlogger>& logger)
		{
		global->passinlogger(logger);
		stochastic->passinlogger(logger);
		local->passinlogger(logger);
		}

	void FMTreplanningtask::setreignore(std::unique_ptr<Models::FMTmodel>& modelcpy, const int& replanningperiod) const
	{
		try {
			std::vector<Core::FMTconstraint>newconstraints;
			for (const Core::FMTconstraint& constraint : modelcpy->getconstraints())
				{
				if (!constraint.isreignore(replanningperiod))
					{
					newconstraints.push_back(constraint);
					}
				}
			modelcpy->setconstraints(newconstraints);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTreplanningtask::setreignore", __LINE__, __FILE__);
		}
	}

	void FMTreplanningtask::setreplicate(std::unique_ptr<Models::FMTmodel>& modelcpy, const int& replanningperiod) const
	{
		try {
			std::vector<Core::FMTconstraint>newconstraints;
			for (const Core::FMTconstraint& basenssconstraint : modelcpy->getconstraints())
				{
				newconstraints.push_back(basenssconstraint.getfromreplicate(getiteration(), replanningperiod));
				}
			modelcpy->setconstraints(newconstraints);
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTreplanningtask::setreplicate", __LINE__, __FILE__);
		}
	}

	void FMTreplanningtask::work()
	{
		try {
			while (!replicateids.empty())
			{
				_logger->logwithlevel("Replanning on replicate " + std::to_string(getiteration()) + " started\n",0);
				
				for (int replanningperiod = 1; replanningperiod <= replanningperiods; ++replanningperiod)
				{
					if (replanningperiod != 1)
					{
						const std::unique_ptr<Models::FMTmodel>globalcopy = std::move(domodelplanning(global,replanningperiod,true));
						if (!globalcopy)//infeasible replicate end here
							{
							//Also write infeasible for stochastic
							writeresults(stochastic->getname(),
								replanningperiods,nullptr, replanningperiod);
							//Write infeasible for local
							writeresults(local->getname(),
								replanningperiods, nullptr, replanningperiod);
							break;
							}
					}

					
					const std::unique_ptr<Models::FMTmodel> stochasticcopy = std::move(domodelplanning(stochastic,replanningperiod,false,false,false));
					for (Core::FMTactualdevelopment& developement : dynamicarea)
						{
						developement.setperiod(0);
						}
					const std::unique_ptr<Models::FMTmodel> localcopy = std::move(domodelplanning(local, replanningperiod,false,true));
					if (!localcopy)//infeasible replicate end here
						{
						//Write infeasible for next global
						writeresults(global->getname(),
								replanningperiods, nullptr, replanningperiod + 1);
						//Also write infeasible for next stochastic
						writeresults(stochastic->getname(),
								replanningperiods, nullptr, replanningperiod + 1);
						break;
						}
					dynamicarea = localcopy->getarea(localcopy->getparameter(Models::FMTintmodelparameters::LENGTH)+1);
					for (Core::FMTactualdevelopment& developement : dynamicarea)
					{
						developement.setperiod(replanningperiod);
					}
				}
				dynamicarea = global->getarea();
				iterationglobalschedule = *baseschedule;
				_logger->logwithlevel("Replanning on replicate " + std::to_string(getiteration()) + " done\n", 0);
				replicateids.pop();
			}
			setstatus(true);
		}catch (...)
		{
			_exhandler->raisefromcatch("", "FMTreplanningtask::work", __LINE__, __FILE__);
		}

	}


	std::unique_ptr<Models::FMTmodel>FMTreplanningtask::domodelplanning(
		const std::shared_ptr<Models::FMTmodel>model,
		const int& replanningperiod,
		bool getsolutionandlocal,
		bool applyscheduleweight,
		bool setdynamicconstraints)
	{
		bool optimal = false;
		try {
			std::unique_ptr<Models::FMTmodel>modelcpy = std::move(model->clone());
			int modelsize = modelcpy->getparameter(Models::FMTintmodelparameters::LENGTH);
			modelcpy->setparameter(Models::FMTintmodelparameters::SEED, static_cast<int>(getiteration()));//For stochastic
			const std::string modelname = modelcpy->getname();
			bool writefirstperiodonly = true;
			modelcpy->setarea(dynamicarea);
			if (setdynamicconstraints)
				{
				modelcpy->setconstraints(dynamicconstraints);
				}
			setreignore(modelcpy, replanningperiod);
			setreplicate(modelcpy, replanningperiod);
			if (applyscheduleweight)//local is here
			{
				bool solvedmodel = false;
				#ifdef FMTWITHOSI
					Models::FMTlpmodel* lpmodel = dynamic_cast<Models::FMTlpmodel*>(modelcpy.get());
					lpmodel->doplanning(false);
					lpmodel->addscheduletoobjective(iterationglobalschedule, globalsolutionweight);
					solvedmodel = lpmodel->initialsolve();
				#else
					solvedmodel = modelcpy->doplanning(true);
				#endif
				if (solvedmodel)
					{
					optimal = true;
				}else {
					_exhandler->raise(Exception::FMTexc::FMTignore,
						"Infeasible model named: " + modelcpy->getname() + " at iteration " + std::to_string(getiteration()) + " at replanning period " + std::to_string(replanningperiod),
						"FMTreplanningtask::domodelplanning", __LINE__, __FILE__);
					modelcpy = std::move(std::unique_ptr<Models::FMTmodel>(nullptr));
				}
				
			
				if (modelsize>1)
					{
					writefirstperiodonly = false;
					}
				dynamicconstraints = modelcpy->getreplanningconstraints("LOCAL", global->getconstraints(), modelsize);
				//lpmodel->writeLP("C:/Users/cyrgu3/Desktop/test/FMT2/FMT/build/debug/bin/Debug/tests/testlocal"+std::to_string(replanningperiod));
			}else {
				if (modelcpy->doplanning(true))
				{
					optimal = true;
					if (getsolutionandlocal)
					{
						dynamicconstraints = modelcpy->getreplanningconstraints("GLOBAL",local->getconstraints(), replanningperiod + 1);
						iterationglobalschedule = modelcpy->getsolution(replanningperiod, true);
						iterationglobalschedule.setperiod(1);
						if (iterationglobalschedule.empty())
							{
							_exhandler->raise(Exception::FMTexc::FMTignore,
								"Empty schedule generated for model "+model->getname()+" at replanning period "+std::to_string(replanningperiod),
								"FMTreplanningtask::domodelplanning", __LINE__, __FILE__);
						}
					}
				}else {
					_exhandler->raise(Exception::FMTexc::FMTignore,
						"Infeasible model named: "+modelcpy->getname()+" at iteration "+std::to_string(getiteration())+ " at replanning period " + std::to_string(replanningperiod),
						"FMTreplanningtask::domodelplanning", __LINE__, __FILE__);

					modelcpy = std::move(std::unique_ptr<Models::FMTmodel>(nullptr));
				}
			}
			writeresults(modelname,modelsize,modelcpy, replanningperiod, writefirstperiodonly);
			return std::move(modelcpy);
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTreplanningtask::domodelplanning", __LINE__, __FILE__);
			}
		return std::move(std::unique_ptr<Models::FMTmodel>(nullptr));
	}

}