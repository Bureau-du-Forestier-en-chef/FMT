/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/
#ifndef FMTTASK_Hm_included
#define FMTTASK_Hm_included
#include <memory>
#include "FMTobject.h"
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#if defined _MSC_VER
	#include "FMTScopedSeTranslator.h"
#endif


/// Namespace for parallel tasking may include multithreading / multiprocessing
namespace Parallel
{
	// DocString: FMTTask
	/**
	Task for parallel jobs needs to implement a clone / work / split functions for your task
	to used it in the taskhandler.
	*/
	class FMTEXPORT FMTTask : public Core::FMTObject
	{
	public:
		// DocString: FMTTask::FMTTask()
		/**
		Default constructor for FMTTask
		*/
		FMTTask();
		// DocString: ~FMTTask()
		/**
		FMTTask default virutal destructor.
		*/
		virtual ~FMTTask() = default;
		// DocString: FMTTask::FMTTask(const FMTTask&)
		/**
		Default copy constructor for FMTTask
		*/
		FMTTask(const FMTTask& rhs);
		// DocString: FMTTask::operator=(const FMTTask&)
		/**
		Default copy constructor for FMTTask
		*/
		FMTTask& operator = (const FMTTask& rhs);
		// DocString: FMTTask::clone()
		/**
		Clone function for FMTTask.
		*/
		virtual std::unique_ptr<FMTTask>clone() const;
		// DocString: FMTTask::split()
		/**
		Function to split the actual task in multiple tasks.
		*/
		virtual std::vector<std::unique_ptr<FMTTask>>split(const unsigned int& numberoftasks) const;
		// DocString: FMTTask::spawn()
		/**
		Will spawn a minimal task from the master task
		*/
		virtual std::unique_ptr<FMTTask>spawn();
		// DocString: FMTTask::work
		/**
		This function is the main job executed by the thread.
		*/
		virtual void work();
		// DocString: FMTTask::run
		/**
		@brief setup the crash handler and call work
		*/
		virtual void run();
		// DocString: FMTTask::finalize
		/**
		If this function is overrided it will be called right after the last task
		is done
		*/
		virtual void finalize();
		// DocString: FMTTask::isdone()
		/**
		Returns true if the job is all done.
		*/
		//bool isdone() const;
		// DocString: FMTTask::getThreadId()
		/**
		@brief Get the thread id of the task.
		*/
		static std::string getThreadId();
		// DocString: FMTTask::setTotalThreads()
		/**
		@brief set the total amount of threads
		@param[in] the number of threads.
		*/
		static void setTotalThreads(const size_t& p_threads);
		// DocString: FMTTaskHandler::done
		///If the task is done true else false
		bool done;
	protected:
		// DocString: FMTTask::mutex
		///Recursive mutex for the task
		static boost::recursive_mutex taskmutex;
		// DocString: FMTTask::tasklogger
		///Logger for solver in parallel...coinmessagehandler does not support concurency.
		///This logger wont print anything so dont use it in parallel.
		std::unique_ptr<Logging::FMTLogger>tasklogger;
		// DocString: FMTTask::setstatus()
		/**
		Change the status of the task
		*/
		//void setstatus(bool status);
		// DocString: FMTTask::split()
		/**
		Split the totalworksize in much possible equal buckets.
		*/
		std::vector<size_t>splitWork(int numberoftasks,const int& totalworksize) const;
		// DocString: FMTTask::decrementWorkingThread()
		/**
		@brief decrement the number of thread working.
		*/
		void decrementWorkingThread();
		// DocString: FMTTask::checkpoint()
		/**
		@brief wait for other thread to finish there jobs, if there job is done then continue.
		*/
		void checkpoint();
		// DocString: FMTTask::_setCrashHandlers
		/**
		@brief set the terminate and abort handler before running global function in thread
		also set the se_translator on windows.
		*/
		void _setCrashHandlers();
	private:
		// DocString: FMTTask::m_workingThreads
		///The number of threads that are working
		static size_t m_workingThreads;
		// DocString: FMTTask::m_allThreads
		///The total number of threads.
		static size_t m_allThreads;
		// DocString: FMTTask::m_checkpoint
		///The checkpoint condition variable.
		static boost::condition_variable m_checkpoint;
		// DocString: FMTTask::m_checkpointMutex
		///Mutex checkpoint
		static boost::mutex m_checkpointMutex;
		#if defined _MSC_VER
		// DocString: FMTTask::m_SeTranslator
		///Translator for windows error
		Exception::FMTScopedSeTranslator m_SeTranslator;
		#endif
	
		
	};

}

#endif
