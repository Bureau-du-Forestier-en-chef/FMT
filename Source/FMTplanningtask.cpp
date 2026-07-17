/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTplanningtask.h"
#include "FMTparallelwriter.h"
#include "FMTmodel.h"
#include "FMToutput.h"
#include "FMTschedule.h"
#include "FMTexceptionhandler.h"
#include <boost/filesystem.hpp>


namespace Parallel
{

	std::list<std::unique_ptr<Models::FMTModel>>FMTPlanningTask::copyModels(const std::list<std::unique_ptr<Models::FMTModel>>& tocopy) const
	{
		std::list<std::unique_ptr<Models::FMTModel>>newmodels;
		try {
			for (const std::unique_ptr<Models::FMTModel>& model : tocopy)
				{
				newmodels.push_back(std::move(model->clone()));
				}
		}catch (...)
		{
			_exhandler->printExceptions("", "FMTPlanningTask::copyModels", __LINE__, __FILE__);
		}
		return newmodels;
	}


	FMTPlanningTask::FMTPlanningTask(const FMTPlanningTask& rhs):
		m_ResultsWriter(rhs.m_ResultsWriter),
		m_Models(copyModels(rhs.m_Models)),
		m_allSchedules(rhs.m_allSchedules),
		m_Outputs(rhs.m_Outputs),
		m_keepModels(rhs.m_keepModels)
	{

		
	}

	FMTPlanningTask& FMTPlanningTask::operator = (const FMTPlanningTask& rhs)
	{
		if (this!=&rhs)
		{
			m_ResultsWriter = rhs.m_ResultsWriter;
			m_Models = copyModels(rhs.m_Models);
			m_allSchedules = rhs.m_allSchedules;
			m_Outputs = rhs.m_Outputs;
			m_keepModels = rhs.m_keepModels;
		}
		return *this;
	}


	FMTPlanningTask::FMTPlanningTask(
		const int& minoutputperiod,
		const int& maxoutputperiod,
		const std::string& outputlocation,
		std::string gdaldriver,
		std::vector<std::string>creationoptions,
		Core::FMToutputlevel outputlevel,
		std::string primaryfilelocatiron):
		m_ResultsWriter(),
		m_Models(),
		m_allSchedules(),
		m_Outputs(),
		m_keepModels(false)
	{
		try {
			m_ResultsWriter = std::shared_ptr<FMTParallelWriter>(new FMTParallelWriter(outputlocation,gdaldriver,outputlevel, creationoptions,minoutputperiod,maxoutputperiod, primaryfilelocatiron));
			m_ResultsWriter->setLayer(boost::filesystem::path(primaryfilelocatiron).stem().string());
		}catch (...)
			{
			_exhandler->printExceptions("", "FMTPlanningTask::FMTPlanningTask", __LINE__, __FILE__);
			}
	}

	void FMTPlanningTask::push_back(const Models::FMTModel& model,
		std::vector<Core::FMTSchedule>schedules, std::vector<Core::FMTOutput>loutputs)
	{
		try {
			m_Models.push_back(std::move(model.clone()));
			m_Models.back()->setParallelLogger(*tasklogger.get());
			const std::string SCENARIO_NAME = m_Models.back()->getName();
			if (!model.getParameter(Models::FMTboolmodelparameters::FORCE_PARTIAL_BUILD)||
				!(!SCENARIO_NAME.empty() && SCENARIO_NAME.find_first_not_of("0123456789") == std::string::npos))
				{
				m_ResultsWriter->setLayer(model.getName());
				}
			m_allSchedules.push_back(schedules);
			m_Outputs.push_back(loutputs);
			//;
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTPlanningTask::push_back", __LINE__, __FILE__);
			}
	}


	std::unique_ptr<FMTTask> FMTPlanningTask::clone() const
		{
		return std::unique_ptr<FMTTask>(new FMTPlanningTask(*this));
		}

	std::vector<std::unique_ptr<FMTTask>>FMTPlanningTask::split(const unsigned int& numberoftasks) const
	{
		std::vector<std::unique_ptr<FMTTask>>tasks;
		try {
			std::list<std::unique_ptr<Models::FMTModel>>allmodels=copyModels(m_Models);
			std::list<std::vector<Core::FMTSchedule>>modelschedules(m_allSchedules);
			std::list<std::vector<Core::FMTOutput>>modeloutputs(m_Outputs);
			for (const size_t tasksize : splitWork(numberoftasks, static_cast<int>(m_Models.size())))
				{
				FMTPlanningTask newtask(*this);
				std::list<std::unique_ptr<Models::FMTModel>>modelsoftask;
				std::list<std::vector<Core::FMTSchedule>>schedulesoftask;
				std::list<std::vector<Core::FMTOutput>>outputsoftask;
				for (int model = 0; model < tasksize; ++model)
					{
					modelsoftask.push_back(std::move(allmodels.front()->clone()));
					schedulesoftask.push_back(modelschedules.front());
					outputsoftask.push_back(modeloutputs.front());
					allmodels.pop_front();
					modelschedules.pop_front();
					modeloutputs.pop_front();
					}
				newtask.m_Models = copyModels(modelsoftask);
				newtask.m_allSchedules = schedulesoftask;
				newtask.m_Outputs = outputsoftask;
				tasks.push_back(std::move(newtask.clone()));
				}
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTPlanningTask::split", __LINE__, __FILE__);
			}
		return tasks;
	}

	std::unique_ptr<FMTTask>FMTPlanningTask::spawn()
	{
		try {
			if (!m_Models.empty())
				{
				FMTPlanningTask newtask(*this);
				std::list<std::unique_ptr<Models::FMTModel>>singlemodel;
				std::list<std::vector<Core::FMTSchedule>>singleschedule;
				std::list<std::vector<Core::FMTOutput>>singleoutputs;
				singlemodel.push_back(std::move(m_Models.front()->clone()));
				singleschedule.push_back(m_allSchedules.front());
				singleoutputs.push_back(m_Outputs.front());
				m_Models.pop_front();
				m_allSchedules.pop_front();
				m_Outputs.pop_front();
				newtask.m_Models.swap(singlemodel);
				newtask.m_allSchedules.swap(singleschedule);
				newtask.m_Outputs.swap(singleoutputs);
				return std::move(newtask.clone());
				}
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTPlanningTask::spawn", __LINE__, __FILE__);
			}
	return std::unique_ptr<FMTTask>(nullptr);
	}

	void FMTPlanningTask::passInLogger(const std::unique_ptr<Logging::FMTLogger>& logger)
		{
		try {
			for (std::unique_ptr<Models::FMTModel>& model : m_Models)
				{
				model->passInLogger(logger);
				}
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTPlanningTask::passInLogger", __LINE__, __FILE__);
			}
		}

	void FMTPlanningTask::setKeepModels()
		{
		m_keepModels = true;
		}


	void FMTPlanningTask::work()
	{
		try {
			std::list<std::unique_ptr<Models::FMTModel>>modelskept;
			while (!m_Models.empty())
			{
				_logger->logWithLevel("Thread:" + getThreadId() + " Planning of " + m_Models.front()->getName() + " started\n",0);
				const bool SOLVE = !m_Models.front()->getParameter(Models::FMTboolmodelparameters::FORCE_PARTIAL_BUILD);
				const bool FEASIBLE = m_Models.front()->doPlanning(SOLVE, m_allSchedules.front());
				if (FEASIBLE||!SOLVE)
					{
					if (FEASIBLE) {
						_logger->logWithLevel("Thread:" + getThreadId() 
							+ " scenario: " + m_Models.front()->getName()
							+ " objective value of: "
							+ std::to_string(m_Models.front()->getObjectiveValue())
							+ "\n", 0);
					}
					m_ResultsWriter->getAndWrite(m_Models.front(), m_Outputs.front());
				}else {
					_logger->logWithLevel("Thread:" + getThreadId() + " infeasible scenario " + m_Models.front()->getName() + "\n", 0);
					}
				_logger->logWithLevel("Thread:" + getThreadId() + " Planning of " + m_Models.front()->getName() + " done\n", 0);
				if (m_keepModels)
					{
					modelskept.push_back(std::move(m_Models.front()));
					}
				m_Models.pop_front();
				m_allSchedules.pop_front();
				m_Outputs.pop_front();
			}
			if (!modelskept.empty())
			{
				for (std::unique_ptr<Models::FMTModel>& model : modelskept)
				{
					m_Models.push_back(std::move(model));
				}
			}
			//setstatus(true);
		}catch (...)
		{
			_exhandler->raiseFromThreadCatch("","FMTPlanningTask::work", __LINE__, __FILE__);
		}
	}

	void FMTPlanningTask::finalize()
	{
		try {
		
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTPlanningTask::finalize", __LINE__, __FILE__);
		}
	}

}