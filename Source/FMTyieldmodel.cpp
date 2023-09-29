/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTyieldmodel.hpp"
#include "FMTyieldrequest.hpp"
#include "FMTgraph.hpp"
#include "FMTlinegraph.hpp"
#include "FMTyields.hpp"
#include "FMTpredictor.hpp"
#include "FMTsrmodel.hpp"
#include <vector>
#include "FMTexceptionhandler.hpp"


namespace Core {

	FMTyieldmodel::~FMTyieldmodel() = default;



	FMTyieldmodel::FMTyieldmodel() : modelName(), modelYields()
	{

	}


	const std::vector<std::string>& FMTyieldmodel::GetModelYields() const
	{
		return modelYields;
	}

	const std::string& FMTyieldmodel::GetModelName() const
	{
		return modelName;
	}

	std::unique_ptr<FMTyieldmodel> FMTyieldmodel::presolve(const FMTmaskfilter& filter,
		const std::vector<FMTtheme>& newthemes) const
	{
	
		return Clone();
	}

	std::unique_ptr<FMTyieldmodel> FMTyieldmodel::postsolve(const FMTmaskfilter& filter,
		const std::vector<FMTtheme>& basethemes) const
	{
		return Clone();
	}

	std::vector<std::vector<double>>FMTyieldmodel::getperiodicvalues() const
	{
		return  std::vector<std::vector<double>>();
	}


}