/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/
#ifndef FMTTASK_Hm_included
#define FMTTASK_Hm_included
#include <memory>
#include "FMTObject.h"
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
	@brief Task for parallel jobs.
	@details Implement the clone, work and split functions of your task to use it in the FMTTaskHandler.
	*/
	class FMTEXPORT FMTTask : public Core::FMTObject
	{
	public:
		// DocString: FMTTask::FMTTask()
		/**
		@brief Default constructor for FMTTask.
		*/
		FMTTask();
		// DocString: ~FMTTask()
		/**
		@brief Default virtual destructor for FMTTask.
		*/
		virtual ~FMTTask() = default;
		// DocString: FMTTask::FMTTask(const FMTTask&)
		/**
		@brief Default copy constructor for FMTTask.
		@param[in] rhs the FMTTask to copy.
		*/
		FMTTask(const FMTTask& rhs);
		// DocString: FMTTask::operator=(const FMTTask&)
		/**
		@brief Default copy assignment operator for FMTTask.
		@param[in] rhs the FMTTask to copy.
		@return a reference to this FMTTask.
		*/
		FMTTask& operator = (const FMTTask& rhs);
		// DocString: FMTTask::clone()
		/**
		@brief Clone function for FMTTask.
		@return a unique pointer to the cloned task.
		*/
		virtual std::unique_ptr<FMTTask>clone() const;
		// DocString: FMTTask::split()
		/**
		@brief Split the actual task into multiple tasks.
		@param[in] numberoftasks the number of tasks.
		@return the split tasks.
		*/
		virtual std::vector<std::unique_ptr<FMTTask>>split(const unsigned int& numberoftasks) const;
		// DocString: FMTTask::spawn()
		/**
		@brief Spawn a minimal task from the master task.
		@return a unique pointer to the spawned task.
		*/
		virtual std::unique_ptr<FMTTask>spawn();
		// DocString: FMTTask::work
		/**
		@brief Main job executed by the thread.
		*/
		virtual void work();
		// DocString: FMTTask::run
		/**
		@brief Set up the crash handler and call work.
		*/
		virtual void run();
		// DocString: FMTTask::finalize
		/**
		@brief Called right after the last task is done, when overridden.
		*/
		virtual void finalize();
		// DocString: FMTTask::isdone()
		/**
		@brief Return true if the job is all done.
		@return true if the job is done else false.
		*/
		//bool isdone() const;
		// DocString: FMTTask::getThreadId()
		/**
		@brief Get the thread id of the task.
		@return the thread id.
		*/
		static std::string getThreadId();
		// DocString: FMTTask::setTotalThreads()
		/**
		@brief Set the total amount of threads.
		@param[in] p_threads the number of threads.
		*/
		static void setTotalThreads(const size_t& p_threads);
		// DocString: FMTTaskHandler::done
		///If the task is done true else false
		bool done;
	protected:
		// DocString: FMTTask::mutex
		///Recursive mutex for the task
		static boost::recursive_mutex m_taskmutex;
		// DocString: FMTTask::m_tasklogger
		///Logger for solver in parallel...coinmessagehandler does not support concurency.
		///This logger wont print anything so dont use it in parallel.
		std::unique_ptr<Logging::FMTLogger>m_tasklogger;
		// DocString: FMTTask::setstatus()
		/**
		@brief Change the status of the task.
		*/
		//void setstatus(bool status);
		// DocString: FMTTask::_splitWork
		/**
		@brief Split the total work size into buckets that are as equal as possible.
		@param[in] numberoftasks the number of tasks.
		@param[in] totalworksize the total work size.
		@return the work size of each bucket.
		*/
		std::vector<size_t>_splitWork(int numberoftasks,const int& totalworksize) const;
		// DocString: FMTTask::_decrementWorkingThread()
		/**
		@brief Decrement the number of working threads.
		*/
		void _decrementWorkingThread();
		// DocString: FMTTask::_checkpoint()
		/**
		@brief Wait for the other threads to finish their jobs, then continue.
		*/
		void _checkpoint();
		// DocString: FMTTask::_setCrashHandlers
		/**
		@brief Set the terminate and abort handlers before running the global function in the thread, and set the se_translator on Windows.
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
