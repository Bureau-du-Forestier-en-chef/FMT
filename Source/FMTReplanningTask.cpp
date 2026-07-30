/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTReplanningTask.h"
#include "FMTParallelWriter.h"
#include "FMTModel.h"
#include "FMTOutput.h"
#include "FMTLpModel.h"
#include "FMTExceptionHandler.h"
#include <boost/filesystem.hpp>



namespace Parallel
{

	FMTReplanningTask::FMTReplanningTask(const Models::FMTModel& globalm,
		const Models::FMTModel& stochasticm,
		const Models::FMTModel& localm,
		const std::string& outputlocation,
		const std::string& gdaldriver,
		const std::vector<std::string>& creationoptions,
		Core::FMToutputlevel outputlevel):
		FMTReplanningTask(globalm, stochasticm, localm,
			globalm.getOutputs(), outputlocation,
			gdaldriver, creationoptions, 1,
			globalm.getParameter(Models::FMTintmodelparameters::LENGTH),
			0.5, outputlevel)
	{

	}

	void FMTReplanningTask::setReplicates(const int& replicatesnumber)
	{
		m_replicateids=std::queue<int>();
		for (int replicateid = 1; replicateid < (replicatesnumber + 1); ++replicateid)
		{
			m_replicateids.push(replicateid);
		}
	}

	void FMTReplanningTask::setReplicates(const int& replicatesnumbermin, const int& replicatesnumbermax)
	{
		m_replicateids = std::queue<int>();
		for (int replicateid = replicatesnumbermin; replicateid <= replicatesnumbermax; ++replicateid)
		{
			m_replicateids.push(replicateid);
		}
	}

	void FMTReplanningTask::setReplanningPeriods(const int& periodsnumber)
		{
		m_replanningperiods = periodsnumber;
		}


	FMTReplanningTask::FMTReplanningTask(
		const Models::FMTModel& globalm,
		const Models::FMTModel& stochasticm,
		const Models::FMTModel& localm,
		const std::vector<Core::FMTOutput>& outputs,
		const std::string& outputlocation,
		const std::string& gdaldriver,
		const std::vector<std::string>& creationoptions,
		const int& replicates,
		const int& replanningperiodssize,
		const double& minimaldrift,
		Core::FMToutputlevel outputlevel,
		const bool writeSchedule) :
		m_resultswriter(),
		m_baseschedule(),
		global(),
		m_stochastic(),
		m_local(),
		m_replicateids(),
		m_dynamicarea(globalm.getArea()),
		m_iterationglobalschedule(),
		m_dynamicconstraints(),
		m_replanningperiods(replanningperiodssize),
		m_writeSchedule(writeSchedule)
	{
		try {
			//passinobject(globalm);
			if (globalm.getName() == stochasticm.getName() || globalm.getName() == localm.getName() || stochasticm.getName() == localm.getName())
			{
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
					"Two Model name is the same in strategic, stochastic or tactic",
					"FMTReplanningTask::FMTReplanningTask", __LINE__, __FILE__);
			}
			global = std::move(globalm.clone());
			m_stochastic = std::move(stochasticm.clone());
			m_local = std::move(localm.clone());
			global->setParameter(Models::FMTboolmodelparameters::PRESOLVE_CAN_REMOVE_STATIC_THEMES, false);
			m_stochastic->setParameter(Models::FMTboolmodelparameters::PRESOLVE_CAN_REMOVE_STATIC_THEMES, false);
			m_local->setParameter(Models::FMTboolmodelparameters::PRESOLVE_CAN_REMOVE_STATIC_THEMES, false);
			global->setParameter(Models::FMTintmodelparameters::UPDATE,1);
			m_stochastic->setParameter(Models::FMTintmodelparameters::UPDATE, 1);
			m_local->setParameter(Models::FMTintmodelparameters::UPDATE, 1);
			m_outputlocation = outputlocation;
			//m_outputlocation.pop_back();
			m_primaryName = boost::filesystem::path(m_outputlocation).stem().string();
			const size_t LENGTH = static_cast<size_t>(global->getParameter(Models::FMTintmodelparameters::LENGTH));
			const size_t AREA_SIZE = global->getArea().size();
			const size_t SCALE_FACTOR = 10;
			const size_t GLOBAL_RESERVE = AREA_SIZE * LENGTH * LENGTH * SCALE_FACTOR;
			const size_t BASE_RESERVE = AREA_SIZE * SCALE_FACTOR;
			Models::FMTSrModel* GlobalPtr = dynamic_cast<Models::FMTSrModel*>(global.get());
			GlobalPtr->setNodeCacheSize(GLOBAL_RESERVE);
			Models::FMTSrModel* LocalPtr = dynamic_cast<Models::FMTSrModel*>(m_local.get());
			LocalPtr->setNodeCacheSize(BASE_RESERVE);
			Models::FMTSrModel* StochasticPtr = dynamic_cast<Models::FMTSrModel*>(m_stochastic.get());
			StochasticPtr->setNodeCacheSize(BASE_RESERVE);
			std::vector<Models::FMTModel*>modelsptr;
			modelsptr.push_back(global.get());
			modelsptr.push_back(m_stochastic.get());
			modelsptr.push_back(m_local.get());
			m_resultswriter = std::shared_ptr<FMTParallelWriter>(new FMTParallelWriter(outputlocation, gdaldriver, outputs, modelsptr, creationoptions, minimaldrift, outputlevel));
			std::unique_ptr<Models::FMTModel>modelcpy = global->clone();
			_logger->logWithLevel("Initial planning started\n", 0);
			if (!modelcpy->doPlanning(true))
			{
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
					"Infeasible Global model",
					"FMTReplanningTask::FMTReplanningTask", __LINE__, __FILE__);
			}
			m_replicateids.push(0);
			_writeResults(global->getName(),
				global->getParameter(Models::FMTintmodelparameters::LENGTH),modelcpy,1);
			const std::string OBJ_VALUE = std::to_string(modelcpy->getObjectiveValue());
			_logger->logWithLevel("Initial planning Obj("+ OBJ_VALUE +") done\n", 0);
			m_replicateids.pop();
			m_baseschedule = std::shared_ptr<Core::FMTSchedule>(new Core::FMTSchedule(modelcpy->getSolution(1, true)));
			m_iterationglobalschedule = *m_baseschedule;
			m_dynamicconstraints = modelcpy->getReplanningConstraints("GLOBAL",m_local->getConstraints(), 1);
			for (int replicateid = 1; replicateid < (replicates+1); ++replicateid)
			{
				m_replicateids.push(replicateid);
			}
			global->setParallelLogger(*m_tasklogger.get());
			m_local->setParallelLogger(*m_tasklogger.get());
			m_stochastic->setParallelLogger(*m_tasklogger.get());
		}
		catch (...)
		{
			_exhandler->printExceptions("", "FMTReplanningTask::FMTReplanningTask", __LINE__, __FILE__);
		}
	}

	std::unique_ptr<Models::FMTModel>FMTReplanningTask::_copySharedModel(const std::shared_ptr<Models::FMTModel>model)
		{
		try {
			//boost::lock_guard<boost::recursive_mutex> guard(taskmutex);
			std::unique_ptr<Models::FMTModel>modelcpy = std::move(model->clone());
			modelcpy->setParallelLogger(*m_tasklogger.get());
			return std::move(modelcpy);
			}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTReplanningTask::_copySharedModel", __LINE__, __FILE__);
			}
		return std::unique_ptr<Models::FMTModel>(nullptr);
		}


	void FMTReplanningTask::finalize()
	{
		try {
			if (m_resultswriter)
			{
				m_resultswriter->setDriftProbability(global->getName(), m_local->getName());
				m_resultswriter->close();
				m_resultswriter = std::shared_ptr<FMTParallelWriter>(nullptr);
			}
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTReplanningTask::finalize", __LINE__, __FILE__);
		}
	}


	std::unique_ptr<FMTTask> FMTReplanningTask::clone() const
		{
		return std::unique_ptr<FMTTask>(new FMTReplanningTask(*this));
		}

	std::vector<std::unique_ptr<FMTTask>>FMTReplanningTask::split(const unsigned int& numberoftasks) const
	{
		std::vector<std::unique_ptr<FMTTask>>tasks;
		try {
			std::queue<int> allreplicates = this->m_replicateids;
			// On aimerait splitter le work en round robin plutôt, donc je fais un vecteur pour créer l'ordonnance qu'on veut
			std::vector<std::queue<int>> queues(numberoftasks);
			size_t idx = 0;
			while (!allreplicates.empty()) {
				const int REPLICA_ID = allreplicates.front();
				allreplicates.pop();

				const size_t TARGET = idx % numberoftasks;
				queues[TARGET].push(REPLICA_ID);

				++idx;
			}

			for (unsigned i = 0; i < numberoftasks; ++i) {
				if (!queues[i].empty())
				{
					FMTReplanningTask newtask(*this);
					newtask.m_replicateids = std::move(queues[i]);

					// clone() retourne un std::unique_ptr<FMTTask>
					tasks.push_back(std::move(newtask.clone()));
				}
			}

			//for (const size_t tasksize : splitWork(numberoftasks, static_cast<int>(replicateids.size())))
			//	{
			//	FMTReplanningTask newtask(*this);
			//	std::queue<int>replicatesoftask;
			//
			//	while (replicatesoftask.size() < tasksize)
			//		{
			//		replicatesoftask.push(allreplicates.front());
			//		allreplicates.pop();
			//		}
			//
			//	newtask.replicateids = replicatesoftask;
			//	tasks.push_back(std::move(newtask.clone()));
			//	}
			
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTReplanningTask::split", __LINE__, __FILE__);
			}
		return tasks;
	}

	std::unique_ptr<FMTTask>FMTReplanningTask::spawn()
	{
		try {
			if (!m_replicateids.empty())
				{
				FMTReplanningTask newtask(*this);
				std::queue<int>singlereplicate;
				singlereplicate.push(m_replicateids.front());
				m_replicateids.pop();
				newtask.m_replicateids.swap(singlereplicate);
				return std::move(newtask.clone());
				}
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTReplanningTask::spawn", __LINE__, __FILE__);
			}
	return std::unique_ptr<FMTTask>(nullptr);
	}

	void FMTReplanningTask::_writeSchedule(const std::unique_ptr<Models::FMTModel>& p_model,
										int p_ReportingPeriod, int p_ModelPeriod) const
	{
		try {
			const std::string SEQ_NAME = m_primaryName + "._seq";
			const std::string replicateFolder = "scenarios/replicat" + std::to_string(_getIteration());
			boost::filesystem::create_directories(m_outputlocation + '/' + replicateFolder);
			const std::string SCHEDULE_PATH = m_outputlocation + '/' + replicateFolder + "/" + SEQ_NAME;
			bool appendExistingSchedule = true;
			if (p_model->getName() == m_stochastic->getName() && p_ReportingPeriod == 1)
			{
				// On v�rifie si le fichier existe d�ja et sinon on mets notre append a false afin de cr�er le fichier et le header
				if (!boost::filesystem::exists(boost::filesystem::path(SCHEDULE_PATH)) || m_writeSchedule)
				{
					appendExistingSchedule = false;
				}
			}
			std::vector<Core::FMTSchedule> scheduleList;
			if (p_model->getName() == m_stochastic->getName()||
				p_model->getName() == m_local->getName())
			{
				scheduleList.push_back(p_model->getSolution(p_ModelPeriod, true));
				//scheduleList.push_back(p_model->getSolution(p_ReportingPeriod, true));
				scheduleList.back().setPeriod(p_ReportingPeriod);
			}else {//Got the strategic here got get the replicate 0 here...
				appendExistingSchedule = false;
				for (int period = 1; period <= p_model->getParameter(Models::FMTintmodelparameters::LENGTH);++period)
					{
					scheduleList.push_back(p_model->getSolution(period, true));
					}
				}
			m_resultswriter->writeSchedules(SCHEDULE_PATH, scheduleList, appendExistingSchedule);

		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTReplanningTask::_writeSchedule", __LINE__, __FILE__);
			}

	}

	void FMTReplanningTask::_writeResults(const std::string& modelname, const int& modellength,
		const std::unique_ptr<Models::FMTModel>& modelptr, const int& replanningperiod, bool onlyfirstperiod)
	{		//modelname = "trategique", modellenght = 20, modelptr ok, replanningpPeriod = 1,  onlyFirst = false
		try {
			if (replanningperiod <= m_replanningperiods)//Dont write outside the replanningsperiods
			{
				int modelsize = modellength;
				int firstPeriod = m_dynamicarea.begin()->getPeriod() + 1;
				int lastPeriod = firstPeriod;
				if (!onlyfirstperiod)
				{
					lastPeriod += modelsize;
					if (firstPeriod == 1)
					{
						--lastPeriod;
					}
				}
				const std::map<std::string, std::vector<std::vector<double>>>results = m_resultswriter->getResults(modelptr, firstPeriod, lastPeriod);
				
				_logger->logWithLevel("Writing results for " + modelname + " first period at: " +
					std::to_string(replanningperiod) + " for replicate " + std::to_string(_getIteration()) + +"\n", 1);
				
				int reportingFirstPeriod = firstPeriod;
				int reportingLastPeriod = lastPeriod;
				if (modelsize == 1)
				{
					reportingFirstPeriod = replanningperiod;
					reportingLastPeriod = replanningperiod;
				}
				if (!modelptr)
				{
					reportingFirstPeriod = replanningperiod;
					reportingLastPeriod = m_replanningperiods;
				}

				if (m_writeSchedule && modelptr && 
					(modelname == m_stochastic->getName() || modelname == m_local->getName() || (modelname == global->getName() && _getIteration() == 0)))
					{
					_writeSchedule(modelptr, reportingFirstPeriod, firstPeriod);
					}
				m_resultswriter->write(modelname, results, reportingFirstPeriod, reportingLastPeriod, _getIteration());
			}
		}catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTReplanningTask::_writeResults", __LINE__, __FILE__);
		}
	}

	const int FMTReplanningTask::_getIteration() const
		{
		//if (replicateids.empty())
		//{
		//	std::cout << "Wtf";
		//}
		return m_replicateids.front();
		}

	void FMTReplanningTask::passInLogger(const std::unique_ptr<Logging::FMTLogger>& logger)
		{
		global->passInLogger(logger);
		m_stochastic->passInLogger(logger);
		m_local->passInLogger(logger);
		}

	void FMTReplanningTask::_setReIgnore(std::unique_ptr<Models::FMTModel>& modelcpy, const int& replanningperiod) const
	{
		try {
			if(modelcpy->gotReIgnore(replanningperiod))
			{
				std::vector<Core::FMTConstraint>newconstraints;
				const std::vector<Core::FMTConstraint> MODEL_CONSTRAINTS = modelcpy->getConstraints();
				newconstraints.reserve(MODEL_CONSTRAINTS.size());
				for (const Core::FMTConstraint& constraint : MODEL_CONSTRAINTS)
				{
					if (!constraint.isReIgnore(replanningperiod))
					{
						newconstraints.push_back(constraint);
					}
				}
				modelcpy->setConstraints(newconstraints);
			}
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTReplanningTask::_setReIgnore", __LINE__, __FILE__);
		}
	}


	void FMTReplanningTask::_setReplicate(std::unique_ptr<Models::FMTModel>& modelcpy, const int& replanningperiod) const
	{
		try {
			if (modelcpy->gotReplicate(replanningperiod))
			{
				modelcpy->setReplicate(_getIteration(), replanningperiod);
				/*std::vector<Core::FMTConstraint>newconstraints;
				const std::vector<Core::FMTConstraint> MODEL_CONSTRAINTS = modelcpy->getConstraints();
				newconstraints.reserve(MODEL_CONSTRAINTS.size());
				for (const Core::FMTConstraint& basenssconstraint : MODEL_CONSTRAINTS)
				{
					newconstraints.push_back(basenssconstraint.getFromReplicate(_getIteration(), replanningperiod));
				}
				modelcpy->setconstraints(newconstraints);*/
			}
			
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTReplanningTask::_setReplicate", __LINE__, __FILE__);
		}
	}

	void FMTReplanningTask::work()
	{
		try {
			const std::vector<Core::FMTConstraint>baselocalconstraints(m_dynamicconstraints);
			while (!m_replicateids.empty())
			{
				_logger->logWithLevel("Replanning on replicate " + std::to_string(_getIteration()) + " started\n",0);
				
				for (int replanningperiod = 1; replanningperiod <= m_replanningperiods; ++replanningperiod)
				{
					//setTerminateStack();
					
					if (replanningperiod != 1)
					{
						//boost::lock_guard<boost::recursive_mutex> guard(taskmutex);
						const std::unique_ptr<Models::FMTModel>globalcopy = std::move(_doModelPlanning(global,replanningperiod,true));
						if (!globalcopy)//infeasible replicate end here
							{
							//Also write infeasible for stochastic
							_writeResults(m_stochastic->getName(),
								m_replanningperiods,nullptr, replanningperiod);
							//Write infeasible for local
							_writeResults(m_local->getName(),
								m_replanningperiods, nullptr, replanningperiod);
							break;
							}
						
					}
					/*if (replanningperiod != 1)
					{
						//Make sure that we are out of scope of the unique_ptr of globalcpy.
						_checkpoint();//reduce heap contention
					}*/
					
					const std::unique_ptr<Models::FMTModel> stochasticcopy = std::move(_doModelPlanning(m_stochastic,replanningperiod,false,false,false));
					m_dynamicarea = stochasticcopy->getArea(replanningperiod + 1,true);
					for (Core::FMTActualDevelopment& development : m_dynamicarea)
						{
						development.setPeriod(replanningperiod - 1);
						}
					const std::unique_ptr<Models::FMTModel> localcopy = std::move(_doModelPlanning(m_local, replanningperiod,false,true));
					if (!localcopy)//infeasible replicate end here
						{
						//Write infeasible for next global
						_writeResults(global->getName(),
								m_replanningperiods, nullptr, replanningperiod + 1);
						//Also write infeasible for next stochastic
						_writeResults(m_stochastic->getName(),
								m_replanningperiods, nullptr, replanningperiod + 1);
						break;
						}
					//dynamicarea = localcopy->getArea(localcopy->getParameter(Models::FMTintmodelparameters::LENGTH)+1);
					m_dynamicarea = localcopy->getArea(localcopy->getParameter(Models::FMTintmodelparameters::LENGTH) + replanningperiod);
					for (Core::FMTActualDevelopment& development : m_dynamicarea)
					{
						development.setPeriod(replanningperiod);
					}
				}
				m_dynamicarea = global->getArea();
				m_dynamicconstraints = baselocalconstraints;
				m_iterationglobalschedule = *m_baseschedule;
				_logger->logWithLevel("Replanning on replicate " + std::to_string(_getIteration()) + " done\n", 0);
				m_replicateids.pop();
			}
			//setStatus(true);
		}catch (...)
		{
			const std::string LOCATION = "on replicate " + std::to_string(_getIteration());
			_exhandler->raiseFromThreadCatch(LOCATION,"FMTReplanningTask::work", __LINE__, __FILE__);
		}

	}


	std::unique_ptr<Models::FMTModel>FMTReplanningTask::_doModelPlanning(
		const std::shared_ptr<Models::FMTModel>model,
		const int& replanningperiod,
		bool getsolutionandlocal,
		bool applyscheduleweight,
		bool setdynamicconstraints)
	{
		bool optimal = false;
		try {
		
			_logger->logWithLevel("starting model planning on "+ model ->getName()+"\n",1);
			std::unique_ptr<Models::FMTModel>modelcpy = _copySharedModel(model);
			int modelsize = modelcpy->getParameter(Models::FMTintmodelparameters::LENGTH);
			const int randomseedperiod = (replanningperiod << 8) + static_cast<int>(_getIteration());
			modelcpy->setParameter(Models::FMTintmodelparameters::SEED, randomseedperiod);//For m_stochastic
			const std::string modelname = modelcpy->getName();
			bool writefirstperiodonly = true;
			modelcpy->setArea(m_dynamicarea);
			if (setdynamicconstraints)
				{
				modelcpy->setConstraints(m_dynamicconstraints);
				}
			_setReIgnore(modelcpy, replanningperiod);
			_setReplicate(modelcpy, replanningperiod);
			if (applyscheduleweight)//m_local is here
			{
				bool solvedmodel = false;
				#ifdef FMTWITHOSI
					Models::FMTLpModel* lpmodel = dynamic_cast<Models::FMTLpModel*>(modelcpy.get());
					lpmodel->doPlanning(false);
					const double  globalsolutionweight = lpmodel->getConstraints().at(0).getScheduleWeight();
					lpmodel->addScheduleToObjective(m_iterationglobalschedule, globalsolutionweight);
					solvedmodel = lpmodel->initialSolve();
					#else
					solvedmodel = modelcpy->doPlanning(true);
				#endif
				if (solvedmodel)
					{
					optimal = true;
					m_dynamicconstraints = modelcpy->getReplanningConstraints("LOCAL", global->getConstraints(), modelsize);
				}else {
					_exhandler->raise(Exception::FMTexc::FMTreplanningwarning,
						"infeasible model named " + modelcpy->getName() + " on replicate " + std::to_string(_getIteration()) + " at replanning period " + std::to_string(replanningperiod),
						"FMTReplanningTask::_doModelPlanning", __LINE__, __FILE__);
					modelcpy = std::move(std::unique_ptr<Models::FMTModel>(nullptr));
				}
				if (modelsize>1)
					{
					writefirstperiodonly = false;
					}
				//lpmodel->writeLp("C:/Users/cyrgu3/Desktop/test/FMT2/FMT/build/debug/bin/Debug/tests/testlocal"+std::to_string(replanningperiod));
			}else {
				if (modelcpy->doPlanning(true))
				{
					optimal = true;
					if (getsolutionandlocal)
					{
						m_dynamicconstraints = modelcpy->getReplanningConstraints("GLOBAL",m_local->getConstraints(), replanningperiod + 1);
						m_iterationglobalschedule = modelcpy->getSolution(replanningperiod, true);
						m_iterationglobalschedule.setPeriod(1);
						if (m_iterationglobalschedule.empty())
							{
							_exhandler->raise(Exception::FMTexc::FMTreplanningwarning,
								"empty schedule generated for model "+model->getName()+" at replanning period "+std::to_string(replanningperiod),
								"FMTReplanningTask::_doModelPlanning", __LINE__, __FILE__);
						}
					}
				}else {
					_exhandler->raise(Exception::FMTexc::FMTreplanningwarning,
						"infeasible model named " + modelcpy->getName() + " on replicate " + std::to_string(_getIteration()) + " at replanning period " + std::to_string(replanningperiod),
						"FMTReplanningTask::_doModelPlanning", __LINE__, __FILE__);

					modelcpy = std::move(std::unique_ptr<Models::FMTModel>(nullptr));
				}
			}
			_writeResults(modelname,modelsize,modelcpy, replanningperiod, writefirstperiodonly);
			_logger->logWithLevel("Model planning done on " + model->getName() + "\n", 1);
			return std::move(modelcpy);
		}catch (...)
			{
			std::string location = model->getName() +
				" replanning period " + std::to_string(replanningperiod);
			if (!m_replicateids.empty())
			{
				location += " replicate " + std::to_string(_getIteration());
			}
			_exhandler->raiseFromCatch(location, "FMTReplanningTask::_doModelPlanning", __LINE__, __FILE__);
			}
		return std::move(std::unique_ptr<Models::FMTModel>(nullptr));
	}

	void FMTReplanningTask::setWriteSchedule(bool p_write)
	{
		m_writeSchedule = p_write;
	}

}
