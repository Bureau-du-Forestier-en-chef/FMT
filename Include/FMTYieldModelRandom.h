/*
Copyright (c) 2024 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTyieldmodelRando_Hm_included
#define FMTyieldmodelRando_Hm_included

#include "FMTYieldModel.h"
#include <random>
#include <string>
#include <vector>
#include <unordered_map>

// DocString: FMTYieldModelRandom
/**
@brief Yield model generating a random index from a discrete distribution using discrete probabilities.
@details The syntax in the yield section looks like: yrandom _PRED(RANDOM,y1,y2,y3,y4).
*/
namespace Core
{
    class FMTYieldModelRandom :
        public FMTYieldModel
    {
    public:
		// DocString: FMTYieldModelRandom::FMTYieldModelRandom(const boost::property_tree::ptree&,const std::string&)
		/**
		@brief Main constructor for the random yield model.
		@param[in] p_jsonProps the json file
		@param[in]p_distribution the probability distribution of the yield.
		*/
		FMTYieldModelRandom(const boost::property_tree::ptree& p_jsonProps, const std::vector<std::string>& p_distribution);
		// DocString: FMTYieldModelRandom::clearRandomYieldsCache
		/**
		@brief Flush the random number cache to redraw new random sequence.
		*/
		void clearRandomYieldsCache() override;
		// DocString: FMTYieldModelRandom::predict
		/**
		@brief predict the yields values using this class.
		@param[in] p_request a valid FMTYieldRequest.
		@return Predicted random values ranging from the distribution lower to upper bound.
		*/
		const std::vector<double>predict(const Core::FMTYieldRequest& p_request) const;
		// DocString: FMTYieldModelRandom::preSolve
		/**
		@brief If the FMTYieldModelRandom contains Core classes it also need to be presolved when presolved is called on the FMTModel.
		By default it will return the same FMTYieldModelRandom.
		@param[in] p_filter the mask filter to apply.
		@param[in] p_newThemes the new themes of the presolved model.
		@return a pointer to a presolved FMTYieldModel.
		*/
		 std::unique_ptr<FMTYieldModel> preSolve(const FMTMaskFilter& p_filter,
			const std::vector<FMTTheme>& p_newThemes) const;
		// DocString: FMTYieldModelRandom::postSolve
		/**
		@brief Postsolve the yieldmodel by default it will return a clone.
		@param[in] p_filter the mask filter to apply.
		@param[in] p_baseThemes the non presolved themes.
		@return a pointer to a postsolved FMTYieldModel.
		*/
		std::unique_ptr<FMTYieldModel> postSolve(const FMTMaskFilter& p_filter,
			const std::vector<FMTTheme>& p_baseThemes) const;
		// DocString: FMTYieldModelRandom::clone
		/**
		@brief clone this FMTYieldModel
		@return a cloned FMTYieldModel.
		*/
		std::unique_ptr<FMTYieldModel>clone() const;
		// DocString: FMTYieldModelRandom:: getModelType
		/**
		@brief Get the model type.
		@return a string of the model type _random.
		*/
		static std::string getModelType();
		// DocString: FMTYieldModelRandom::setModel
		/**
		@brief set The model to the yielmodel and change the seed.!
		@param[in] p_modelPtr the pointer to the actual model. This can be cast to different type of model...
		*/
		virtual void setModel(Models::FMTModel* p_modelPtr);
	private:
		// DocString: FMTYieldModelRandom::_getNormalizedYields
		/**
		@brief Get the yields value of p_yield based on p_request.
		@param[in] p_yields, a valid yields contained in the yield section.
		@param[in] p_request, a valid request from the model.
		@return Yields normalized yield values based on total value multiply by 100.
		*/
		std::vector<size_t> _getNormalizedYields(const std::vector<std::string>& p_yields,const Core::FMTYieldRequest& p_request) const;
		// DocString: FMTYieldModelRandom::getYield
		/**
		@brief REturn a randomly selected index
		@param[in] p_distribution
		@return a double randomly generated.
		*/
		double _getRandomIndex(const std::vector<size_t>& p_distribution) const;
		// DocString: FMTYieldModelRandom::getYield
		/**
		@brief If the seed of the FMTModel parameters changed from the last call then update the m_seed and the m_generator.
		@param[in] p_request a valid request.
		*/
		void _updateGeneratorAndSeed(const Core::FMTYieldRequest& p_request) const;
		// DocString: FMTYieldModelRandom::m_cache
		///the cache value of the random number 
		mutable std::unordered_map<int,double>m_cache;
		// DocString: FMTYieldModelRandom::m_useCache
		///tell if cache on or off...
		bool m_useCache;
    };
}


#endif

