/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/
#ifndef FMTTASK_H_INCLUDED
#define FMTTASK_H_INCLUDED
#include <memory>
#include "FMTobject.h"
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#if defined _WIN32
	#include "FMTScopedSeTranslator.h"
#endif


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
	public:
		// DocString: FMTtask::FMTtask()
		/**
		Default constructor for FMTtask
		*/
		FMTtask();
		// DocString: ~FMTtask()
		/**
		FMTtask default virutal destructor.
		*/
		virtual ~FMTtask() = default;
		// DocString: FMTtask::FMTtask(const FMTtask&)
		/**
		Default copy constructor for FMTtask
		*/
		FMTtask(const FMTtask& rhs);
		// DocString: FMTtask::operator=(const FMTtask&)
		/**
		Default copy constructor for FMTtask
		*/
		FMTtask& operator = (const FMTtask& rhs);
		// DocString: FMTtask::clone()
		/**
		Clone function for FMTtask.
		*/
		virtual std::unique_ptr<FMTtask>clone() const;
		// DocString: FMTtask::split()
		/**
		Function to split the actual task in multiple tasks.
		*/
		virtual std::vector<std::unique_ptr<FMTtask>>split(const unsigned int& numberoftasks) const;
		// DocString: FMTtask::spawn()
		/**
		Will spawn a minimal task from the master task
		*/
		virtual std::unique_ptr<FMTtask>spawn();
		// DocString: FMTtask::work
		/**
		This function is the main job executed by the thread.
		*/
		virtual void work();
		// DocString: FMTtask::run
		/**
		@brief setup the crash handler and call work
		*/
		virtual void run();
		// DocString: FMTtask::finalize
		/**
		If this function is overrided it will be called right after the last task
		is done
		*/
		virtual void finalize();
		// DocString: FMTtask::isdone()
		/**
		Returns true if the job is all done.
		*/
		bool isdone() const;
		// DocString: FMTtask::getthreadid()
		/**
		@brief Get the thread id of the task.
		*/
		static std::string getThreadId();
		// DocString: FMTtask::setTotalThreads()
		/**
		@brief set the total amount of threads
		@param[in] the number of threads.
		*/
		static void setTotalThreads(const size_t& p_threads);
		// DocString: FMTtaskhandler::done
		///If the task is done true else false
		bool done;
	protected:
		// DocString: FMTtask::mutex
		///Recursive mutex for the task
		static boost::recursive_mutex taskmutex;
		// DocString: FMTtask::tasklogger
		///Logger for solver in parallel...coinmessagehandler does not support concurency.
		///This logger wont print anything so dont use it in parallel.
		std::unique_ptr<Logging::FMTlogger>tasklogger;
		// DocString: FMTtask::setstatus()
		/**
		Change the status of the task
		*/
		void setstatus(bool status);
		// DocString: FMTtask::split()
		/**
		Split the totalworksize in much possible equal buckets.
		*/
		std::vector<size_t>splitwork(int numberoftasks,const int& totalworksize) const;
		// DocString: FMTtask::decrementWorkingThread()
		/**
		@brief decrement the number of thread working.
		*/
		void decrementWorkingThread();
		// DocString: FMTtask::checkpoint()
		/**
		@brief wait for other thread to finish there jobs, if there job is done then continue.
		*/
		void checkpoint();
		// DocString: FMTtask::_setCrashHandlers
		/**
		@brief set the terminate and abort handler before running global function in thread
		also set the se_translator on windows.
		*/
		void _setCrashHandlers();
	private:
		// DocString: FMTtask::m_workingThreads
		///The number of threads that are working
		static size_t m_workingThreads;
		// DocString: FMTtask::m_allThreads
		///The total number of threads.
		static size_t m_allThreads;
		// DocString: FMTtask::m_checkpoint
		///The checkpoint condition variable.
		static boost::condition_variable m_checkpoint;
		// DocString: FMTtask::m_checkpointMutex
		///Mutex checkpoint
		static boost::mutex m_checkpointMutex;
		#if defined _WIN32
		// DocString: FMTtask::m_SeTranslator
		///Translator for windows error
		Exception::FMTScopedSeTranslator m_SeTranslator;
		#endif
	
		
	};

}

#endif
