/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTNSSMODEL_H
#define FMTNSSMODEL_H

#ifdef FMTWITHOSI

#include "FMTsrmodel.h"
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
	FMTNssModel stands for non spatial simulation model. This model is mainly used to simulate
	stochastics actions during the local replanning phase. Before using any kind of spatially explicit model
	sometime using a simple non spatial model can help to understand the basic idea of simulation.
	*/
	class FMTEXPORT FMTNssModel : public FMTSrModel
	{
		public:
			// DocString: FMTNssModel()
			/**
			Default constructor of FMTNssModel.
			*/
			FMTNssModel();
			// DocString: ~FMTNssModel()
			/**
			Default destructor of FMTNssModel.
			*/
			~FMTNssModel()=default;
			// DocString: FMTNssModel(const FMTNssModel&)
			/**
			Default copy constructor of FMTNssModel.
			*/
			FMTNssModel(const FMTNssModel& rhs);
			// DocString: FMTNssModel::operator=
			/**
			Default copy assignment of MTnssmodel.
			*/
			FMTNssModel& operator=(const FMTNssModel& rhs) = default;
			// DocString: FMTNssModel(const FMTModel&,unsigned int)
			/**
			Constructor for FMTNssModel taking a FMTModel and a seed to initialize the random number generator.
			*/
			FMTNssModel(const FMTModel& rhs, unsigned int seed);
			
			// DocString: FMTNssModel(FMTNssModel&&)
			/**
			Default move constructor for FMTNssModel.
			*/
			FMTNssModel(FMTNssModel&& rhs)=default;
			// DocString: FMTNssModel::operator=(FMTNssModel&& rhs) 
			/**
			Default move assignment for FMTNssModel.
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
			This function do a non spatial simulation based on the area constraints in the optimize section.
			*/
			void simulate();
			// DocString: FMTNssModel::presolve
			/**
			This function use a vector of developments and the actual transitions of the model and return new unique pointer to presolved FMTModel.
			The function can reduce the number of global themes/actions/transitions/yields/lifespans/outputs/constraints data if the model is badly formulated.
			*/
			virtual std::unique_ptr<FMTModel>presolve(std::vector<Core::FMTActualDevelopment> optionaldevelopments = std::vector<Core::FMTActualDevelopment>()) const;
			// DocString: FMTNssModel::clone
			/**
			Get a clone of the FMTNssModel
			*/
			virtual std::unique_ptr<FMTModel>clone() const;
			// DocString: FMTModel::build
			/**
			This function will use the function simulate over the number of period set as LENGTH in model parameters.
			*/
			virtual bool build(std::vector<Core::FMTSchedule> schedules=std::vector<Core::FMTSchedule>());
			// DocString: FMTModel::solve
			/**
			There is no solve since it's only a simulation. The build phase simulate over the LENGTH given in model parameters. 
			*/
			virtual bool solve()
			{
				return true;
			}
			// DocString: FMTNssModel::setParameter(const FMTboolmodelparameters, const bool)
			/**
			Override setter for boolmodelparameters.
			*/
			bool setParameter(const FMTboolmodelparameters& key, const bool& value) override;
			// DocString: FMTNssModel::getCopy
			/**
			This function returns a copy of the FMTModel of the selected period.
			If period = 0 it returns the FMTModel::getCopy if period > 0 then it returns
			a copy of the FMTModel based on the developments of the FMTGraph of the FMTLpModel.
			Need to have a builded graph with a solution to use this function.
			*/
			virtual std::unique_ptr<FMTModel> getCopy(int period = 0) const;
		private:
			// DocString: FMTNssModel::save
			/**
			Save function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
			*/
			friend class boost::serialization::access;
			template<class Archive>
			void save(Archive& ar, const unsigned int version) const
			{
				ar& boost::serialization::make_nvp("model", boost::serialization::base_object<FMTSrModel>(*this));

			}
			// DocString: FMTNssModel::load
			/**
			Load function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
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
			@brief Get the first operable action for the development
			@param[in] development the development to test operability
			@param[in] targest the targeted actions.
			@param[in] the outputs values.
			@return a pair with output index and pointer to action nullptr returned if no operables.
			*/
			std::pair<size_t, const Core::FMTAction*> getFirstOperable(const Core::FMTDevelopment& development,
				std::vector<std::vector<const Core::FMTAction*>> targets,
				const std::vector<const Core::FMTOutput*>& alloutputs) const;
			// DocString: FMTNssModel::getActionsTargets
			/**
			@brief takea vector of pointer to outputs and buildup a vector of actions length containing nullptr and pointer to outputs.
			to action related to each outputs.
			@param[in] vectors of outputs ptr
			@return a vector of pair of action / outputs.
			*/
			std::vector<std::set<size_t>> getActionsTargets(const std::vector<const Core::FMTOutput*>& p_allOutputs) const;
			// DocString: FMTNssModel::updateOutputs
			/**
			@brief Update the targeted output value and the outputs index remove the p_index from p_actionsoutputs and remove dev 
			valculated value from p_targets
			@param[in] p_development the development that we update the value with
			@param[in] p_paths Path to newly generated devs.
			@param[in] p_action the action index
			@param[in] p_devArea the area of the development harvested
			@param[in] p_targets the targeted values
			@param[in] p_actionsoutputs the outputs index for each acitons
			@param[in] p_allOutputs the outputs to update.
			@return the harvested area of the dev.
			*/
			double updateOutputs(const Core::FMTDevelopment& p_development,
								const std::vector<Core::FMTDevelopmentPath>& p_paths,
								const int& p_action, const double& p_devArea,
								std::vector<double>& p_targets, std::vector<std::set<size_t>>& p_actionsoutputs,
								const std::vector<const Core::FMTOutput*>& p_allOutput) const;
			// DocString: FMTNssModel(const FMTSrModel&,unsigned int)
			/**
			Constructor for FMTNssModel taking a FMTSrModel and a seed to initialize the random number generator.
			*/
			FMTNssModel(const FMTSrModel& rhs, unsigned int seed);
			// DocString: FMTNssModel::swapPtr
			/**
			@brief swap from unique_ptr of model
			@param[in] a unique pointer to a FMTModel.
			*/
			virtual void swapPtr(std::unique_ptr<FMTModel>& rhs);

	};
}

BOOST_CLASS_EXPORT_KEY(Models::FMTNssModel)
#endif 
#endif

