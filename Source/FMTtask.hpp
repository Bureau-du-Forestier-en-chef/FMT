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
		// DocString: FMTtaskhandler::mutex
		///Recursive mutex for the task
		mutable boost::recursive_mutex taskmutex;
		protected:
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
		FMTtask() = default;
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
		FMTtask& operator = (const FMTtask& rhs) = default;
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
	};

}



#endif
