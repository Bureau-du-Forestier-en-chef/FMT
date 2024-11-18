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

namespace Parallel
{


	FMTtaskhandler::FMTtaskhandler(const FMTtask& maintask, unsigned int maxthread):
		maxnumberofthread(std::min(boost::thread::hardware_concurrency(), maxthread)),
		alltasks()
		{
		try {
			alltasks.push_back(std::move(maintask.clone()));
		}catch (...)
			{
			_exhandler->printexceptions("FMTtaskhandler reference constructor",
				"FMTtaskhandler::FMTtaskhandler", __LINE__, __FILE__);
			}
		}

	FMTtaskhandler::FMTtaskhandler(const std::unique_ptr<FMTtask>& maintask,
		unsigned int maxthread) :
		maxnumberofthread(std::min(boost::thread::hardware_concurrency(), maxthread)),
		alltasks()
	{
		
		alltasks.push_back(std::move(maintask->clone()));
	}

	FMTtaskhandler::FMTtaskhandler(const FMTtaskhandler& rhs):
		maxnumberofthread(rhs.maxnumberofthread),
		alltasks()
		{
		for (const std::unique_ptr<FMTtask>& task : rhs.alltasks)
			{
			alltasks.push_back(std::move(task->clone()));
			}

		}

	FMTtaskhandler& FMTtaskhandler::operator =(const FMTtaskhandler& rhs)
		{
		if (this!=&rhs)
			{
			maxnumberofthread = rhs.maxnumberofthread;
			alltasks.clear();
			for (const std::unique_ptr<FMTtask>& task : rhs.alltasks)
				{
				alltasks.push_back(std::move(task->clone()));
				alltasks.push_back(std::move(task->clone()));
				}
			}
		return *this;
		}

	const std::vector<std::unique_ptr<FMTtask>>& FMTtaskhandler::gettasks() const
		{
		return alltasks;
		}
	
	void FMTtaskhandler::splittasks()
		{
		try {
			if (alltasks.size()>1)
				{
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Too much task to split",
					"FMTtask::split", __LINE__, __FILE__);
				}
			std::vector<std::unique_ptr<FMTtask>> newtasks = std::move(alltasks.at(0)->split(maxnumberofthread));
			alltasks.swap(newtasks);
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTtaskhandler::splittasks", __LINE__, __FILE__);
			}
		}

	void FMTtaskhandler::finalize(std::unique_ptr<FMTtask>& lasttask)
	{
		try {
			lasttask->finalize();
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTtaskhandler::finalize", __LINE__, __FILE__);
		}

	}

	void FMTtaskhandler::_interruptWork(boost::thread& p_thread)
		{
		if (p_thread.joinable())
			{
			try {
				p_thread.interrupt();
			}catch(...){};
			}
		}

	void FMTtaskhandler::conccurentrun()
		{
		std::vector<boost::thread>workers;
		try {
			const std::chrono::time_point<std::chrono::high_resolution_clock>tasksstart = getclock();
			splittasks();
			FMTtask::setTotalThreads(alltasks.size());
			for (std::unique_ptr<FMTtask>& task : alltasks)
				{
				workers.push_back(boost::thread(&FMTtask::work,task.get()));
				}
			for (boost::thread& worker : workers)
				{
				worker.join();
				}
			checksignals();
			if (!alltasks.empty())
			{
				finalize(alltasks.back());
			}
			logtasktime(tasksstart);
		}catch (...)
			{
			for (boost::thread& worker : workers)
				{
				_interruptWork(worker);
				}
				_exhandler->printexceptions("", "FMTtaskhandler::conccurentrun", __LINE__, __FILE__);
			}

		}

	void FMTtaskhandler::passinlogger(const std::unique_ptr<Logging::FMTlogger>& logger)
		{
		for (std::unique_ptr<FMTtask>& task : alltasks)
			{
			task->passinlogger(logger);
			}
		}

	void FMTtaskhandler::ondemandrun()
	{
		std::list<boost::thread>workers;
		try {
			const std::chrono::time_point<std::chrono::high_resolution_clock>tasksstart = getclock();
			if (alltasks.size() != 1)
			{
				_exhandler->raise(Exception::FMTexc::FMTfunctionfailed, "Need to have one master task for ondemandrun",
					"FMTtaskhandler::ondemandrun", __LINE__, __FILE__);
			}
			std::list<std::unique_ptr<FMTtask>>tasks;
			unsigned int taskid = 0;
			std::unique_ptr<FMTtask>newtask = alltasks.at(0)->spawn();
			while (taskid < maxnumberofthread && newtask)
			{
				tasks.push_back(std::move(newtask));
				workers.push_back(boost::thread(&FMTtask::work, tasks.back().get()));
				newtask = std::move(alltasks.at(0)->spawn());
				++taskid;
			}
			while (!tasks.empty())
			{
				std::list<std::unique_ptr<FMTtask>>::iterator taskit = tasks.begin();
				std::list<boost::thread>::const_iterator threadit = workers.begin();
				for (size_t taskid = 0; taskid < tasks.size(); ++taskid)
				{
					if ((*taskit)->isdone())
					{
						if (!newtask)//Finalize 
						{
							finalize(*taskit);
						}
						tasks.erase(taskit);
						workers.erase(threadit);
						if (newtask)
						{
							tasks.push_back(std::move(newtask));
							workers.push_back(boost::thread(&FMTtask::work, tasks.back().get()));
							newtask = std::move(alltasks.at(0)->spawn());
						}
						break;
					}
					checksignals();
					++taskit;
					++threadit;
				}
			}
			logtasktime(tasksstart);
		}catch (...)
			{
			for (boost::thread& worker : workers)
				{
				_interruptWork(worker);
				}
			_exhandler->printexceptions("", "FMTtaskhandler::ondemandrun", __LINE__, __FILE__);
			}
	}

	void FMTtaskhandler::logtasktime(const std::chrono::time_point<std::chrono::high_resolution_clock>& startime) const
	{
		try {
			_logger->logwithlevel("All tasks completed " + getdurationinseconds(startime) + "\n", 0);
		}
		catch (...)
		{
			_exhandler->printexceptions("", "FMTtaskhandler::logtasktime", __LINE__, __FILE__);
		}
	}

}