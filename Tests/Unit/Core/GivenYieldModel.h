/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#ifndef TESTING_GIVENYIELDMODEL_HEADER
#define TESTING_GIVENYIELDMODEL_HEADER

// A yield model of the unit tests of Core. FMTYieldModel is abstract, meant to be implemented by machine learning
// models: those that FMTYieldParser builds compute their predictions from an ONNX file or from the graph of a model. This
// one returns the predictions and the periodic values it is given, so that the classes that use a yield model can be
// tested without a file or a graph.

#include "FMTYieldModel.h"
#include "FMTYieldRequest.h"

#include <memory>
#include <string>
#include <vector>

namespace Models
{
	class FMTModel;
}

namespace Testing
{
	class GivenYieldModel : public Core::FMTYieldModel
	{
	public:
		// A model of the file p_fileName, of the source yields p_sources, which predicts p_predictions and whose
		// periodic values are p_periodicValues (none: the model cannot be turned into a time yield).
		GivenYieldModel(const std::string& p_fileName, const std::vector<std::string>& p_sources,
			const std::vector<double>& p_predictions,
			const std::vector<std::vector<double>>& p_periodicValues = std::vector<std::vector<double>>()) :
			Core::FMTYieldModel(), m_predictions(p_predictions), m_periodicValues(p_periodicValues)
		{
			m_modelName = p_fileName;
			m_modelYields = p_sources;
		}

		std::unique_ptr<Core::FMTYieldModel> clone() const override
		{
			return std::unique_ptr<Core::FMTYieldModel>(new GivenYieldModel(*this));
		}

		const std::vector<double> predict(const Core::FMTYieldRequest&) const override
		{
			return m_predictions;
		}

		std::vector<std::vector<double>> getPeriodicValues() const override
		{
			return m_periodicValues;
		}

		// The model that FMTYieldModel::setModel gave it.
		const Models::FMTModel* getModel() const
		{
			return m_modelPtr;
		}

	private:
		std::vector<double> m_predictions;
		std::vector<std::vector<double>> m_periodicValues;
	};
}

#endif
