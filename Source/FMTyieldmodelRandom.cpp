/*
Copyright (c) 2024 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTyieldmodelRandom.h"
#include "FMTexceptionhandler.h"
#include "FMTyieldrequest.h"
#include "FMTgraphvertextoyield.h"
#include "FMTyieldhandler.h"
#include "FMTmodel.h"
#include <boost/property_tree/json_parser.hpp>
#include <memory>

namespace Core
{

	FMTyieldmodelRandom::FMTyieldmodelRandom(const boost::property_tree::ptree& p_jsonProps, const std::vector<std::string>& p_distribution):
		FMTyieldmodel(), m_cache(), m_useCache(false)
		{
		boost::property_tree::ptree::const_assoc_iterator modelNameIt = p_jsonProps.find(JSON_PROP_MODEL_NAME);
		modelName = modelNameIt->second.data();
		boost::property_tree::ptree::const_assoc_iterator modelCacheIt = p_jsonProps.find("caching");
		m_useCache = modelCacheIt->second.get_value<bool>();
		modelYields = p_distribution;
		}

	void FMTyieldmodelRandom::clearRandomYieldsCache()
		{
		if (m_useCache)
			{
			m_cache.clear();
			}
		}

	std::vector<size_t> FMTyieldmodelRandom::getNormalizedYields(const std::vector<std::string>& p_yields, const Core::FMTyieldrequest& p_request) const
	{
		std::vector<size_t> values(p_yields.size());
		try {
			std::vector<double>baseValues(p_yields.size(),0.0);
			double totalValue = 0;
			size_t nameId = 0;
			for (const std::string& yieldName : p_yields)
			{
				bool gotYield = false;
				for (const FMTyieldrequest::const_iterator yield : p_request.getdatas())
				{
					if ((yield->second)->containsyield(yieldName))
					{
						const double VALUE = std::abs((yield->second)->get(yieldName, p_request));
						totalValue += VALUE;
						baseValues[nameId] = VALUE;
						gotYield = true;
						break;
					}
				}
				if (!gotYield)
				{
					_exhandler->raise(Exception::FMTexc::FMTmissingyield,
						yieldName + " for developement " + std::string(p_request.getdevelopment()),
						"FMTyieldmodelRandom::getNormalizedYields", __LINE__, __FILE__, Core::FMTsection::Yield);
				}
				++nameId;
			}
			size_t i = 0;
			for (const double& value : baseValues)
			{
				values[i] = static_cast<size_t>((value/ totalValue)*100);
				i++;
			}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTyieldmodelRandom::getNormalizedYields", __LINE__, __FILE__, Core::FMTsection::Yield);
			}
		return values;
	}


	double FMTyieldmodelRandom::getRandomIndex(const std::vector<size_t>& p_distribution) const
	{
		double choice = 0;
		try {
			std::discrete_distribution<size_t> distribution(p_distribution.cbegin(), p_distribution.cend());
			const size_t GENERATED = distribution(*m_modelPtr->getGeneratorPtr());
			choice = static_cast<double>(GENERATED);
		}catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldmodelRandom::getRandomIndex", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return choice;
	}

	void FMTyieldmodelRandom::setModel(Models::FMTmodel* p_modelPtr)
	{
		FMTyieldmodel::setModel(p_modelPtr);
	}

	const std::vector<double>FMTyieldmodelRandom::Predict(const Core::FMTyieldrequest& p_request) const
	{
		std::vector<double>result;
		try {
			double value = 0;
			if (m_useCache && m_cache.find(p_request.getdevelopment().getperiod()) != m_cache.end())
			{
				value = m_cache.at(p_request.getdevelopment().getperiod());
			}else {
				const std::vector<size_t>DISTRIBUTION = getNormalizedYields(modelYields, p_request);
				value = getRandomIndex(DISTRIBUTION);		
				if (m_useCache)
					{
					m_cache[p_request.getdevelopment().getperiod()] = value;
					}
			}
			result.push_back(value);
			
		}catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldmodelRandom::Predict", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return result;
	}

	std::unique_ptr<FMTyieldmodel>FMTyieldmodelRandom::Clone() const
		{
		return std::unique_ptr<FMTyieldmodel>(new FMTyieldmodelRandom(*this));
		}

	std::unique_ptr<FMTyieldmodel> FMTyieldmodelRandom::presolve(const FMTmaskfilter& p_filter,
										const std::vector<FMTtheme>& p_newThemes) const
	{
		try {
			return Clone();
		}catch (...)
			{
			_exhandler->raisefromcatch(GetModelName(), "FMTyieldmodelRandom::presolve", __LINE__, __FILE__, Core::FMTsection::Yield);
			}
		return std::unique_ptr<FMTyieldmodel>(nullptr);
	}

	std::unique_ptr<FMTyieldmodel> FMTyieldmodelRandom::postsolve(const FMTmaskfilter& p_filter,
		const std::vector<FMTtheme>& p_baseThemes) const
	{
		return Clone();
	}
	std::string FMTyieldmodelRandom::GetModelType()
	{
		return "RANDOM";
	}


}
