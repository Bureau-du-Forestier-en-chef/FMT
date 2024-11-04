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
#include <memory>

namespace Core
{

	FMTyieldmodelVolToBiomass::FMTyieldmodelVolToBiomass(const boost::property_tree::ptree& p_jsonProps,
		const std::vector<std::string>& p_yields, const Core::FMTmask& p_mask) :
		m_cache(), m_mask(p_mask)
	{
		boost::property_tree::ptree::const_assoc_iterator modelNameIt = p_jsonProps.find(JSON_PROP_MODEL_NAME);
		modelName = modelNameIt->second.data();
		modelYields = p_yields;
	}

	const std::vector<double>FMTyieldmodelVolToBiomass::Predict(const Core::FMTyieldrequest& p_request) const
	{
		std::vector<double> Predictions;
		try {
			if (m_cache.empty())
			{
				const std::vector<FMToutput> OUTPUTS = getOutputs();
				Predictions = getOutputValues(OUTPUTS);
				m_cache = Predictions;
			}
			else {
				Predictions = m_cache;
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldmodelVolToBiomass::Predict", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return Predictions;
	}


	std::unique_ptr<FMTyieldmodel> FMTyieldmodelVolToBiomass::presolve(const FMTmaskfilter& p_filter,
		const std::vector<FMTtheme>& p_newThemes) const
	{
		FMTyieldmodelVolToBiomass newPresolved(*this);
		try {
			newPresolved.m_mask = newPresolved.m_mask.presolve(p_filter, p_newThemes);
		}
		catch (...)
		{
			_exhandler->raisefromcatch(GetModelName(), "FMTyieldmodelVolToBiomass::presolve", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return std::unique_ptr<FMTyieldmodel>(new FMTyieldmodelVolToBiomass(newPresolved));
	}

	std::unique_ptr<FMTyieldmodel> FMTyieldmodelVolToBiomass::postsolve(const FMTmaskfilter& p_filter,
		const std::vector<FMTtheme>& p_baseThemes) const
	{
		FMTyieldmodelVolToBiomass newPostsolved(*this);
		newPostsolved.m_mask.postsolve(p_filter, p_baseThemes);
		return std::unique_ptr<FMTyieldmodel>(new FMTyieldmodelVolToBiomass(newPostsolved));
	}

	std::unique_ptr<FMTyieldmodel>FMTyieldmodelVolToBiomass::Clone() const
	{
		return std::unique_ptr<FMTyieldmodel>(new FMTyieldmodelVolToBiomass(*this));
	}

	std::string FMTyieldmodelVolToBiomass::GetModelType()
	{
		return "UNIT_COVERAGE";
	}

	std::vector<FMToutput> FMTyieldmodelVolToBiomass::getOutputs() const
	{
		std::vector<FMToutput>outputs;
		try {
			for (const std::string& yld : modelYields)
			{
				std::vector<Core::FMToutputsource>sources;
				sources.push_back(Core::FMToutputsource(Core::FMTspec(), m_mask, Core::FMTotar::inventory, yld));
				outputs.push_back(Core::FMToutput(yld, yld, yld, sources, std::vector<Core::FMToperator>()));
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldmodelVolToBiomass::getOutputs", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return outputs;
	}

	std::vector<double> FMTyieldmodelVolToBiomass::getOutputValues(const std::vector<FMToutput>& p_outputs) const
	{
		std::vector<double>returnedValues(p_outputs.size());
		try {
			if (m_modelPtr == nullptr)
			{
				_exhandler->raise(Exception::FMTfunctionfailed,
					"No available model",
					"FMTyieldmodelVolToBiomass::getOutputValuesl", __LINE__, __FILE__);
			}
			size_t outId = 0;
			const int PERIOD_TARGET = m_modelPtr->getAreaPeriod();
			for (const FMToutput& OUTPUT : p_outputs)
			{
				returnedValues[outId] = m_modelPtr->getoutput(OUTPUT, PERIOD_TARGET, Core::FMToutputlevel::totalonly).at("Total");
				outId += 1;
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", " FMTyieldmodelVolToBiomass::getOutputValues", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return returnedValues;
	}

}


