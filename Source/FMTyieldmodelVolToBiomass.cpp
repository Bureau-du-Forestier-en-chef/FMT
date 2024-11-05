/*
Copyright (c) 2024 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTyieldmodelVolToBiomass.h"
#include <vector>
#include <unordered_map>
#include "FMTtheme.h"
#include "FMToutput.h"
#include "FMTmodel.h"
#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem.hpp>
#include <memory>
#include "FMTparser.h"

#define M_E 2.71828182845904523536

namespace Core
{

	FMTyieldmodelVolToBiomass::FMTyieldmodelVolToBiomass(const boost::property_tree::ptree& p_jsonProps,
		const std::vector<std::string>& p_yields) :
		m_equation1(),
		m_equation2(),
		m_equation3(),
		m_equation4To7()

	{
		boost::property_tree::ptree::const_assoc_iterator modelNameIt = p_jsonProps.find(JSON_PROP_MODEL_NAME);
		modelName = modelNameIt->second.data();
		modelYields = p_yields;
		_setParametersFromCsv(p_jsonProps,m_JSON_EQ1_FILE,{"a","b","volm"}, m_equation1.data());
		_setParametersFromCsv(p_jsonProps, m_JSON_EQ2_FILE,{ "a","b","k","cap"},m_equation2.data());
		_setParametersFromCsv(p_jsonProps, m_JSON_EQ3_FILE,{ "a","b","k","cap" },m_equation3.data());
		_setParametersFromCsv(p_jsonProps, m_JSON_EQ47_FILE,{ "a1","a2","a3","b1","b2","b3","c1","c2","c3" }, m_equation4To7.data());

	}

	const std::vector<double>FMTyieldmodelVolToBiomass::Predict(const Core::FMTyieldrequest& p_request) const
	{
		std::vector<double> Predictions;
		try {
			const double MERCHANTABLE_VOLUME = p_request.getYield(modelYields.back());
			const double MERCHANTABLE_BIOMASS = _getMerchantableBiomass(MERCHANTABLE_VOLUME);
			const double NON_MERCHANTABLE_BIOMASS = _getNonMerchantableBiomass(MERCHANTABLE_BIOMASS);
			const double SAPLING_BIOMASS = _getSaplingBiomass(MERCHANTABLE_BIOMASS + NON_MERCHANTABLE_BIOMASS);
			const double STEM_FACTOR = _getStemWoodFactor(MERCHANTABLE_VOLUME);
			const double TOTAL_ABG_BIOMASS = (MERCHANTABLE_BIOMASS + NON_MERCHANTABLE_BIOMASS + SAPLING_BIOMASS) / STEM_FACTOR;
			const double BARK_BIOMASS = _getBarkBiomass(MERCHANTABLE_VOLUME, TOTAL_ABG_BIOMASS);
			const double BRANCHES_BIOMASS = _getBranchesBiomass(MERCHANTABLE_VOLUME, TOTAL_ABG_BIOMASS);
			const double FOLIAGE_BIOMASS = _getFoliageBiomass(MERCHANTABLE_VOLUME, TOTAL_ABG_BIOMASS);
			Predictions.reserve(7);
			Predictions.push_back(MERCHANTABLE_BIOMASS);
			Predictions.push_back(NON_MERCHANTABLE_BIOMASS);
			Predictions.push_back(SAPLING_BIOMASS);
			Predictions.push_back(TOTAL_ABG_BIOMASS);
			Predictions.push_back(BARK_BIOMASS);
			Predictions.push_back(BRANCHES_BIOMASS);
			Predictions.push_back(FOLIAGE_BIOMASS);
			
		}catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldmodelVolToBiomass::Predict", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return Predictions;
	}


	std::unique_ptr<FMTyieldmodel> FMTyieldmodelVolToBiomass::presolve(const FMTmaskfilter& p_filter,
		const std::vector<FMTtheme>& p_newThemes) const
	{
		return std::unique_ptr<FMTyieldmodel>(new FMTyieldmodelVolToBiomass(*this));
	}

	std::unique_ptr<FMTyieldmodel> FMTyieldmodelVolToBiomass::postsolve(const FMTmaskfilter& p_filter,
		const std::vector<FMTtheme>& p_baseThemes) const
	{
		return std::unique_ptr<FMTyieldmodel>(new FMTyieldmodelVolToBiomass(*this));
	}

	std::unique_ptr<FMTyieldmodel>FMTyieldmodelVolToBiomass::Clone() const
	{
		return std::unique_ptr<FMTyieldmodel>(new FMTyieldmodelVolToBiomass(*this));
	}

	std::string FMTyieldmodelVolToBiomass::GetModelType()
	{
		return "VOLUME_TO_BIOMASS";
	}

	double FMTyieldmodelVolToBiomass::_getMerchantableBiomass(const double& p_grossVolume) const
	{
		return m_equation1[0] * std::pow(std::min(p_grossVolume, m_equation1[2]), m_equation1[1]);
	}

	double FMTyieldmodelVolToBiomass::_getNonMerchantableBiomass(const double& p_merchantableBiomass) const
	{
		const double NON_MERCH_FACTOR = std::min(m_equation2[2] + m_equation2[0] * 
													std::pow(p_merchantableBiomass, m_equation2[1]), m_equation2[3]);
		return p_merchantableBiomass * NON_MERCH_FACTOR - p_merchantableBiomass;
	}

	double FMTyieldmodelVolToBiomass::_getSaplingBiomass(const double& p_MerchAndNonMerchBiomass) const
	{
		const double SAPLING_FACTOR = std::min(m_equation3[2] + m_equation3[0] *
										std::pow(p_MerchAndNonMerchBiomass, m_equation3[1]), m_equation3[3]);
		return p_MerchAndNonMerchBiomass * SAPLING_FACTOR - p_MerchAndNonMerchBiomass;
	}

	double FMTyieldmodelVolToBiomass::_getStemWoodFactor(const double& p_MerchantableVolume) const
	{
		const double VOL_LOG = std::log(p_MerchantableVolume + 5);
		const double STEM_WOOD_PROP = 1 / (1 + (std::pow(M_E,m_equation4To7[0]+ m_equation4To7[1]* p_MerchantableVolume+ m_equation4To7[2] * VOL_LOG))
											 + (std::pow(M_E, m_equation4To7[3] + m_equation4To7[4] * p_MerchantableVolume + m_equation4To7[5] * VOL_LOG)) 
											 + (std::pow(M_E, m_equation4To7[6] + m_equation4To7[7] * p_MerchantableVolume + m_equation4To7[8] * VOL_LOG)));
		return STEM_WOOD_PROP;
	}

	double FMTyieldmodelVolToBiomass::_getBarkBiomass(const double& p_MerchantableVolume,
		const double& p_totalAboveGroundBiomass) const
	{
		const double VOL_LOG = std::log(p_MerchantableVolume + 5);
		const double BARK_PROP = std::pow(M_E, m_equation4To7[0] + m_equation4To7[1] * p_MerchantableVolume + m_equation4To7[2] * VOL_LOG) / 
			(1 + (std::pow(M_E, m_equation4To7[0] + m_equation4To7[1] * p_MerchantableVolume + m_equation4To7[2] * VOL_LOG))
			+ (std::pow(M_E, m_equation4To7[3] + m_equation4To7[4] * p_MerchantableVolume + m_equation4To7[5] * VOL_LOG))
			+ (std::pow(M_E, m_equation4To7[6] + m_equation4To7[7] * p_MerchantableVolume + m_equation4To7[8] * VOL_LOG)));
		return p_totalAboveGroundBiomass * BARK_PROP;
	}

	double FMTyieldmodelVolToBiomass::_getBranchesBiomass(const double& p_MerchantableVolume,
		const double& p_totalAboveGroundBiomass) const
	{
		const double VOL_LOG = std::log(p_MerchantableVolume + 5);
		const double BRANCHES_PROP = std::pow(M_E, m_equation4To7[3] + m_equation4To7[4] * p_MerchantableVolume + m_equation4To7[5] * VOL_LOG) /
			(1 + (std::pow(M_E, m_equation4To7[0] + m_equation4To7[1] * p_MerchantableVolume + m_equation4To7[2] * VOL_LOG))
				+ (std::pow(M_E, m_equation4To7[3] + m_equation4To7[4] * p_MerchantableVolume + m_equation4To7[5] * VOL_LOG))
				+ (std::pow(M_E, m_equation4To7[6] + m_equation4To7[7] * p_MerchantableVolume + m_equation4To7[8] * VOL_LOG)));
		return p_totalAboveGroundBiomass * BRANCHES_PROP;
	}

	double FMTyieldmodelVolToBiomass::_getFoliageBiomass(const double& p_MerchantableVolume,
		const double& p_totalAboveGroundBiomass) const
	{
		const double VOL_LOG = std::log(p_MerchantableVolume + 5);
		const double FOLIAGE_PROP = std::pow(M_E, m_equation4To7[6] + m_equation4To7[7] * p_MerchantableVolume + m_equation4To7[8] * VOL_LOG) /
			(1 + (std::pow(M_E, m_equation4To7[0] + m_equation4To7[1] * p_MerchantableVolume + m_equation4To7[2] * VOL_LOG))
				+ (std::pow(M_E, m_equation4To7[3] + m_equation4To7[4] * p_MerchantableVolume + m_equation4To7[5] * VOL_LOG))
				+ (std::pow(M_E, m_equation4To7[6] + m_equation4To7[7] * p_MerchantableVolume + m_equation4To7[8] * VOL_LOG)));
		return p_totalAboveGroundBiomass * FOLIAGE_PROP;
	}

	void FMTyieldmodelVolToBiomass::_setParametersFromCsv(
		const boost::property_tree::ptree& p_json,
		const std::string& p_fileTarget,
		const std::vector<std::string>& p_parameters,
		double* p_data) const
	{
		const boost::filesystem::path FMT_DLL(getruntimelocation());
		boost::property_tree::ptree::const_assoc_iterator PATH_TO_EQ = p_json.find(p_fileTarget);
		boost::filesystem::path EQ_PATH(PATH_TO_EQ->second.data());
		const std::string EQ_STR_PATH = (FMT_DLL / EQ_PATH).string();
		Parser::FMTparser csvParser;
		const std::vector<std::vector<std::string>> CSV_DATA = csvParser.readcsv(EQ_STR_PATH, ',');
		bool foundParameters = false;
		const int FIRST_DATA = 6;
		size_t rowIndex = 1;
		while (!foundParameters && rowIndex < CSV_DATA.size())
		{
			if (std::equal(CSV_DATA[rowIndex].begin(), CSV_DATA[rowIndex].begin() + FIRST_DATA-1, modelYields.begin()))
			{
				size_t columnId = FIRST_DATA;
				for (const std::string& PARAMETER : p_parameters)
				{
					if (PARAMETER == CSV_DATA[0][columnId])
					{
						*p_data = std::stod(CSV_DATA[rowIndex][columnId]);
						++p_data;
					}
					++columnId;
				}
				foundParameters = true;
			}
			++rowIndex;
		}
		if (!foundParameters)
			{
			std::string key;
			for (const std::string& KEY : modelYields)
				{
				key += KEY + " ";
				}
			key.pop_back();
			_exhandler->raise(Exception::FMTexc::FMTfunctionfailed,
				"Cant find parameters for "+ key,
				"FMTyieldmodelVolToBiomass::_setParametersFromCsv", __LINE__, __FILE__);
			}

	}

	

}


