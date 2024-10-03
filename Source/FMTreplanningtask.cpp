/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTreplanningtask.h"
#include "FMTparallelwriter.h"
#include "FMTmodel.h"
#include "FMToutput.h"
#include "FMTlpmodel.h"
#include "FMTexceptionhandler.h"
#include <boost/filesystem.hpp>


namespace Parallel
{

	FMTreplanningtask::FMTreplanningtask(const Models::FMTmodel& globalm,
		const Models::FMTmodel& stochasticm,
		const Models::FMTmodel& localm,
		const std::string& outputlocation,
		const std::string& gdaldriver,
		const std::vector<std::string>& creationoptions,
		Core::FMToutputlevel outputlevel):
		FMTreplanningtask(globalm, stochasticm, localm,
			globalm.getoutputs(),outputlocation,
			gdaldriver, creationoptions,1,
			globalm.getparameter(Models::FMTintmodelparameters::LENGTH),
			0.5,outputlevel)
	{

	}

	void FMTreplanningtask::setreplicates(const int& replicatesnumber)
	{
		replicateids=std::queue<int>();
		for (int replicateid = 1; replicateid < (replicatesnumber + 1); ++replicateid)
		{
			replicateids.push(replicateid);
		}
	}

	void FMTreplanningtask::setreplanningperiods(const int& periodsnumber)
		{
		replanningperiods = periodsnumber;
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
		const double& minimaldrift,
		Core::FMToutputlevel outputlevel,
		const bool writeSchedule) :
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
		m_writeSchedule(writeSchedule)
	{
		try {
			//passinobject(globalm);
			if (globalm.getname() == stochasticm.getname() || globalm.getname() == localm.getname() || stochasticm.getname() == localm.getname())
			{
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
					"Two Model name is the same in strategic, stochastic or tactic",
					"FMTreplanningtask::FMTreplanningtask", __LINE__, __FILE__);
			}
			global = std::move(globalm.clone());
			stochastic = std::move(stochasticm.clone());
			local = std::move(localm.clone());
			global->setparameter(Models::FMTboolmodelparameters::PRESOLVE_CAN_REMOVE_STATIC_THEMES, false);
			stochastic->setparameter(Models::FMTboolmodelparameters::PRESOLVE_CAN_REMOVE_STATIC_THEMES, false);
			local->setparameter(Models::FMTboolmodelparameters::PRESOLVE_CAN_REMOVE_STATIC_THEMES, false);
			global->setparameter(Models::FMTintmodelparameters::UPDATE,1);
			stochastic->setparameter(Models::FMTintmodelparameters::UPDATE, 1);
			local->setparameter(Models::FMTintmodelparameters::UPDATE, 1);
			m_outputlocation = outputlocation;
			//m_outputlocation.pop_back();
			m_primaryName = boost::filesystem::path(m_outputlocation).stem().string();
			const size_t LENGTH = static_cast<size_t>(global->getparameter(Models::FMTintmodelparameters::LENGTH));
			const size_t AREA_SIZE = global->getarea().size();
			const size_t SCALE_FACTOR = 10;
			const size_t GLOBAL_RESERVE = AREA_SIZE * LENGTH * LENGTH * SCALE_FACTOR;
			const size_t BASE_RESERVE = AREA_SIZE * SCALE_FACTOR;
			Models::FMTsrmodel* GlobalPtr = dynamic_cast<Models::FMTsrmodel*>(global.get());
			GlobalPtr->setNodeCacheSize(GLOBAL_RESERVE);
			Models::FMTsrmodel* LocalPtr = dynamic_cast<Models::FMTsrmodel*>(local.get());
			LocalPtr->setNodeCacheSize(BASE_RESERVE);
			Models::FMTsrmodel* StochasticPtr = dynamic_cast<Models::FMTsrmodel*>(stochastic.get());
			StochasticPtr->setNodeCacheSize(BASE_RESERVE);
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
			const std::string OBJ_VALUE = std::to_string(modelcpy->getobjectivevalue());
			_logger->logwithlevel("Initial planning Obj("+ OBJ_VALUE +") done\n", 0);
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
			_exhandler->printexceptions("", "FMTreplanningtask::FMTreplanningtask", __LINE__, __FILE__);
		}
	}

	std::unique_ptr<Models::FMTmodel>FMTreplanningtask::copysharedmodel(const std::shared_ptr<Models::FMTmodel>model)
		{
		try {
			//boost::lock_guard<boost::recursive_mutex> guard(taskmutex);
			std::unique_ptr<Models::FMTmodel>modelcpy = std::move(model->clone());
			modelcpy->setparallellogger(*tasklogger.get());
			return std::move(modelcpy);
			}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTreplanningtask::copysharedmodel", __LINE__, __FILE__);
			}
		return std::unique_ptr<Models::FMTmodel>(nullptr);
		}


	void FMTreplanningtask::finalize()
	{
		try {
			if (resultswriter)
			{
				/////
				resultswriter->setdriftprobability(global->getname(), local->getname());///testt
				////
				resultswriter->close();
				resultswriter = std::shared_ptr<FMTparallelwriter>(nullptr);
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTreplanningtask::finalize", __LINE__, __FILE__);
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
			std::queue<int>allreplicates=this->replicateids;
			for (const size_t tasksize : splitwork(numberoftasks, static_cast<int>(replicateids.size())))
				{
				FMTreplanningtask newtask(*this);
				std::queue<int>replicatesoftask;
				while (replicatesoftask.size() < tasksize)
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
		const std::unique_ptr<Models::FMTmodel>& modelptr, const int& replanningperiod, bool onlyfirstperiod)
	{		//modelname = "trategique", modellenght = 20, modelptr ok, replanningpPeriod = 1,  onlyFirst = false
		try {
			if (replanningperiod <= replanningperiods)//Dont write outside the replanningsperiods
			{
				int modelsize = modellength;
				int firstperiod = dynamicarea.begin()->getperiod() + 1;
				int lastperiod = firstperiod;
				// Extraire la partie apr�s le dernier s�parateur
				std::string seqName;
				//m_writeSchedule ? seqName = m_primaryName + "_Replicate" + std::to_string(getiteration()) + ".seq" : seqName = m_primaryName + ".seq";
				std::string schedulePath;
				if (m_writeSchedule && getiteration() >= 1)
				{
					// Assigner le nom du fichier de sortie dans scenario/
					seqName = m_primaryName + "._seq";
					const std::string replicateFolder = "scenarios/replicat" + std::to_string(getiteration());
					boost::filesystem::create_directories(m_outputlocation + '/' + replicateFolder);
					schedulePath = m_outputlocation + '/' + replicateFolder + "/" + seqName;
				}
				else 
				{
					seqName = m_primaryName + ".seq";
					schedulePath = m_outputlocation + '/' + seqName;
				}
 				std::vector<Core::FMTschedule> scheduleList;
				bool appendExistingSchedule = true;
				/*if (!modelptr)//infeasible!
				{
					//put NAN everywhere the size of 
					modelsize = (replanningperiods - replanningperiod) + 1;
					onlyfirstperiod = false;
					std::cout << onlyfirstperiod << std::endl;
					firstperiod = replanningperiod;
				}*/
				if (!onlyfirstperiod)
				{
					lastperiod += modelsize;
					if (firstperiod == 1)
					{
						--lastperiod;
					}
				}
				const std::map<std::string, std::vector<std::vector<double>>>results = resultswriter->getresults(modelptr, firstperiod, lastperiod);
				
				_logger->logwithlevel("Thread:" + getthreadid() + " Writing results for " + modelname + " first period at: " +
					std::to_string(replanningperiod) + " for replicate " + std::to_string(getiteration()) + +"\n", 1);
				
				int reportingFirstPeriod = firstperiod;
				int reportingLastPeriod = lastperiod;
				if (modelsize == 1)
				{
					reportingFirstPeriod = replanningperiod;
					reportingLastPeriod = replanningperiod;
				}
				if (!modelptr)
				{
					reportingFirstPeriod = replanningperiod;
					reportingLastPeriod = replanningperiods;
				}

				if (modelptr && (modelname == stochastic->getname() || modelname == local->getname()))
				{
					if (modelname == stochastic->getname() && replanningperiod == 1)
					{
						// On v�rifie si le fichier existe d�ja et sinon on mets notre append a false afin de cr�er le fichier et le header
						if(!boost::filesystem::exists(boost::filesystem::path(schedulePath)) || m_writeSchedule)
						{
							appendExistingSchedule = false;
						}
					}
					std::vector<Core::FMTschedule> scheduleList;
					scheduleList.reserve(1);
					// for loop / getSolution schedule in vector ajuster resultwriter afin de scheduleParser::write
					int schedulePeriod = reportingFirstPeriod;
					for (int i = firstperiod; i <= lastperiod; ++i)
					{
						Core::FMTschedule schedule = modelptr->getsolution(i, true);
						schedule.setperiod(schedulePeriod);
						scheduleList.push_back(schedule);
						++schedulePeriod;
					}
					resultswriter->writeSchedules(schedulePath, scheduleList, appendExistingSchedule);
				}
				
				resultswriter->write(modelname, results, reportingFirstPeriod, reportingLastPeriod, getiteration());
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

	void FMTreplanningtask::passinlogger(const std::unique_ptr<Logging::FMTlogger>& logger)
		{
		global->passinlogger(logger);
		stochastic->passinlogger(logger);
		local->passinlogger(logger);
		}

	void FMTreplanningtask::setreignore(std::unique_ptr<Models::FMTmodel>& modelcpy, const int& replanningperiod) const
	{
		try {
			if(modelcpy->gotReIgnore(replanningperiod))
			{
				std::vector<Core::FMTconstraint>newconstraints;
				const std::vector<Core::FMTconstraint> MODEL_CONSTRAINTS = modelcpy->getconstraints();
				newconstraints.reserve(MODEL_CONSTRAINTS.size());
				for (const Core::FMTconstraint& constraint : MODEL_CONSTRAINTS)
				{
					if (!constraint.isreignore(replanningperiod))
					{
						newconstraints.push_back(constraint);
					}
				}
				modelcpy->setconstraints(newconstraints);
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTreplanningtask::setreignore", __LINE__, __FILE__);
		}
	}

	void FMTreplanningtask::setreplicate(std::unique_ptr<Models::FMTmodel>& modelcpy, const int& replanningperiod) const
	{
		try {
			if (modelcpy->gotReplicate(replanningperiod))
			{
				modelcpy->setReplicate(getiteration(), replanningperiod);
				/*std::vector<Core::FMTconstraint>newconstraints;
				const std::vector<Core::FMTconstraint> MODEL_CONSTRAINTS = modelcpy->getconstraints();
				newconstraints.reserve(MODEL_CONSTRAINTS.size());
				for (const Core::FMTconstraint& basenssconstraint : MODEL_CONSTRAINTS)
				{
					newconstraints.push_back(basenssconstraint.getfromreplicate(getiteration(), replanningperiod));
				}
				modelcpy->setconstraints(newconstraints);*/
			}
			
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTreplanningtask::setreplicate", __LINE__, __FILE__);
		}
	}

	void FMTreplanningtask::work()
	{
		try {
			const std::vector<Core::FMTconstraint>baselocalconstraints(dynamicconstraints);
			while (!replicateids.empty())
			{
				_logger->logwithlevel("Thread:" + getthreadid() + " Replanning on replicate " + std::to_string(getiteration()) + " started\n",0);
				
				for (int replanningperiod = 1; replanningperiod <= replanningperiods; ++replanningperiod)
				{
					
					if (replanningperiod != 1)
					{
						//boost::lock_guard<boost::recursive_mutex> guard(taskmutex);
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
					/*if (replanningperiod != 1)
					{
						//Make sure that we are out of scope of the unique_ptr of globalcpy.
						checkpoint();//reduce heap contention
					}*/
					
					const std::unique_ptr<Models::FMTmodel> stochasticcopy = std::move(domodelplanning(stochastic,replanningperiod,false,false,false));
					dynamicarea = stochasticcopy->getarea(replanningperiod + 1,true);
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
				dynamicconstraints = baselocalconstraints;
				iterationglobalschedule = *baseschedule;
				_logger->logwithlevel("Thread:" + getthreadid() + " Replanning on replicate " + std::to_string(getiteration()) + " done\n", 0);
				replicateids.pop();
			}
			setstatus(true);
		}catch (...)
		{
			_exhandler->raisefromthreadcatch("","FMTreplanningtask::work", __LINE__, __FILE__);
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
			_logger->logwithlevel("Thread:" + getthreadid() + " starting model planning on "+ model ->getname()+"\n",1);
			std::unique_ptr<Models::FMTmodel>modelcpy = copysharedmodel(model);
			int modelsize = modelcpy->getparameter(Models::FMTintmodelparameters::LENGTH);
			const int randomseedperiod = (replanningperiod << 8) + static_cast<int>(getiteration());
			modelcpy->setparameter(Models::FMTintmodelparameters::SEED, randomseedperiod);//For stochastic
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
					const double  globalsolutionweight = lpmodel->getconstraints().at(0).getscheduleweight();
					lpmodel->addscheduletoobjective(iterationglobalschedule, globalsolutionweight);
					solvedmodel = lpmodel->initialsolve();
					#else
					solvedmodel = modelcpy->doplanning(true);
				#endif
				if (solvedmodel)
					{
					optimal = true;
					dynamicconstraints = modelcpy->getreplanningconstraints("LOCAL", global->getconstraints(), modelsize);
				}else {
					_exhandler->raise(Exception::FMTexc::FMTreplanningwarning,
						"on thread " + getthreadid() + ", infeasible model named " + modelcpy->getname() + " on replicate " + std::to_string(getiteration()) + " at replanning period " + std::to_string(replanningperiod),
						"FMTreplanningtask::domodelplanning", __LINE__, __FILE__);
					modelcpy = std::move(std::unique_ptr<Models::FMTmodel>(nullptr));
				}
				if (modelsize>1)
					{
					writefirstperiodonly = false;
					}
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
							_exhandler->raise(Exception::FMTexc::FMTreplanningwarning,
								"on thread " + getthreadid() + ", empty schedule generated for model "+model->getname()+" at replanning period "+std::to_string(replanningperiod),
								"FMTreplanningtask::domodelplanning", __LINE__, __FILE__);
						}
					}
				}else {
					_exhandler->raise(Exception::FMTexc::FMTreplanningwarning,
						"on thread "+ getthreadid() + ", infeasible model named " + modelcpy->getname() + " on replicate " + std::to_string(getiteration()) + " at replanning period " + std::to_string(replanningperiod),
						"FMTreplanningtask::domodelplanning", __LINE__, __FILE__);

					modelcpy = std::move(std::unique_ptr<Models::FMTmodel>(nullptr));
				}
			}
			writeresults(modelname,modelsize,modelcpy, replanningperiod, writefirstperiodonly);
			_logger->logwithlevel("Thread:" + getthreadid() + " Model planning done on " + model->getname() + "\n", 1);
			return std::move(modelcpy);
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTreplanningtask::domodelplanning", __LINE__, __FILE__);
			}
		return std::move(std::unique_ptr<Models::FMTmodel>(nullptr));
	}

	void FMTreplanningtask::setWriteSchedule(bool p_write)
	{
		m_writeSchedule = p_write;
	}

}
