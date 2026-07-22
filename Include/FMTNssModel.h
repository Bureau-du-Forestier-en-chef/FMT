/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTNSSMODEL_H
#define FMTNSSMODEL_H

#ifdef FMTWITHOSI

#include "FMTSrModel.h"
#include <sstream>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/export.hpp>
#include <random>


namespace Models
{
	// DocString: FMTNssModel
	/**
	@brief Non spatial simulation model, mainly used to simulate stochastic actions during the local replanning phase.
	*/
	class FMTEXPORT FMTNssModel : public FMTSrModel
	{
		public:
			// DocString: FMTNssModel()
			/**
			@brief Default constructor for FMTNssModel.
			*/
			FMTNssModel();
			// DocString: ~FMTNssModel()
			/**
			@brief Default destructor for FMTNssModel.
			*/
			~FMTNssModel()=default;
			// DocString: FMTNssModel(const FMTNssModel&)
			/**
			@brief Copy constructor for FMTNssModel.
			@param[in] rhs the FMTNssModel to copy.
			*/
			FMTNssModel(const FMTNssModel& rhs);
			// DocString: FMTNssModel::operator=
			/**
			@brief Copy assignment operator for FMTNssModel.
			@param[in] rhs the FMTNssModel to copy.
			@return a reference to this FMTNssModel.
			*/
			FMTNssModel& operator=(const FMTNssModel& rhs) = default;
			// DocString: FMTNssModel(const FMTModel&,unsigned int)
			/**
			@brief Construct a FMTNssModel from a model and a seed for the random number generator.
			@param[in] rhs the model.
			@param[in] seed the seed.
			*/
			FMTNssModel(const FMTModel& rhs, unsigned int seed);
			
			// DocString: FMTNssModel(FMTNssModel&&)
			/**
			@brief Default move constructor for FMTNssModel.
			@param[in,out] rhs the model to move from.
			*/
			FMTNssModel(FMTNssModel&& rhs)=default;
			// DocString: FMTNssModel::operator=(FMTNssModel&& rhs) 
			/**
			@brief Default move assignment for FMTNssModel.
			@param[in,out] rhs the model to move from.
			@return a reference to this FMTNssModel.
			*/
			FMTNssModel& operator =(FMTNssModel&& rhs) =default;
			// DocString: FMTNssModel::setParameter(const FMTintmodelparameters,const int&)
			/**
			@brief set int parameters to nss model.
			@param[in] p_key the int key to change.
			@param[in] p_value to set to the p_key.
			@return true if the parameter is set.
			*/
			virtual bool setParameter(const FMTintmodelparameters& p_key, const int& p_value);
			// DocString: FMTNssModel::simulate
			/**
			@brief Do a non spatial simulation based on the area constraints in the optimize section.
			*/
			void simulate();
			// DocString: FMTNssModel::presolve
			/**
			@brief Return a presolved copy of the model.
			@param[in] optionaldevelopments the optional developments.
			@return the presolved model.
			*/
			virtual std::unique_ptr<FMTModel>presolve(std::vector<Core::FMTActualDevelopment> optionaldevelopments = std::vector<Core::FMTActualDevelopment>()) const;
			// DocString: FMTNssModel::clone
			/**
			@brief Get a clone of the FMTNssModel.
			@return a unique pointer to the cloned model.
			*/
			virtual std::unique_ptr<FMTModel>clone() const;
			// DocString: FMTModel::build
			/**
			@brief Build the model by simulating over the number of periods set as LENGTH.
			@param[in] schedules the schedules.
			@return true if the build succeeded else false.
			*/
			virtual bool build(std::vector<Core::FMTSchedule> schedules=std::vector<Core::FMTSchedule>());
			// DocString: FMTModel::solve
			/**
			@brief Return true; there is no solve since this is a simulation.
			@return true.
			*/
			virtual bool solve()
			{
				return true;
			}
			// DocString: FMTNssModel::setParameter(const FMTboolmodelparameters, const bool)
			/**
			@brief Override setter for bool model parameters.
			@param[in] key the parameter key.
			@param[in] value the value to set.
			@return true if the parameter is set else false.
			*/
			bool setParameter(const FMTboolmodelparameters& key, const bool& value) override;
			// DocString: FMTNssModel::getCopy
			/**
			@brief Return a copy of the model for the selected period.
			@param[in] period the period.
			@return the copied model.
			*/
			virtual std::unique_ptr<FMTModel> getCopy(int period = 0) const;
		private:
			// DocString: FMTNssModel::save
			/**
			@brief Save function used for serialization to do multiprocessing across multiple cpus (pickle in Python).
			@tparam Archive the archive type.
			@param[in,out] ar the archive to save to.
			@param[in] version the serialization version.
			*/
			friend class boost::serialization::access;
			template<class Archive>
			void save(Archive& ar, const unsigned int version) const
			{
				ar& boost::serialization::make_nvp("model", boost::serialization::base_object<FMTSrModel>(*this));

			}
			// DocString: FMTNssModel::load
			/**
			@brief Load function used for serialization to do multiprocessing across multiple cpus (pickle in Python).
			@tparam Archive the archive type.
			@param[in,out] ar the archive to load from.
			@param[in] version the serialization version.
			*/
			template<class Archive>
			void load(Archive& ar, const unsigned int version)
			{
				ar& boost::serialization::make_nvp("model", boost::serialization::base_object<FMTSrModel>(*this));

			}
			BOOST_SERIALIZATION_SPLIT_MEMBER()
			// DocString: FMTNssModel::constraintsToTarget
			/**
			@brief Using the constraints generate random level or determinist level of output values in targets
			@param[out] p_targets the targeted values 1 per outputs.
			@param[in] p_period the targeted period
			@return a vector of output pointers.
			*/
			std::vector<const Core::FMTOutput*> constraintsToTarget(std::vector<double>& p_targets, const int& p_period);
			// DocString: FMTNssModel::gotOutputForDev
			/**
			@brief Return true if a output is found for the development
			@param[in] p_development the development from which we want to find an output.
			@param[in] p_outputs the vector of outputs we look in for the action.
			@param[in] p_outputIds the output ids that we can test 
			@return true if found else false.
			*/
			bool gotOutputForDev(const Core::FMTDevelopment& p_development,
								const std::vector<const Core::FMTOutput*>& p_outputs,
								const std::set<size_t>& p_outputIds) const;
			// DocString: FMTNssModel::getFirstOperable
			/**
			@brief Get the first operable action for a development.
			@param[in] development the development to test operability.
			@param[in] targets the targeted actions.
			@param[in] alloutputs the outputs values.
			@return a pair with the output index and a pointer to the action, nullptr if no operable action.
			*/
			std::pair<size_t, const Core::FMTAction*> getFirstOperable(const Core::FMTDevelopment& development,
				std::vector<std::vector<const Core::FMTAction*>> targets,
				const std::vector<const Core::FMTOutput*>& alloutputs) const;
			// DocString: FMTNssModel::getActionsTargets
			/**
			@brief Build a vector containing the output ids related to each action.
			@param[in] p_allOutputs the outputs.
			@return a vector of output id sets per action.
			*/
			std::vector<std::set<size_t>> getActionsTargets(const std::vector<const Core::FMTOutput*>& p_allOutputs) const;
			// DocString: FMTNssModel::updateOutputs
			/**
			@brief Update the targeted output values and the output indices, removing the development calculated value from the targets.
			@param[in] p_development the development.
			@param[in] p_paths the paths to the newly generated developments.
			@param[in] p_action the action index.
			@param[in] p_devArea the area of the development harvested.
			@param[in,out] p_targets the targeted values.
			@param[in,out] p_actionsoutputs the output indices for each action.
			@param[in] p_allOutput the outputs to update.
			@return the harvested area of the development.
			*/
			double updateOutputs(const Core::FMTDevelopment& p_development,
								const std::vector<Core::FMTDevelopmentPath>& p_paths,
								const int& p_action, const double& p_devArea,
								std::vector<double>& p_targets, std::vector<std::set<size_t>>& p_actionsoutputs,
								const std::vector<const Core::FMTOutput*>& p_allOutput) const;
			// DocString: FMTNssModel(const FMTSrModel&,unsigned int)
			/**
			@brief Construct a FMTNssModel from a FMTSrModel and a seed for the random number generator.
			@param[in] rhs the model.
			@param[in] seed the seed.
			*/
			FMTNssModel(const FMTSrModel& rhs, unsigned int seed);
			// DocString: FMTNssModel::swapPtr
			/**
			@brief Swap this model for the element at the end of the unique pointer.
			@param[in,out] rhs the unique pointer to swap with.
			*/
			virtual void swapPtr(std::unique_ptr<FMTModel>& rhs);

	};
}

BOOST_CLASS_EXPORT_KEY(Models::FMTNssModel)
#endif 
#endif

