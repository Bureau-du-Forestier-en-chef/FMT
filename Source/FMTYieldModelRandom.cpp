/*
Copyright (c) 2024 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTYieldModelRandom.h"
#include "FMTExceptionHandler.h"
#include "FMTYieldRequest.h"
#include "FMTGraphVertexToYield.h"
#include "FMTYieldHandler.h"
#include "FMTModel.h"
#include <boost/property_tree/json_parser.hpp>
#include <memory>

namespace Core
{

	FMTYieldModelRandom::FMTYieldModelRandom(const boost::property_tree::ptree& p_jsonProps, const std::vector<std::string>& p_distribution):
		FMTYieldModel(), m_cache(), m_useCache(false)
		{
		boost::property_tree::ptree::const_assoc_iterator modelNameIt = p_jsonProps.find(m_JSON_PROP_MODEL_NAME);
		m_modelName = modelNameIt->second.data();
		boost::property_tree::ptree::const_assoc_iterator modelCacheIt = p_jsonProps.find("caching");
		m_useCache = modelCacheIt->second.get_value<bool>();
		m_modelYields = p_distribution;
		}

	void FMTYieldModelRandom::clearRandomYieldsCache()
		{
		if (m_useCache)
			{
			m_cache.clear();
			}
		}

	std::vector<size_t> FMTYieldModelRandom::_getNormalizedYields(const std::vector<std::string>& p_yields, const Core::FMTYieldRequest& p_request) const
	{
		std::vector<size_t> values(p_yields.size());
		try {
			std::vector<double>baseValues(p_yields.size(),0.0);
			double totalValue = 0;
			size_t nameId = 0;
			for (const std::string& yieldName : p_yields)
			{
				bool gotYield = false;
				for (const FMTYieldRequest::const_iterator yield : p_request.getDatas())
				{
					if ((yield->second)->containsYield(yieldName))
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
						yieldName + " for developement " + std::string(p_request.getDevelopment()),
						"FMTYieldModelRandom::_getNormalizedYields", __LINE__, __FILE__, Core::FMTsection::Yield);
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
			_exhandler->raiseFromCatch("", "FMTYieldModelRandom::_getNormalizedYields", __LINE__, __FILE__, Core::FMTsection::Yield);
			}
		return values;
	}


	double FMTYieldModelRandom::_getRandomIndex(const std::vector<size_t>& p_distribution) const
	{
		double choice = 0;
		try {
			std::discrete_distribution<size_t> distribution(p_distribution.cbegin(), p_distribution.cend());
			const size_t GENERATED = distribution(*m_modelPtr->getGeneratorPtr());
			choice = static_cast<double>(GENERATED);
		}catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTYieldModelRandom::_getRandomIndex", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return choice;
	}

	void FMTYieldModelRandom::setModel(Models::FMTModel* p_modelPtr)
	{
		FMTYieldModel::setModel(p_modelPtr);
	}

	const std::vector<double>FMTYieldModelRandom::predict(const Core::FMTYieldRequest& p_request) const
	{
		std::vector<double>result;
		try {
			double value = 0;
			if (m_useCache && m_cache.find(p_request.getDevelopment().getPeriod()) != m_cache.end())
			{
				value = m_cache.at(p_request.getDevelopment().getPeriod());
			}else {
				const std::vector<size_t>DISTRIBUTION = _getNormalizedYields(m_modelYields, p_request);
				value = _getRandomIndex(DISTRIBUTION);		
				if (m_useCache)
					{
					m_cache[p_request.getDevelopment().getPeriod()] = value;
					}
			}
			result.push_back(value);
			
		}catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTYieldModelRandom::predict", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return result;
	}

	std::unique_ptr<FMTYieldModel>FMTYieldModelRandom::Clone() const
		{
		return std::unique_ptr<FMTYieldModel>(new FMTYieldModelRandom(*this));
		}

	std::unique_ptr<FMTYieldModel> FMTYieldModelRandom::presolve(const FMTMaskFilter& p_filter,
										const std::vector<FMTTheme>& p_newThemes) const
	{
		try {
			return Clone();
		}catch (...)
			{
			_exhandler->raiseFromCatch(getModelName(), "FMTYieldModelRandom::presolve", __LINE__, __FILE__, Core::FMTsection::Yield);
			}
		return std::unique_ptr<FMTYieldModel>(nullptr);
	}

	std::unique_ptr<FMTYieldModel> FMTYieldModelRandom::postSolve(const FMTMaskFilter& p_filter,
		const std::vector<FMTTheme>& p_baseThemes) const
	{
		return Clone();
	}
	std::string FMTYieldModelRandom::getModelType()
	{
		return "RANDOM";
	}


}
