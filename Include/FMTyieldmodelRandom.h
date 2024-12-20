/*
Copyright (c) 2024 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTyieldmodelRando_H_INCLUDED
#define FMTyieldmodelRando_H_INCLUDED

#include "FMTyieldmodel.h"
#include <random>
#include <string>
#include <vector>
#include <unordered_map>

// DocString: FMTyieldmodelRandom
/**
@class FMTyieldmodelRandom
@brief This yield model generates random index from a discret distribution
using discrete probability. The syntax in the yield section should look like this
yrandom _PRED(RANDOM,y1,y2,y3,y4)
*/
namespace Core
{
    class FMTyieldmodelRandom :
        public FMTyieldmodel
    {
    public:
		// DocString: FMTyieldmodelRandom::FMTyieldmodelRandom(const boost::property_tree::ptree&,const std::string&)
		/**
		@brief Main constructor for the random yield model.
		@param[in] p_jsonProps the json file
		@param[in]p_distribution the probability distribution of the yield.
		*/
		FMTyieldmodelRandom(const boost::property_tree::ptree& p_jsonProps, const std::vector<std::string>& p_distribution);
		// DocString: FMTyieldmodelRandom::clearRandomYieldsCache
		/**
		@brief Flush the random number cache to redraw new random sequence.
		*/
		void clearRandomYieldsCache() override;
		// DocString: FMTyieldmodelRandom::Predict
		/**
		@brief Predict the yields values using this class.
		@param[in] p_request a valid FMTyieldrequest.
		@return Predicted random values ranging from the distribution lower to upper bound.
		*/
		const std::vector<double>Predict(const Core::FMTyieldrequest& p_request) const;
		// DocString: FMTyieldmodelRandom::presolve
		/**
		@brief If the FMTyieldmodelRandom contains Core classes it also need to be presolved when presolved is called on the FMTmodel.
		By default it will return the same FMTyieldmodelRandom.
		@param[in] p_filter the mask filter to apply.
		@param[in] p_newThemes the new themes of the presolved model.
		@return a pointer to a presolved FMTyieldmodel.
		*/
		 std::unique_ptr<FMTyieldmodel> presolve(const FMTmaskfilter& p_filter,
			const std::vector<FMTtheme>& p_newThemes) const;
		// DocString: FMTyieldmodelRandom::postsolve
		/**
		@brief Postsolve the yieldmodel by default it will return a clone.
		@param[in] p_filter the mask filter to apply.
		@param[in] p_baseThemes the non presolved themes.
		@return a pointer to a postsolved FMTyieldmodel.
		*/
		std::unique_ptr<FMTyieldmodel> postsolve(const FMTmaskfilter& p_filter,
			const std::vector<FMTtheme>& p_baseThemes) const;
		// DocString: FMTyieldmodelRandom::Clone
		/**
		@brief Clone this FMTyieldmodel
		@return a cloned FMTyieldmodel.
		*/
		std::unique_ptr<FMTyieldmodel>Clone() const;
		// DocString: FMTyieldmodelRandom:: GetModelType
		/**
		@brief Get the model type.
		@return a string of the model type _random.
		*/
		static std::string GetModelType();
		// DocString: FMTyieldmodelRandom::setModel
		/**
		@brief set The model to the yielmodel and change the seed.!
		@param[in] p_modelPtr the pointer to the actual model. This can be cast to different type of model...
		*/
		virtual void setModel(Models::FMTmodel* p_modelPtr);
	private:
		// DocString: FMTyieldmodelRandom::getNormalizedYields
		/**
		@brief Get the yields value of p_yield based on p_request.
		@param[in] p_yields, a valid yields contained in the yield section.
		@param[in] p_request, a valid request from the model.
		@return Yields normalized yield values based on total value multiply by 100.
		*/
		std::vector<size_t> getNormalizedYields(const std::vector<std::string>& p_yields,const Core::FMTyieldrequest& p_request) const;
		// DocString: FMTyieldmodelRandom::getYield
		/**
		@brief REturn a randomly selected index
		@param[in] p_distribution
		@return a double randomly generated.
		*/
		double getRandomIndex(const std::vector<size_t>& p_distribution) const;
		// DocString: FMTyieldmodelRandom::getYield
		/**
		@brief If the seed of the FMTmodel parameters changed from the last call then update the m_seed and the m_generator.
		@param[in] p_request a valid request.
		*/
		void updateGeneratorAndSeed(const Core::FMTyieldrequest& p_request) const;
		// DocString: FMTyieldmodelRandom::m_cache
		///the cache value of the random number 
		mutable std::unordered_map<int,double>m_cache;
		// DocString: FMTyieldmodelRandom::m_useCache
		///tell if cache on or off...
		bool m_useCache;
    };
}


#endif

