/*
Copyright (c) 2024 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTyieldmodelUnitCoverag_H_INCLUDED
#define FMTyieldmodelUnitCoverag_H_INCLUDED

#include "FMTyieldmodel.h"
#include "FMTmask.h"
#include <vector>

namespace Core
{
	class FMToutput;
	// DocString: FMTyieldmodelUnitCoverage
	/**
	@classFMTyieldmodelUnitCoverage
	@brief This yield model calculate the sum of a yield value over a spatial unit...at period 0.
	*/
	class FMTyieldmodelUnitCoverage :
		public FMTyieldmodel
	{
	public:
		// DocString: FFMTyieldmodelUnitCoverage::FMTyieldmodelUnitCoverage(const boost::property_tree::ptree&,const std::string&)
		/**
		@brief Main constructor for the unit coverage model.
		@param[in] p_jsonProps the json file
		@param[in] p_yields the input yields
		@param[in] p_mask the mask of the yields we are looking at.
		*/
		FMTyieldmodelUnitCoverage(const boost::property_tree::ptree& p_jsonProps,
			const std::vector<std::string>& p_yields, const Core::FMTmask& p_mask);
		// DocString: FMTyieldmodelUnitCoverage::Predict
		/**
		@brief Predict the yields values using this class.
		@param[in] p_request a valid FMTyieldrequest.
		@return calculate the sum of each yields throught the spatial unit.
		*/
		const std::vector<double>Predict(const Core::FMTyieldrequest& p_request) const;
		// DocString: FMTyieldmodelRandom::presolve
		/**
		@brief If the FMTyieldmodelUnitCoverage contains Core classes it also need to be presolved when presolved is called on the FMTmodel.
		By default it will return the same FMTyieldmodelUnitCoverage.
		@param[in] p_filter the mask filter to apply.
		@param[in] p_newThemes the new themes of the presolved model.
		@return a pointer to a presolved FMTyieldmodel.
		*/
		std::unique_ptr<FMTyieldmodel> presolve(const FMTmaskfilter& p_filter,
			const std::vector<FMTtheme>& p_newThemes) const;
		// DocString: FMTyieldmodelUnitCoverage::postsolve
		/**
		@brief Postsolve the yieldmodel by default it will return a clone.
		@param[in] p_filter the mask filter to apply.
		@param[in] p_baseThemes the non presolved themes.
		@return a pointer to a postsolved FMTyieldmodel.
		*/
		std::unique_ptr<FMTyieldmodel> postsolve(const FMTmaskfilter& p_filter,
			const std::vector<FMTtheme>& p_baseThemes) const;
		// DocString: FMTyieldmodelUnitCoverage::Clone
		/**
		@brief Clone this FMTyieldmodel
		@return a cloned FMTyieldmodel.
		*/
		std::unique_ptr<FMTyieldmodel>Clone() const;
		// DocString: FMTyieldmodelUnitCoverage::GetModelType
		/**
		@brief Get the model type.
		@return a string of the model type _random.
		*/
		static std::string GetModelType();
	private:
		// DocString: FMTyieldmodelUnitCoverage::getOutputs
		/**
		@brief get the relative outputs of the yields
		@return Yields normalized yield values based on total value multiply by 100.
		*/
		std::vector<FMToutput> getOutputs() const;
		// DocString: FMTyieldmodelUnitCoverage::getOutputValues
		/**
		@brief Calculate the output values based on the p_outputs for a p_period.
		@param[in] the vector of output to evaluate
		@return vector of values of calculated outputs.
		*/
		std::vector<double> getOutputValues(const std::vector<FMToutput>& p_outputs) const;
		// DocString: FMTyieldmodelUnitCoverage::m_cache
		///the cache values of the outputs
		mutable std::vector<double>m_cache;
		// DocString: FMTyieldmodelUnitCoverage::m_mask
		///The mask of the yield.
		Core::FMTmask m_mask;

	};
}


#endif

