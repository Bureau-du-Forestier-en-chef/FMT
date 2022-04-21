/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTopareaschedulertask_H_INCLUDED
#define FMTopareaschedulertask_H_INCLUDED

#include "FMTutility.hpp"
#include "FMTtask.hpp"
#include <vector>
#include <memory>
#include <string>
#include <chrono>
#include <boost/thread.hpp>

namespace Core
{
	class FMToutputnode;
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
		std::chrono::time_point<std::chrono::high_resolution_clock>getstoppoint(const double& timegap) const;
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
		virtual ~FMTopareaschedulertask();
		// DocString: FMTopareaschedulertask::clone
		/**
		Clone function for FMTopareaschedulertask
		*/
		std::unique_ptr<FMTtask>clone() const;
		// DocString: FMTopareaschedulertask::FMTopareaschedulertask(...)
		/**
		Using a model the constructor will call doplanning of the model.
		So you have to set the length and other parameters before. Then the task will obtain the operatingareascheme
		from the FMTlpmodel and setup everything else.
		*/
		FMTopareaschedulertask(const Models::FMTlpmodel& model,
			const std::vector<Heuristics::FMToperatingareascheme>& opareas,
			const Core::FMToutputnode& node,
			const std::string& outputlocation,
			const std::string& outputyieldname,
			const unsigned int& maxiterations,
			const double& maxtime);
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
		// DocString: FMTopareaschedulertaskk::passinlogger
		/**
		Pass the logger
		*/
		void passinlogger(const std::shared_ptr<Logging::FMTlogger>& logger) override;

	};

}



#endif
