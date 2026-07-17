/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTopareaschedulertask_Hm_included
#define FMTopareaschedulertask_Hm_included

#include "FMTutility.h"
#include "FMTtask.h"
#include <vector>
#include <memory>
#include <string>
#include <chrono>
#include <boost/thread.hpp>
#include "FMToutput.h"

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
	The opareascheduler is a task used to find suboptimal schedule for operating area for a Models::FMTLpModel.
	It keeps the best scheduler in memory and try to modify is actual schedule to get a better objective value.
	It will generate time yields for the operating area.
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
		Solve the initial model and return the objective value.
		*/
		double solveInitialModel(Models::FMTLpModel& model) const;
		// DocString: FMTOpAreaSchedulerTask::setInitialScheduler
		/**
		Set the initialscheduler from the lpmodel using the operating areas and the output node.
		*/
		void setInitialScheduler(Models::FMTLpModel& model,
			const std::vector<Heuristics::FMTOperatingAreaScheme>& opareas,const Core::FMTOutputNode& node);
		// DocString: FMTOpAreaSchedulerTask::getStopPoint
		/**
		Get the time at which the solving should stop.
		*/
		std::chrono::time_point<std::chrono::high_resolution_clock>getStopPoint(const int& timegap) const;
		// DocString: FMTOpAreaSchedulerTask::getConstraintsSolution
		/**
		Get the outputs and constraints solution of the best scheduler and fill constraints and outputs.
		*/
		void getConstraintsSolution(std::vector<Core::FMTOutput>& outputs,std::vector<Core::FMTConstraint>& constraints) const;
		// DocString: FMTOpAreaSchedulerTask::writeFinalModel
		/**
		Using the solution of the best heuristic just rebuild the base model without anyconstraint and apply the solution found.
		Then get the folder of the resulting solution et write the entire model and the solution.
		*/
		void writeFinalModel() const;
		// DocString: FMTOpAreaSchedulerTask::writeSolution
		/**
		Using the solution location write down the solution.
		*/
		void writeSolution() const;
		// DocString: FMTOpAreaSchedulerTask::gotInitialSolution
		/**
		Returns true if the bestscheduler is not empty and have an initial solution.
		*/
		bool gotInitialSolution() const;
		// DocString: FMTOpAreaSchedulerTask::evaluateAndCopy
		/**
		evaluate if the actualscheduler is better than the bestscheduler. If it is push the actualscheduler to the best scheduler.
		Else take the solution from the bestscheduler.
		*/
		void evaluateAndCopy();
		// DocString: FMTOpAreaSchedulerTask::goodToGo
		/**
		Return true if iterations > 0 and still time left.
		*/
		bool goodToGo() const;
		// DocString: FMTOpAreaSchedulerTask::getReturnTimeFromOutput
		/**
		Will return a new vector of FMTOperatingAreaScheme with new return_time value based on the output calculed
		in the model for each operaging area.
		*/
		std::vector<Heuristics::FMTOperatingAreaScheme> getReturnTimeFromOutput(Models::FMTLpModel& model,
			const std::vector<Heuristics::FMTOperatingAreaScheme>& opareas,
			const Core::FMTOutput& output) const;
	public:
		// DocString: FMTOpAreaSchedulerTask::FMTOpAreaSchedulerTask()
		/**
		Default constructor for FMTOpAreaSchedulerTask
		*/
		FMTOpAreaSchedulerTask() = default;
		// DocString: FMTOpAreaSchedulerTask::FMTOpAreaSchedulerTask(const FMTOpAreaSchedulerTask&)
		/**
		Default copy constructor for FMTOpAreaSchedulerTask
		*/
		FMTOpAreaSchedulerTask(const FMTOpAreaSchedulerTask& rhs);
		// DocString: FMTOpAreaSchedulerTask::operator=(const FMTOpAreaSchedulerTask&)
		/**
		Default copy assignement for FMTOpAreaSchedulerTask
		*/
		FMTOpAreaSchedulerTask& operator = (const FMTOpAreaSchedulerTask& rhs);
		// DocString: FMTOpAreaSchedulerTask::~FMTOpAreaSchedulerTask()
		/**
		We need to define a destructor to take care of writing the  solution.
		*/
		virtual ~FMTOpAreaSchedulerTask()=default;
		// DocString: FMTOpAreaSchedulerTask::clone
		/**
		Clone function for FMTOpAreaSchedulerTask
		*/
		std::unique_ptr<FMTTask>clone() const;
		// DocString: FMTOpAreaSchedulerTask::FMTOpAreaSchedulerTask(...)
		/**
		Using a model the constructor will call doPlanning of the model.
		So you have to set the length and other parameters before. Then the task will obtain the operatingareascheme
		from the FMTLpModel and setup everything else. If the returntime_output is non empty then it will use it to get the result
		for each FMTOperatingAreaScheme and set the value to the returntime using the initial solution.
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
		The split fonction that split the main task into multiple tasks of operating area scheduler.
		*/
		virtual std::vector<std::unique_ptr<FMTTask>>split(const unsigned int& numberoftasks) const;
		// DocString: FMTOpAreaSchedulerTask::spawn
		/**
		Will spawn a minimal task from the master task
		*/
		virtual std::unique_ptr<FMTTask>spawn();
		// DocString: FMTOpAreaSchedulerTask::work
		/**
		Main function that do the operating area scheduling task
		*/
		virtual void work();
		// DocString: FMTOpAreaSchedulerTask::finalize
		/**
		Write the solution...
		*/
		virtual void finalize();
		// DocString: FMTopareaschedulertaskk::passInLogger
		/**
		Pass the logger
		*/
		void passInLogger(const std::unique_ptr<Logging::FMTLogger>& logger) override;

	};

}



#endif
