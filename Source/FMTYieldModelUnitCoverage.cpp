/*
Copyright (c) 2024 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTYieldModelUnitCoverage.h"
#include <vector>
#include <unordered_map>
#include "FMTTheme.h"
#include "FMTOutput.h"
#include "FMTModel.h"
#include <boost/property_tree/json_parser.hpp>
#include <memory>

namespace Core
{

	FMTYieldModelUnitCoverage::FMTYieldModelUnitCoverage(const boost::property_tree::ptree& p_jsonProps,
		const std::vector<std::string>& p_yields, const Core::FMTMask& p_mask):
		m_cache(), m_mask(p_mask)
	{
		boost::property_tree::ptree::const_assoc_iterator modelNameIt = p_jsonProps.find(m_JSON_PROP_MODEL_NAME);
		m_modelName = modelNameIt->second.data();
		m_modelYields = p_yields;
	}

	const std::vector<double>FMTYieldModelUnitCoverage::predict(const Core::FMTYieldRequest& p_request) const
	{
		std::vector<double> Predictions;
		try {
			if (m_cache.empty())
				{
				const std::vector<FMTOutput> OUTPUTS = getOutputs();
				Predictions = getOutputValues(OUTPUTS);
				m_cache = Predictions;
			}else {
				Predictions = m_cache;
				}
		}catch (...)
			{
			_exhandler->raiseFromCatch("", "FMTYieldModelUnitCoverage::predict", __LINE__, __FILE__, Core::FMTsection::Yield);
			}
	return Predictions;
	}


	std::unique_ptr<FMTYieldModel> FMTYieldModelUnitCoverage::preSolve(const FMTMaskFilter& p_filter,
		const std::vector<FMTTheme>& p_newThemes) const
	{
		FMTYieldModelUnitCoverage newPresolved(*this);
		try {
			newPresolved.m_mask = newPresolved.m_mask.preSolve(p_filter, p_newThemes);
		}catch (...)
		{
			_exhandler->raiseFromCatch(getModelName(), "FMTYieldModelUnitCoverage::preSolve", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return std::unique_ptr<FMTYieldModel>(new FMTYieldModelUnitCoverage(newPresolved));
	}

	std::unique_ptr<FMTYieldModel> FMTYieldModelUnitCoverage::postSolve(const FMTMaskFilter& p_filter,
		const std::vector<FMTTheme>& p_baseThemes) const
	{
		FMTYieldModelUnitCoverage newPostsolved(*this);
		newPostsolved.m_mask.postSolve(p_filter, p_baseThemes);
		return std::unique_ptr<FMTYieldModel>(new FMTYieldModelUnitCoverage(newPostsolved));
	}

	std::unique_ptr<FMTYieldModel>FMTYieldModelUnitCoverage::clone() const
	{
		return std::unique_ptr<FMTYieldModel>(new FMTYieldModelUnitCoverage(*this));
	}

	std::string FMTYieldModelUnitCoverage::getModelType()
	{
		return "UNIT_COVERAGE";
	}

	std::vector<FMTOutput> FMTYieldModelUnitCoverage::getOutputs() const
	{
		std::vector<FMTOutput>outputs;
		try {
			for (const std::string& yld : m_modelYields)
				{
				std::vector<Core::FMTOutputSource>sources;
				sources.push_back(Core::FMTOutputSource(Core::FMTSpec(), m_mask, Core::FMTotar::inventory, yld));
				outputs.push_back(Core::FMTOutput(yld, yld, yld, sources, std::vector<Core::FMTOperator>()));
				}
		}catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTYieldModelUnitCoverage::getOutputs", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return outputs;
	}

	std::vector<double> FMTYieldModelUnitCoverage::getOutputValues(const std::vector<FMTOutput>& p_outputs) const
	{
		std::vector<double>returnedValues(p_outputs.size());
		try {
			if (m_modelPtr==nullptr)
			{
				_exhandler->raise(Exception::FMTfunctionfailed,
					"No available model",
					"FMTYieldModelUnitCoverage::getOutputValues", __LINE__, __FILE__);
			}
			size_t outId = 0;
			const int PERIOD_TARGET = m_modelPtr->getAreaPeriod();
			for (const FMTOutput& OUTPUT : p_outputs)
			{
				returnedValues[outId] = m_modelPtr->getOutput(OUTPUT, PERIOD_TARGET, Core::FMToutputlevel::totalonly).at("Total");
				outId += 1;
			}
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", " FMTyieldmodelUnitCoverage::getOutputValues", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return returnedValues;
	}

}


