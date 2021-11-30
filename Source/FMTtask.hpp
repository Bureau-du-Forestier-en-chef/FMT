/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/
#ifndef FMTTASK_H_INCLUDED
#define FMTTASK_H_INCLUDED
#include <memory>
#include "FMTobject.hpp"
#include <boost/thread/thread.hpp>

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
		// DocString: FMTtask::worker
		///The thread used for the task.
		boost::thread worker;
	protected:
		// DocString: FMTreplanningtask::work
		/**
		This function is the main job executed by the thread.
		*/
		virtual void work();
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
		virtual std::vector<std::unique_ptr<FMTtask>>split(const size_t& numberoftasks) const;
		// DocString: FMTreplanningtask::join()
		/**
		The join function for the worker.
		*/
		virtual void join();
		// DocString: FMTreplanningtask::run()
		/**
		The run function is used by the taskhandler to run multiple task.
		*/
		virtual void run();

	};

}



#endif
