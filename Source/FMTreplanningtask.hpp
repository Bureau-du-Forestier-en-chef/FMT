/*
Copyright (c) 2019 Gouvernement du Qu�bec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTreplanningtask_H_INCLUDED
#define FMTreplanningtask_H_INCLUDED

#include "FMTtask.hpp"
#include "FMTactualdevelopment.hpp"
#include "FMTschedule.hpp"
#include <vector>
#include <queue>
#include <memory>
#include <string>


namespace Models
{
	class FMTmodel;
}

namespace Core
{
	class FMToutput;
}



/// Namespace for parallel tasking may include multithreading / multiprocessing
namespace Parallel
{
	class FMTparallelwriter;
	// DocString: FMTreplanningtask
	/**
	The replanning uses global/stochastic/local as abstract model to simulate natural disturbances  
	over a certain amount of replanning periods for multiple replications. Those replications may generate different results
	caused by the pseudo random constraints and seed used in the simulations / optimization of each abstract models.
	*/
	class FMTEXPORT FMTreplanningtask : public FMTtask
	{
		// DocString: FMTreplanningtask::resultswriter
		///Concurrent results writer who writes the results of each abstract model after each call to doplanning
		std::shared_ptr<FMTparallelwriter>resultswriter;
		// DocString: FMTreplanningtask::baseschedule
		///This is the schedule of the solved global model at period 1.
		std::shared_ptr<Core::FMTschedule>baseschedule;
		// DocString: FMTreplanningtask::global
		///The shared global model between each task
		std::shared_ptr<Models::FMTmodel>global;//base area is in here.
		// DocString: FMTreplanningtask::stochastic
		///The stochastic model used in replanning
		std::shared_ptr<Models::FMTmodel>stochastic;
		// DocString: FMTreplanningtask::local
		///The local model used by the replanning
		std::shared_ptr<Models::FMTmodel>local;
		// DocString: FMTreplanningtask::replicateids
		///The replicates queue that the task needs to do
		std::queue<int>replicateids;
		// DocString: FMTreplanningtask::dynamicarea
		///This is the dynamic area that changes at each replanning step
		std::vector<Core::FMTactualdevelopment>dynamicarea;
		// DocString: FMTreplanningtask::iterationglobalschedule
		///This is the schedule that the global model generates after a call to doplanning
		Core::FMTschedule iterationglobalschedule;
		// DocString: FMTreplanningtask::localconstraints
		///Local constraints set using the results obtained in the global model.
		std::vector<Core::FMTconstraint>localconstraints;
		// DocString: FMTreplanningtask::replanningperiods
		///The number of replanning periods the task needs to do.
		int replanningperiods;
		// DocString: FMTreplanningtask::globalsolutionweight;
		///The weight of the schedule of the global model applied to the local model.
		double globalsolutionweight;
		// DocString: FMTreplanningtask::domodelplanning
		/**
		The main do planning function that call doplanning on model for a given replanning period and will keep solution and constraints
		id getsolutionandlocal = true and will apply the schedule weight if applyscheduleweight = true.
		*/
		std::unique_ptr<Models::FMTmodel>domodelplanning(
			const std::shared_ptr<Models::FMTmodel>model,
			const int& replanningperiod,
			bool getsolutionandlocal = false,
			bool applyscheduleweight = false);
		// DocString: FMTreplanningtask::writeresults
		/**
		Write the results from a model pointer, if onlyfirsperiod = true will only write first period else write the whole thing.
		*/
		void writeresults(const std::unique_ptr<Models::FMTmodel>& modelptr,bool onlyfirstperiod = false);
		// DocString: FMTreplanningtask::getiteration
		/**
		Get the actual iteration done by the task (front in the queue)
		*/
		const int getiteration() const;
	public:
		// DocString: FMTreplanningtask::FMTreplanningtask()
		/**
		Default constructor for FMTreplanningtask
		*/
		FMTreplanningtask() = default;
		// DocString: FMTreplanningtask::FMTreplanningtask(const FMTreplanningtask&)
		/**
		Default copy constructor for FMTreplanningtask
		*/
		FMTreplanningtask(const FMTreplanningtask& rhs) = default;
		// DocString: FMTreplanningtask::operator=(const FMTreplanningtask&)
		/**
		Default copy assignement for FMTreplanningtask
		*/
		FMTreplanningtask& operator = (const FMTreplanningtask& rhs) = default;
		// DocString: FMTreplanningtask::clone
		/**
		Clone function for FMTreplanningtask
		*/
		std::unique_ptr<FMTtask>clone() const;
		// DocString: FMTreplanningtask::FMTreplanningtask(...)
		/**
		This is the one and only constructor for that replanningtask, need to have common sections, outputs to write,outputlocation(outfile),
		gdaldriver of the output file, the number of replicates, the number of replanning period and the weight of the global schedule applied 
		to the localschedule.
		*/
		FMTreplanningtask(const Models::FMTmodel& globalm,
						const Models::FMTmodel& stochasticm,
						const Models::FMTmodel& localm,
						const std::vector<Core::FMToutput>& outputs,
						const std::string& outputlocation,
						const std::string& gdaldriver,
						const int& replicates,
						const int& replanningperiodssize,
						const double& globalwweight);
		// DocString: FMTreplanningtask::split(...)
		/**
		The split fonction that split the main task into multiple tasks of replanning.
		*/
		virtual std::vector<std::unique_ptr<FMTtask>>split(const unsigned int& numberoftasks) const;
		// DocString: FMTreplanningtask::split()
		/**
		Will spawn a minimal task from the master task
		*/
		virtual std::unique_ptr<FMTtask>spawn();
		// DocString: FMTreplanningtask::work
		/**
		Main function that do the replanning task
		*/
		virtual void work();

	};

}



#endif