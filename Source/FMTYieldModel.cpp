/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTYieldModel.h"
#include "FMTYieldRequest.h"
#include "FMTGraph.hpp"
#include "FMTLineGraph.h"
#include "FMTYields.h"
#include "FMTPredictor.h"
#include "FMTSrModel.h"
#include <vector>
#include "FMTExceptionHandler.h"
#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem.hpp>


namespace Core {

	FMTYieldModel::~FMTYieldModel() = default;



	FMTYieldModel::FMTYieldModel() : m_modelName(), m_modelYields(), m_modelPtr()
	{

	}


	const std::vector<std::string>& FMTYieldModel::getModelYields() const
	{
		return m_modelYields;
	}

	const std::string& FMTYieldModel::getModelName() const
	{
		return m_modelName;
	}

	std::unique_ptr<FMTYieldModel> FMTYieldModel::preSolve(const FMTMaskFilter& filter,
		const std::vector<FMTTheme>& newthemes) const
	{
	
		return clone();
	}

	std::unique_ptr<FMTYieldModel> FMTYieldModel::postSolve(const FMTMaskFilter& filter,
		const std::vector<FMTTheme>& basethemes) const
	{
		return clone();
	}

	std::vector<std::vector<double>>FMTYieldModel::getPeriodicValues() const
	{
		return  std::vector<std::vector<double>>();
	}

	void FMTYieldModel::clearRandomYieldsCache()
	{

	}

	void FMTYieldModel::setModel(Models::FMTModel* p_modelPtr)
	{
		m_modelPtr = p_modelPtr;
	}

	FMTYieldModel::operator std::string() const
	{
		std::string value = "";
		try {
			const std::string completename = getModelName();
			const boost::filesystem::path modelpath(completename);
			const boost::filesystem::path dir = modelpath.parent_path();
			const std::string shortmodelname = dir.stem().string();
			std::string data(shortmodelname);
			for (const std::string yield : getModelYields())
			{
				data += ("," + yield);
			}
			value = " _PRED(" + data + ")\n";
		}
		catch (...)
		{
			_exhandler->raiseFromCatch("", "FMTYieldModel::operator std::string()", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return value;
	}


}