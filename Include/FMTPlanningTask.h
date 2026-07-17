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
	This task make it easier to carry multiple parallel optimization of different FMTModel using the doPlanning function.
	*/
	class FMTEXPORT FMTPlanningTask : public FMTTask
	{
	public:
		// DocString: FMTPlanningTask::FMTReplanningTask()
		/**
		Default constructor for FMTPlanningTask
		*/
		FMTPlanningTask() = default;
		// DocString: FMTPlanningTask::FMTPlanningTask(const FMTPlanningTask&)
		/**
		Default copy constructor for FMTPlanningTask
		*/
		FMTPlanningTask(const FMTPlanningTask& rhs);
		// DocString: FMTPlanningTask::operator=(const FMTPlanningTask&)
		/**
		Default copy assignement for FMTPlanningTask
		*/
		FMTPlanningTask& operator = (const FMTPlanningTask& rhs);
		// DocString: FMTPlanningTask::~FMTPlanningTask()
		/**
		We need to define a destructor to take care of writing drift probabilities.
		*/
		virtual ~FMTPlanningTask()=default;
		// DocString: FMTPlanningTask::clone
		/**
		Clone function for FMTPlanningTask
		*/
		std::unique_ptr<FMTTask>clone() const;
		// DocString: FMTPlanningTask::setKeepModels
		/**
		This function will force the task to keep the models... may consume memory.
		*/
		void setKeepModels();
		// DocString: FMTPlanningTask::push_back
		/**
		Push a new FMTModel in the task queue with optional schedule.
		Dont forget to just keep FMTOutput that you want to get values in the FMTModel.
		*/
		void push_back(const Models::FMTModel& model,
			std::vector<Core::FMTSchedule>schedules = std::vector<Core::FMTSchedule>(),
			std::vector<Core::FMTOutput>louputs = std::vector<Core::FMTOutput>());
		// DocString: FMTPlanningTask::FMTPlanningTask(...)
		/**
		You need to initialize the task mainly with the output parser parameters.
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
		The split fonction that split the main task into multiple tasks of planning.
		*/
		virtual std::vector<std::unique_ptr<FMTTask>>split(const unsigned int& numberoftasks) const;
		// DocString: FMTPlanningTask::spawn
		/**
		Will spawn a minimal task from the master task
		*/
		virtual std::unique_ptr<FMTTask>spawn();
		// DocString: FMTPlanningTask::work
		/**
		Main function that do the planning task
		*/
		virtual void work();
		// DocString: FMTPlanningTask::finalize
		/**
		Do nothing...
		*/
		virtual void finalize();
		// DocString: FMTPlanningTask::passInLogger
		/**
		Pass the logger
		*/
		void passInLogger(const std::unique_ptr<Logging::FMTLogger>& logger) override;
		// DocString: FMTPlanningTask::getModelsFromDynamicCast
		/**
		Get the models pointer casted in a given model type.
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
		Copy models for unique model...
		*/
		std::list<std::unique_ptr<Models::FMTModel>>copyModels(const std::list<std::unique_ptr<Models::FMTModel>>& tocopy) const;

	};

}



#endif
