/*
Copyright (c) 2024 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTyieldmodelUnitCoverage.hpp"
#include <vector>
#include <unordered_map>
#include "FMTtheme.hpp"
#include "FMToutput.hpp"
#include "FMTmodel.hpp"
#include <boost/property_tree/json_parser.hpp>
#include <memory>

namespace Core
{

	FMTyieldmodelUnitCoverage::FMTyieldmodelUnitCoverage(const boost::property_tree::ptree& p_jsonProps,
		const std::vector<std::string>& p_yields, const Core::FMTmask& p_mask):
		m_cache(), m_mask(p_mask)
	{
		boost::property_tree::ptree::const_assoc_iterator modelNameIt = p_jsonProps.find(JSON_PROP_MODEL_NAME);
		modelName = modelNameIt->second.data();
		modelYields = p_yields;
	}

	const std::vector<double>FMTyieldmodelUnitCoverage::Predict(const Core::FMTyieldrequest& p_request) const
	{
		std::vector<double> Predictions;
		try {
			if (m_cache.empty())
				{
				const std::vector<FMToutput> OUTPUTS = getOutputs();
				Predictions = getOutputValues(OUTPUTS);
				m_cache = Predictions;
			}else {
				Predictions = m_cache;
				}
		}catch (...)
			{
			_exhandler->raisefromcatch("", "FMTyieldmodelUnitCoverage::Predict", __LINE__, __FILE__, Core::FMTsection::Yield);
			}
	return Predictions;
	}


	std::unique_ptr<FMTyieldmodel> FMTyieldmodelUnitCoverage::presolve(const FMTmaskfilter& p_filter,
		const std::vector<FMTtheme>& p_newThemes) const
	{
		FMTyieldmodelUnitCoverage newPresolved(*this);
		newPresolved.m_mask.presolve(p_filter, p_newThemes);
		return std::unique_ptr<FMTyieldmodel>(new FMTyieldmodelUnitCoverage(newPresolved));
	}

	std::unique_ptr<FMTyieldmodel> FMTyieldmodelUnitCoverage::postsolve(const FMTmaskfilter& p_filter,
		const std::vector<FMTtheme>& p_baseThemes) const
	{
		FMTyieldmodelUnitCoverage newPostsolved(*this);
		newPostsolved.m_mask.postsolve(p_filter, p_baseThemes);
		return std::unique_ptr<FMTyieldmodel>(new FMTyieldmodelUnitCoverage(newPostsolved));
	}

	std::unique_ptr<FMTyieldmodel>FMTyieldmodelUnitCoverage::Clone() const
	{
		return std::unique_ptr<FMTyieldmodel>(new FMTyieldmodelUnitCoverage(*this));
	}

	std::string FMTyieldmodelUnitCoverage::GetModelType()
	{
		return "UNIT_COVERAGE";
	}

	std::vector<FMToutput> FMTyieldmodelUnitCoverage::getOutputs() const
	{
		std::vector<FMToutput>outputs;
		try {
			for (const std::string& yld : modelYields)
				{
				std::vector<Core::FMToutputsource>sources;
				sources.push_back(Core::FMToutputsource(Core::FMTspec(), m_mask, Core::FMTotar::inventory, yld));
				outputs.push_back(Core::FMToutput(yld, yld, yld, sources, std::vector<Core::FMToperator>()));
				}
		}catch (...)
		{
			_exhandler->raisefromcatch("", "FMTyieldmodelUnitCoverage::getOutputs", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return outputs;
	}

	std::vector<double> FMTyieldmodelUnitCoverage::getOutputValues(const std::vector<FMToutput>& p_outputs) const
	{
		std::vector<double>returnedValues(p_outputs.size());
		try {
			if (m_modelPtr==nullptr)
			{
				_exhandler->raise(Exception::FMTfunctionfailed,
					"No available model",
					"FMTyieldmodelUnitCoverage::getOutputValuesl", __LINE__, __FILE__);
			}
			size_t outId = 0;
			for (const FMToutput& OUTPUT : p_outputs)
			{
				returnedValues[outId] = m_modelPtr->getoutput(OUTPUT, 0, Core::FMToutputlevel::totalonly).at("Total");
				outId += 1;
			}
		}
		catch (...)
		{
			_exhandler->raisefromcatch("", " FMTyieldmodelUnitCoverage::getOutputValues", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return returnedValues;
	}

}


