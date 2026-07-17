/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTtaskhandler.h"
#include "FMTtask.h"
#include <boost/thread.hpp>
#include <functional>
#include <list>
#if defined FMTWITHPYTHON
	#include "boost/python.h"
#endif
#include "FMTexceptionhandler.h"
#include "FMTWorkerTask.h"

namespace Parallel
{
	FMTTaskHandler::FMTTaskHandler(const FMTTask& maintask, unsigned int maxthread):
		maxnumberofthread(std::min(boost::thread::hardware_concurrency(), maxthread)),
		alltasks()
		{
		try {
			alltasks.push_back(std::move(maintask.clone()));
		}catch (...)
			{
			_exhandler->printExceptions("FMTtaskhandler reference constructor",
				"FMTTaskHandler::FMTTaskHandler", __LINE__, __FILE__);
			}
		}

	FMTTaskHandler::FMTTaskHandler(const std::unique_ptr<FMTTask>& maintask,
		unsigned int maxthread) :
		maxnumberofthread(std::min(boost::thread::hardware_concurrency(), maxthread)),
		alltasks()
	{
		
		alltasks.push_back(std::move(maintask->clone()));
	}

	FMTTaskHandler::FMTTaskHandler(const FMTTaskHandler& rhs):
		maxnumberofthread(rhs.maxnumberofthread),
		alltasks()
		{
		for (const std::unique_ptr<FMTTask>& task : rhs.alltasks)
			{
			alltasks.push_back(std::move(task->clone()));
			}

		}

	FMTTaskHandler& FMTTaskHandler::operator =(const FMTTaskHandler& rhs)
		{
		if (this!=&rhs)
			{
			maxnumberofthread = rhs.maxnumberofthread;
			alltasks.clear();
			for (const std::unique_ptr<FMTTask>& task : rhs.alltasks)
				{
				alltasks.push_back(std::move(task->clone()));
				alltasks.push_back(std::move(task->clone()));
				}
			}
		return *this;
		}

	const std::vector<std::unique_ptr<FMTTask>>& FMTTaskHandler::getTasks() const
		{
		return alltasks;
		}
	
	void FMTTaskHandler::splitTasks()
		{
		try {
			if (alltasks.size()>1)
				{
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Too much task to split",
					"FMTTask::split", __LINE__, __FILE__);
				}
			std::vector<std::unique_ptr<FMTTask>> newtasks = std::move(alltasks.at(0)->split(maxnumberofthread));
			alltasks.swap(newtasks);
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTTaskHandler::splitTasks", __LINE__, __FILE__);
			}
		}

	void FMTTaskHandler::finalize(std::unique_ptr<FMTTask>& lasttask)
	{
		try {
			lasttask->finalize();
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTTaskHandler::finalize", __LINE__, __FILE__);
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
			splitTasks();
			FMTTask::setTotalThreads(alltasks.size());
			for (std::unique_ptr<FMTTask>& task : alltasks)
				{
				workers.push_back(boost::thread(&FMTTask::run,task.get()));
				}
			for (boost::thread& worker : workers)
				{
				worker.join();
				}
			checkSignals();
			if (!alltasks.empty())
			{
				finalize(alltasks.back());
			}
			logTaskTime(tasksstart);
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
		for (std::unique_ptr<FMTTask>& task : alltasks)
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

			if (alltasks.size() != 1)
			{
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
					"Need to have one master task for ondemandrun",
					"FMTTaskHandler::onDemandRun", __LINE__, __FILE__);
			}

			if (maxnumberofthread <= 0)
			{
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
					"Invalid thread count",
					"FMTTaskHandler::onDemandRun", __LINE__, __FILE__);
			}

			FMTTask::setTotalThreads(static_cast<size_t>(maxnumberofthread));

			std::unique_ptr<FMTTask> newtask = alltasks.at(0)->spawn();

			while (workers.size() < static_cast<size_t>(maxnumberofthread) && newtask)
			{
				workers.emplace_back(std::move(newtask));
				newtask = alltasks.at(0)->spawn();
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
							newtask = alltasks.at(0)->spawn();
						}
					}
					else
					{
						++it;
					}
				}
				checkSignals();
			}

			finalize(alltasks.back());
			logTaskTime(tasksstart);
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

	void FMTTaskHandler::logTaskTime(const std::chrono::time_point<std::chrono::high_resolution_clock>& startime) const
	{
		try {
			_logger->logWithLevel("All tasks completed " + getDurationInSeconds(startime) + "\n", 0);
		}
		catch (...)
		{
			_exhandler->printExceptions("", "FMTTaskHandler::logTaskTime", __LINE__, __FILE__);
		}
	}

}