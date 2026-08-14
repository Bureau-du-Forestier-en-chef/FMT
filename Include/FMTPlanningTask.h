/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTplanningtask_Hm_included
#define FMTplanningtask_Hm_included

#include "FMTTask.h"
#include <list>
#include <memory>
#include "FMTutility.h"


namespace Models
{
	class FMTModel;
}

namespace Core
{
	class FMTOutput;
	class FMTSchedule;
}



/// Namespace for parallel tasking may include multithreading / multiprocessing
namespace Parallel
{
	class FMTParallelWriter;
	// DocString: FMTPlanningTask
	/**
	@brief Task to carry multiple parallel optimizations of different FMTModel using the doPlanning function.
	*/
	class FMTEXPORT FMTPlanningTask : public FMTTask
	{
	public:
		// DocString: FMTPlanningTask::FMTPlanningTask()
		/**
		@brief Default constructor for FMTPlanningTask.
		*/
		FMTPlanningTask() = default;
		// DocString: FMTPlanningTask::FMTPlanningTask(const FMTPlanningTask&)
		/**
		@brief Default copy constructor for FMTPlanningTask.
		@param[in] rhs the FMTPlanningTask to copy.
		*/
		FMTPlanningTask(const FMTPlanningTask& rhs);
		// DocString: FMTPlanningTask::operator=(const FMTPlanningTask&)
		/**
		@brief Default copy assignment operator for FMTPlanningTask.
		@param[in] rhs the FMTPlanningTask to copy.
		@return a reference to this FMTPlanningTask.
		*/
		FMTPlanningTask& operator = (const FMTPlanningTask& rhs);
		// DocString: FMTPlanningTask::~FMTPlanningTask()
		/**
		@brief Destructor for FMTPlanningTask, taking care of writing the drift probabilities.
		*/
		virtual ~FMTPlanningTask()=default;
		// DocString: FMTPlanningTask::clone
		/**
		@brief Clone function for FMTPlanningTask.
		@return a unique pointer to the cloned task.
		*/
		std::unique_ptr<FMTTask>clone() const;
		// DocString: FMTPlanningTask::setKeepModels
		/**
		@brief Force the task to keep the models, which may consume memory.
		*/
		void setKeepModels();
		// DocString: FMTPlanningTask::push_back
		/**
		@brief Push a new FMTModel in the task queue with an optional schedule.
		@details Keep only the outputs you want to get values from in the FMTModel.
		@param[in] model the model.
		@param[in] schedules the schedules.
		@param[in] louputs the outputs.
		*/
		void push_back(const Models::FMTModel& model,
			std::vector<Core::FMTSchedule>schedules = std::vector<Core::FMTSchedule>(),
			std::vector<Core::FMTOutput>louputs = std::vector<Core::FMTOutput>());
		// DocString: FMTPlanningTask::FMTPlanningTask(...)
		/**
		@brief Construct a FMTPlanningTask, mainly with the output parser parameters.
		@param[in] minoutputperiod the minimal output period.
		@param[in] maxoutputperiod the maximal output period.
		@param[in] outputlocation the output location.
		@param[in] gdaldriver the GDAL driver.
		@param[in] creationoptions the creation options.
		@param[in] outputlevel the output level.
		@param[in] primaryfilelocation the primary file location.
		*/
		FMTPlanningTask(const int& minoutputperiod,
					const int& maxoutputperiod,
					const std::string& outputlocation,
					std::string gdaldriver = "CSV",
					std::vector<std::string>creationoptions= std::vector<std::string>(),
					Core::FMToutputlevel outputlevel = Core::FMToutputlevel::standard,
					std::string primaryfilelocation = std::string());
		// DocString: FMTPlanningTask::split
		/**
		@brief Split the main task into multiple planning tasks.
		@param[in] numberoftasks the number of tasks.
		@return the split tasks.
		*/
		virtual std::vector<std::unique_ptr<FMTTask>>split(const unsigned int& numberoftasks) const;
		// DocString: FMTPlanningTask::spawn
		/**
		@brief Spawn a minimal task from the master task.
		@return a unique pointer to the spawned task.
		*/
		virtual std::unique_ptr<FMTTask>spawn();
		// DocString: FMTPlanningTask::work
		/**
		@brief Main function doing the planning task.
		*/
		virtual void work();
		// DocString: FMTPlanningTask::finalize
		/**
		@brief Do nothing at finalization.
		*/
		virtual void finalize();
		// DocString: FMTPlanningTask::passInLogger
		/**
		@brief Pass in the logger.
		@param[in] logger the logger.
		*/
		void passInLogger(const std::unique_ptr<Logging::FMTLogger>& logger) override;
		// DocString: FMTPlanningTask::getModelsFromDynamicCast
		/**
		@brief Return the models dynamically cast to the requested model type.
		@tparam ptrtype the model pointer type.
		@return the casted models.
		*/
		template<class ptrtype>
		const std::vector<const ptrtype*> getModelsFromDynamicCast() const
		{
			std::vector<const ptrtype*>castedptr;
			for (const std::unique_ptr<Models::FMTModel>& model : m_Models)
			{
				castedptr.push_back(dynamic_cast<const ptrtype*>(model.get()));
			}
			return castedptr;
		}
		// DocString: FMTPlanningTask::SupportsMultiThreading()
		/**
		@brief Check if the task support multithreading
		@return true if it support
		*/
		bool SupportsMultiThreading() const override;
	private:
		// DocString: FMTPlanningTask::m_ResultsWriter
		///Concurrent results writer who writes the results of each abstract model after each call to doPlanning
		std::shared_ptr<FMTParallelWriter>m_ResultsWriter;
		// DocString: FMTPlanningTask::m_Models
		///Abstracts models that need to be build and solve by the task.
		std::list<std::unique_ptr<Models::FMTModel>>m_Models;
		// DocString: FMTPlanningTask::m_allSchedules
		///All schedules of FMTModel if we only want to do playback
		std::list<std::vector<Core::FMTSchedule>>m_allSchedules;
		// DocString: FMTPlanningTask::m_Outputs
		///Abstracts outputs that the results need to be write by the task.
		std::list<std::vector<Core::FMTOutput>>m_Outputs;
		// DocString: FMTPlanningTask::m_keepModels
		///If keep models is true the models will be kept after work
		bool m_keepModels;
		// DocString: FMTPlanningTask::copyModels
		/**
		@brief Return a copy of a list of unique models.
		@param[in] tocopy the models to copy.
		@return the copied models.
		*/
		std::list<std::unique_ptr<Models::FMTModel>>copyModels(const std::list<std::unique_ptr<Models::FMTModel>>& tocopy) const;

	};

}



#endif
