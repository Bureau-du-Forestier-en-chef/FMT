/*
Copyright (c) 2024 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTyieldmodelUnitCoverag_Hm_included
#define FMTyieldmodelUnitCoverag_Hm_included

#include "FMTyieldmodel.h"
#include "FMTmask.h"
#include <vector>

namespace Core
{
	class FMTOutput;
	// DocString: FMTYieldModelUnitCoverage
	/**
	@classFMTyieldmodelUnitCoverage
	@brief This yield model calculate the sum of a yield value over a spatial unit...at period 0.
	*/
	class FMTYieldModelUnitCoverage :
		public FMTYieldModel
	{
	public:
		// DocString: FFMTyieldmodelUnitCoverage::FMTYieldModelUnitCoverage(const boost::property_tree::ptree&,const std::string&)
		/**
		@brief Main constructor for the unit coverage model.
		@param[in] p_jsonProps the json file
		@param[in] p_yields the input yields
		@param[in] p_mask the mask of the yields we are looking at.
		*/
		FMTYieldModelUnitCoverage(const boost::property_tree::ptree& p_jsonProps,
			const std::vector<std::string>& p_yields, const Core::FMTMask& p_mask);
		// DocString: FMTYieldModelUnitCoverage::predict
		/**
		@brief predict the yields values using this class.
		@param[in] p_request a valid FMTYieldRequest.
		@return calculate the sum of each yields throught the spatial unit.
		*/
		const std::vector<double>predict(const Core::FMTYieldRequest& p_request) const;
		// DocString: FMTYieldModelRandom::presolve
		/**
		@brief If the FMTYieldModelUnitCoverage contains Core classes it also need to be presolved when presolved is called on the FMTModel.
		By default it will return the same FMTYieldModelUnitCoverage.
		@param[in] p_filter the mask filter to apply.
		@param[in] p_newThemes the new themes of the presolved model.
		@return a pointer to a presolved FMTYieldModel.
		*/
		std::unique_ptr<FMTYieldModel> presolve(const FMTMaskFilter& p_filter,
			const std::vector<FMTTheme>& p_newThemes) const;
		// DocString: FMTYieldModelUnitCoverage::postSolve
		/**
		@brief Postsolve the yieldmodel by default it will return a clone.
		@param[in] p_filter the mask filter to apply.
		@param[in] p_baseThemes the non presolved themes.
		@return a pointer to a postsolved FMTYieldModel.
		*/
		std::unique_ptr<FMTYieldModel> postSolve(const FMTMaskFilter& p_filter,
			const std::vector<FMTTheme>& p_baseThemes) const;
		// DocString: FMTYieldModelUnitCoverage::Clone
		/**
		@brief Clone this FMTYieldModel
		@return a cloned FMTYieldModel.
		*/
		std::unique_ptr<FMTYieldModel>Clone() const;
		// DocString: FMTYieldModelUnitCoverage::getModelType
		/**
		@brief Get the model type.
		@return a string of the model type _random.
		*/
		static std::string getModelType();
	private:
		// DocString: FMTYieldModelUnitCoverage::getOutputs
		/**
		@brief get the relative outputs of the yields
		@return Yields normalized yield values based on total value multiply by 100.
		*/
		std::vector<FMTOutput> getOutputs() const;
		// DocString: FMTYieldModelUnitCoverage::getOutputValues
		/**
		@brief Calculate the output values based on the p_outputs for a p_period.
		@param[in] the vector of output to evaluate
		@return vector of values of calculated outputs.
		*/
		std::vector<double> getOutputValues(const std::vector<FMTOutput>& p_outputs) const;
		// DocString: FMTYieldModelUnitCoverage::m_cache
		///the cache values of the outputs
		mutable std::vector<double>m_cache;
		// DocString: FMTYieldModelUnitCoverage::m_mask
		///The mask of the yield.
		Core::FMTMask m_mask;

	};
}


#endif

