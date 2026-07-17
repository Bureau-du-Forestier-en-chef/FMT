/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/
#ifndef FMTTASKHANDLER_Hm_included
#define FMTTASKHANDLER_Hm_included
#include <memory>
#include "FMTobject.h"

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
	Main class for running concurrent tasks at the same time.
	It uses the virtual function split() from FMTTask to split the task in multiple task and can run it concurrently
	*/
	class FMTEXPORT FMTTaskHandler : public Core::FMTObject
	{
	
	public:
		// DocString: FMTTaskHandler::FMTTaskHandler(const std::unique_ptr<FMTTask>&,unsigned int)
		/**
		Construct the handler with the number of threads you want.
		*/
		FMTTaskHandler(const std::unique_ptr<FMTTask>& maintask,
						unsigned int maxthread = 0);
		// DocString: ~FMTTaskHandler()
		/**
		Default destructor for FMTTaskHandler.
		*/
		~FMTTaskHandler()=default;
		// DocString: FMTTaskHandler::FMTTaskHandler(const Parallel::FMTTask&,unsigned int)
		/**
		Abstract constructor for Python and R...you need to pass a FMTTask to this constructor.
		*/
		FMTTaskHandler(const FMTTask& maintask,
			unsigned int maxthread = 0);
		// DocString: FMTTaskHandler::FMTTaskHandler()
		/**
		Default constructor for FMTTaskHandler
		*/
		FMTTaskHandler() = default;
		// DocString: FMTTaskHandler::FMTTaskHandler(const FMTTaskHandler)
		/**
		Default copy constructor for FMTTaskHandler
		*/
		FMTTaskHandler(const FMTTaskHandler& rhs);
		// DocString: FMTTaskHandler::operator=
		/**
		Default copy assignement for FMTTaskHandler
		*/
		FMTTaskHandler& operator =(const FMTTaskHandler& rhs);
		// DocString: FMTTaskHandler::getTasks
		/**
		Return reference to the task handled by the task handler.
		*/
		const std::vector<std::unique_ptr<FMTTask>>& getTasks() const;
		// DocString: FMTTaskHandler::getTasksFromDynamicCast
		/**
		Return reference to the task already in the ptrype.
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
		Will use the split task virtual function to
		generate all the tasks at the same time.
		*/
		void conccurentRun();
		// DocString: FMTTaskHandler::onDemandRun
		/**
		Will use the spawn task virtual function to generate task from the master task and add to a list
		If the task is completed it will be removed from the list.
		*/
		void onDemandRun();
		// DocString: FMTTaskHandler::passInLogger
		/**
		We need to override the passinlogger for the osisolverinterface
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
		Will split all task in alltasks and replace it with new splitted tasks
		*/
		void splitTasks();
		// DocString: FMTTaskHandler::finalize
		/**
		This function will call the FMTTask::finalize function when the last task is done.
		*/
		void finalize(std::unique_ptr<FMTTask>& lasttask);
		// DocString: FMTTaskHandler::logTaskTime
		/**
		Juste write down all the time it took to run all tasks
		*/
		void logTaskTime(const std::chrono::time_point<std::chrono::high_resolution_clock>& startime) const;
		// DocString: FMTTaskHandler::_interruptWork
		/**
		@brief interupt the work of a thread in case on a exception to make sure to dont get a std::terminate if joinable.
		@p_threads the threads to interrupt
		*/
		static void _interruptWork(boost::thread& p_thread);

	};

}

#endif
