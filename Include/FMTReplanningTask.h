/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTreplanningtask_Hm_included
#define FMTreplanningtask_Hm_included

#include "FMTTask.h"
#include "FMTActualDevelopment.h"
#include "FMTSchedule.h"
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
	@brief Replanning task using global, stochastic and local abstract models to simulate natural disturbances over several replanning periods for multiple replications.
	@details The replications may generate different results because of the pseudo random constraints and seed used in the simulations and optimizations of each abstract model.
	*/
	class FMTEXPORT FMTReplanningTask : public FMTTask
	{
		
	public:
		// DocString: FMTReplanningTask::FMTReplanningTask()
		/**
		@brief Default constructor for FMTReplanningTask.
		*/
		FMTReplanningTask() = default;
		// DocString: FMTReplanningTask::FMTReplanningTask(const FMTReplanningTask&)
		/**
		@brief Default copy constructor for FMTReplanningTask.
		@param[in] rhs the FMTReplanningTask to copy.
		*/
		FMTReplanningTask(const FMTReplanningTask& rhs) = default;
		// DocString: FMTReplanningTask::operator=(const FMTReplanningTask&)
		/**
		@brief Default copy assignment operator for FMTReplanningTask.
		@param[in] rhs the FMTReplanningTask to copy.
		@return a reference to this FMTReplanningTask.
		*/
		FMTReplanningTask& operator = (const FMTReplanningTask& rhs) = default;
		// DocString: FMTReplanningTask::~FMTReplanningTask()
		/**
		@brief Default destructor for FMTReplanningTask.
		*/
		virtual ~FMTReplanningTask() = default;
		// DocString: FMTReplanningTask::clone
		/**
		@brief Clone function for FMTReplanningTask.
		@return a unique pointer to the cloned task.
		*/
		std::unique_ptr<FMTTask>clone() const;
		// DocString: FMTReplanningTask::FMTReplanningTask(...)
		/**
		@brief Construct a FMTReplanningTask from the models and the replanning parameters.
		@details The models need to have common sections.
		@param[in] globalm the global model.
		@param[in] stochasticm the stochastic model.
		@param[in] localm the local model.
		@param[in] outputs the outputs to write.
		@param[in] outputlocation the output location.
		@param[in] gdaldriver the GDAL driver of the output file.
		@param[in] creationoptions the creation options.
		@param[in] replicates the number of replicates.
		@param[in] replanningperiodssize the number of replanning periods.
		@param[in] minimaldrift the minimal drift.
		@param[in] outputlevel the output level.
		@param[in] writeSchedule if true writes a schedule for each replicate.
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
		@brief Short constructor for the replanning task using the outputs of the global model and 0.5 as minimal drift.
		@details You then need to set the number of replanning periods, the global weight and the number of replicates.
		@param[in] globalm the global model.
		@param[in] stochasticm the stochastic model.
		@param[in] localm the local model.
		@param[in] outputlocation the output location.
		@param[in] gdaldriver the GDAL driver.
		@param[in] creationoptions the creation options.
		@param[in] outputlevel the output level.
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
		@brief Set the number of replicates to use.
		@param[in] replicatesnumber the number of replicates.
		*/
		void setReplicates(const int& replicatesnumber);
		// DocString: FMTReplanningTask::setReplicates
		/**
		@brief Set the range of replicates to use.
		@param[in] replicatesnumbermin the minimal replicate number.
		@param[in] replicatesnumbermax the maximal replicate number.
		*/
		void setReplicates(const int& replicatesnumbermin, const int& replicatesnumbermax);
		// DocString: FMTReplanningTask::setReplanningPeriods
		/**
		@brief Set the number of replanning periods.
		@param[in] periodsnumber the number of periods.
		*/
		void setReplanningPeriods(const int& periodsnumber);
		// DocString: FMTReplanningTask::split
		/**
		@brief Split the main task into multiple replanning tasks.
		@param[in] numberoftasks the number of tasks.
		@return the split tasks.
		*/
		virtual std::vector<std::unique_ptr<FMTTask>>split(const unsigned int& numberoftasks) const;
		// DocString: FMTReplanningTask::spwan
		/**
		@brief Spawn a minimal task from the master task.
		@return a unique pointer to the spawned task.
		*/
		virtual std::unique_ptr<FMTTask>spawn();
		// DocString: FMTReplanningTask::work
		/**
		@brief Main function doing the replanning task.
		*/
		virtual void work();
		// DocString: FMTReplanningTask::finalize
		/**
		@brief Take care of writing the drift probabilities at finalization.
		*/
		virtual void finalize();
		// DocString: FMTReplanningTask::passInLogger
		/**
		@brief Pass in the logger.
		@param[in] logger the logger.
		*/
		void passInLogger(const std::unique_ptr<Logging::FMTLogger>& logger) override;
		// DocString: FMTReplanningTask::setWriteSchedule
		/**
		@brief Set whether a replanning schedule is written for each replicate.
		@param[in] p_write if true writes a schedule for each replicate.
		*/
		void setWriteSchedule(const bool p_write);
	private:
		// DocString: FMTReplanningTask::m_resultswriter
		///Concurrent results writer who writes the results of each abstract model after each call to doPlanning
		std::shared_ptr<FMTParallelWriter>m_resultswriter;
		// DocString: FMTReplanningTask::m_baseschedule
		///This is the schedule of the solved global model at period 1.
		std::shared_ptr<Core::FMTSchedule>m_baseschedule;
		// DocString: FMTReplanningTask::global
		///The shared global model between each task
		std::shared_ptr<Models::FMTModel>global;//base area is in here.
		// DocString: FMTReplanningTask::m_stochastic
		///The stochastic model used in replanning
		std::shared_ptr<Models::FMTModel>m_stochastic;
		// DocString: FMTReplanningTask::m_local
		///The local model used by the replanning
		std::shared_ptr<Models::FMTModel>m_local;
		// DocString: FMTReplanningTask::m_replicateids
		///The replicates queue that the task needs to do
		std::queue<int>m_replicateids;
		// DocString: FMTReplanningTask::m_dynamicarea
		///This is the dynamic area that changes at each replanning step
		std::vector<Core::FMTActualDevelopment>m_dynamicarea;
		// DocString: FMTReplanningTask::m_iterationglobalschedule
		///This is the schedule that the global model generates after a call to doPlanning
		Core::FMTSchedule m_iterationglobalschedule;
		// DocString: FMTReplanningTask::m_dynamicconstraints
		///Local constraints set using the results obtained in the global or local model.
		std::vector<Core::FMTConstraint>m_dynamicconstraints;
		// DocString: FMTReplanningTask::m_replanningperiods
		///The number of replanning periods the task needs to do.
		int m_replanningperiods;
		// DocString: FMTReplanningTask::appendSchedule
		///A boolean for write Schedule in replanning,true, = create a schedule for each replicate.
		bool m_writeSchedule;
		std::string m_outputlocation;
		// DocString: FMTReplanningTask::_copySharedModel
		/**
		@brief Return a copy of a shared model, locking the function to avoid racing between threads because of the common solver logger.
		@param[in] model the model to copy.
		@return the copied model.
		*/
		std::unique_ptr<Models::FMTModel>_copySharedModel(const std::shared_ptr<Models::FMTModel>model);
		// DocString: FMTReplanningTask::_doModelPlanning
		/**
		@brief Call doPlanning on a model for a given replanning period, keeping the solution and constraints and applying the schedule weight when requested.
		@param[in] model the model.
		@param[in] replanningperiod the replanning period.
		@param[in] getsolutionandlocal if true keeps the solution and local constraints.
		@param[in] applyscheduleweight if true applies the schedule weight.
		@param[in] setdynamicconstraints if true sets the dynamic constraints.
		@return the planned model.
		*/
		std::unique_ptr<Models::FMTModel>_doModelPlanning(
			const std::shared_ptr<Models::FMTModel>model,
			const int& replanningperiod,
			bool getsolutionandlocal = false,
			bool applyscheduleweight = false,
			bool setdynamicconstraints = true);
		// DocString: FMTReplanningTask::_writeResults
		/**
		@brief Write the results from a model pointer.
		@param[in] modelname the model name.
		@param[in] modellength the model length.
		@param[in] modelptr the model.
		@param[in] replanningperiod the replanning period.
		@param[in] onlyfirstperiod if true writes only the first period, otherwise the whole horizon.
		*/
		void _writeResults(const std::string& modelname, const int& modellength,
			const std::unique_ptr<Models::FMTModel>& modelptr, const int& replanningperiod, bool onlyfirstperiod = false);
		// DocString: FMTReplanningTask::_writeSchedule
		/**
		@brief Write down the schedule for the stochastic and tactic models.
		@param[in] p_model the model.
		@param[in] p_ReportingPeriod the reporting period.
		@param[in] p_ModelPeriod the model period.
		*/
		void _writeSchedule(const std::unique_ptr<Models::FMTModel>& p_model,
							int p_ReportingPeriod, int p_ModelPeriod) const;
		// DocString: FMTReplanningTask::_getIteration
		/**
		@brief Get the actual iteration done by the task (front of the queue).
		@return the iteration.
		*/
		const int _getIteration() const;
		// DocString: FMTReplanningTask::_setReIgnore
		/**
		@brief Remove the reignore constraints based on the replanning period.
		@param[in,out] modelcpy the model.
		@param[in] replanningperiod the replanning period.
		*/
		void _setReIgnore(std::unique_ptr<Models::FMTModel>& modelcpy, const int& replanningperiod) const;
		// DocString: FMTReplanningTask::_setReplicate
		/**
		@brief Adjust the constraints based on the replicate keyword.
		@param[in,out] modelcpy the model.
		@param[in] replanningperiod the replanning period.
		*/
		void _setReplicate(std::unique_ptr<Models::FMTModel>& modelcpy, const int& replanningperiod) const;
		// DocString: FMTReplanningTask::m_primaryName
		/**
		Primary name extracted to the path.
		*/
		std::string m_primaryName;
	};

}



#endif
