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

	std::list<std::unique_ptr<Models::FMTmodel>>FMTplanningtask::copymodels(const std::list<std::unique_ptr<Models::FMTmodel>>& tocopy) const
	{
		std::list<std::unique_ptr<Models::FMTmodel>>newmodels;
		try {
			for (const std::unique_ptr<Models::FMTmodel>& model : tocopy)
				{
				newmodels.push_back(std::move(model->clone()));
				}
		}catch (...)
		{
			_exhandler->printexceptions("", "FMTplanningtask::copymodels", __LINE__, __FILE__);
		}
		return newmodels;
	}


	FMTplanningtask::FMTplanningtask(const FMTplanningtask& rhs):
		m_ResultsWriter(rhs.m_ResultsWriter),
		m_Models(copymodels(rhs.m_Models)),
		m_allSchedules(rhs.m_allSchedules),
		m_Outputs(rhs.m_Outputs),
		m_keepModels(rhs.m_keepModels)
	{

		
	}

	FMTplanningtask& FMTplanningtask::operator = (const FMTplanningtask& rhs)
	{
		if (this!=&rhs)
		{
			m_ResultsWriter = rhs.m_ResultsWriter;
			m_Models = copymodels(rhs.m_Models);
			m_allSchedules = rhs.m_allSchedules;
			m_Outputs = rhs.m_Outputs;
			m_keepModels = rhs.m_keepModels;
		}
		return *this;
	}


	FMTplanningtask::FMTplanningtask(
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
			m_ResultsWriter = std::shared_ptr<FMTparallelwriter>(new FMTparallelwriter(outputlocation,gdaldriver,outputlevel, creationoptions,minoutputperiod,maxoutputperiod, primaryfilelocatiron));
			m_ResultsWriter->setLayer(boost::filesystem::path(primaryfilelocatiron).stem().string());
		}catch (...)
			{
			_exhandler->printexceptions("", "FMTplanningtask::FMTplanningtask", __LINE__, __FILE__);
			}
	}

	void FMTplanningtask::push_back(const Models::FMTmodel& model,
		std::vector<Core::FMTschedule>schedules, std::vector<Core::FMToutput>loutputs)
	{
		try {
			m_Models.push_back(std::move(model.clone()));
			m_Models.back()->setparallellogger(*tasklogger.get());
			const std::string SCENARIO_NAME = m_Models.back()->getname();
			if (!model.getparameter(Models::FMTboolmodelparameters::FORCE_PARTIAL_BUILD)||
				!(!SCENARIO_NAME.empty() && SCENARIO_NAME.find_first_not_of("0123456789") == std::string::npos))
				{
				m_ResultsWriter->setLayer(model.getname());
				}
			m_allSchedules.push_back(schedules);
			m_Outputs.push_back(loutputs);
			//;
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTplanningtask::push_back", __LINE__, __FILE__);
			}
	}


	std::unique_ptr<FMTtask> FMTplanningtask::clone() const
		{
		return std::unique_ptr<FMTtask>(new FMTplanningtask(*this));
		}

	std::vector<std::unique_ptr<FMTtask>>FMTplanningtask::split(const unsigned int& numberoftasks) const
	{
		std::vector<std::unique_ptr<FMTtask>>tasks;
		try {
			std::list<std::unique_ptr<Models::FMTmodel>>allmodels=copymodels(m_Models);
			std::list<std::vector<Core::FMTschedule>>modelschedules(m_allSchedules);
			std::list<std::vector<Core::FMToutput>>modeloutputs(m_Outputs);
			for (const size_t tasksize : splitwork(numberoftasks, static_cast<int>(m_Models.size())))
				{
				FMTplanningtask newtask(*this);
				std::list<std::unique_ptr<Models::FMTmodel>>modelsoftask;
				std::list<std::vector<Core::FMTschedule>>schedulesoftask;
				std::list<std::vector<Core::FMToutput>>outputsoftask;
				for (int model = 0; model < tasksize; ++model)
					{
					modelsoftask.push_back(std::move(allmodels.front()->clone()));
					schedulesoftask.push_back(modelschedules.front());
					outputsoftask.push_back(modeloutputs.front());
					allmodels.pop_front();
					modelschedules.pop_front();
					modeloutputs.pop_front();
					}
				newtask.m_Models = copymodels(modelsoftask);
				newtask.m_allSchedules = schedulesoftask;
				newtask.m_Outputs = outputsoftask;
				tasks.push_back(std::move(newtask.clone()));
				}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTplanningtask::split", __LINE__, __FILE__);
			}
		return tasks;
	}

	std::unique_ptr<FMTtask>FMTplanningtask::spawn()
	{
		try {
			if (!m_Models.empty())
				{
				FMTplanningtask newtask(*this);
				std::list<std::unique_ptr<Models::FMTmodel>>singlemodel;
				std::list<std::vector<Core::FMTschedule>>singleschedule;
				std::list<std::vector<Core::FMToutput>>singleoutputs;
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
			_exhandler->raisefromcatch("", "FMTplanningtask::spawn", __LINE__, __FILE__);
			}
	return std::unique_ptr<FMTtask>(nullptr);
	}

	void FMTplanningtask::passinlogger(const std::unique_ptr<Logging::FMTlogger>& logger)
		{
		try {
			for (std::unique_ptr<Models::FMTmodel>& model : m_Models)
				{
				model->passinlogger(logger);
				}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTplanningtask::passinlogger", __LINE__, __FILE__);
			}
		}

	void FMTplanningtask::setkeepmodels()
		{
		m_keepModels = true;
		}


	void FMTplanningtask::work()
	{
		try {
			std::list<std::unique_ptr<Models::FMTmodel>>modelskept;
			while (!m_Models.empty())
			{
				_logger->logwithlevel("Thread:" + getThreadId() + " Planning of " + m_Models.front()->getname() + " started\n",0);
				const bool SOLVE = !m_Models.front()->getparameter(Models::FMTboolmodelparameters::FORCE_PARTIAL_BUILD);
				const bool FEASIBLE = m_Models.front()->doplanning(SOLVE, m_allSchedules.front());
				if (FEASIBLE||!SOLVE)
					{
					m_ResultsWriter->getandwrite(m_Models.front(), m_Outputs.front());
				}else {
					_logger->logwithlevel("Thread:" + getThreadId() + " infeasible scenario " + m_Models.front()->getname() + "\n", 0);
					}
				_logger->logwithlevel("Thread:" + getThreadId() + " Planning of " + m_Models.front()->getname() + " done\n", 0);
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
				for (std::unique_ptr<Models::FMTmodel>& model : modelskept)
				{
					m_Models.push_back(std::move(model));
				}
			}
			//setstatus(true);
		}catch (...)
		{
			_exhandler->raisefromthreadcatch("","FMTplanningtask::work", __LINE__, __FILE__);
		}
	}

	void FMTplanningtask::finalize()
	{
		try {
		
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTplanningtask::finalize", __LINE__, __FILE__);
		}
	}

}