/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTTaskHandler.h"
#include "FMTTask.h"
#include <boost/thread.hpp>
#include <functional>
#include <list>
#if defined FMTWITHPYTHON
	#include "boost/python.h"
#endif
#include "FMTExceptionHandler.h"
#include "FMTWorkerTask.h"

namespace Parallel
{
	FMTTaskHandler::FMTTaskHandler(const FMTTask& maintask, unsigned int maxthread):
		m_maxnumberofthread(),
		m_alltasks()
		{
		try {
			_setMaximumNumberOfThreads(maintask, maxthread);
			m_alltasks.push_back(std::move(maintask.clone()));
		}catch (...)
			{
			_exhandler->printExceptions("FMTtaskhandler reference constructor",
				"FMTTaskHandler::FMTTaskHandler", __LINE__, __FILE__);
			}
		}

	FMTTaskHandler::FMTTaskHandler(const std::unique_ptr<FMTTask>& maintask,
		unsigned int maxthread) :
		m_maxnumberofthread(),
		m_alltasks()
	{
		_setMaximumNumberOfThreads(*maintask, maxthread);
		m_alltasks.push_back(std::move(maintask->clone()));
	}

	FMTTaskHandler::FMTTaskHandler(const FMTTaskHandler& rhs):
		m_maxnumberofthread(rhs.m_maxnumberofthread),
		m_alltasks()
		{
		for (const std::unique_ptr<FMTTask>& task : rhs.m_alltasks)
			{
			m_alltasks.push_back(std::move(task->clone()));
			}

		}

	FMTTaskHandler& FMTTaskHandler::operator =(const FMTTaskHandler& rhs)
		{
		if (this!=&rhs)
			{
			m_maxnumberofthread = rhs.m_maxnumberofthread;
			m_alltasks.clear();
			for (const std::unique_ptr<FMTTask>& task : rhs.m_alltasks)
				{
				m_alltasks.push_back(std::move(task->clone()));
				m_alltasks.push_back(std::move(task->clone()));
				}
			}
		return *this;
		}

	const std::vector<std::unique_ptr<FMTTask>>& FMTTaskHandler::getTasks() const
		{
		return m_alltasks;
		}
	
	void FMTTaskHandler::_splitTasks()
		{
		try {
			if (m_alltasks.size()>1)
				{
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Too much task to split",
					"FMTTask::split", __LINE__, __FILE__);
				}
			std::vector<std::unique_ptr<FMTTask>> newtasks = std::move(m_alltasks.at(0)->split(m_maxnumberofthread));
			m_alltasks.swap(newtasks);
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTTaskHandler::_splitTasks", __LINE__, __FILE__);
			}
		}

	void FMTTaskHandler::_finalize(std::unique_ptr<FMTTask>& lasttask)
	{
		try {
			lasttask->finalize();
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTTaskHandler::_finalize", __LINE__, __FILE__);
		}

	}

	void FMTTaskHandler::_setMaximumNumberOfThreads(const FMTTask& p_MainTask,
													unsigned int p_UserDecision)
		{
		if (p_MainTask.SupportsMultiThreading())
			{
			m_maxnumberofthread = std::min(boost::thread::hardware_concurrency(), p_UserDecision);
		}else{
			if (p_UserDecision>1)
				{
				_exhandler->raise(Exception::FMTexc::FMTignore,
					"The maximum number of thread will be set to 1 because de task does not support multithreading"
					, "FMTTaskHandler::_setMaximumNumberOfThreads", __LINE__, __FILE__);
				}
			m_maxnumberofthread =  1U;
			}
		}

	void FMTTaskHandler::_interruptWork(boost::thread& p_thread)
		{
		if (p_thread.joinable())
			{
			try {
				p_thread.interrupt();
			}catch(...){};
			}
		}

	void FMTTaskHandler::conccurentRun()
		{
		std::vector<boost::thread>workers;
		try {
			const std::chrono::time_point<std::chrono::high_resolution_clock>tasksstart = getClock();
			_splitTasks();
			FMTTask::setTotalThreads(m_alltasks.size());
			for (std::unique_ptr<FMTTask>& task : m_alltasks)
				{
				workers.push_back(boost::thread(&FMTTask::run,task.get()));
				}
			for (boost::thread& worker : workers)
				{
				worker.join();
				}
			checkSignals();
			if (!m_alltasks.empty())
			{
				_finalize(m_alltasks.back());
			}
			_logTaskTime(tasksstart);
		}catch (...)
			{
			for (boost::thread& worker : workers)
				{
				_interruptWork(worker);
				}
				_exhandler->printExceptions("", "FMTTaskHandler::conccurentRun", __LINE__, __FILE__);
			}

		}

	void FMTTaskHandler::passInLogger(const std::unique_ptr<Logging::FMTLogger>& logger)
		{
		// TODO GAB faire un warning si on passe par ici on créer un nouveau logger qui va chier en multithreads
		for (std::unique_ptr<FMTTask>& task : m_alltasks)
			{
			task->passInLogger(logger);
			}
		}

	void FMTTaskHandler::onDemandRun()
	{
		std::list<FMTWorkerTask> workers;

		try
		{
			const auto tasksstart = getClock();

			if (m_alltasks.size() != 1)
			{
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
					"Need to have one master task for ondemandrun",
					"FMTTaskHandler::onDemandRun", __LINE__, __FILE__);
			}

			if (m_maxnumberofthread <= 0)
			{
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
					"Invalid thread count",
					"FMTTaskHandler::onDemandRun", __LINE__, __FILE__);
			}

			FMTTask::setTotalThreads(static_cast<size_t>(m_maxnumberofthread));

			std::unique_ptr<FMTTask> newtask = m_alltasks.at(0)->spawn();

			while (workers.size() < static_cast<size_t>(m_maxnumberofthread) && newtask)
			{
				workers.emplace_back(std::move(newtask));
				newtask = m_alltasks.at(0)->spawn();
			}

			while (!workers.empty())
			{
				for (auto it = workers.begin(); it != workers.end();)
				{
					if (it->isDone())
					{
						it = workers.erase(it);
						if (newtask)
						{
							workers.emplace_back(std::move(newtask));
							newtask = m_alltasks.at(0)->spawn();
						}
					}
					else
					{
						++it;
					}
				}
				checkSignals();
			}

			_finalize(m_alltasks.back());
			_logTaskTime(tasksstart);
		}
		catch (...)
		{
			for (FMTWorkerTask & wt : workers)
			{
				_interruptWork(wt.getThread());
			}
			workers.clear();

			_exhandler->printExceptions("", "FMTTaskHandler::onDemandRun",
				__LINE__, __FILE__);
		}
	}

	void FMTTaskHandler::_logTaskTime(const std::chrono::time_point<std::chrono::high_resolution_clock>& startime) const
	{
		try {
			_logger->logWithLevel("All tasks completed " + getDurationInSeconds(startime) + "\n", 0);
		}
		catch (...)
		{
			_exhandler->printExceptions("", "FMTTaskHandler::_logTaskTime", __LINE__, __FILE__);
		}
	}

}