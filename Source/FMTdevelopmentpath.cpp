/*
Copyright (c) 2019 Gouvernement du Québec

SPDX-License-Identifier: LiLiQ-R-1.1
License-Filename: LICENSES/EN/LiLiQ-R11unicode.txt
*/

#include "FMTdevelopmentpath.hpp"

namespace Core

{ 
	FMTdevelopmentpath::FMTdevelopmentpath(): development(), proportion()
		{

		}
	FMTdevelopmentpath::~FMTdevelopmentpath() {}
	FMTdevelopmentpath::FMTdevelopmentpath(const FMTdevelopmentpath& rhs) : development(), proportion(rhs.proportion)
		{
		development = std::unique_ptr<FMTdevelopment>(new FMTdevelopment(*rhs.development));
		}
	FMTdevelopmentpath::FMTdevelopmentpath(const FMTdevelopment& ldevelopment,
		const double& lproportion) :development(), proportion(lproportion)
		{
		development = std::unique_ptr<FMTdevelopment>(new FMTdevelopment(ldevelopment));
		}
	FMTdevelopmentpath& FMTdevelopmentpath::operator = (const FMTdevelopmentpath& rhs)
		{
		if (this!=&rhs)
			{
			development = std::unique_ptr<FMTdevelopment>(new FMTdevelopment(*rhs.development));
			proportion = rhs.proportion;
			}
		return *this;
		}

}


