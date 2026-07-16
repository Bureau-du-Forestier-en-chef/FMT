/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTyieldmodel.h"
#include "FMTyieldrequest.h"
#include "FMTgraph.hpp"
#include "FMTlinegraph.h"
#include "FMTyields.h"
#include "FMTpredictor.h"
#include "FMTsrmodel.h"
#include <vector>
#include "FMTexceptionhandler.h"
#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem.hpp>


namespace Core {

	FMTyieldmodel::~FMTyieldmodel() = default;



	FMTyieldmodel::FMTyieldmodel() : modelName(), modelYields(), m_modelPtr()
	{

	}


	const std::vector<std::string>& FMTyieldmodel::getModelYields() const
	{
		return modelYields;
	}

	const std::string& FMTyieldmodel::getModelName() const
	{
		return modelName;
	}

	std::unique_ptr<FMTyieldmodel> FMTyieldmodel::presolve(const FMTMaskFilter& filter,
		const std::vector<FMTTheme>& newthemes) const
	{
	
		return Clone();
	}

	std::unique_ptr<FMTyieldmodel> FMTyieldmodel::postSolve(const FMTMaskFilter& filter,
		const std::vector<FMTTheme>& basethemes) const
	{
		return Clone();
	}

	std::vector<std::vector<double>>FMTyieldmodel::getPeriodicValues() const
	{
		return  std::vector<std::vector<double>>();
	}

	void FMTyieldmodel::clearRandomYieldsCache()
	{

	}

	void FMTyieldmodel::setModel(Models::FMTmodel* p_modelPtr)
	{
		m_modelPtr = p_modelPtr;
	}

	FMTyieldmodel::operator std::string() const
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
			_exhandler->raiseFromCatch("", "FMTyieldmodel::operator std::string()", __LINE__, __FILE__, Core::FMTsection::Yield);
		}
		return value;
	}


}