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
#include <array>

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
		// DocString: FMTyieldmodelVolToBiomass::JSON_EQ1_FILE_PATH
		///equation1 file
		const std::string m_JSON_EQ1_FILE = "appendix2_table3";
		// DocString: FMTyieldmodelVolToBiomass::JSON_EQ2_FILE_PATH
		///equation2 file
		const std::string m_JSON_EQ2_FILE = "appendix2_table4";
		// DocString: FMTyieldmodelVolToBiomass::JSON_EQ3_FILE_PATH
		///equation3 file
		const std::string m_JSON_EQ3_FILE = "appendix2_table5";
		// DocString: FMTyieldmodelVolToBiomass::JSON_EQ47_FILE_PATH
		///equation 4 to 7 file
		const std::string m_JSON_EQ47_FILE = "appendix2_table6";
		// DocString: FMTyieldmodelVolToBiomass::m_equation1
		///equation 1 parameters a,b,volm
		std::array<double,3>m_equation1;
		// DocString: FMTyieldmodelVolToBiomass::m_equation2
		///equation 2 parameters a,b,k,cap
		std::array<double,4>m_equation2;
		// DocString: FMTyieldmodelVolToBiomass::m_equation3
		///equation 3 parameters a,b,k,cap
		std::array<double,4>m_equation3;
		// DocString: FMTyieldmodelVolToBiomass::m_equation4
		///equation 4 to 7 parameters a1,a2,a3,b1,b2,b3,c1,c2,c3
		std::array<double, 9>m_equation4To7;
		// DocString: FMTyieldmodelVolToBiomass::_getMerchantableBiomass
		/**
		@brief Get merchantable biomass using gross merchantable volume
		@param[in] merchantable volume.
		@return The merchantable biomass
		*/
		double _getMerchantableBiomass(const double& p_grossVolume) const;
		// DocString: FMTyieldmodelVolToBiomass::_getNonMerchantableBiomass
		/**
		@brief Get merchantable biomass using merchantable biomass
		@param[in] merchantable biomass.
		@return The non merchantable biomass
		*/
		double _getNonMerchantableBiomass(const double& p_merchantableBiomass) const;
		// DocString: FMTyieldmodelVolToBiomass::_getSaplingBiomass
		/**
		@brief Get merchantable sapling biomass using gross merchantable volume
		@param[in] non merchantable n merchantable biomass.
		@return The sapling biomass
		*/
		double _getSaplingBiomass(const double& p_MerchAndNonMerchBiomass) const;
		// DocString: FMTyieldmodelVolToBiomass::_getStemWoodFactor
		/**
		@brief Get the Stem wood Biomass
		@param[in] merchantable volume
		@return the Stem Wood biomass
		*/
		double _getStemWoodFactor(const double& p_MerchantableVolume) const;
		// DocString: FMTyieldmodelVolToBiomass::_getBarkBiomass
		/**
		@brief Get the Bark Biomass
		@param[in] merchantable volume
		@param[in] Total Biomass
		@return the Bark biomass
		*/
		double _getBarkBiomass(const double& p_MerchantableVolume,
								const double& p_totalAboveGroundBiomass) const;
		// DocString: FMTyieldmodelVolToBiomass::_getBranchesBiomass
		/**
		@brief Get the branches Biomass
		@param[in] merchantable volume
		@param[in] Total Biomass
		@return the branches biomass
		*/
		double _getBranchesBiomass(const double& p_MerchantableVolume,
			const double& p_totalAboveGroundBiomass) const;
		// DocString: FMTyieldmodelVolToBiomass::_getFoliageBiomass
		/**
		@brief Get the foliage Biomass
		@param[in] merchantable volume
		@param[in] Total Biomass
		@return foliage biomass
		*/
		double _getFoliageBiomass(const double& p_MerchantableVolume,
			const double& p_totalAboveGroundBiomass) const;
		// DocString: FMTyieldmodelVolToBiomass::_setParametersFromCsv
		/**
		@brief Load the parameters from csv files.
		@param[in] json config file
		@param[in] p_file to look in
		@param[in] p_parameters parameter to grab
		@return parameters in double format
		*/
		void _setParametersFromCsv(
			const boost::property_tree::ptree& p_json,
			const std::string& p_fileTarget,
			const std::vector<std::string>& p_parameters,
			double* p_data) const;
	};
}


#endif

