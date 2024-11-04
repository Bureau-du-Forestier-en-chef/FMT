/*
Copyright (c) 2024 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef FMTyieldmodelVolToBiomass_H_INCLUDED
#define FMTyieldmodelVolToBiomass_H_INCLUDED

#include "FMTyieldmodel.h"
#include "FMTmask.h"
#include <vector>

namespace Core
{
	// DocString: FMTyieldmodelVolToBiomass
	/**
	Volume to biomass conversion from https://ostrnrcan-dostrncan.canada.ca/entities/publication/29ed30f7-ba82-4750-8d5c-e043b4cab898
	To get the total above ground biomass from the gross merchantable volume.
	*/
	class FMTyieldmodelVolToBiomass :
		public FMTyieldmodel
	{
	public:
		// DocString: FMTyieldmodelVolToBiomass::FMTyieldmodelVolToBiomass(const boost::property_tree::ptree&,const std::string&)
		/**
		* 
		@brief Main constructor for volume to biomass converter
		@param[in] p_jsonProps the json file
		@param[in] p_yields the input yields
		*/
		FMTyieldmodelVolToBiomass(const boost::property_tree::ptree& p_jsonProps,
									const std::vector<std::string>& p_yields);
		// DocString: FMTyieldmodelVolToBiomass::Predict
		/**
		@brief Predict the yields values using this class.
		@param[in] p_request a valid FMTyieldrequest.
		@return calculate the sum of each yields throught the spatial unit.
		*/
		const std::vector<double>Predict(const Core::FMTyieldrequest& p_request) const;
		// DocString: FMTyieldmodelVolToBiomass::presolve
		/**
		@brief If the FMTyieldmodelVolToBiomass contains Core classes it also need to be presolved when presolved is called on the FMTmodel.
		By default it will return the same FMTyieldmodelVolToBiomass.
		@param[in] p_filter the mask filter to apply.
		@param[in] p_newThemes the new themes of the presolved model.
		@return a pointer to a presolved FMTyieldmodel.
		*/
		std::unique_ptr<FMTyieldmodel> presolve(const FMTmaskfilter& p_filter,
			const std::vector<FMTtheme>& p_newThemes) const;
		// DocString: FMTyieldmodelVolToBiomass::postsolve
		/**
		@brief Postsolve the yieldmodel by default it will return a clone.
		@param[in] p_filter the mask filter to apply.
		@param[in] p_baseThemes the non presolved themes.
		@return a pointer to a postsolved FMTyieldmodel.
		*/
		std::unique_ptr<FMTyieldmodel> postsolve(const FMTmaskfilter& p_filter,
			const std::vector<FMTtheme>& p_baseThemes) const;
		// DocString: FMTyieldmodelVolToBiomass::Clone
		/**
		@brief Clone this FMTyieldmodel
		@return a cloned FMTyieldmodel.
		*/
		std::unique_ptr<FMTyieldmodel>Clone() const;
		// DocString: FMTyieldmodelVolToBiomass::GetModelType
		/**
		@brief Get the model type.
		@return a string of the model type _random.
		*/
		static std::string GetModelType();
	private:
		// DocString: FMTyieldmodelVolToBiomass::m_equation1
		///equation 1 a,b
		std::array<float,2>m_equation1;
		// DocString: FMTyieldmodelVolToBiomass::m_equation2
		///equation 2 a,b,k,cap
		std::array<float,4>m_equation2;
		// DocString: FMTyieldmodelVolToBiomass::m_equation3
		///equation 3 a,b,k,cap
		std::array<float,4>m_equation3;
		// DocString: FMTyieldmodelVolToBiomass::_getMerchantableBiomass
		/**
		@brief Get merchantable biomass using gross merchantable volume
		@param[in] merchantable volume.
		@return The merchantable biomass
		*/
		float _getMerchantableBiomass(const float& p_grossVolume) const;
		// DocString: FMTyieldmodelVolToBiomass::_getNonMerchantableBiomass
		/**
		@brief Get merchantable biomass using merchantable biomass
		@param[in] merchantable biomass.
		@return The non merchantable biomass
		*/
		float _getNonMerchantableBiomass(const float& p_merchantableBiomass) const;
		// DocString: FMTyieldmodelVolToBiomass::_getSaplingBiomass
		/**
		@brief Get merchantable sapling biomass using gross merchantable volume
		@param[in] non merchantable biomass.
		@return The sapling biomass
		*/
		float _getSaplingBiomass(const float& p_nonMerchantableBiomass) const;
	};
}


#endif

