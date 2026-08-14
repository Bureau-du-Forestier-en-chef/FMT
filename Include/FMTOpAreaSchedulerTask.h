/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTopareaschedulertask_Hm_included
#define FMTopareaschedulertask_Hm_included

#include "FMTutility.h"
#include "FMTTask.h"
#include <vector>
#include <memory>
#include <string>
#include <chrono>
#include <boost/thread.hpp>
#include "FMTOutput.h"

namespace Core
{
	class FMTOutputNode;
	class FMTConstraint;
}


namespace Models
{
	class FMTLpModel;
}

namespace Heuristics
{
	class FMTOperatingAreaScheme;
	class FMTOperatingAreaScheduler;
}

/// Namespace for parallel tasking may include multithreading / multiprocessing
namespace Parallel
{
	// DocString: FMTOpAreaSchedulerTask
	/**
	@brief Task to find a suboptimal schedule for the operating areas of a FMTLpModel.
	@details It keeps the best scheduler in memory and tries to modify its schedule to get a better objective value, generating time yields for the operating areas.
	*/
	class FMTEXPORT FMTOpAreaSchedulerTask : public FMTTask
	{
		// DocString: FMTOpAreaSchedulerTask::generalmutex
		///When you want to get/set the bestscheduler or change the number of iterations.
		static boost::mutex generalmutex;
		// DocString: FMTOpAreaSchedulerTask::bestscheduler;
		///Best scheduler so far
		std::shared_ptr<Heuristics::FMTOperatingAreaScheduler>bestscheduler;
		// DocString: FMTOpAreaSchedulerTask::actualscheduler
		///The main task scheduler of the task
		std::unique_ptr<Heuristics::FMTOperatingAreaScheduler>actualscheduler;
		// DocString: FMTOpAreaSchedulerTask::solutionlocation
		///The solution location of the selected schedule
		static std::string solutionlocation;
		// DocString: FMTOpAreaSchedulerTask::iterations
		///The number of iterations left
		static unsigned int iterations;
		// DocString: FMTOpAreaSchedulerTask::stoptime
		///Stop time of all the tasks
		static std::chrono::time_point<std::chrono::high_resolution_clock>stoptime;
		// DocString: FMTOpAreaSchedulerTask::irelax_objective
		///The relaxed objective value.
		static double relax_objective;
		// DocString: FMTOpAreaSchedulerTask::outyldname
		///Name of the output yield.
		static std::string outyldname;
		// DocString: FMTOpAreaSchedulerTask::basemodel
		///The non build base lpmodel copy (only use for writing the solution).
		static std::unique_ptr<Models::FMTLpModel>basemodel;
		// DocString: FMTOpAreaSchedulerTask::lastspawned
		///Last thread id and seed used by the spawner.
		int lastspawned;
		// DocString: FMTOpAreaSchedulerTask::solveInitialModel
		/**
		@brief Solve the initial model and return the objective value.
		@param[in] model the model.
		@return the objective value.
		*/
		double solveInitialModel(Models::FMTLpModel& model) const;
		// DocString: FMTOpAreaSchedulerTask::setInitialScheduler
		/**
		@brief Set the initial scheduler from the LP model using the operating areas and the output node.
		@param[in,out] model the model.
		@param[in] opareas the operating areas.
		@param[in] node the output node.
		*/
		void setInitialScheduler(Models::FMTLpModel& model,
			const std::vector<Heuristics::FMTOperatingAreaScheme>& opareas,const Core::FMTOutputNode& node);
		// DocString: FMTOpAreaSchedulerTask::getStopPoint
		/**
		@brief Get the time at which the solving should stop.
		@param[in] timegap the time gap.
		@return the stop time point.
		*/
		std::chrono::time_point<std::chrono::high_resolution_clock>getStopPoint(const int& timegap) const;
		// DocString: FMTOpAreaSchedulerTask::getConstraintsSolution
		/**
		@brief Fill the outputs and constraints with the solution of the best scheduler.
		@param[in,out] outputs the outputs.
		@param[in,out] constraints the constraints.
		*/
		void getConstraintsSolution(std::vector<Core::FMTOutput>& outputs,std::vector<Core::FMTConstraint>& constraints) const;
		// DocString: FMTOpAreaSchedulerTask::writeFinalModel
		/**
		@brief Rebuild the base model without any constraint, apply the solution of the best heuristic and write the entire model and the solution.
		*/
		void writeFinalModel() const;
		// DocString: FMTOpAreaSchedulerTask::writeSolution
		/**
		@brief Write the solution using the solution location.
		*/
		void writeSolution() const;
		// DocString: FMTOpAreaSchedulerTask::gotInitialSolution
		/**
		@brief Return true if the best scheduler is not empty and has an initial solution.
		@return true if there is an initial solution else false.
		*/
		bool gotInitialSolution() const;
		// DocString: FMTOpAreaSchedulerTask::evaluateAndCopy
		/**
		@brief Evaluate if the actual scheduler is better than the best scheduler, pushing the actual scheduler to the best one if it is, otherwise taking the solution from the best scheduler.
		*/
		void evaluateAndCopy();
		// DocString: FMTOpAreaSchedulerTask::goodToGo
		/**
		@brief Return true if there are iterations left and time left.
		@return true if the task can continue else false.
		*/
		bool goodToGo() const;
		// DocString: FMTOpAreaSchedulerTask::getReturnTimeFromOutput
		/**
		@brief Return a new vector of operating area schemes with a return time value based on the output computed in the model for each operating area.
		@param[in,out] model the model.
		@param[in] opareas the operating areas.
		@param[in] output the output.
		@return the operating area schemes with the new return time.
		*/
		std::vector<Heuristics::FMTOperatingAreaScheme> getReturnTimeFromOutput(Models::FMTLpModel& model,
			const std::vector<Heuristics::FMTOperatingAreaScheme>& opareas,
			const Core::FMTOutput& output) const;
	public:
		// DocString: FMTOpAreaSchedulerTask::FMTOpAreaSchedulerTask()
		/**
		@brief Default constructor for FMTOpAreaSchedulerTask.
		*/
		FMTOpAreaSchedulerTask() = default;
		// DocString: FMTOpAreaSchedulerTask::FMTOpAreaSchedulerTask(const FMTOpAreaSchedulerTask&)
		/**
		@brief Default copy constructor for FMTOpAreaSchedulerTask.
		@param[in] rhs the FMTOpAreaSchedulerTask to copy.
		*/
		FMTOpAreaSchedulerTask(const FMTOpAreaSchedulerTask& rhs);
		// DocString: FMTOpAreaSchedulerTask::operator=(const FMTOpAreaSchedulerTask&)
		/**
		@brief Default copy assignment operator for FMTOpAreaSchedulerTask.
		@param[in] rhs the FMTOpAreaSchedulerTask to copy.
		@return a reference to this FMTOpAreaSchedulerTask.
		*/
		FMTOpAreaSchedulerTask& operator = (const FMTOpAreaSchedulerTask& rhs);
		// DocString: FMTOpAreaSchedulerTask::~FMTOpAreaSchedulerTask()
		/**
		@brief Destructor for FMTOpAreaSchedulerTask, taking care of writing the solution.
		*/
		virtual ~FMTOpAreaSchedulerTask()=default;
		// DocString: FMTOpAreaSchedulerTask::clone
		/**
		@brief Clone function for FMTOpAreaSchedulerTask.
		@return a unique pointer to the cloned task.
		*/
		std::unique_ptr<FMTTask>clone() const;
		// DocString: FMTOpAreaSchedulerTask::FMTOpAreaSchedulerTask(...)
		/**
		@brief Construct a FMTOpAreaSchedulerTask from a model, calling doPlanning on it and obtaining the operating area schemes from the FMTLpModel.
		@details The length and other parameters must be set before; if the return time output is non empty it is used to set the return time of each operating area scheme using the initial solution.
		@param[in] model the model.
		@param[in] opareas the operating areas.
		@param[in] node the output node.
		@param[in] outputlocation the output location.
		@param[in] outputyieldname the output yield name.
		@param[in] maxiterations the maximum number of iterations.
		@param[in] maxtime the maximum time.
		@param[in] returntime_output the return time output.
		*/
		FMTOpAreaSchedulerTask(const Models::FMTLpModel& model,
			const std::vector<Heuristics::FMTOperatingAreaScheme>& opareas,
			const Core::FMTOutputNode& node,
			const std::string& outputlocation,
			const std::string& outputyieldname,
			const unsigned int& maxiterations,
			const int& maxtime,
			Core::FMTOutput returntime_output = Core::FMTOutput());
		// DocString: FMTOpAreaSchedulerTask::split
		/**
		@brief Split the main task into multiple operating area scheduler tasks.
		@param[in] numberoftasks the number of tasks.
		@return the split tasks.
		*/
		virtual std::vector<std::unique_ptr<FMTTask>>split(const unsigned int& numberoftasks) const;
		// DocString: FMTOpAreaSchedulerTask::spawn
		/**
		@brief Spawn a minimal task from the master task.
		@return a unique pointer to the spawned task.
		*/
		virtual std::unique_ptr<FMTTask>spawn();
		// DocString: FMTOpAreaSchedulerTask::work
		/**
		@brief Main function doing the operating area scheduling task.
		*/
		virtual void work();
		// DocString: FMTOpAreaSchedulerTask::finalize
		/**
		@brief Write the solution at finalization.
		*/
		virtual void finalize();
		// DocString: FMTopareaschedulertask::passInLogger
		/**
		@brief Pass in the logger.
		@param[in] logger the logger.
		*/
		void passInLogger(const std::unique_ptr<Logging::FMTLogger>& logger) override;
		// DocString: FMTopareaschedulertask::SupportsMultiThreading()
		/**
		@brief Check if the task support multithreading
		@return true if it support
		*/
		bool SupportsMultiThreading() const override;

	};

}



#endif
