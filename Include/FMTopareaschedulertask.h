/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTopareaschedulertask_H_INCLUDED
#define FMTopareaschedulertask_H_INCLUDED

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
	class FMToutputnode;
	class FMTconstraint;
}


namespace Models
{
	class FMTlpmodel;
}

namespace Heuristics
{
	class FMToperatingareascheme;
	class FMToperatingareascheduler;
}

/// Namespace for parallel tasking may include multithreading / multiprocessing
namespace Parallel
{
	// DocString: FMTopareaschedulertask
	/**
	The opareascheduler is a task used to find suboptimal schedule for operating area for a Models::FMTlpmodel.
	It keeps the best scheduler in memory and try to modify is actual schedule to get a better objective value.
	It will generate time yields for the operating area.
	*/
	class FMTEXPORT FMTopareaschedulertask : public FMTtask
	{
		// DocString: FMTopareaschedulertask::generalmutex
		///When you want to get/set the bestscheduler or change the number of iterations.
		static boost::mutex generalmutex;
		// DocString: FMTopareaschedulertask::bestscheduler;
		///Best scheduler so far
		std::shared_ptr<Heuristics::FMToperatingareascheduler>bestscheduler;
		// DocString: FMTopareaschedulertask::actualscheduler
		///The main task scheduler of the task
		std::unique_ptr<Heuristics::FMToperatingareascheduler>actualscheduler;
		// DocString: FMTopareaschedulertask::solutionlocation
		///The solution location of the selected schedule
		static std::string solutionlocation;
		// DocString: FMTopareaschedulertask::iterations
		///The number of iterations left
		static unsigned int iterations;
		// DocString: FMTopareaschedulertask::stoptime
		///Stop time of all the tasks
		static std::chrono::time_point<std::chrono::high_resolution_clock>stoptime;
		// DocString: FMTopareaschedulertask::irelax_objective
		///The relaxed objective value.
		static double relax_objective;
		// DocString: FMTopareaschedulertask::outyldname
		///Name of the output yield.
		static std::string outyldname;
		// DocString: FMTopareaschedulertask::basemodel
		///The non build base lpmodel copy (only use for writing the solution).
		static std::unique_ptr<Models::FMTlpmodel>basemodel;
		// DocString: FMTopareaschedulertask::lastspawned
		///Last thread id and seed used by the spawner.
		int lastspawned;
		// DocString: FMTopareaschedulertask::solveinitialmodel
		/**
		Solve the initial model and return the objective value.
		*/
		double solveinitialmodel(Models::FMTlpmodel& model) const;
		// DocString: FMTopareaschedulertask::setinitialscheduler
		/**
		Set the initialscheduler from the lpmodel using the operating areas and the output node.
		*/
		void setinitialscheduler(Models::FMTlpmodel& model,
			const std::vector<Heuristics::FMToperatingareascheme>& opareas,const Core::FMToutputnode& node);
		// DocString: FMTopareaschedulertask::getstoppoint
		/**
		Get the time at which the solving should stop.
		*/
		std::chrono::time_point<std::chrono::high_resolution_clock>getstoppoint(const int& timegap) const;
		// DocString: FMTopareaschedulertask::getconstraintssolution
		/**
		Get the outputs and constraints solution of the best scheduler and fill constraints and outputs.
		*/
		void getconstraintssolution(std::vector<Core::FMToutput>& outputs,std::vector<Core::FMTconstraint>& constraints) const;
		// DocString: FMTopareaschedulertask::writefinalmodel
		/**
		Using the solution of the best heuristic just rebuild the base model without anyconstraint and apply the solution found.
		Then get the folder of the resulting solution et write the entire model and the solution.
		*/
		void writefinalmodel() const;
		// DocString: FMTopareaschedulertask::writesolution
		/**
		Using the solution location write down the solution.
		*/
		void writesolution() const;
		// DocString: FMTopareaschedulertask::gotinitialsolution
		/**
		Returns true if the bestscheduler is not empty and have an initial solution.
		*/
		bool gotinitialsolution() const;
		// DocString: FMTopareaschedulertask::evaluateandcopy
		/**
		Evaluate if the actualscheduler is better than the bestscheduler. If it is push the actualscheduler to the best scheduler.
		Else take the solution from the bestscheduler.
		*/
		void evaluateandcopy();
		// DocString: FMTopareaschedulertask::goodtogo
		/**
		Return true if iterations > 0 and still time left.
		*/
		bool goodtogo() const;
		// DocString: FMTopareaschedulertask::getreturntimefromoutput
		/**
		Will return a new vector of FMToperatingareascheme with new return_time value based on the output calculed
		in the model for each operaging area.
		*/
		std::vector<Heuristics::FMToperatingareascheme> getreturntimefromoutput(Models::FMTlpmodel& model,
			const std::vector<Heuristics::FMToperatingareascheme>& opareas,
			const Core::FMToutput& output) const;
	public:
		// DocString: FMTopareaschedulertask::FMTopareaschedulertask()
		/**
		Default constructor for FMTopareaschedulertask
		*/
		FMTopareaschedulertask() = default;
		// DocString: FMTopareaschedulertask::FMTopareaschedulertask(const FMTopareaschedulertask&)
		/**
		Default copy constructor for FMTopareaschedulertask
		*/
		FMTopareaschedulertask(const FMTopareaschedulertask& rhs);
		// DocString: FMTopareaschedulertask::operator=(const FMTopareaschedulertask&)
		/**
		Default copy assignement for FMTopareaschedulertask
		*/
		FMTopareaschedulertask& operator = (const FMTopareaschedulertask& rhs);
		// DocString: FMTopareaschedulertask::~FMTopareaschedulertask()
		/**
		We need to define a destructor to take care of writing the  solution.
		*/
		virtual ~FMTopareaschedulertask()=default;
		// DocString: FMTopareaschedulertask::clone
		/**
		Clone function for FMTopareaschedulertask
		*/
		std::unique_ptr<FMTtask>clone() const;
		// DocString: FMTopareaschedulertask::FMTopareaschedulertask(...)
		/**
		Using a model the constructor will call doplanning of the model.
		So you have to set the length and other parameters before. Then the task will obtain the operatingareascheme
		from the FMTlpmodel and setup everything else. If the returntime_output is non empty then it will use it to get the result
		for each FMToperatingareascheme and set the value to the returntime using the initial solution.
		*/
		FMTopareaschedulertask(const Models::FMTlpmodel& model,
			const std::vector<Heuristics::FMToperatingareascheme>& opareas,
			const Core::FMToutputnode& node,
			const std::string& outputlocation,
			const std::string& outputyieldname,
			const unsigned int& maxiterations,
			const int& maxtime,
			Core::FMToutput returntime_output = Core::FMToutput());
		// DocString: FMTopareaschedulertask::split
		/**
		The split fonction that split the main task into multiple tasks of operating area scheduler.
		*/
		virtual std::vector<std::unique_ptr<FMTtask>>split(const unsigned int& numberoftasks) const;
		// DocString: FMTopareaschedulertask::spawn
		/**
		Will spawn a minimal task from the master task
		*/
		virtual std::unique_ptr<FMTtask>spawn();
		// DocString: FMTopareaschedulertask::work
		/**
		Main function that do the operating area scheduling task
		*/
		virtual void work();
		// DocString: FMTopareaschedulertask::finalize
		/**
		Write the solution...
		*/
		virtual void finalize();
		// DocString: FMTopareaschedulertaskk::passinlogger
		/**
		Pass the logger
		*/
		void passinlogger(const std::unique_ptr<Logging::FMTlogger>& logger) override;

	};

}



#endif
