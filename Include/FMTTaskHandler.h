/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/
#ifndef FMTTASKHANDLER_Hm_included
#define FMTTASKHANDLER_Hm_included
#include <memory>
#include "FMTObject.h"

namespace boost
	{
	class thread;
	}

/// Namespace for parallel tasking may include multithreading / multiprocessing
namespace Parallel
{
	class FMTTask;
	// DocString: FMTTaskHandler
	/**
	@brief Main class for running concurrent tasks at the same time.
	@details It uses the virtual split function of FMTTask to split the task into multiple tasks and run them concurrently.
	*/
	class FMTEXPORT FMTTaskHandler : public Core::FMTObject
	{
	
	public:
		// DocString: FMTTaskHandler::FMTTaskHandler(const std::unique_ptr<FMTTask>&,unsigned int)
		/**
		@brief Construct the handler from a main task and a number of threads.
		@param[in] maintask the main task.
		@param[in] maxthread the maximum number of threads.
		*/
		FMTTaskHandler(const std::unique_ptr<FMTTask>& maintask,
						unsigned int maxthread = 0);
		// DocString: ~FMTTaskHandler()
		/**
		@brief Default destructor for FMTTaskHandler.
		*/
		~FMTTaskHandler()=default;
		// DocString: FMTTaskHandler::FMTTaskHandler(const Parallel::FMTTask&,unsigned int)
		/**
		@brief Constructor for Python and R taking a FMTTask by reference.
		@param[in] maintask the main task.
		@param[in] maxthread the maximum number of threads.
		*/
		FMTTaskHandler(const FMTTask& maintask,
			unsigned int maxthread = 0);
		// DocString: FMTTaskHandler::FMTTaskHandler()
		/**
		@brief Default constructor for FMTTaskHandler.
		*/
		FMTTaskHandler() = default;
		// DocString: FMTTaskHandler::FMTTaskHandler(const FMTTaskHandler)
		/**
		@brief Default copy constructor for FMTTaskHandler.
		@param[in] rhs the FMTTaskHandler to copy.
		*/
		FMTTaskHandler(const FMTTaskHandler& rhs);
		// DocString: FMTTaskHandler::operator=
		/**
		@brief Default copy assignment operator for FMTTaskHandler.
		@param[in] rhs the FMTTaskHandler to copy.
		@return a reference to this FMTTaskHandler.
		*/
		FMTTaskHandler& operator =(const FMTTaskHandler& rhs);
		// DocString: FMTTaskHandler::getTasks
		/**
		@brief Return a reference to the tasks handled by the task handler.
		@return the tasks.
		*/
		const std::vector<std::unique_ptr<FMTTask>>& getTasks() const;
		// DocString: FMTTaskHandler::getTasksFromDynamicCast
		/**
		@brief Return the tasks dynamically cast to the requested pointer type.
		@tparam ptrtype the task pointer type.
		@return the casted tasks.
		*/
		template<class ptrtype>
		const std::vector<const ptrtype*> getTasksFromDynamicCast() const
		{
			std::vector<const ptrtype*>castedptr;
			for (const std::unique_ptr<FMTTask>& task : alltasks)
			{
				castedptr.push_back(dynamic_cast<const ptrtype*>(task.get()));
			}
			return castedptr;
		}
		// DocString: FMTTaskHandler::conccurentRun
		/**
		@brief Use the split task virtual function to generate all the tasks and run them at the same time.
		*/
		void conccurentRun();
		// DocString: FMTTaskHandler::onDemandRun
		/**
		@brief Use the spawn task virtual function to generate tasks from the master task and add them to a list, removing completed tasks.
		*/
		void onDemandRun();
		// DocString: FMTTaskHandler::passInLogger
		/**
		@brief Pass in the logger, overridden for the OsiSolverInterface.
		@param[in] logger the logger.
		*/
		void passInLogger(const std::unique_ptr<Logging::FMTLogger>& logger) override;
	private:
		// DocString: FMTTaskHandler::maxnumberofthread
		///Maximal number of threads
		unsigned int maxnumberofthread;
		// DocString: FMTTaskHandler::alltasks
		///All the tasks that are running
		std::vector<std::unique_ptr<FMTTask>>alltasks;
		// DocString: FMTTaskHandler::splitTasks
		/**
		@brief Split all the tasks and replace them with the newly split tasks.
		*/
		void splitTasks();
		// DocString: FMTTaskHandler::finalize
		/**
		@brief Call FMTTask::finalize when the last task is done.
		@param[in,out] lasttask the last task.
		*/
		void finalize(std::unique_ptr<FMTTask>& lasttask);
		// DocString: FMTTaskHandler::logTaskTime
		/**
		@brief Log the time it took to run all the tasks.
		@param[in] startime the start time.
		*/
		void logTaskTime(const std::chrono::time_point<std::chrono::high_resolution_clock>& startime) const;
		// DocString: FMTTaskHandler::_interruptWork
		/**
		@brief Interrupt the work of a thread on an exception to avoid a std::terminate if it is joinable.
		@param[in] p_thread the thread to interrupt.
		*/
		static void _interruptWork(boost::thread& p_thread);

	};

}

#endif
