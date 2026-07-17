/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTreplanningtask_Hm_included
#define FMTreplanningtask_Hm_included

#include "FMTtask.h"
#include "FMTactualdevelopment.h"
#include "FMTschedule.h"
#include <vector>
#include <queue>
#include <memory>
#include <string>
#include "FMTutility.h"


namespace Models
{
	class FMTModel;
}

namespace Core
{
	class FMTOutput;
}



/// Namespace for parallel tasking may include multithreading / multiprocessing
namespace Parallel
{
	class FMTParallelWriter;
	// DocString: FMTReplanningTask
	/**
	The replanning uses global/stochastic/local as abstract model to simulate natural disturbances  
	over a certain amount of replanning periods for multiple replications. Those replications may generate different results
	caused by the pseudo random constraints and seed used in the simulations / optimization of each abstract models.
	*/
	class FMTEXPORT FMTReplanningTask : public FMTTask
	{
		
	public:
		// DocString: FMTReplanningTask::FMTReplanningTask()
		/**
		Default constructor for FMTReplanningTask
		*/
		FMTReplanningTask() = default;
		// DocString: FMTReplanningTask::FMTReplanningTask(const FMTReplanningTask&)
		/**
		Default copy constructor for FMTReplanningTask
		*/
		FMTReplanningTask(const FMTReplanningTask& rhs) = default;
		// DocString: FMTReplanningTask::operator=(const FMTReplanningTask&)
		/**
		Default copy assignement for FMTReplanningTask
		*/
		FMTReplanningTask& operator = (const FMTReplanningTask& rhs) = default;
		// DocString: FMTReplanningTask::~FMTReplanningTask()
		/**
		Default destructor
		*/
		virtual ~FMTReplanningTask() = default;
		// DocString: FMTReplanningTask::clone
		/**
		Clone function for FMTReplanningTask
		*/
		std::unique_ptr<FMTTask>clone() const;
		// DocString: FMTReplanningTask::FMTReplanningTask(...)
		/**
		The models need to have common sections, outputs to write,outputlocation(outfile),
		gdaldriver of the output file, the number of replicates, the number of replanning period and the weight of the global schedule applied 
		to the localschedule.
		*/
		FMTReplanningTask(const Models::FMTModel& globalm,
						const Models::FMTModel& stochasticm,
						const Models::FMTModel& localm,
						const std::vector<Core::FMTOutput>& outputs,
						const std::string& outputlocation,
						const std::string& gdaldriver,
						const std::vector<std::string>& creationoptions,
						const int& replicates,
						const int& replanningperiodssize,
						const double& minimaldrift,
						Core::FMToutputlevel outputlevel,
						const bool writeSchedule = false);
		// DocString: FMTReplanningTask::FMTReplanningTask(const Models::FMTModel&,const Models::FMTModel&,const Models::FMTModel&)
		/**
		Short constructor for replanning task will use the output of globalmodel as outputs.
		0.5 as minimal drift...
		You then need to set the number of replanning periods,globalweight, and the number of replicates.
		*/
		FMTReplanningTask(const Models::FMTModel& globalm,
			const Models::FMTModel& stochasticm,
			const Models::FMTModel& localm,
			const std::string& outputlocation,
			const std::string& gdaldriver,
			const std::vector<std::string>& creationoptions,
			Core::FMToutputlevel outputlevel);
		// DocString: FMTReplanningTask::setReplicates
		/**
		Set the number of replicates to use...
		*/
		void setReplicates(const int& replicatesnumber);
		// DocString: FMTReplanningTask::setReplicates
		/**
		Set the number of replicates to use...
		*/
		void setReplicates(const int& replicatesnumbermin, const int& replicatesnumbermax);
		// DocString: FMTReplanningTask::setReplanningPeriods
		/**
		Set the number oof replanning periods.
		*/
		void setReplanningPeriods(const int& periodsnumber);
		// DocString: FMTReplanningTask::split
		/**
		The split fonction that split the main task into multiple tasks of replanning.
		*/
		virtual std::vector<std::unique_ptr<FMTTask>>split(const unsigned int& numberoftasks) const;
		// DocString: FMTReplanningTask::spwan
		/**
		Will spawn a minimal task from the master task
		*/
		virtual std::unique_ptr<FMTTask>spawn();
		// DocString: FMTReplanningTask::work
		/**
		Main function that do the replanning task
		*/
		virtual void work();
		// DocString: FMTReplanningTask::finalize
		/**
		We need to take care of writing drift probabilities.
		*/
		virtual void finalize();
		// DocString: FMTReplanningTask::passInLogger
		/**
		Pass the logger
		*/
		void passInLogger(const std::unique_ptr<Logging::FMTLogger>& logger) override;
		// DocString: FMTReplanningTask::passInLogger
		/**
		Set variable tu write replanning schedule.
		Write schedule = write a schedule for each replicate.
		*/
		void setWriteSchedule(const bool p_write);
	private:
		// DocString: FMTReplanningTask::resultswriter
		///Concurrent results writer who writes the results of each abstract model after each call to doPlanning
		std::shared_ptr<FMTParallelWriter>resultswriter;
		// DocString: FMTReplanningTask::baseschedule
		///This is the schedule of the solved global model at period 1.
		std::shared_ptr<Core::FMTSchedule>baseschedule;
		// DocString: FMTReplanningTask::global
		///The shared global model between each task
		std::shared_ptr<Models::FMTModel>global;//base area is in here.
		// DocString: FMTReplanningTask::stochastic
		///The stochastic model used in replanning
		std::shared_ptr<Models::FMTModel>stochastic;
		// DocString: FMTReplanningTask::local
		///The local model used by the replanning
		std::shared_ptr<Models::FMTModel>local;
		// DocString: FMTReplanningTask::replicateids
		///The replicates queue that the task needs to do
		std::queue<int>replicateids;
		// DocString: FMTReplanningTask::dynamicarea
		///This is the dynamic area that changes at each replanning step
		std::vector<Core::FMTActualDevelopment>dynamicarea;
		// DocString: FMTReplanningTask::iterationglobalschedule
		///This is the schedule that the global model generates after a call to doPlanning
		Core::FMTSchedule iterationglobalschedule;
		// DocString: FMTReplanningTask::dynamicconstraints
		///Local constraints set using the results obtained in the global or local model.
		std::vector<Core::FMTConstraint>dynamicconstraints;
		// DocString: FMTReplanningTask::replanningperiods
		///The number of replanning periods the task needs to do.
		int replanningperiods;
		// DocString: FMTReplanningTask::appendSchedule
		///A boolean for write Schedule in replanning,true, = create a schedule for each replicate.
		bool m_writeSchedule;
		std::string m_outputlocation;
		// DocString: FMTReplanningTask::copySharedModel
		/**
		Solver logger make it not save to clone a solver with a common logger.
		So we need to lock this function to make sure that there's no racing between threads.
		*/
		std::unique_ptr<Models::FMTModel>copySharedModel(const std::shared_ptr<Models::FMTModel>model);
		// DocString: FMTReplanningTask::doModelPlanning
		/**
		The main do planning function that call doPlanning on model for a given replanning period and will keep solution and constraints
		id getsolutionandlocal = true and will apply the schedule weight if applyscheduleweight = true.
		*/
		std::unique_ptr<Models::FMTModel>doModelPlanning(
			const std::shared_ptr<Models::FMTModel>model,
			const int& replanningperiod,
			bool getsolutionandlocal = false,
			bool applyscheduleweight = false,
			bool setdynamicconstraints = true);
		// DocString: FMTReplanningTask::writeResults
		/**
		Write the results from a model pointer, if onlyfirsperiod = true will only write first period else write the whole thing.
		*/
		void writeResults(const std::string& modelname, const int& modellength,
			const std::unique_ptr<Models::FMTModel>& modelptr, const int& replanningperiod, bool onlyfirstperiod = false);
		// DocString: FMTReplanningTask::_writeSchedule
		/**
		@biref write down the schedule for the stochastic and tactic
		@param[in] p_model the model
		@param[in] p_ReportingPeriod
		@param[in] p_ModelPeriod the models period
		*/
		void _writeSchedule(const std::unique_ptr<Models::FMTModel>& p_model,
							int p_ReportingPeriod, int p_ModelPeriod) const;
		// DocString: FMTReplanningTask::getIteration
		/**
		Get the actual iteration done by the task (front in the queue)
		*/
		const int getIteration() const;
		// DocString: FMTReplanningTask::setReIgnore
		/**
		Remove reignore constraints based on the replanning period.
		*/
		void setReIgnore(std::unique_ptr<Models::FMTModel>& modelcpy, const int& replanningperiod) const;
		// DocString: FMTReplanningTask::setReplicate
		/**
		Ajuste constraints based on the replicate keyword
		*/
		void setReplicate(std::unique_ptr<Models::FMTModel>& modelcpy, const int& replanningperiod) const;
		// DocString: FMTReplanningTask::setReplicate
		/**
		Primary name extracted to the path.
		*/
		std::string m_primaryName;
	};

}



#endif
