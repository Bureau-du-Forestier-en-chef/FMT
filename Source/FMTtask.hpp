/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/
#ifndef FMTTASK_H_INCLUDED
#define FMTTASK_H_INCLUDED
#include <memory>
#include "FMTobject.hpp"
#include <boost/thread/recursive_mutex.hpp>

namespace Logger
{
	class FMTlogger;
}


/// Namespace for parallel tasking may include multithreading / multiprocessing
namespace Parallel
{
	// DocString: FMTtask
	/**
	Task for parallel jobs needs to implement a clone / work / split functions for your task
	to used it in the taskhandler.
	*/
	class FMTEXPORT FMTtask : public Core::FMTobject
	{
		// DocString: FMTtaskhandler::alltasks
		///If the task is done true else false
		bool done;
		protected:
		// DocString: FMTtaskhandler::mutex
		///Recursive mutex for the task
		static boost::recursive_mutex taskmutex;
		// DocString: FMTtaskhandler::tasklogger
		///Logger for solver in parallel...coinmessagehandler does not support concurency.
		///This logger wont print anything so dont use it in parallel.
		std::unique_ptr<Logging::FMTlogger>tasklogger;
		// DocString: FMTreplanningtask::setstatus()
		/**
		Change the status of the task
		*/
		void setstatus(bool status);
	public:
		// DocString: FMTreplanningtask::FMTtask()
		/**
		Default constructor for FMTtask
		*/
		FMTtask();
		// DocString: ~FMTtask()
		/**
		FMTtask default virutal destructor.
		*/
		virtual ~FMTtask() = default;
		// DocString: FMTreplanningtask::FMTtask(const FMTtask&)
		/**
		Default copy constructor for FMTtask
		*/
		FMTtask(const FMTtask& rhs);
		// DocString: FMTreplanningtask::operator=(const FMTtask&)
		/**
		Default copy constructor for FMTtask
		*/
		FMTtask& operator = (const FMTtask& rhs);
		// DocString: FMTreplanningtask::clone()
		/**
		Clone function for FMTtask.
		*/
		virtual std::unique_ptr<FMTtask>clone() const;
		// DocString: FMTreplanningtask::split()
		/**
		Function to split the actual task in multiple tasks.
		*/
		virtual std::vector<std::unique_ptr<FMTtask>>split(const unsigned int& numberoftasks) const;
		// DocString: FMTreplanningtask::spawn()
		/**
		Will spawn a minimal task from the master task
		*/
		virtual std::unique_ptr<FMTtask>spawn();
		// DocString: FMTreplanningtask::work
		/**
		This function is the main job executed by the thread.
		*/
		virtual void work();
		// DocString: FMTreplanningtask::isdone()
		/**
		Returns true if the job is all done.
		*/
		bool isdone() const;
		// DocString: FMTreplanningtask::getthreadid()
		/**
		Get the thread id of the task.
		*/
		std::string getthreadid() const;
	};

}



#endif
