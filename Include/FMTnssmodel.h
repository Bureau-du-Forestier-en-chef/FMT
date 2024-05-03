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
	// DocString: FMTnssmodel
	/**
	FMTnssmodel stands for non spatial simulation model. This model is mainly used to simulate
	stochastics actions during the local replanning phase. Before using any kind of spatialy explicit model
	sometime using a simple non spatial model can help to understand the basic idea of simulation.
	*/
	class FMTEXPORT FMTnssmodel : public FMTsrmodel
	{
		public:
			// DocString: FMTnssmodel()
			/**
			Default constructor of FMTnssmodel.
			*/
			FMTnssmodel();
			// DocString: ~FMTnssmodel()
			/**
			Default destructor of FMTnssmodel.
			*/
			~FMTnssmodel()=default;
			// DocString: FMTnssmodel(const FMTnssmodel&)
			/**
			Default copy constructor of FMTnssmodel.
			*/
			FMTnssmodel(const FMTnssmodel& rhs);
			// DocString: FMTnssmodel::operator=
			/**
			Default copy assignment of MTnssmodel.
			*/
			FMTnssmodel& operator=(const FMTnssmodel& rhs) = default;
			// DocString: FMTnssmodel(const FMTmodel&,unsigned int)
			/**
			Constructor for FMTnssmodel taking a FMTmodel and a seed to initialize the random number generator.
			*/
			FMTnssmodel(const FMTmodel& rhs, unsigned int seed);
			
			// DocString: FMTnssmodel(FMTnssmodel&&)
			/**
			Default move constructor for FMTnssmodel.
			*/
			FMTnssmodel(FMTnssmodel&& rhs)=default;
			// DocString: FMTnssmodel::operator=(FMTnssmodel&& rhs) 
			/**
			Default move assignment for FMTnssmodel.
			*/
			FMTnssmodel& operator =(FMTnssmodel&& rhs) =default;
			// DocString: FMTnssmodel::setparameter(const FMTintmodelparameters,const int&)
			/**
			@brief set int parameters to nss model.
			@param[in] p_key the int key to change.
			@param[in] p_value to set to the p_key.
			@return true if the parameter is set.
			*/
			virtual bool setparameter(const FMTintmodelparameters& p_key, const int& p_value);
			// DocString: FMTnssmodel::simulate
			/**
			This function do a non spatial simulation based on the area constraints in the optimize section.
			*/
			void simulate();
			// DocString: FMTnssmodel::presolve
			/**
			This function use a vector of developments and the actual transitions of the model and return new unique pointer to presolved FMTmodel.
			The function can reduce the number of global themes/actions/transitions/yields/lifespans/outputs/constraints data if the model is badly formulated.
			*/
			virtual std::unique_ptr<FMTmodel>presolve(std::vector<Core::FMTactualdevelopment> optionaldevelopments = std::vector<Core::FMTactualdevelopment>()) const;
			// DocString: FMTnssmodel::clone
			/**
			Get a clone of the FMTnssmodel
			*/
			virtual std::unique_ptr<FMTmodel>clone() const;
			// DocString: FMTmodel::build
			/**
			This function will use the function simulate over the number of period set as LENGTH in model parameters.
			*/
			virtual bool build(std::vector<Core::FMTschedule> schedules=std::vector<Core::FMTschedule>());
			// DocString: FMTmodel::solve
			/**
			There is no solve since it's only a simulation. The build phase simulate over the LENGTH given in model parameters. 
			*/
			virtual bool solve()
			{
				return true;
			}
			// DocString: FMTnssmodel::setparameter(const FMTboolmodelparameters, const bool)
			/**
			Override setter for boolmodelparameters.
			*/
			bool setparameter(const FMTboolmodelparameters& key, const bool& value) override;
			// DocString: FMTnssmodel::getcopy
			/**
			This function returns a copy of the FMTmodel of the selected period.
			If period = 0 it returns the FMTmodel::getcopy if period > 0 then it returns
			a copy of the FMTmodel based on the developements of the FMTgraph of the FMTlpmodel.
			Need to have a builded graph with a solution to use this function.
			*/
			virtual std::unique_ptr<FMTmodel> getcopy(int period = 0) const;
		private:
			// DocString: FMTnssmodel::save
			/**
			Save function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
			*/
			friend class boost::serialization::access;
			template<class Archive>
			void save(Archive& ar, const unsigned int version) const
			{
				ar& boost::serialization::make_nvp("model", boost::serialization::base_object<FMTsrmodel>(*this));
				std::stringstream basegenerator;
				basegenerator << generator;
				const std::string basegeneratorstring(basegenerator.str());
				ar& BOOST_SERIALIZATION_NVP(basegeneratorstring);

			}
			// DocString: FMTnssmodel::load
			/**
			Load function is for serialization, used to do multiprocessing across multiple cpus (pickle in Pyhton)
			*/
			template<class Archive>
			void load(Archive& ar, const unsigned int version)
			{
				ar& boost::serialization::make_nvp("model", boost::serialization::base_object<FMTsrmodel>(*this));
				std::string basegeneratorstring;
				ar& BOOST_SERIALIZATION_NVP(basegeneratorstring);
				std::stringstream(basegeneratorstring) >> generator;

			}
			BOOST_SERIALIZATION_SPLIT_MEMBER()
				// DocString: FMTnssmodel::generator
				///This simulation model need to have it's own random number generator
				std::default_random_engine generator;
			// DocString: FMTnssmodel::constraintsToTarget
			/**
			@brief Using the constraints generate random level or determinist level of output values in targets
			@param[out] p_targets the targeted values 1 per outputs.
			@param[in] p_period the targeted period
			@return a vector of output pointers.
			*/
			std::vector<const Core::FMToutput*> constraintsToTarget(std::vector<double>& p_targets, const int& p_period);
			// DocString: FMTnssmodel::gotOutputForDev
			/**
			@brief Return true if a output is found for the development
			@param[in] p_development the development from which we want to find an output.
			@param[in] p_outputs the vector of outputs we look in for the action.
			@param[in] p_outputIds the output ids that we can test 
			@return true if found else false.
			*/
			bool gotOutputForDev(const Core::FMTdevelopment& p_development,
								const std::vector<const Core::FMToutput*>& p_outputs,
								const std::set<size_t>& p_outputIds) const;
			// DocString: FMTnssmodel::getFirstOperable
			/**
			@brief Get the first operable action for the developement
			@param[in] development the development to test operability
			@param[in] targest the targeted actions.
			@param[in] the outputs values.
			@return a pair with output index and pointer to action nullptr returned if no operables.
			*/
			std::pair<size_t, const Core::FMTaction*> getFirstOperable(const Core::FMTdevelopment& development,
				std::vector<std::vector<const Core::FMTaction*>> targets,
				const std::vector<const Core::FMToutput*>& alloutputs) const;
			// DocString: FMTnssmodel::getActionsTargets
			/**
			@brief takea vector of pointer to outputs and buildup a vector of actions length containing nullptr and pointer to outputs.
			to action related to each outputs.
			@param[in] vectors of outputs ptr
			@return a vector of pair of action / outputs.
			*/
			std::vector<std::set<size_t>> getActionsTargets(const std::vector<const Core::FMToutput*>& p_allOutputs) const;
			// DocString: FMTnssmodel::UpdateOutputs
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
			double UpdateOutputs(const Core::FMTdevelopment& p_development,
								const std::vector<Core::FMTdevelopmentpath>& p_paths,
								const int& p_action, const double& p_devArea,
								std::vector<double>& p_targets, std::vector<std::set<size_t>>& p_actionsoutputs,
								const std::vector<const Core::FMToutput*>& p_allOutput) const;
			// DocString: FMTnssmodel(const FMTsrmodel&,unsigned int)
			/**
			Constructor for FMTnssmodel taking a FMTsrmodel and a seed to initialize the random number generator.
			*/
			FMTnssmodel(const FMTsrmodel& rhs, unsigned int seed);
			// DocString: FMTnssmodel::swap_ptr
			/**
			@brief swap from unique_ptr of model
			@param[in] a unique pointer to a FMTmodel.
			*/
			virtual void swap_ptr(const std::unique_ptr<FMTmodel>& rhs);

	};
}

BOOST_CLASS_EXPORT_KEY(Models::FMTnssmodel)
#endif 
#endif

