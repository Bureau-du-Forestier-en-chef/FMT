/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTplanningtask_H_INCLUDED
#define FMTplanningtask_H_INCLUDED

#include "FMTtask.hpp"
#include <list>
#include <memory>
#include "FMTutility.hpp"


namespace Models
{
	class FMTmodel;
}

namespace Core
{
	class FMToutput;
	class FMTschedule;
}



/// Namespace for parallel tasking may include multithreading / multiprocessing
namespace Parallel
{
	class FMTparallelwriter;
	// DocString: FMTplanningtask
	/**
	This task make it easier to carry multiple parallel optimization of different FMTmodel using the doplanning function.
	*/
	class FMTEXPORT FMTplanningtask : public FMTtask
	{
		// DocString: FMTplanningtask::resultswriter
		///Concurrent results writer who writes the results of each abstract model after each call to doplanning
		std::shared_ptr<FMTparallelwriter>resultswriter;
		// DocString: FMTplanningtask::models
		///Abstracts models that need to be build and solve by the task.
		std::list<std::unique_ptr<Models::FMTmodel>>models;
		// DocString: FMTplanningtask::schedules
		///All schedules of FMTmodel if we only want to do playback
		std::list<std::vector<Core::FMTschedule>>allschedules;
		// DocString: FMTplanningtask::keepmodels
		///If keep models is true the models will be kept after work
		bool keepmodels;
		// DocString: FMTplanningtask::copymodels
		/**
		Copy models for unique model...
		*/
		std::list<std::unique_ptr<Models::FMTmodel>>copymodels(const std::list<std::unique_ptr<Models::FMTmodel>>& tocopy) const;
	public:
		// DocString: FMTplanningtask::FMTreplanningtask()
		/**
		Default constructor for FMTplanningtask
		*/
		FMTplanningtask() = default;
		// DocString: FMTplanningtask::FMTplanningtask(const FMTplanningtask&)
		/**
		Default copy constructor for FMTplanningtask
		*/
		FMTplanningtask(const FMTplanningtask& rhs);
		// DocString: FMTplanningtask::operator=(const FMTplanningtask&)
		/**
		Default copy assignement for FMTplanningtask
		*/
		FMTplanningtask& operator = (const FMTplanningtask& rhs);
		// DocString: FMTplanningtask::~FMTplanningtask()
		/**
		We need to define a destructor to take care of writing drift probabilities.
		*/
		virtual ~FMTplanningtask()=default;
		// DocString: FMTplanningtask::clone
		/**
		Clone function for FMTplanningtask
		*/
		std::unique_ptr<FMTtask>clone() const;
		// DocString: FMTplanningtask::setkeepmodels
		/**
		This function will force the task to keep the models... may consume memory.
		*/
		void setkeepmodels();
		// DocString: FMTplanningtask::push_back
		/**
		Push a new FMTmodel in the task queue with optional schedule.
		Dont forget to just keep FMToutput that you want to get values in the FMTmodel.
		*/
		void push_back(const Models::FMTmodel& model,
			std::vector<Core::FMTschedule>schedules = std::vector<Core::FMTschedule>());
		// DocString: FMTplanningtask::FMTplanningtask(...)
		/**
		You need to initialize the task mainly with the output parser parameters.
		*/
		FMTplanningtask(const int& minoutputperiod,
					const int& maxoutputperiod,
					const std::string& outputlocation,
					std::string gdaldriver = "CSV",
					std::vector<std::string>creationoptions= std::vector<std::string>(),
					Core::FMToutputlevel outputlevel = Core::FMToutputlevel::standard,
					std::string primaryfilelocatiron = std::string());
		// DocString: FMTplanningtask::split
		/**
		The split fonction that split the main task into multiple tasks of planning.
		*/
		virtual std::vector<std::unique_ptr<FMTtask>>split(const unsigned int& numberoftasks) const;
		// DocString: FMTplanningtask::spawn
		/**
		Will spawn a minimal task from the master task
		*/
		virtual std::unique_ptr<FMTtask>spawn();
		// DocString: FMTplanningtask::work
		/**
		Main function that do the planning task
		*/
		virtual void work();
		// DocString: FMTplanningtask::finalize
		/**
		Do nothing...
		*/
		virtual void finalize();
		// DocString: FMTplanningtask::passinlogger
		/**
		Pass the logger
		*/
		void passinlogger(const std::shared_ptr<Logging::FMTlogger>& logger) override;
		// DocString: FMTplanningtask::getmodelsfromdynamiccast
		/**
		Get the models pointer casted in a given model type.
		*/
		template<class ptrtype>
		const std::vector<const ptrtype*> getmodelsfromdynamiccast() const
		{
			std::vector<const ptrtype*>castedptr;
			for (const std::unique_ptr<Models::FMTmodel>& model : models)
			{
				castedptr.push_back(dynamic_cast<const ptrtype*>(model.get()));
			}
			return castedptr;
		}

	};

}



#endif
